#include "Animator.h"
#include "components/Body2D.h"
#include "components/Button.h"
#include "components/Camera.h"
#include "components/Canvas.h"
#include "components/CharacterController.h"
#include "components/Component.h"
#include "components/Light.h"
#include "components/Mesh.h"
#include "components/Script.h"
#include "components/Sprite.h"
#include "components/Text.h"
#include "components/TileMap.h"
#include "components/Transform.h"
#include "GameObject.h"
#include "GuiCore.h"
#include "Inspector.h"
#include "managers/AnimationManager.h"
#include "managers/Assets.h"
#include "managers/LuaManager.h"
#include "managers/ProjectManager.h"
#include "managers/SceneManager.h"
#include "managers/Settings.h"
#include "managers/TileSetManager.h"
#include "Modals.h"
#include "physics/Joint2D.h"
#include "physics/PhysicsManager.h"
#include "physics/Shape2D.h"
#include "render/VulkanManager.h"
#include "scripting/CPPScriptMethods.h"
#include "TagList.h"
#include "tools/FileHelper.h"
#include "tools/Logger.h"
#include "tools/Numbers.h"

#include "imgui.h"
#include "tools/Vector2.h"
#include <X11/Xlib.h>
#include <cstdint>
#include <id.h>
#include <string>

namespace FL = FlatEngine;


namespace FlatGui 
{
	namespace Inspector
    {	
		bool RenderIsActiveCheckbox(bool& b_isActive)
		{
			FL::GuiCore::MoveScreenCursor(0, 3);
			bool b_checked = FL::GuiCore::RenderCheckbox("Active", b_isActive);			
			FL::GuiCore::RenderSeparator(3, 3);	

			return b_checked;
		}

		void BeginComponent(FL::Component* component, FL::Component*& queuedForDelete)
		{
			bool b_isCollapsed = component->IsCollapsed();
			long ownerID = component->GetOwnerID();

			std::string componentType = component->GetTypeString();
			std::string componentID = component->GetTypeString() + std::to_string(ownerID);

			// Begin Component
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::Assets::assetManager.GetColor("componentBg"));			
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, FL::Vector2(0, 0));							
			ImGui::BeginChild(componentID.c_str(), FL::Vector2(0, 0), FL::GuiCore::autoResizeChildFlags);		
			// {
								
				// Component Name													
				FL::GuiCore::RenderSectionHeader("  " + componentType, 0, 0, "componentSectionHeaderBg", "componentSectionHeaderSeparator");				
				FL::GuiCore::MoveScreenCursor(2, -22);
				ImGui::Image(FL::Assets::assetManager.GetTexture(FL::ComponentTypeStrings[component->GetType()]), FL::Vector2(16));

				if (component->GetType() == FL::ComponentType_Transform)
				{
					FL::GuiCore::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 18, -20);					
				}
				else
				{
					FL::GuiCore::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 36, -20);
					if (FL::GuiCore::RenderImageButton("##trashIcon-" + std::to_string(ownerID), FL::Assets::assetManager.GetTexture("trash")))
					{
						queuedForDelete = component;
					}
					ImGui::SameLine(0, 2);
				}

				std::string expandString = b_isCollapsed ? "expand" : "expandFlipped";
				if (FL::GuiCore::RenderImageButton("##expandIcon-" + std::to_string(ownerID), FL::Assets::assetManager.GetTexture(expandString)))				
					component->SetCollapsed(!b_isCollapsed);				

				if (b_isCollapsed)				
					FL::GuiCore::MoveScreenCursor(0,1); // reveals bottom separator on header				
				else
				{					
					bool b_isActive = component->IsActive();
					if (RenderIsActiveCheckbox(b_isActive))
					{
						component->SetActive(b_isActive);
					}
				}

			// ...
			//
			//  } // EndComponent();
		}

		void EndComponent(FL::Component* component)
		{
			if (!component->IsCollapsed())
			{
				FL::GuiCore::MoveScreenCursor(0, 3); // Bottom component padding
			}

			// }
			ImGui::EndChild(); // Component

			ImGui::PopStyleVar();			
			ImGui::PopStyleColor();	

			if (component->IsCollapsed())
			{
				FL::GuiCore::MoveScreenCursor(0, -5);
			}
			else 
			{
				FL::GuiCore::RenderSeparator(0,-1);
			}
		}

		void RenderTransformComponent(FL::Transform* transform)
		{	
			FL::Vector3 position = transform->GetPosition();
			FL::Vector3 rotation = transform->GetRotation();
			FL::Vector3 scale = transform->GetScale();
			
			std::vector<std::string> valueColors = { "transformXBGLight", "transformYBGLight", "transformZBGLight", "transformWBGLight" };	
			FL::Vector2 tableSize = FL::Vector2(ImGui::GetContentRegionAvail().x, 0);
			FL::GuiCore::TableProps positionProps = FL::GuiCore::TableProps("##TransformComponentTable", "Position", tableSize);
			positionProps.labelWidth = 68;
			positionProps.valueLabelColors = valueColors;
			FL::GuiCore::TableProps rotationProps = FL::GuiCore::TableProps("##TransformComponentTable", "Rotation", tableSize);
			rotationProps.labelWidth = 68;
			rotationProps.valueLabelColors = valueColors;
			FL::GuiCore::TableProps scaleProps = FL::GuiCore::TableProps("##TransformComponentTable", "Scale", tableSize);
			scaleProps.labelWidth = 68;
			scaleProps.valueLabelColors = valueColors;
			scaleProps.min = 0;

			if (FL::GuiCore::RenderVector3Table(positionProps, position)) transform->SetPosition(position);		
			if (FL::GuiCore::RenderVector3Table(rotationProps, rotation)) transform->SetRotation(rotation);			
			if (FL::GuiCore::RenderVector3Table(scaleProps, scale)) transform->SetScale(scale);
		}

		bool RenderPivotSelectionButtons(std::string componentType, FL::Pivot& pivot)
		{				
			ImGui::Text("%s", "Pivot Point: ");
			ImGui::SameLine();
			ImGui::Text("%s", FL::F_PivotStrings[pivot].c_str());
			FL::GuiCore::MoveScreenCursor(0, 1);		

			bool b_pivotChanged = false;
			FL::Vector2 cellSize =FL::Vector2(ImGui::GetContentRegionAvail().x, 72);
			FL::Vector2 cursorScreen = ImGui::GetCursorScreenPos();

			// TopLeft, Top, TopRight
			ImGui::GetWindowDrawList()->AddRectFilled(cursorScreen,FL::Vector2(cursorScreen.x + cellSize.x, cursorScreen.y + cellSize.y), FL::Assets::assetManager.GetColor32("pivotSelectionBg"));
			FL::GuiCore::MoveScreenCursor(5, 5);
			if (FL::GuiCore::RenderImageButton("##" + componentType + "PivotTopLeftButton", FL::Assets::assetManager.GetTexture("upLeft"),FL::Vector2(16, 16),0 ,FL::Vector2(1, 1), "buttonBorder", "imageButtonDark", "imageButtonTint", "pivotButtonHovered"))
			{
				pivot = FL::Pivot::PivotTopLeft;
				b_pivotChanged = true;
			}
			ImGui::SameLine(0, 3);
			if (FL::GuiCore::RenderImageButton("##" + componentType + "PivotTopButton", FL::Assets::assetManager.GetTexture("up"),FL::Vector2(16, 16), 0, FL::Vector2(1, 1), "buttonBorder", "imageButtonDark", "imageButtonTint", "pivotButtonHovered"))
			{
				pivot = FL::Pivot::PivotTop;
				b_pivotChanged = true;
			}
			ImGui::SameLine(0, 3);
			if (FL::GuiCore::RenderImageButton("##" + componentType + "PivotTopRightButton", FL::Assets::assetManager.GetTexture("upRight"),FL::Vector2(16, 16), 0, FL::Vector2(1, 1), "buttonBorder", "imageButtonDark", "imageButtonTint", "pivotButtonHovered"))
			{
				pivot = FL::Pivot::PivotTopRight;
				b_pivotChanged = true;
			}

			// Left, Center, Right
			FL::GuiCore::MoveScreenCursor(5, 0);
			if (FL::GuiCore::RenderImageButton("##" + componentType + "PivotLeftButton", FL::Assets::assetManager.GetTexture("left"),FL::Vector2(16), 0, FL::Vector2(1), "buttonBorder", "imageButtonDark", "imageButtonTint", "pivotButtonHovered"))
			{
				pivot = FL::Pivot::PivotLeft;
				b_pivotChanged = true;
			}
			ImGui::SameLine(0, 3);
			if (FL::GuiCore::RenderImageButton("##" + componentType + "PivotCenterButton", FL::Assets::assetManager.GetTexture("center"),FL::Vector2(16), 0, FL::Vector2(1), "buttonBorder", "imageButtonDark", "imageButtonTint", "pivotButtonHovered"))
			{
				pivot = FL::Pivot::PivotCenter;
				b_pivotChanged = true;
			}
			ImGui::SameLine(0, 3);
			if (FL::GuiCore::RenderImageButton("##" + componentType + "PivotRightButton", FL::Assets::assetManager.GetTexture("right"),FL::Vector2(16), 0, FL::Vector2(1), "buttonBorder", "imageButtonDark", "imageButtonTint", "pivotButtonHovered"))
			{
				pivot = FL::Pivot::PivotRight;
				b_pivotChanged = true;
			}

			// BottomLeft, Bottom, BottomRight		
			FL::GuiCore::MoveScreenCursor(5, 0);
			if (FL::GuiCore::RenderImageButton("##" + componentType + "PivotBottomLeftButton", FL::Assets::assetManager.GetTexture("downLeft"),FL::Vector2(16), 0, FL::Vector2(1), "buttonBorder", "imageButtonDark", "imageButtonTint", "pivotButtonHovered"))
			{
				pivot = FL::Pivot::PivotBottomLeft;
				b_pivotChanged = true;
			}
			ImGui::SameLine(0, 3);
			if (FL::GuiCore::RenderImageButton("##" + componentType + "PivotBottomButton", FL::Assets::assetManager.GetTexture("down"),FL::Vector2(16), 0, FL::Vector2(1), "buttonBorder", "imageButtonDark", "imageButtonTint", "pivotButtonHovered"))
			{
				pivot = FL::Pivot::PivotBottom;
				b_pivotChanged = true;
			}
			ImGui::SameLine(0, 3);
			if (FL::GuiCore::RenderImageButton("##" + componentType + "PivotBottomRightButton", FL::Assets::assetManager.GetTexture("downRight"),FL::Vector2(16), 0, FL::Vector2(1), "buttonBorder", "imageButtonDark", "imageButtonTint", "pivotButtonHovered"))
			{
				pivot = FL::Pivot::PivotBottomRight;
				b_pivotChanged = true;
			}
					
			return b_pivotChanged;
		}

		void RenderSpriteComponent(FL::Sprite* sprite)
		{
			std::string path = FL::FileHelper::GetFilenameFromPath(sprite->GetPath(), true);
			int textureWidth = sprite->GetTextureWidth();
			int textureHeight = sprite->GetTextureHeight();
			FL::Vector2 textureScale = sprite->GetScale();
			FL::Pivot pivotPoint = sprite->GetPivotPoint();
			std::string pivotString = sprite->GetPivotPointString();
			float xScale = textureScale.x;
			float yScale = textureScale.y;
			int renderOrder = sprite->GetRenderOrder();
			FL::Vector2 offset = sprite->GetOffset();
			float xOffset = offset.x;
			float yOffset = offset.y;
			std::string pathString = "Path: ";
			std::string textureWidthString = std::to_string(textureWidth) + "px";
			std::string textureHeightString = std::to_string(textureHeight) + "px";
			FL::Vector4 tintColor = sprite->GetTintColor();
			long ownerID = sprite->GetOwnerID();		

			int droppedValue = -1;
			std::string openedPath = "";
			if (FL::GuiCore::DropInputCanOpenFiles("##InputSpritePath", "File", FL::FileHelper::GetFilenameFromPath(path, true), FL::GuiCore::fileExplorerTarget, droppedValue, openedPath, "Drop images here from File Explorer"))
			{
				if (droppedValue >= 0)
				{
					std::filesystem::path fsPath(FL::GuiCore::selectedFiles[droppedValue - 1]);
					if (fsPath.extension() == ".png" || fsPath.extension() == ".jpg" || fsPath.extension() == ".tif" || fsPath.extension() == ".webp" || fsPath.extension() == ".jxl")
					{
						sprite->SetTexture(fsPath.string());
					}
					else
					{
						FL::Logger::log.Err("File must be of type .png to drop here.");
					}
				}
				else if (droppedValue == -2)
				{
					sprite->RemoveTexture();
				}
				else if (openedPath != "")
				{
					sprite->SetTexture(openedPath);
				}
			}

			FL::GuiCore::RenderSeparator(2, 3);
			
			if (FL::GuiCore::PushTable("##SpriteProperties" + std::to_string(ownerID), 2))
			{
				if (FL::GuiCore::RenderFloatDragTableRow("##xSpriteScaleDrag" + std::to_string(ownerID), "X Scale", xScale, 0.1f, -FLT_MAX, FLT_MAX))
				{
					sprite->SetScale(FL::Vector2(xScale, yScale));
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##ySpriteScaleDrag" + std::to_string(ownerID), "Y Scale", yScale, 0.1f, -FLT_MAX, FLT_MAX))
				{
					sprite->SetScale(FL::Vector2(xScale, yScale));
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##xSpriteOffsetDrag" + std::to_string(ownerID), "X Offset", xOffset, 0.1f, -FLT_MAX, FLT_MAX))
				{
					sprite->SetOffset(FL::Vector2(xOffset, yOffset));
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##ySpriteOffsetDrag" + std::to_string(ownerID), "Y Offset", yOffset, 0.1f, -FLT_MAX, FLT_MAX))
				{
					sprite->SetOffset(FL::Vector2(xOffset, yOffset));
				}
				if (FL::GuiCore::RenderIntDragTableRow("##renderOrder" + std::to_string(ownerID), "Render Order", renderOrder, 1, 0, (int)FL::VulkanManager::maxSpriteLayers))
				{
					sprite->SetRenderOrder(renderOrder);
				}				
				FL::GuiCore::PopTable();
				FL::GuiCore::RenderTextTable(FL::GuiCore::TableProps("##textureWidth" + std::to_string(ownerID), "Texture width"), {textureWidthString});
				FL::GuiCore::RenderTextTable(FL::GuiCore::TableProps("##textureHeight" + std::to_string(ownerID), "Texture height"), {textureHeightString});
			}

			FL::GuiCore::RenderSeparator(3, 3);

			if (RenderPivotSelectionButtons("Sprite", pivotPoint))
			{
				sprite->SetPivotPoint(pivotPoint);
			}

			FL::GuiCore::RenderSeparator(6, 3);

			// Tint color picker
			std::string tintID = "##SpriteTintColor" + std::to_string(ownerID) + "-" + std::to_string(ownerID);		
			if (ImGui::ColorEdit4(tintID.c_str(), (float*)&tintColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
			{
				sprite->SetTintColor(tintColor);
			}
			ImGui::SameLine(0, 5);
			ImGui::Text("%s", "Tint color");
		}

		void RenderCameraComponent(FL::Camera* camera)
		{			
			bool b_isPrimary = camera->IsPrimary();
			long ownerID = camera->GetOwnerID();
			bool b_follow = camera->b_shouldFollow;
			std::string followingName = "";
			long toFollowID = camera->toFollowID;
			FL::GameObject* followingObject = FL::SceneManager::loadedScene.GetObjectByID(toFollowID);

			float nearClippingDistance = camera->nearClippingDistance;
			float farClippingDistance = camera->farClippingDistance;
			float perspectiveAngle = camera->perspectiveAngle;

			if (toFollowID != -1 && followingObject != nullptr)
			{
				followingName = FL::SceneManager::loadedScene.GetObjectByID(toFollowID)->GetName();
			}
			else if (followingObject == nullptr)
			{
				camera->toFollowID = -1;
			}
			

			FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##orthonearClip" + std::to_string(ownerID), "Near Clip (Ortho)"), camera->orthoNearClippingDistance);
			FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##orthofarClip" + std::to_string(ownerID), "Far Clip (Ortho)"), camera->orthoFarClippingDistance);
			FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##nearClip" + std::to_string(ownerID), "Near Clip"), camera->nearClippingDistance);				
			FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##farClip" + std::to_string(ownerID), "Far Clip"), camera->farClippingDistance);								
			FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##perspectiveAngle" + std::to_string(ownerID), "Perspective Angle", FL::Vector2(), 0.1f, -180.0, 180), camera->perspectiveAngle);																
			FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##cameraFollowSmoothing" + std::to_string(ownerID), "Follow smoothing", FL::Vector2(), 0.01f, 0, 1), camera->followSmoothing);									
			int gridStep = (int)camera->gridStep;			
			if (FL::GuiCore::RenderInt32Table(FL::GuiCore::TableProps("##gridStep" + std::to_string(ownerID), "Pixels/Grid Square", FL::Vector2(), 1, FL::SceneView::minGridStep, FL::SceneView::maxGridStep), gridStep)) { if (gridStep > 0) camera->gridStep = (uint32_t)gridStep;}							
			FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("##Orthographic" + std::to_string(ownerID), "Orthographic"), camera->b_orthographic);

			FL::GuiCore::RenderSeparator(3, 3);

			int droppedValue = -1;
			if (FL::GuiCore::DropInput("##CameraFollowObject", "Following", followingName, "DND_HIERARCHY_OBJECT", droppedValue, "Drag a GameObject here from the Hierarchy"))
			{
				if (FL::SceneManager::loadedScene.GetObjectByID(droppedValue) != nullptr || droppedValue == -1)
				{
					camera->toFollowID = droppedValue;
				}
			}

			ImGui::BeginDisabled(toFollowID == -1);
			FL::GuiCore::RenderCheckbox("Follow", camera->b_shouldFollow);
			ImGui::EndDisabled();

			FL::GuiCore::RenderSeparator(3, 3);
				
			if (FL::GuiCore::RenderCheckbox("Is Primary Camera", b_isPrimary))
			{
				camera->SetPrimaryCamera(b_isPrimary);
			}		

			// Frustrum color picker
			// std::string frustrumID = "##FrustrumColor" + std::to_string(ownerID);
			// ImVec4 color = ImVec4(frustrumColor.x / 255.0f, frustrumColor.y / 255.0f, frustrumColor.z / 255.0f, frustrumColor.w / 255.0f);
			// ImGui::ColorEdit4(frustrumID.c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
			// ImGui::SameLine(0, 5);
			// ImGui::Text("%s", "Frustrum color");
			// camera->SetFrustrumColor(ImVec4(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f));
		}

		void RenderScriptComponent(FL::Script* script)
		{
			std::vector<std::string> allScriptNames = FL::LuaManager::luaScriptNames;
			long ownerID = script->GetOwnerID();
			int luaScriptCount = (int)FL::LuaManager::luaScriptNames.size();
			for (std::string CPPScript : FL::cppScriptNames)
			{
				allScriptNames.push_back(CPPScript);
			}

			if (FL::GuiCore::RenderButton("Add Script",FL::Vector2(80, 20)))
			{
				script->AddScript();
			}
			ImGui::SameLine();
			ImGui::Text("CREATE NEW SCRIPTS:"); ImGui::SameLine(); 
			if (FL::GuiCore::RenderButton("lua",FL::Vector2(30, 20)))
			{
				Modals::b_openLuaModal = true;
			}
			ImGui::SameLine(0,5); 			
			if (FL::GuiCore::RenderButton("C++",FL::Vector2(30, 20)))
			{
				Modals::b_openCPPModal = true;
			}

			FL::GuiCore::RenderSectionHeader("", 3.0f, 3.0f, "sectionHeaderEmptyBg", "separator");			

			int scriptCounter = 0;
			FL::ScriptData* scriptQueuedForDelete = nullptr;
			for (FL::ScriptData& scriptData : script->GetScripts())
			{
				int currentScript = 0;
				for (int i = 0; i < allScriptNames.size(); i++)
				{
					if (scriptData.name == allScriptNames[i])
					{
						currentScript = i;
						break;
					}
				}

				if (FL::GuiCore::RenderSelectable("##SelectLuaScript_" + std::to_string(scriptCounter), allScriptNames, currentScript, "selectableSecondaryBg", ImGui::GetContentRegionAvail().x - 23))
				{
					scriptData.SetAttachedScript(allScriptNames[currentScript]);
				}
				
				ImGui::SameLine(0,2);
				
				if (FL::GuiCore::RenderImageButton("##deleteScriptData_" + std::to_string(scriptCounter), FL::Assets::assetManager.GetTexture("trash"), FL::Vector2(15), 0.0f, FL::Vector2(3)))
				{
					scriptQueuedForDelete = &scriptData;
				}

				if (currentScript < allScriptNames.size() && currentScript != 0)
				{					
					FL::GuiCore::RenderLuaParametersTable(std::to_string(scriptCounter), scriptData.name + " Parameters", scriptData.scriptParamContainer);
				}

				scriptCounter++;
			}

			if (scriptQueuedForDelete != nullptr)
			{
				script->RemoveScript(scriptQueuedForDelete);
			}
		}

		void RenderButtonComponent(FL::Button* button)
		{
			float activeWidth = button->GetActiveWidth();
			float activeHeight = button->GetActiveHeight();
			FL::Vector2 activeOffset = button->GetActiveOffset();
			int activeLayer = button->GetActiveLayer();	
			bool b_leftClick = button->GetLeftClick();
			bool b_rightClick = button->GetRightClick();
			FL::LuaManager::LuaParameter functionParams = button->GetFunctionParams();
			long ownerID = button->GetOwnerID();
			std::string functionName = functionParams.name;
			// bool b_cppEvent = functionParams.b_cppEvent;
			// bool b_luaEvent = functionParams.b_luaEvent;

			if (FL::GuiCore::PushTable("##ButtonProperties" + std::to_string(ownerID), 2))
			{
				if (FL::GuiCore::RenderIntDragTableRow("##activeLayer" + std::to_string(ownerID), "Active layer", activeLayer, 1, 20, 20))
				{
					button->SetActiveLayer(activeLayer);
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##activeWidth" + std::to_string(ownerID), "Active width", activeWidth, 0.1f, 0, 1000))
				{
					button->SetActiveDimensions(activeWidth, activeHeight);
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##activeHeight" + std::to_string(ownerID), "Active height", activeHeight, 0.1f, 0, 1000))
				{
					button->SetActiveDimensions(activeWidth, activeHeight);
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##activeoffsetx" + std::to_string(ownerID), "X Offset", activeOffset.x, 0.1f, -FLT_MAX, FLT_MAX))
				{
					button->SetActiveOffset(activeOffset);
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##activeoffsety" + std::to_string(ownerID), "Y Offset", activeOffset.y, 0.1f, -FLT_MAX, FLT_MAX))
				{
					button->SetActiveOffset(activeOffset);
				}
				if (FL::GuiCore::RenderCheckboxTableRow("##leftClickableCheckbox" + std::to_string(ownerID), "Left Click", b_leftClick))
				{
					button->SetLeftClick(b_leftClick);
				}
				if (FL::GuiCore::RenderCheckboxTableRow("##rightClickableCheckbox" + std::to_string(ownerID), "Right Click", b_rightClick))
				{
					button->SetRightClick(b_rightClick);
				}
				FL::GuiCore::PopTable();

				std::string choices[2] = { "C++", "Lua" };
				std::string currentChoice = "";

				// if (b_cppEvent)
				// {
				// 	currentChoice = "C++";
				// }
				// else if (b_luaEvent)
				// {
				// 	currentChoice = "Lua";
				// }

				std::string cppRadioID = "C++ Function##" + std::to_string(ownerID);
				std::string luaRadioID = "Lua Function##" + std::to_string(ownerID);

				// if (ImGui::RadioButton(cppRadioID.c_str(), currentChoice == choices[0]))
				// {
				// 	currentChoice = choices[0];
				// 	functionParams->b_cppEvent = true;
				// 	functionParams->b_luaEvent = false;
				// }
				// if (ImGui::RadioButton(luaRadioID.c_str(), currentChoice == choices[1]))
				// {
				// 	currentChoice = choices[1];
				// 	functionParams->b_cppEvent = false;
				// 	functionParams->b_luaEvent = true;
				// }

				FL::GuiCore::RenderSeparator(1, 1);

				// if (functionParams->b_cppEvent)
				// {
				// 	int currentEventFunction = 0;
				// 	std::vector<std::string> eventFunctions = { "- none -" };

				// 	for (std::map<std::string, void (*)(GameObject*, Animation::S_EventFunctionParam)>::iterator iter = FL::F_CPPAnimationEventFunctions.begin(); iter != FL::F_CPPAnimationEventFunctions.end(); iter++)
				// 	{
				// 		eventFunctions.push_back(iter->first);
				// 	}

				// 	for (int i = 0; i < eventFunctions.size(); i++)
				// 	{
				// 		if (functionParams->functionName == eventFunctions[i])
				// 		{
				// 			currentEventFunction = i;
				// 		}
				// 	}

				// 	if (eventFunctions.size())
				// 	{
				// 		FL::GuiCore::MoveScreenCursor(0, 3);
				// 		ImGui::Text("%s", "Callback Function:");
				// 		ImGui::SameLine();
				// 		FL::GuiCore::MoveScreenCursor(0, -3);
				// 		std::string comboID = "##EventFunctionName";
				// 		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				// 		if (FL::GuiCore::RenderCombo(comboID, eventFunctions[currentEventFunction], eventFunctions, currentEventFunction))
				// 		{
				// 			functionParams->functionName = eventFunctions[currentEventFunction];
				// 		}		
				// 		FL::GuiCore::MoveScreenCursor(0, -6);
				// 	}
				// 	else
				// 	{
				// 		ImGui::TextWrapped("Add C++ callback functions using AddCPPAnimationEventFunction() in attached C++ script.");
				// 	}
				// }

				// if (functionParams->b_luaEvent)
				// {
					FL::GuiCore::RenderInput("##ButtonEventName", "Callback Function", functionParams.name);
				// }

				if (FL::GuiCore::PushTable("##ButtonEventParameters", 2))
				{
					FL::GuiCore::RenderInputTableRow("##ButtonEventParamString" + std::to_string(ownerID), "String", functionParams.p_string, false);
					FL::GuiCore::RenderIntDragTableRow("##ButtonEventParamInt" + std::to_string(ownerID), "Int", functionParams.p_int32, 1, -INT_MAX, INT_MAX);
					// FL::GuiCore::RenderIntDragTableRow("##ButtonEventParamLong" + std::to_string(ownerID), "Long", functionParams.p_int64, 1, -INT_MAX, INT_MAX);
					FL::GuiCore::RenderFloatDragTableRow("##ButtonEventParamFloat" + std::to_string(ownerID), "Float", functionParams.p_float, 0.001f, -FLT_MAX, FLT_MAX);
					FL::GuiCore::RenderFloatDragTableRow("##ButtonEventParamVector2X" + std::to_string(ownerID), "Vector2 X", functionParams.p_vec2.x, 0.001f, -FLT_MAX, FLT_MAX);
					FL::GuiCore::RenderFloatDragTableRow("##ButtonEventParamVector2Y" + std::to_string(ownerID), "Vector2 Y", functionParams.p_vec2.y, 0.001f, -FLT_MAX, FLT_MAX);
					FL::GuiCore::RenderCheckboxTableRow("##ButtonEventParamBoolean" + std::to_string(ownerID), "Boolean", functionParams.p_bool);
					FL::GuiCore::PopTable();
				}

				FL::GuiCore::MoveScreenCursor(0, 3);								
			}
		}

		void RenderCanvasComponent(FL::Canvas* canvas)
		{
			float canvasWidth = canvas->GetWidth();
			float canvasHeight = canvas->GetHeight();
			int layerNumber = canvas->GetLayerNumber();
			bool b_blocksLayers = canvas->GetBlocksLayers();
			long ownerID = canvas->GetOwnerID();

			if (FL::GuiCore::PushTable("##CanvasProperties" + std::to_string(ownerID), 2))
			{
				if (FL::GuiCore::RenderIntDragTableRow("##layerNumber" + std::to_string(ownerID), "Canvas layer", layerNumber, 1, 20, 20))
				{
					canvas->SetLayerNumber(layerNumber);
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##Canvas width" + std::to_string(ownerID), "Width", canvasWidth, 0.1f, 0.1f, FLT_MAX))
				{
					canvas->SetDimensions(canvasWidth, canvasHeight);
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##Canvas height" + std::to_string(ownerID), "Height", canvasHeight, 0.1f, 0.1f, FLT_MAX))
				{
					canvas->SetDimensions(canvasWidth, canvasHeight);
				}
				FL::GuiCore::PopTable();
			}

			FL::GuiCore::RenderSeparator(3, 3);

			if (FL::GuiCore::RenderCheckbox("Blocks Layers", b_blocksLayers))
			{
				canvas->SetBlocksLayers(b_blocksLayers);
			}
		}

		void RenderAnimationComponent(FL::Animation* animation)
		{
			long ownerID = animation->GetOwnerID();
			std::vector<FL::AnimationData> &animations = animation->GetAnimations();

			int droppedAnimValue = -1;
			std::string openedAnimPath = "";
			static std::string newAnimationName = "";
			static std::string newAnimationPath = "";

			ImGui::Text("ADD ANIMATIONS");
			FL::GuiCore::MoveScreenCursor(0, 3);

			FL::GuiCore::RenderInput("##NewAnimationName", "Name", newAnimationName, false);

			FL::GuiCore::MoveScreenCursor(0, 3);

			if (FL::GuiCore::DropInputCanOpenFiles("##AnimationPathInspectorwindow-" + std::to_string(ownerID), "File", FL::FileHelper::GetFilenameFromPath(newAnimationPath, true), FL::GuiCore::fileExplorerTarget, droppedAnimValue, openedAnimPath, "Drop animation files here from the File Explorer"))
			{
				if (droppedAnimValue >= 0)
				{
					std::filesystem::path fsPath(FL::GuiCore::selectedFiles[droppedAnimValue - 1]);
					if (fsPath.extension() == ".anm")
					{
						newAnimationPath = fsPath.string();
					}
					else
					{
						FL::Logger::log.Err("File must be of type .anm to drop here.");
					}
				}
				else if (droppedAnimValue == -2)
				{
					newAnimationPath = "";
				}
				else if (openedAnimPath != "")
				{
					newAnimationPath = openedAnimPath;
				}
			}

			FL::GuiCore::MoveScreenCursor(0, 3);

			ImGui::BeginDisabled(newAnimationPath == "" || newAnimationName == "");
			if (FL::GuiCore::RenderButton("Add Animation"))
			{
				if (!animation->ContainsName(newAnimationName) && FL::FileHelper::DoesFileExist(newAnimationPath))
				{
					animation->AddAnimation(newAnimationName, newAnimationPath);
					newAnimationName = "";
					newAnimationPath = "";
				}
				else
				{
					if (animation->ContainsName(newAnimationName))
					{
						FL::Logger::log.Err("Name already taken in this Animation component, please choose a different one.");
					}
				}
			}
			ImGui::EndDisabled();


			if (animations.size() > 0)
			{
				FL::GuiCore::RenderSeparator(4, 4);
				ImGui::Text("ATTACHED ANIMATIONS");
				FL::GuiCore::MoveScreenCursor(0, 3);
			}

			int IDCounter = 0;
			int queuedAnimationForDelete = -1;
			for (FL::AnimationData& animData : animations)
			{
				std::string currentAnimationName = animData.name;

				if (FL::GuiCore::RenderInput("##NewAnimationName" + std::to_string(IDCounter), "Name", currentAnimationName, false))
				{
					animData.name = currentAnimationName;
				}

				FL::GuiCore::MoveScreenCursor(0, 3);

				int droppedAnimDataValue = -1;
				std::string openedAnimDataPath = animData.path;
				if (FL::GuiCore::DropInputCanOpenFiles("##AnimationPathInspectorWindow-" + std::to_string(IDCounter), "File", FL::FileHelper::GetFilenameFromPath(openedAnimDataPath, true), FL::GuiCore::fileExplorerTarget, droppedAnimDataValue, openedAnimDataPath, "Drop animation files here from the File Explorer"))
				{
					if (droppedAnimDataValue >= 0)
					{
						std::filesystem::path fsPath(FL::GuiCore::selectedFiles[droppedAnimDataValue - 1]);
						if (fsPath.extension() == ".anm")
						{
							animData.path = fsPath.string();
						}
						else
						{
							FL::Logger::log.Err("File must be of type .anm to drop here.");
						}
					}
					else if (droppedAnimDataValue == -2)
					{
						animData.path = "";
					}
					else if (openedAnimDataPath != "")
					{
						animData.path = openedAnimDataPath;
					}
				}

				FL::GuiCore::MoveScreenCursor(0, 4);

				ImGui::BeginDisabled(animData.path == "");
				if (FL::GuiCore::RenderButton("Preview##" + std::to_string(IDCounter)))
				{
					animation->Play(animData.name);
				}
				ImGui::EndDisabled();

				if (animData.path != "")
				{
					ImGui::SameLine(0, 5);
				}

				ImGui::BeginDisabled(animData.path == "" || !animData.b_playing);
				if (FL::GuiCore::RenderButton("Stop##" + std::to_string(IDCounter)))
				{
					animation->Stop(animData.name);
				}
				ImGui::EndDisabled();

				if (animData.path != "")
				{
					ImGui::SameLine(0, 5);
				}

				ImGui::BeginDisabled(animData.path == "");
				if (FL::GuiCore::RenderButton("Edit##" + std::to_string(IDCounter)))
				{
					FL::Settings::settings.b_showAnimator = true;
					Animator::loadedAnimation = FL::AnimationManager::LoadAnimationFile(animData.path);
					FL::ProjectManager::loadedProject.loadedAnimationPath = animData.path;
				}
				ImGui::EndDisabled();

				ImGui::SameLine(0, 10);

				if (FL::GuiCore::RenderButton("Delete##" + std::to_string(IDCounter)))
				{
					queuedAnimationForDelete = IDCounter;
				}
		
				//if (FL::GuiCore::RenderCheckbox("Play on Start", animData.b_playing))
				//{
				//	animData.b_playing = !animData.b_playing;
				//}

				if (animData.name != animations.back().name)
				{
					FL::GuiCore::RenderSeparator(4, 6);
				}

				IDCounter++;
			}

			if (queuedAnimationForDelete != -1)
			{
				std::vector<FL::AnimationData>::iterator index = animations.begin() + queuedAnimationForDelete;
				animations.erase(index);
			}
		}

		void RenderAudioComponent(FL::Audio* audio)
		{
			long ownerID = audio->GetOwnerID();
			std::vector<FL::SoundData>& sounds = audio->GetSounds();

			static std::string path = "";
			static std::string name = "";
			static bool b_isNewAudioMusic = false;

			FL::GuiCore::RenderSectionHeader("Add Audio");

			FL::GuiCore::RenderInput("##NameNewAudioDataObject", "Name", name, false);

			FL::GuiCore::MoveScreenCursor(0, 3);

			int droppedValue = -1;
			std::string openedPath = "";
			if (FL::GuiCore::DropInputCanOpenFiles("##AddAudioFile", "File", FL::FileHelper::GetFilenameFromPath(path, true), FL::GuiCore::fileExplorerTarget, droppedValue, openedPath, "Drop font files here from File Explorer"))
			{
				if (droppedValue >= 0)
				{
					std::filesystem::path fsPath(FL::GuiCore::selectedFiles[droppedValue - 1]);
					if (fsPath.extension() == ".wav" || fsPath.extension() == ".mp3")
					{
						path = FL::GuiCore::selectedFiles[droppedValue - 1];
					}
					else
					{
						FL::Logger::log.Err("File must be of type audio to drop here.");
					}
				}
				else if (droppedValue == -2)
				{
					path = "";
				}
				else if (openedPath != "")
				{
					path = openedPath;
				}
			}
			FL::GuiCore::MoveScreenCursor(0, 4);

			ImGui::BeginDisabled(path == "" || name == "");
			if (FL::GuiCore::RenderButton("Add Audio"))
			{
				if (!audio->ContainsName(name) && FL::FileHelper::DoesFileExist(path))
				{
					audio->AddSound(name, path);
					path = "";
					name = "";
				}
				else
				{
					if (audio->ContainsName(name))
					{
						FL::Logger::log.Err("Name already taken in this Audio component, please choose a different one.");
					}
				}
			}
			ImGui::EndDisabled();

			if (sounds.size() > 0)
			{
				FL::GuiCore::RenderSeparator(4, 4);
				FL::GuiCore::RenderSectionHeader("Attached Audio Files");
			}

			// Show existing Sounds in this Audio component
			int IDCounter = 0;
			for (std::vector<FL::SoundData>::iterator soundIter = sounds.begin(); soundIter != sounds.end(); soundIter++)
			{
				FL::SoundData& sound = (*soundIter);
				std::string audioPath = sound.path;
				std::string audioName = sound.name;
				std::string newName = audioName;			
				int newDroppedValue = -1;
				std::string inputId = "##audioPath_" + std::to_string(ownerID) + sound.name + std::to_string(IDCounter);

				if (FL::GuiCore::RenderInput("##NameExistingAudioDataObject" + std::to_string(IDCounter), "Name", audioName, false))
				{				
					sound.name = audioName;
				}
				FL::GuiCore::MoveScreenCursor(0, 4);

				if (FL::GuiCore::DropInput(inputId, "File", FL::FileHelper::GetFilenameFromPath(audioPath, true), FL::GuiCore::fileExplorerTarget, newDroppedValue, "Drop audio files here from the Explorer window"))
				{
					if (newDroppedValue != -1 && FL::GuiCore::selectedFiles.size() >= newDroppedValue)
					{
						std::filesystem::path fsPath(FL::GuiCore::selectedFiles[newDroppedValue - 1]);
						if (fsPath.extension() == ".wav" || fsPath.extension() == ".mp3")
						{
							audioPath = FL::GuiCore::selectedFiles[newDroppedValue - 1];
							sound.path = audioPath;
							audio->LoadAudio(sound);
						}
						else
						{
							FL::Logger::log.Err("File must be of type audio to drop here.");
						}
					}
					else if (newDroppedValue == -1)
					{
						audioPath = "";
						sound.path = "";					
					}
				}
				FL::GuiCore::MoveScreenCursor(0, 4);
				
				ImGui::BeginDisabled(sound.name == "" || sound.path == "");
				// {

					// Play Audio
					ImGui::BeginDisabled(!(!sound.b_isMusic || !audio->IsMusicPlaying(sound.name)));
					if (FL::GuiCore::RenderImageButton("##ImageButtonPlay" + sound.name, FL::Assets::assetManager.GetTexture("play")))
					{
						audio->Play(sound.name);
					}
					ImGui::SameLine(0, 5);
					ImGui::EndDisabled();

					// Pause Audio
					ImGui::BeginDisabled((!(!sound.b_isMusic || audio->IsMusicPlaying(sound.name)) || !sound.b_isMusic));
					if (FL::GuiCore::RenderImageButton("##ImageButtonPause" + sound.name, FL::Assets::assetManager.GetTexture("pause")))
					{
						audio->Pause(sound.name);
					}
					ImGui::EndDisabled();
					ImGui::SameLine(0, 5);

					// Stop Audio
					ImGui::BeginDisabled((sound.b_isMusic && !audio->IsMusicPlaying(sound.name)));
					if (FL::GuiCore::RenderImageButton("##ImageButtonStop" + sound.name, FL::Assets::assetManager.GetTexture("stop")))
					{
						audio->Stop(sound.name);
					}
					ImGui::EndDisabled();

				// }
				ImGui::EndDisabled();

				ImGui::SameLine(ImGui::GetContentRegionAvail().x - 90, 0);

				if (FL::GuiCore::RenderButton("Remove Sound##"+ sound.name))
				{
					sounds.erase(soundIter);
					break;
				}

				if (sound.name != sounds.back().name)
				{
					FL::GuiCore::RenderSeparator(4, 6);
				}

				IDCounter++;
			}
		}

		void RenderTextComponent(FL::Text* text)
		{
			std::shared_ptr<FL::Texture> texture = text->GetTexture();
			float textureWidth = (float)texture->GetWidth();
			float textureHeight = (float)texture->GetHeight();
			int renderOrder = text->GetRenderOrder();
			int fontSize = text->GetFontSize();
			FL::Pivot pivotPoint = text->GetPivotPoint();
			FL::Vector4 color = text->GetColor();
			FL::Vector2 offset = text->GetOffset();
			float xOffset = offset.x;
			float yOffset = offset.y;
			long ownerID = text->GetOwnerID();

			std::string textText = text->GetText();
			if (FL::GuiCore::RenderInput("##TextContent" + std::to_string(ownerID), "Text", textText))
			{
				text->SetText(textText);
				text->LoadText();
			}

			FL::GuiCore::MoveScreenCursor(0, 3);
			
			std::string fontPath = text->GetFontPath();
			int droppedValue = -1;		
			if (FL::GuiCore::DropInputCanOpenFiles("##InputFontPath", "Font", FL::FileHelper::GetFilenameFromPath(fontPath, true), FL::GuiCore::fileExplorerTarget, droppedValue, fontPath, "Drop font files here from File Explorer"))
			{
				if (droppedValue >= 0)
				{
					std::filesystem::path fsPath(FL::GuiCore::selectedFiles[droppedValue - 1]);
					if (fsPath.extension() == ".ttf")
					{
						text->SetFontPath(fsPath.string());
					}
					else
					{
						FL::Logger::log.Err("File must be of type .ttf to drop here.");
					}
				}
				else if (droppedValue == -2)
				{
					text->SetFontPath("");
				}
				else if (fontPath != "")
				{
					text->SetFontPath(fontPath);
				}
			}

			FL::GuiCore::RenderSeparator(3, 3);

			FL::GuiCore::RenderTextTable(FL::GuiCore::TableProps("##textWidth" + std::to_string(ownerID), "Text width"), { std::to_string(textureWidth) });
			FL::GuiCore::RenderTextTable(FL::GuiCore::TableProps("##textHeight" + std::to_string(ownerID), "Text height"), { std::to_string(textureHeight) });

			if (FL::GuiCore::PushTable("##TextProperties" + std::to_string(ownerID), 2))
			{				
				if (FL::GuiCore::RenderIntDragTableRow("##textFontSize" + std::to_string(ownerID), "Font size", fontSize, 1, 0, 1000))
				{
					text->SetFontSize(fontSize);
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##xTextOffset" + std::to_string(ownerID), "X offset", xOffset, 0.1f, -FLT_MAX, FLT_MAX))
				{
					text->SetOffset(FL::Vector2(xOffset, yOffset));
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##yTextOffset" + std::to_string(ownerID), "Y offset", yOffset, 0.1f, -FLT_MAX, FLT_MAX))
				{
					text->SetOffset(FL::Vector2(xOffset, yOffset));
				}
				if (FL::GuiCore::RenderIntDragTableRow("##TextRenderOrder" + std::to_string(ownerID), "Render Order", renderOrder, 1, 0, (int)FL::VulkanManager::maxSpriteLayers))
				{
					text->SetRenderOrder(renderOrder);
				}
				FL::GuiCore::PopTable();
			}

			FL::GuiCore::RenderSeparator(3, 3);

			if (RenderPivotSelectionButtons("Text", pivotPoint))
			{
				text->SetPivotPoint(pivotPoint);
			}

			FL::GuiCore::RenderSeparator(6, 3);

			// Tint color picker
			std::string tintID = "##TextColor" + std::to_string(ownerID) + "-" + std::to_string(ownerID);
			if (ImGui::ColorEdit4(tintID.c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
			{
				text->SetColor(color);
			}
			ImGui::SameLine(0, 5);
			ImGui::Text("%s", "Text color");
		}

		void RenderCharacterControllerComponent(FL::CharacterController* characterController)
		{
			float maxAcceleration = characterController->GetMaxAcceleration();								
			float maxSpeed = characterController->GetMaxSpeed();
			float airControl = characterController->GetAirControl();
			bool b_isMoving = characterController->IsMoving();
			// FL::Shape& capsule = characterController->GetCapsule();
			// float radius = shapeProps.radius;
			// float capsuleLength = shapeProps.capsuleLength;
			// bool b_horizontal = shapeProps.b_horizontal;
			long ownerID = characterController->GetOwnerID();
			std::string isMovingString = "false";
			if (characterController->IsMoving())
			{
				isMovingString = "true";
			}

			bool b_changed = false;

			if (FL::GuiCore::PushTable("##CharacterControllerProps" + std::to_string(ownerID), 2))
			{
				if (FL::GuiCore::RenderFloatDragTableRow("##MaxAccelerationDrag" + std::to_string(ownerID), "Max Acceleration", maxAcceleration, 0.01f, 0.0f, 20.0f))
				{
					characterController->SetMaxAcceleration(maxAcceleration);
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##MaxSpeedDrag" + std::to_string(ownerID), "Max Speed", maxSpeed, 0.01f, 0.0f, 1000.0f))
				{
					characterController->SetMaxSpeed(maxSpeed);
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##AirControlDrag" + std::to_string(ownerID), "Air Control", airControl, 0.01f, 0.0f, 1000.0f))
				{
					characterController->SetAirControl(airControl);
				}				
				// if (FL::GuiCore::RenderFloatDragTableRow("##CharacterControllerShapeRadius" + std::to_string(ownerID), "Radius", radius, 0.01f, 0.01f, FLT_MAX))
				// {
				// 	capsule.SetRadius(radius);
				// }
				// if (FL::GuiCore::RenderFloatDragTableRow("##CharacterControllerCapsuleLength" + std::to_string(ownerID), "Length", capsuleLength, 0.01f, 0.01f, FLT_MAX))
				// {
				// 	capsule.SetCapsuleLength(capsuleLength);
				// }
				// if (FL::GuiCore::RenderFloatDragTableRow("##CharacterControllerCapsuleHeight" + std::to_string(ownerID), "Radii", radius, 0.01f, 0.01f, FLT_MAX))
				// {
				// 	capsule.SetRadius(radius);
				// }
				FL::GuiCore::PopTable();
			}
			FL::GuiCore::RenderTextTable(FL::GuiCore::TableProps("##IsMoving" + std::to_string(ownerID), "Is Moving"), { isMovingString });

			// b_changed |= FL::GuiCore::RenderCheckbox(" Horizontal", shapeProps.b_horizontal);

			// if (b_changed)			
			// 	capsule.RecreateShape();	
		}
		
		// Shapes
		bool RenderBoxProps(auto&& sData)
		{
			b2ShapeId shapeID = sData.shapeID;
			std::string ID = "shape_" + std::to_string(shapeID.index1) + "_" + std::to_string(shapeID.world0);
			bool b_changed = false;
			bool b_light = true;
						
			
			b_changed |= FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##ShapeCornerRadius" + ID, "Corner Radius", FL::Vector2(), 0.01f, 0.0f), sData.cornerRadius);
			b_changed |= FL::GuiCore::RenderVector2Table(FL::GuiCore::TableProps("##BoxDimensions" + ID, "Dimensions", FL::Vector2(), 0.01f, 0.0f), sData.dimensions);			
			b_changed |= FL::GuiCore::RenderVector2Table(FL::GuiCore::TableProps("##PositionOffset" + ID, "Pos. Offset"), sData.offset);
			
			float rotationOffset = FL::Numbers::RadiansToDegrees(b2Rot_GetAngle(sData.rotationOffset));	
			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##RotationOffset" + ID, "Rotation Offset"), rotationOffset)) sData.SetRotationOffset(rotationOffset);
			FL::GuiCore::RenderSeparator(4,0);

			return b_changed;	
		}

		bool RenderCircleProps(auto&& sData)
		{
			b2ShapeId shapeID = sData.shapeID;
			std::string ID = "shape_" + std::to_string(shapeID.index1) + "_" + std::to_string(shapeID.world0);						
			bool b_changed = false;
			bool b_light = true;
									
			b_changed |= FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##ShapeRadius" + ID, "Radius", FL::Vector2(), 0.001f, 0.001f), sData.radius);
			b_changed |= FL::GuiCore::RenderVector2Table(FL::GuiCore::TableProps("##PositionOffset" + ID, "Pos. Offset"), sData.offset);

			FL::GuiCore::RenderSeparator(4,0);

			return b_changed;
		}


		bool RenderCapsuleProps(auto&& sData)
		{
			b2ShapeId shapeID = sData.shapeID;
			std::string ID = "shape_" + std::to_string(shapeID.index1) + "_" + std::to_string(shapeID.world0);			
			float radius = sData.radius;
			float capsuleLength = sData.length;		
			bool b_changed = false;
			bool b_light = true;
						
			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##CapsuleLength" + ID, "Length", FL::Vector2(), 0.001f, 0.001f), capsuleLength)) sData.SetLength(capsuleLength);
			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##Radii" + ID, "Radii", FL::Vector2(), 0.001f, 0.001f), radius)) sData.SetRadius(radius);
			b_changed |= FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("##HorizontalCapsule_" + ID, "Horizontal"), sData.b_horizontal);
			FL::GuiCore::RenderVector2Table(FL::GuiCore::TableProps("##PositionOffset" + ID, "Pos. Offset"), sData.offset);
			float rotationOffset = FL::Numbers::RadiansToDegrees(b2Rot_GetAngle(sData.rotationOffset));	
			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##RotationOffset" + ID, "Rotation Offset"), rotationOffset)) sData.SetRotationOffset(rotationOffset);					
			FL::GuiCore::RenderSeparator(4,0);

			return b_changed;
		}

		bool RenderPolygonProps(auto&& sData)
		{
			b2ShapeId shapeID = sData.shapeID;
			std::string ID = "shape_" + std::to_string(shapeID.index1) + "_" + std::to_string(shapeID.world0);						
			std::vector<FL::Vector2> points = sData.points;			
			int pointCount = (int)points.size();			
			bool b_changed = false;
			bool b_light = true;
						
			b_changed |= FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##ShapeCornerRadius" + ID, "Corner Radius", FL::Vector2(), 0.01f, 0.0f), sData.cornerRadius);
			FL::GuiCore::RenderSeparator(4,0);

			if (!sData.b_editingPoints)
			{
				if (FL::GuiCore::RenderButton("Edit Points##Polygon_" + ID))
				{
					sData.b_editingPoints = true;
				}
			}
			else
			{
				if (FL::GuiCore::RenderButton("Stop editing##Polygon_" + ID))
				{
					sData.b_editingPoints = false;
				}
			}

			ImGui::SameLine();	
			FL::GuiCore::MoveScreenCursor(0, 3);
			FL::GuiCore::RenderCheckbox(" Show points##Polygon_" + ID, sData.b_showPoints);
			if (sData.b_showPoints)
			{
				FL::GuiCore::RenderSeparator(3,0);				
				for (int i = 0; i < pointCount; i++)
				{
					b_changed |= FL::GuiCore::RenderVector2Table(FL::GuiCore::TableProps("##ShapePointPos" + ID + std::to_string(i), "Index " + std::to_string(i) + " Pos"), sData.points[i]);					
				}
			}

			FL::GuiCore::RenderSeparator(3,0);

			return b_changed;
		}

		bool RenderChainProps(auto&& sData)
		{
			b2ChainId chainID = sData.chainID;
			std::string ID = "chain_" + std::to_string(chainID.index1) + "_" + std::to_string(chainID.world0);				
			bool b_changed = false;		
			bool b_light = true;

			b_changed |= FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("##LoopEndpoints" + ID, "Loop endpoints"), sData.b_isLoop);
			FL::GuiCore::RenderSeparator(4,3);			

			if (!sData.b_editingPoints)
			{
				if (FL::GuiCore::RenderButton("Edit Points##Polygon_" + ID))
				{
					sData.b_editingPoints = true;
				}
			}
			else
			{
				if (FL::GuiCore::RenderButton("Stop editing##Polygon_" + ID))
				{
					sData.b_editingPoints = false;
				}
			}

			ImGui::SameLine();
			FL::GuiCore::MoveScreenCursor(0, 3);
			FL::GuiCore::RenderCheckbox(" Show points##Chain_" + ID, sData.b_showPoints);

			if (sData.b_showPoints)
			{
				FL::GuiCore::RenderSeparator(3,0);
				bool b_pointLight = true;
				for (int i = 0; i < (int)sData.points.size(); i++)
				{
					b_changed |= FL::GuiCore::RenderVector2Table(FL::GuiCore::TableProps("##ShapePointXPos" + ID + std::to_string(i), "Index " + std::to_string(i) + " Pos"), sData.points[i]);
					b_pointLight = !b_pointLight;
				}
			}			
			
			FL::GuiCore::RenderSeparator(3,0);

			return b_changed;
		}

		void RenderShape2DComponentProps(FL::Shape2D* shape, b2ShapeId& shapeToDelete, b2ChainId& chainToDelete)
		{
			b2ShapeId shapeID = shape->GetShapeID();
			b2ChainId chainID = shape->GetChainID();
			FL::ShapeType2D shapeType = shape->GetType();		
			std::string ID = "";
			if (shapeType != FL::ShapeType2D::ShapeType2D_Chain)			
				ID = " (shape index:" + std::to_string(shapeID.index1) + " world:" + std::to_string(shapeID.world0) + ")";			
			else			
				ID = " (chain index:" + std::to_string(chainID.index1) + " world:" + std::to_string(chainID.world0) + ")";			
			std::string shapeString = FL::ShapeType2DStrings[(int)shapeType] + ID;		

			FL::GuiCore::RenderSectionHeader("  " + shapeString, 0, 0, "sectionHeaderBg", "shapeSectionHeaderSeparator");
			FL::GuiCore::MoveScreenCursor(2, -22);
			ImGui::Image(FL::Assets::assetManager.GetTexture(FL::ShapeType2DStrings[shape->GetType()]), FL::Vector2(16));

			FL::GuiCore::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 36, -20);
			if (FL::GuiCore::RenderImageButton("##trashIcon-" + ID, FL::Assets::assetManager.GetTexture("trash")))
			{
				shapeToDelete = shapeID;
				chainToDelete = chainID;
			}			
			ImGui::SameLine(0,2);			
			std::string expandString = shape->b_isCollapsed ? "expand" : "expandFlipped";
			if (FL::GuiCore::RenderImageButton("##expandIcon-" + ID, FL::Assets::assetManager.GetTexture(expandString)))				
				shape->b_isCollapsed = !shape->b_isCollapsed;	

			if (shape->b_isCollapsed)
				return;

			bool b_changed = false;
			
			b_changed |= FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##" + shapeString + "Density" + ID, "Density", FL::Vector2(), 0.001f, 0.001f), shape->density);
			b_changed |= FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##" + shapeString + "Friction" + ID, "Friction", FL::Vector2(), 0.001f, 0.001f), shape->friction);
			b_changed |= FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##" + shapeString + "Restitution" + ID, "Restitution", FL::Vector2(), 0.001f, 0.001f), shape->restitution);
			b_changed |= FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("##EnableSensorEvents" + ID, "Enable Sensor Events"), shape->b_enableSensorEvents);
			b_changed |= FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("##EnableContactEvents" + ID, "Enable Contact Events"), shape->b_enableContactEvents);
			b_changed |= FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("##ShapeIsSensor" + ID, "Is Sensor"), shape->b_isSensor);
			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##ChainTangentSpeed" + ID, "Tangent Speed", FL::Vector2(), 0.01f, 0.0f), shape->tangentSpeed)) shape->SetTangentSpeed(shape->tangentSpeed);
			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##ChainRollingResistance" + ID, "Rolling Resistance", FL::Vector2(), 0.01f, 0.0f), shape->rollingResistance)) shape->SetRollingResistance(shape->rollingResistance);		

			std::visit([shape, &b_changed](auto&& sData) -> void
			{
				using T = std::decay_t<decltype(sData)>;
				if constexpr (std::is_same_v<T, FL::BoxShape2DData>)
				{
					b_changed |= RenderBoxProps(sData);
				}
				else if constexpr (std::is_same_v<T, FL::CircleShape2DData>)
				{
					b_changed |= RenderCircleProps(sData);
				}
				else if constexpr (std::is_same_v<T, FL::CapsuleShape2DData>)
				{
					b_changed |= RenderCapsuleProps(sData);
				}
				else if constexpr (std::is_same_v<T, FL::PolygonShape2DData>)
				{
					b_changed |= RenderPolygonProps(sData);
				}
				else if constexpr (std::is_same_v<T, FL::ChainShape2DData>)
				{
					b_changed |= RenderChainProps(sData);
				}
			}, shape->shapeData);
			
			if (b_changed)			
				FL::PhysicsManager::gamePhysics2D.RecreateShape(shape);	

			FL::GuiCore::MoveScreenCursor(0, 3);
		}

		// Joints
		void RenderDistanceJointProps(auto&& jData, std::string ID)
		{						
			float dampingRatio = jData.dampingRatio;
			bool b_enableLimit = jData.b_enableLimit;
			bool b_enableMotor = jData.b_enableMotor;
			bool b_enableSpring = jData.b_enableSpring;
			float hertz = jData.hertz;
			float minLength = jData.minLength;
			float maxLength = jData.maxLength;
			float length = jData.length;
			float maxMotorForce = jData.maxMotorForce;
			float motorSpeed = jData.motorSpeed;

			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##DistanceJointLength" + ID, "Length", FL::Vector2(), 0.1f, 0.1f), length)) jData.SetLength(length);
			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##DistanceJointMinLength" + ID, "Min Length", FL::Vector2(), 0.1f, 0.001f), minLength)) jData.SetLengthRange(minLength, maxLength);
			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##DistanceJointMaxLength" + ID, "Max Length", FL::Vector2(), 0.1f, 0.001f), maxLength)) jData.SetLengthRange(minLength, maxLength);				
			if (FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("#Enable Spring" + ID, "Spring Enabled"), b_enableSpring)) jData.SetEnableSpring(b_enableSpring);						
			if (jData.b_enableSpring)
			{
				if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##DampingRatio" + ID, "Spring Damping Ratio", FL::Vector2(), 0.001f, 0), dampingRatio)) jData.SetSpringDampingRatio(dampingRatio);		
				if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##SpringHertz" + ID, "Spring Hertz", FL::Vector2(), 0.1f, 0.001f), hertz)) jData.SetSpringHertz(hertz);	
			}
			if (FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("Enable Motor##" + ID, "Motor Enabled"), b_enableMotor)) jData.SetEnableMotor(b_enableMotor);			
			if (jData.b_enableMotor)
			{
				if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##MotorSpeed" + ID, "Motor Speed", FL::Vector2(), 0.1f, 0.001f), motorSpeed)) jData.SetMotorSpeed(motorSpeed);			
				if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##MaxMotorForce" + ID, "Max Motor Force", FL::Vector2(), 0.1f, 0.001f), maxMotorForce)) jData.SetMaxMotorForce(maxMotorForce);		
			}
			if (FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("Enable Limit##" + ID, "Enable Limit"), b_enableLimit)) jData.SetEnableLimit(b_enableLimit);

			FL::GuiCore::MoveScreenCursor(0, 3.0f);
		}

		void RenderPrismaticJointProps(auto&& jData, std::string ID)
		{			
			float dampingRatio = jData.dampingRatio;
			bool b_enableLimit = jData.b_enableLimit;
			bool b_enableMotor = jData.b_enableMotor;
			bool b_enableSpring = jData.b_enableSpring;
			float hertz = jData.hertz;
			FL::Vector2 localAxisA = jData.localAxisA;
			float lowerTranslation = jData.lowerTranslation;
			float upperTranslation = jData.upperTranslation;
			float maxMotorForce = jData.maxMotorForce;
			float motorSpeed = jData.motorSpeed;
			float targetTranslation = jData.targetTranslation;

			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##LowerTranslation" + ID, "Lower Translation"), lowerTranslation)) jData.SetTranslationRange(lowerTranslation, upperTranslation);		
			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##UpperTranslation" + ID, "Upper Translation"), upperTranslation)) jData.SetTranslationRange(lowerTranslation, upperTranslation);		
			// if (FL::GuiCore::RenderVector2Table("##TranslationTarget" + ID, "Translation Target", localAxisA, FL::Vector2(), 0, "noEditTableRowFieldBg", std::vector<std::string>(), true, 0.1f, -FLT_MAX, FLT_MAX)) jData.Ax(localAxisA);	
			// if (FL::GuiCore::RenderVector2Table("##LocalAxisA" + ID, "Local Axis A", localAxisA, FL::Vector2(), 0, "noEditTableRowFieldBg", std::vector<std::string>(), true, 0.1f, -FLT_MAX, FLT_MAX)) jData.SetLocalAxisA(localAxisA);	
			if (FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("#Enable Spring" + ID, "Spring Enabled"), b_enableSpring)) jData.SetEnableSpring(b_enableSpring);						
			if (jData.b_enableSpring)
			{
				if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##DampingRatio" + ID, "Spring Damping Ratio", FL::Vector2(), 0.1f, 0), dampingRatio)) jData.SetSpringDampingRatio(dampingRatio);		
				if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##SpringHertz" + ID, "Spring Hertz", FL::Vector2(), 0.1f, 0), hertz)) jData.SetSpringHertz(hertz);	
			}
			if (FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("Enable Motor##" + ID, "Motor Enabled"), b_enableMotor)) jData.SetEnableMotor(b_enableMotor);			
			if (jData.b_enableMotor)
			{
				if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##MotorSpeed" + ID, "Motor Speed", FL::Vector2(), 0.1f, 0), motorSpeed)) jData.SetMotorSpeed(motorSpeed);			
				if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##MaxMotorForce" + ID, "Max Motor Force", FL::Vector2(), 0.1f, 0), maxMotorForce)) jData.SetMaxMotorForce(maxMotorForce);		
			}
			if (FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("Enable Limit##" + ID, "Enable Limit"), b_enableLimit)) jData.SetEnableLimit(b_enableLimit);

			FL::GuiCore::MoveScreenCursor(0, 3.0f);
		}

		void RenderRevoluteJointProps(auto&& jData, std::string ID)
		{
		}

		void RenderMouseJointProps(auto&& jData, std::string ID)
		{		
		}

		void RenderWeldJointProps(auto&& jData, std::string ID)
		{
		}

		void RenderMotorJointProps(auto&& jData, std::string ID)
		{
		}

		void RenderWheelJointProps(auto&& jData, std::string ID)
		{
		}

		void RenderJoint2DComponentProps(FL::Joint2D* joint, long& jointIDToDelete)
		{		
			long jointID = joint->GetID();
			long ownerID = joint->GetOwnerID();				
			FL::JointType2D jointType = joint->GetType();
			std::string jointTypeString = FL::JointType2DStrings[(int)jointType];
			std::string ID = " (id:" + std::to_string(jointID) + " index:" + std::to_string(joint->GetJointID().index1) + " world:" + std::to_string(joint->GetJointID().world0) + " gen:" + std::to_string(joint->GetJointID().generation) + ")";
			std::string jointString = FL::JointType2DStrings[(int)jointType] + ID;

			FL::GuiCore::RenderSectionHeader("  " + jointString, 0, 0, "sectionHeaderBg", "shapeSectionHeaderSeparator");		
			FL::GuiCore::MoveScreenCursor(2, -22);
			ImGui::Image(FL::Assets::assetManager.GetTexture(FL::JointType2DStrings[joint->GetType()]), FL::Vector2(16));

			FL::GuiCore::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 36, -20);
			if (FL::GuiCore::RenderImageButton("##trashIcon-" + ID, FL::Assets::assetManager.GetTexture("trash")))
			{
				jointIDToDelete = jointID;
			}
			ImGui::SameLine(0,2);			
			std::string expandString = joint->b_isCollapsed ? "expand" : "expandFlipped";
			if (FL::GuiCore::RenderImageButton("##expandIcon-" + ID, FL::Assets::assetManager.GetTexture(expandString)))				
				joint->b_isCollapsed = !joint->b_isCollapsed;	

			if (joint->b_isCollapsed)
				return;

			FL::Body2D* bodyA = joint->GetBodyA();
			FL::Body2D* bodyB = joint->GetBodyB();	
			bool b_collideConnected = joint->DoesCollideConnected();
			FL::Vector2 anchorA = joint->GetAnchorA();
			FL::Vector2 anchorB = joint->GetAnchorB();

			int droppedObjectID = -1;		
			std::string bodyAName = bodyA != nullptr ? bodyA->GetOwningObject()->GetName() : "";
			std::string bodyBName = bodyB != nullptr ? bodyB->GetOwningObject()->GetName() : "";

			FL::GuiCore::MoveScreenCursor(0, 6.0f);
			
			// if (FL::GuiCore::DropInput("##InputBodyA" + ID, "BodyA", bodyAName, FL::GuiCore::hierarchyTarget, droppedObjectID, "Drag and drop GameObjects from the Hierarchy to assign it's Body component."))
			// {
			// 	if (droppedObjectID >= 0)
			// 	{
			// 		joint->SetBodyAID(droppedObjectID);
			// 	}
			// }
			if (FL::GuiCore::DropInput("##InputBodyB" + ID, "Connected Body2D", bodyBName, FL::GuiCore::hierarchyTarget, droppedObjectID, "Drag and drop Body2D GameObjects from the Hierarchy."))
			{
				if (droppedObjectID >= 0)
				{
					joint->SetBodyBID(droppedObjectID);
				}
			}

			if (FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("##CollideConnected", "Collide connected"), b_collideConnected)) joint->SetCollideConnected(b_collideConnected);
			if (FL::GuiCore::RenderVector2Table(FL::GuiCore::TableProps("#AnchorA" + ID, "Anchor A"), anchorA)) joint->SetAnchorA(anchorA);
			if (FL::GuiCore::RenderVector2Table(FL::GuiCore::TableProps("#AnchorB" + ID, "Anchor B"), anchorB)) joint->SetAnchorB(anchorB);

			std::visit([ID](auto&& jData) -> void
			{
				using T = std::decay_t<decltype(jData)>;
				if constexpr (std::is_same_v<T, FL::DistanceJoint2DData>)
				{
					RenderDistanceJointProps(jData, ID);
				}
				else if constexpr (std::is_same_v<T, FL::RevoluteJoint2DData>)
				{
					RenderRevoluteJointProps(jData, ID);
				}
				else if constexpr (std::is_same_v<T, FL::PrismaticJoint2DData>)
				{
					RenderPrismaticJointProps(jData, ID);
				}
				else if constexpr (std::is_same_v<T, FL::MouseJoint2DData>)
				{
					RenderMouseJointProps(jData, ID);
				}
				else if constexpr (std::is_same_v<T, FL::WeldJoint2DData>)
				{
					RenderWeldJointProps(jData, ID);
				}
				else if constexpr (std::is_same_v<T, FL::WheelJoint2DData>)
				{
					RenderWheelJointProps(jData, ID);
				}
			}, joint->jointData);	
		}

		void RenderBody2DComponent(FL::Body2D* body)
		{		
			long ownerID = body->GetOwnerID();
			FL::Vector2 linearVelocity = body->GetLinearVelocity();
			float angularVelocity = body->GetAngularVelocity();			
			int currentType = body->type;
			std::vector<std::string> types = { "Static", "Kinematic", "Dynamic" };
			std::string comboID = "##BoxBodyTypeCombo";
			bool b_light = true;

			if (FL::GuiCore::RenderComboTable(FL::GuiCore::TableProps(comboID, "Body Type"), types[body->type], types, currentType)) body->SetBodyType((b2BodyType)currentType);
			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##BodyGravityScale" + std::to_string(ownerID), "Gravity Scale"), body->gravityScale)) body->SetGravityScale(body->gravityScale);	
			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##BodyLinearDamping" + std::to_string(ownerID), "Linear Damp", FL::Vector2(), 0.01f, 0.0f), body->linearDamping)) body->SetLinearDamping(body->linearDamping);
			if (FL::GuiCore::RenderFloatTable(FL::GuiCore::TableProps("##BodyAngularDamping" + std::to_string(ownerID), "Angular Damp", FL::Vector2(), 0.01f, 0.0f), body->angularDamping)) body->SetAngularDamping(body->angularDamping);			
			if (FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("##LockRotation" + std::to_string(ownerID), "Lock Rotation"), body->b_lockedRotation)) body->SetLockedRotation(body->b_lockedRotation);
			if (FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("##LockX-Axis" + std::to_string(ownerID), "Lock X-Axis"), body->b_lockedXAxis)) body->SetLockedXAxis(body->b_lockedXAxis);
			if (FL::GuiCore::RenderBoolTable(FL::GuiCore::TableProps("##LockY-Axis" + std::to_string(ownerID), "Lock Y-Axis"), body->b_lockedYAxis)) body->SetLockedYAxis(body->b_lockedYAxis);
			FL::GuiCore::RenderTextTable(FL::GuiCore::TableProps("##VelocityX" + std::to_string(ownerID), "X Vel."), { std::to_string(linearVelocity.x) });								
			FL::GuiCore::RenderTextTable(FL::GuiCore::TableProps("##VelocityY" + std::to_string(ownerID), "Y Vel."), { std::to_string(linearVelocity.y) });			
			FL::GuiCore::RenderTextTable(FL::GuiCore::TableProps("##AngularVelocity" + std::to_string(ownerID), "Angular Vel."), { std::to_string(angularVelocity) });						
			FL::GuiCore::RenderSeparator(4, 3);		

			if (body->GetShapes().size() == 0)
			{		
				ImGui::TextWrapped("* WARNING *\n\nA Body without a shape attached has 0.0 mass and will not move.");
			}		

			FL::GuiCore::RenderButton("Add Shape",FL::Vector2(ImGui::GetContentRegionAvail().x, 0));
			if (ImGui::BeginPopupContextItem("##AddShape", ImGuiPopupFlags_MouseButtonLeft))
			{
				FL::GuiCore::PushMenuStyles();
				for (int i = 1; i < FL::ShapeType2DStrings.size(); i++)
				{
					if (FL::GuiCore::MenuItem(FL::ShapeType2DStrings[i].c_str()))
					{
						body->AddShape((FL::ShapeType2D)i);
						ImGui::CloseCurrentPopup();
					}
				}		
				FL::GuiCore::PopMenuStyles();
				ImGui::EndMenu();
			}

			if (body->GetShapes().size())
			{						
				b2ShapeId shapeToDelete = b2_nullShapeId;
				b2ChainId chainToDelete = b2_nullChainId;

				std::vector<FL::Shape2D*> shapes = body->GetShapes();

				for (int i = 0; i < shapes.size(); i++)
				{
					RenderShape2DComponentProps(shapes[i], shapeToDelete, chainToDelete);
					
					if (!shapes[i]->b_isCollapsed || i == shapes.size() - 1 )
					{
						FL::GuiCore::MoveScreenCursor(0, 3);
					}
				}

				if (b2Shape_IsValid(shapeToDelete))
				{
					body->RemoveShape(shapeToDelete);
				}
				if (b2Chain_IsValid(chainToDelete))
				{
					body->RemoveChain(chainToDelete);
				}
			}
			
			FL::GuiCore::RenderButton("Add Joint", FL::Vector2(ImGui::GetContentRegionAvail().x, 0));
			if (ImGui::BeginPopupContextItem("##AddJoint", ImGuiPopupFlags_MouseButtonLeft))
			{
				FL::GuiCore::PushMenuStyles();
				for (int i = 1; i < FL::JointType2D_Size; i++)
				{
					if (FL::GuiCore::MenuItem(FL::JointType2DStrings[i].c_str()))
					{				
						body->AddJoint((FL::JointType2D)i);
						ImGui::CloseCurrentPopup();
					}
				}
				FL::GuiCore::PopMenuStyles();
				ImGui::EndMenu();
			}

			if (body->GetJoints().size() > 0)
			{
				long jointIDToDelete = -1;			
				std::vector<FL::Joint2D*> joints = body->GetJoints();

				for (int i = 0; i < joints.size(); i++)
				{					
					RenderJoint2DComponentProps(joints[i], jointIDToDelete);

					if (!joints[i]->b_isCollapsed || i == joints.size() - 1)
					{
						FL::GuiCore::MoveScreenCursor(0, 3);
					}
				}

				if (jointIDToDelete != -1)
					body->RemoveJoint(jointIDToDelete);
			}
		}

		void RenderTileMapComponent(FL::TileMap* tileMap)
		{
			long ownerID = tileMap->GetOwnerID();
			int width = tileMap->GetWidth();
			int height = tileMap->GetHeight();
			int tileWidth = tileMap->GetTileWidth();
			int tileHeight = tileMap->GetTileHeight();
			int renderOrder = tileMap->GetRenderOrder();
			std::vector<std::string> tileSets = tileMap->GetTileSets();

			if (FL::GuiCore::PushTable("##tileMapProps" + std::to_string(ownerID), 2))
			{
				if (FL::GuiCore::RenderIntDragTableRow("##Width" + std::to_string(ownerID), "Width", width, 1, 1, INT_MAX))
				{
					tileMap->SetWidth(width);
					//tileMap->CalculateColliderValues();
				}
				if (FL::GuiCore::RenderIntDragTableRow("##Height" + std::to_string(ownerID), "Height", height, 1, 1, INT_MAX))
				{
					tileMap->SetHeight(height);
					//tileMap->RecalcCollisionAreaValues();
				}
				//if (FL::GuiCore::RenderIntDragTableRow("##TileWidth" + std::to_string(ID), "Tile Width", tileWidth, 1, 1, INT_MAX))
				//{
				//	tileMap->SetTileWidth(tileWidth);
				//	//tileMap->RecalcCollisionAreaValues();
				//}
				//if (FL::GuiCore::RenderIntDragTableRow("##TileHeight" + std::to_string(ID), "Tile Height", tileHeight, 1, 1, INT_MAX))
				//{
				//	tileMap->SetTileHeight(tileHeight);
				//	//tileMap->RecalcCollisionAreaValues();
				//}
				if (FL::GuiCore::RenderIntDragTableRow("##RenderOrder" + std::to_string(ownerID), "Render Order", renderOrder, 1, 0, FL::VulkanManager::maxSpriteLayers))
				{
					tileMap->SetRenderOrder(renderOrder);
				}
				FL::GuiCore::PopTable();
			}

			FL::GuiCore::RenderSeparator(4, 4);

			FL::GuiCore::RenderSectionHeader("Add TileSets");

			static int currentSelectableTileSet = 0;
			static int currentSelectableCollisionArea = 0;
			std::string activeTileSet = tileMap->GetSelectedTileSet();
			std::vector<std::string> tileSetNames;

			// Collect TileSets not already in this TileMap that are available to add to it
			// for (int i = 0; i < FL::F_TileSets.size(); i++)
			// {
			// 	bool b_alreadyInTileMap = false;

			// 	for (int j = 0; j < tileSets.size(); j++)
			// 	{
			// 		if (FL::F_TileSets[i].GetName() == tileSets[j])
			// 		{
			// 			b_alreadyInTileMap = true;
			// 		}
			// 	}

			// 	if (!b_alreadyInTileMap)
			// 	{
			// 		tileSetNames.push_back(FL::F_TileSets[i].GetName());
			// 	}
			// }
			

			FL::GuiCore::RenderSelectable("##SelectTileSet", tileSetNames, currentSelectableTileSet);
			ImGui::BeginDisabled(tileSetNames.size() == 0);
			if (FL::GuiCore::RenderButton("Add to Palettes",FL::Vector2(120, 20)))
			{
				if (tileSetNames.size() >= currentSelectableTileSet + 1)
				{
					tileMap->AddTileSet(tileSetNames[currentSelectableTileSet]);
				}
			}
			ImGui::EndDisabled();
		

			FL::GuiCore::RenderSeparator(4, 4);

			FL::GuiCore::RenderSectionHeader("Tile Palettes");

			if (tileSets.size() > 0)
			{
				ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::Assets::assetManager.GetColor("innerWindow"));
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding,FL::Vector2(0, 0));
				FL::GuiCore::PushMenuStyles();
				if (ImGui::BeginTable("##TileSetsTable", 1, FL::GuiCore::tableFlags))
				{
					ImGui::TableSetupColumn("##TileSets", 0, ImGui::GetContentRegionAvail().x);

					for (std::string tileSetName : tileSets)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);

						ImGuiTreeNodeFlags nodeFlags;
						std::string treeID = "##SelectActiveTileSetTree";

						// If node selected
						if (activeTileSet == tileSetName)
						{
							nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
						}
						else
						{
							nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
						}

						// render a leaf
						ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, "%s", tileSetName.c_str());
						if (ImGui::IsItemClicked())
						{
							tileMap->SetSelectedTileSet(tileSetName);
						}
					}

					ImGui::EndTable();
				}
				FL::GuiCore::PopMenuStyles();
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
			}
			else
			{
				ImGui::TextWrapped("No TileSets attached to this TileMap.");
				FL::GuiCore::MoveScreenCursor(0, 3);
			}

			FL::Vector2 tileSetTilesStart = ImGui::GetCursorScreenPos();
			FL::Vector2 regionAvailable = ImGui::GetContentRegionMax();

			// Render Tiles within selected TileSet
			FL::TileSetManager::TileSet* selectedTileSet = nullptr;// FL::GetTileSet(activeTileSet);
			if (selectedTileSet != nullptr)
			{
				// Tile icon size slider
				FL::GuiCore::MoveScreenCursor(10, 10);
				ImGui::Text("%s", "Icon Size:");			
				static float iconSize = 50;
				FL::GuiCore::MoveScreenCursor(10, 2);
				FL::GuiCore::RenderSliderFloat("##TileMapIconSizeSlider", iconSize, 0.5f, 16, 128, ImGui::GetContentRegionAvail().x - 10, 0);
				FL::GuiCore::RenderSeparator(3, 3);

				// Render Palette Tile Icons
				float availableWidth = ImGui::GetWindowSize().x;
				int iconsThisRow = 0;
				std::map<int, std::pair<FL::Vector2,FL::Vector2>> allTiles;

				allTiles = selectedTileSet->GetTileSet();

				float horizontalSpacing = 5;
				float verticalSpacing = 2;
				int maxIconsPerRow = (int)(((regionAvailable.x - 20) / (iconSize + (horizontalSpacing))) - 1);

				for (int index : selectedTileSet->GetTileSetIndices())
				{
					// Drawing the first button in the row
					if (iconsThisRow == 0)
					{
						FL::GuiCore::MoveScreenCursor(horizontalSpacing + 5, verticalSpacing);
					}
						
					if (selectedTileSet->GetTexture() != nullptr)
					{
						FL::Vector2 currentPos = ImGui::GetCursorScreenPos();
						VkDescriptorSet texture = selectedTileSet->GetTexture()->GetTexture();
						int textureWidth = selectedTileSet->GetTexture()->GetWidth();
						int textureHeight = selectedTileSet->GetTexture()->GetHeight();
						FL::Vector2 uvStart;
						FL::Vector2 uvEnd;

						if (allTiles.count(index) > 0)
						{
							uvStart = allTiles.at(index).first;
							uvEnd = allTiles.at(index).second;
							uvStart = FL::Vector2(uvStart.x / textureWidth, uvStart.y / textureHeight);
							uvEnd = FL::Vector2(uvEnd.x / textureWidth, uvEnd.y / textureHeight);

							std::string tileButtonID = "##TileSelect" + std::to_string(index);
							if (FL::GuiCore::RenderImageButton(tileButtonID, texture,FL::Vector2(iconSize, iconSize), 0,FL::Vector2(1), "buttonBorder", "imageButton", "imageButtonTint", "tileSetIconHovered", "imageButtonActive", uvStart, uvEnd))
							{
								std::pair<std::string, int> tileBrushPair = { selectedTileSet->GetName(), index };
								// FL::F_tileSetAndIndexOnBrush = tileBrushPair;
								// FL::F_CursorMode = FL::CURSOR_MODE::CURSOR_MODE_TILE_BRUSH;
							}

							if (iconsThisRow < maxIconsPerRow)
							{
								ImGui::SetCursorScreenPos(FL::Vector2(currentPos.x + iconSize + horizontalSpacing, currentPos.y)); // Ready to draw the next button
								iconsThisRow++;
							}
							else
							{
								iconsThisRow = 0;
							}
						}
					}
				}
				FL::GuiCore::MoveScreenCursor(0, iconSize + verticalSpacing + 10);				
			}
			
			ImGui::SetCursorScreenPos(FL::Vector2(tileSetTilesStart.x, ImGui::GetCursorScreenPos().y + 2));
			FL::GuiCore::RenderSeparator(6, 3);



			//FL::GuiCore::RenderSectionHeader("Collision Areas");

			//std::map<std::string, std::vector<FL::CollisionAreaData>> &collisionAreas = tileMap->GetCollisionAreas();
			//static std::string selectedCollisionArea = "";
			//static int currentSelectedColArea = 0;
			//static std::string collisionAreaLabel = "";
			//std::vector<std::string> areaNames;
			//for (std::pair<std::string, std::vector<FL::CollisionAreaData>> collisionArea : collisionAreas)
			//{
			//	areaNames.push_back(collisionArea.first);
			//}

			//// Create new Collision Area
			//ImGui::TextWrapped("New collision area name:");		
			//ImGui::SameLine(ImGui::GetContentRegionAvail().x - 48, 0);
			//
			//FL::GuiCore::MoveScreenCursor(0, -3);
			//ImGui::BeginDisabled(collisionAreaLabel == "");
			//if (FL::GuiCore::RenderButton("Create"))
			//{
			//	if (!tileMap->ContainsCollisionAreaLabel(collisionAreaLabel))
			//	{
			//		std::vector<FL::CollisionAreaData> newData = std::vector<FL::CollisionAreaData>();
			//		collisionAreas.emplace(collisionAreaLabel, newData);
			//		collisionAreaLabel = "";
			//	}
			//	else
			//	{
			//		FL::Logger::log.Err("Collision area label already being used in this TileMap, please choose a different one.");
			//	}
			//}
			//ImGui::EndDisabled();

			//FL::GuiCore::RenderInput("##CollisionAreaLabel" + std::to_string(ID), "", collisionAreaLabel, false);


			//FL::GuiCore::RenderSeparator(3, 3);


			//if (areaNames.size() > 0)
			//{
			//	ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::Assets::assetManager.GetColor("innerWindow"));
			//	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding,FL::Vector2(0, 0));
			//	FL::GuiCore::PushMenuStyles();
			//	if (ImGui::BeginTable("#CollisionAreasTable", 1, FL::GuiCore::F_tableFlags))
			//	{
			//		ImGui::TableSetupColumn("##CollisionArea", 0, ImGui::GetContentRegionAvail().x);

			//		for (std::string areaName : areaNames)
			//		{
			//			ImGui::TableNextRow();
			//			ImGui::TableSetColumnIndex(0);

			//			ImGuiTreeNodeFlags nodeFlags;
			//			std::string treeID = "##SelectActiveTileSetTree";

			//			// If node selected
			//			if (selectedCollisionArea == areaName)
			//			{
			//				nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			//			}
			//			else
			//			{
			//				nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			//			}

			//			// render a leaf
			//			ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, areaName.c_str());
			//			if (ImGui::IsItemClicked())
			//			{
			//				tileMap->SetSelectedCollisionArea(areaName);
			//				selectedCollisionArea = areaName;
			//			}
			//		}

			//		ImGui::EndTable();
			//	}
			//	FL::GuiCore::PopMenuStyles();
			//	ImGui::PopStyleVar();
			//	ImGui::PopStyleColor();
			//}

			//if (selectedCollisionArea != "")
			//{
			//	if (FL::GuiCore::RenderButton("Draw Collision Areas"))
			//	{
			//		FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
			//		FG_collisionAreasBuffer.clear();
			//	}
			//	ImGui::SameLine(0, 5);
			//	ImGui::BeginDisabled(FG_collisionAreasBuffer.size() == 0);
			//	if (FL::GuiCore::RenderButton("Generate Colliders"))
			//	{
			//		FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
			//		tileMap->SetCollisionAreaValues(selectedCollisionArea, FG_collisionAreasBuffer);
			//		FG_collisionAreasBuffer.clear();
			//	}
			//	ImGui::EndDisabled();
			//}

			//if (areaNames.size() > 0)
			//{			
			//	FL::GuiCore::MoveScreenCursor(0, 5);
			//}
		}

		void RenderMeshComponent(FL::Mesh* mesh)
		{
			long ownerID = mesh->GetOwnerID();		
			std::shared_ptr<FL::Model> model = mesh->GetModel();
			std::string modelFileName = "";
			if (model != nullptr)
			{
				modelFileName = FL::FileHelper::GetFilenameFromPath(model->GetModelPath(), true);
			}
			std::shared_ptr<FL::Material> material = mesh->GetSceneViewMaterial();
			std::string materialName = "";
			
			std::map<std::string, glm::vec4>& uboVec4s = mesh->GetUBOVec4s();


			if (material != nullptr)
			{
				materialName = material->GetName();	

				if (materialName == "fl_empty")
				{
					materialName = "";
				}
			}

			int droppedObjValue = -1;
			std::string openedObjPath = "";
			if (FL::GuiCore::DropInputCanOpenFiles("##InputObjFilePath", "Model   ", modelFileName, FL::GuiCore::fileExplorerTarget, droppedObjValue, openedObjPath, "Drop .obj files here from File Explorer"))
			{
				if (droppedObjValue >= 0)
				{
					std::filesystem::path fsPath(FL::GuiCore::selectedFiles[droppedObjValue - 1]);
					if (fsPath.extension() == ".obj")
					{
						mesh->SetModel(fsPath.string());
						if (material != nullptr)
						{
							mesh->CreateResources();
						}
					}
					else
					{
						FL::Logger::log.Err("File must be of type .obj to drop here.");
					}
				}
				else if (droppedObjValue == -2)
				{
					mesh->SetModel("");
				}
				else if (openedObjPath != "")
				{
					mesh->SetModel(openedObjPath);
					if (material != nullptr)
					{
						mesh->CreateResources();
					}
				}
			}

			int droppedMaterialValue = -1;
			std::string openedMaterialPath = "";
			if (FL::GuiCore::DropInputCanOpenFiles("##InputMaterialFilePath", "Material", materialName, FL::GuiCore::fileExplorerTarget, droppedMaterialValue, openedMaterialPath, "Drop .mat files here from File Explorer"))
			{
				if (droppedMaterialValue >= 0)
				{
					std::filesystem::path fsPath(FL::GuiCore::selectedFiles[droppedMaterialValue - 1]);
					if (fsPath.extension() == ".mat")
					{					
						mesh->SetMaterial(FL::FileHelper::GetFilenameFromPath(fsPath.string()));
						mesh->CreateResources();
						material = mesh->GetSceneViewMaterial();
					}
					else
					{
						FL::Logger::log.Err("File must be of type .obj to drop here.");
					}
				}
				else if (droppedMaterialValue == -2)
				{
					// Remove reference
				}
				else if (openedMaterialPath != "")
				{				
					mesh->SetMaterial(FL::FileHelper::GetFilenameFromPath(openedMaterialPath));
					mesh->CreateResources();
				}
			}

			if (material != nullptr)
			{
				// Sampler2Ds
				std::map<uint32_t, FL::TexturePipelineData>* texturesShaderData = material->GetTexturesPipelineData();
				std::map<uint32_t, FL::Texture>& meshTextures = mesh->GetTextures();			

				int textureCounter = 0;
				for (std::map<uint32_t, FL::TexturePipelineData>::iterator iter = texturesShaderData->begin(); iter != texturesShaderData->end(); iter++)
				{
					int droppedTextureValue = -1;
					std::string openedTexturePath = "";
					std::string textureName = "";
					if (meshTextures.count(iter->first))
					{
						textureName = FL::FileHelper::GetFilenameFromPath(meshTextures.at(iter->first).GetTexturePath());
						if (textureName == "resourceNotPresent")
						{
							textureName = "";
						}
					}

					std::string shaderStageString = "";
					switch (iter->second.shaderStage)
					{
					case VK_SHADER_STAGE_VERTEX_BIT:
						shaderStageString = "Vertex Sampled Tex binding = " + std::to_string(iter->first + 1);
						break;

					case VK_SHADER_STAGE_FRAGMENT_BIT:
						shaderStageString = "Fragment Sampled Tex binding = " + std::to_string(iter->first + 1);
						break;

					default:
						break;
					}

					ImGui::Text("%s", shaderStageString.c_str());
					if (FL::GuiCore::DropInputCanOpenFiles("##InputMaterialTextureFilePath" + std::to_string(textureCounter), "", textureName, FL::GuiCore::fileExplorerTarget, droppedTextureValue, openedTexturePath, "Drop image files here from File Explorer"))
					{
						if (droppedTextureValue >= 0)
						{
							std::filesystem::path fsPath(FL::GuiCore::selectedFiles[droppedTextureValue - 1]);
							if (fsPath.extension() == ".png" || fsPath.extension() == ".jpg")
							{							
								if (meshTextures.count(iter->first) && meshTextures.at(iter->first).LoadFromFile(fsPath.string()))
								{
									mesh->CreateResources();
								}
							}
							else
							{
								FL::Logger::log.Err("File must be of type .png or .jpg to drop here.");
							}
						}
						else if (droppedTextureValue == -2)
						{
							// Remove reference
							if (meshTextures.at(iter->first).LoadFromFile(FL::Assets::assetManager.GetTextureObject("resourceNotPresent")->GetTexturePath()))
							{
								mesh->CreateResources();
							}
						}
						else if (openedTexturePath != "")
						{
							std::filesystem::path fsPath(openedTexturePath);
							if (fsPath.extension() == ".png" || fsPath.extension() == ".jpg")
							{
								if (meshTextures.at(iter->first).LoadFromFile(openedTexturePath))
								{
									mesh->CreateResources();
								}
							}	
							else
							{
								FL::Logger::log.Err("File must be of type .png or .jpg to drop here.");
							}
						}
					}
					textureCounter++;
				}

				std::map<uint32_t, std::string> vec4Names = material->GetUBOVec4Names();

				for (std::map<uint32_t, std::string>::iterator iter = vec4Names.begin(); iter != vec4Names.end(); iter++)
				{
					std::string vec4Name = iter->second;
					if (uboVec4s.count(vec4Name))
					{
						glm::vec4 glmVec4 = uboVec4s.at(vec4Name);
						FL::Vector4 uboVec4 = FL::Vector4(glmVec4.x, glmVec4.y, glmVec4.z, glmVec4.w);
						if (FL::GuiCore::RenderVector4Table(FL::GuiCore::TableProps("##MaterialVec4UBO" + std::to_string(ownerID), vec4Name), uboVec4)) uboVec4s.at(vec4Name) = glm::vec4(uboVec4.x, uboVec4.y, uboVec4.z, uboVec4.w);
					}
					else
					{
						uboVec4s.emplace(vec4Name, glm::vec4()); // Maybe redundant
					}
				}
			}
		}

		void RenderLightComponent(FL::Light* light)
		{
			long ownerID = light->GetOwnerID();		
			FL::Vector3 direction = light->GetDirection();
			float xDir = direction.x;
			float yDir = direction.y;
			float zDir = direction.z;
			FL::Vector4 color = light->GetColor();
			float colorX = color.x;
			float colorY = color.y;
			float colorZ = color.z;
			float colorW = color.w;

			if (FL::GuiCore::PushTable("##LightProperties" + std::to_string(ownerID), 2))
			{
				if (FL::GuiCore::RenderFloatDragTableRow("##XDirection" + std::to_string(ownerID), "X Direction", xDir, 0.1f, -FLT_MAX, FLT_MAX))
				{
					light->SetDirection(FL::Vector3(xDir, yDir, zDir));
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##YDirection" + std::to_string(ownerID), "Y Direction", yDir, 0.1f, -FLT_MAX, FLT_MAX))
				{
					light->SetDirection(FL::Vector3(xDir, yDir, zDir));
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##ZDirection" + std::to_string(ownerID), "Z Direction", zDir, 0.1f, -FLT_MAX, FLT_MAX))
				{
					light->SetDirection(FL::Vector3(xDir, yDir, zDir));
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##ColorX" + std::to_string(ownerID), "Red", colorX, 0.001f, 0, 1))
				{
					light->SetColor(FL::Vector4(colorX, colorY, colorZ, colorW));
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##ColorY" + std::to_string(ownerID), "Green", colorY, 0.001f, 0, 1))
				{
					light->SetColor(FL::Vector4(colorX, colorY, colorZ, colorW));
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##ColorZ" + std::to_string(ownerID), "Blue", colorZ, 0.001f, 0, 1))
				{
					light->SetColor(FL::Vector4(colorX, colorY, colorZ, colorW));
				}
				if (FL::GuiCore::RenderFloatDragTableRow("##ColorW" + std::to_string(ownerID), "Alpha", colorW, 0.001f, 0, 1))
				{
					light->SetColor(FL::Vector4(colorX, colorY, colorZ, colorW));
				}
				FL::GuiCore::PopTable();
			}
		}
	}

	bool RenderTagListTable(std::string ID, std::string fieldName, FL::TagList* tagList)
	{
		bool b_changed = false;
		bool b_hasTag = tagList->HasTag(fieldName);
		bool b_collidesTag = tagList->CollidesTag(fieldName);
		std::string hasTagID = "Has##" + fieldName + "CheckboxHasTagID";
		std::string collidesTagID = "Collides##" + fieldName + "CheckboxCollideTagID";
	
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);		
		ImGui::Text(" %s", fieldName.c_str());			
		ImGui::TableSetColumnIndex(1);				
		if (FL::GuiCore::RenderCheckbox(hasTagID.c_str(), b_hasTag))
		{
			tagList->ToggleTag(fieldName);
			b_changed = true;
		}
		ImGui::TableSetColumnIndex(2);		
		if (FL::GuiCore::RenderCheckbox(collidesTagID.c_str(), b_collidesTag))
		{
			tagList->ToggleCollides(fieldName);
			b_changed = true;
		}

		ImGui::PushID(ID.c_str());
		ImGui::PopID();

		return b_changed;
	}

	void RenderInspector(bool& b_show, std::string windowID, long focusedID)
	{
		if (!b_show)
			return;
		
		FL::GuiCore::b_currentTableLight = true;
		long objectQueuedForDeletion = -1;
				
		if (FL::GuiCore::BeginWindow("Inspector##" + windowID, b_show))
		{
			if (focusedID == -1 || FL::SceneManager::loadedScene.GetObjectByID(focusedID) == nullptr)
			{
				FL::GuiCore::EndWindow(); // Inspector	
				return;
			}

			if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))			
				FL::ProjectManager::loadedProject.lastFocusedID = focusedID;

			FL::GameObject* focusedObject = FL::SceneManager::loadedScene.GetObjectByID(focusedID);
			
			std::string nameLabel = "Name";
			std::string objectName = focusedObject->GetName();
			if (FL::GuiCore::RenderInput("##GameObjectName", nameLabel, objectName))
			{
				focusedObject->SetName(objectName);
			}

			bool b_isActive = focusedObject->IsActive();
			FL::GuiCore::MoveScreenCursor(3, 2);
			if (FL::GuiCore::RenderCheckbox("Active", b_isActive))
			{
				focusedObject->SetActive(b_isActive);
			}
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 90, 5);
			FL::GuiCore::MoveScreenCursor(0, -2);

			static FL::Vector2 mousePos = ImGui::GetCursorScreenPos();
			FL::TagList &tagList = focusedObject->GetTagList();			
			if (FL::GuiCore::RenderButton("Tags"))
			{
				mousePos = FL::Vector2(ImGui::GetIO().MousePos.x - 200, ImGui::GetIO().MousePos.y);
				ImGui::SetNextWindowPos(mousePos);
			}


			FL::GuiCore::PushMenuStyles();
			if (ImGui::BeginPopupContextItem("TagsPopup", ImGuiPopupFlags_MouseButtonLeft))
			{
				if (FL::GuiCore::PushTable("TagsTable", 3))
				{
					for (std::string tag : FL::Assets::assetManager.GetTags())
					{
						std::string tableRowId = tag + "TagCheckboxTableRow";
						RenderTagListTable(tableRowId.c_str(), tag, &tagList);
					}
					FL::GuiCore::PopTable();
				}
				ImGui::EndPopup();					
			}
			FL::GuiCore::PopMenuStyles();


			// Three Dots More Options Button
			ImGui::SameLine(0, 5);
			FL::GuiCore::MoveScreenCursor(0, -1);
			FL::GuiCore::RenderImageButton("##InspectorMoreButton", FL::Assets::assetManager.GetTexture("threeDots"),FL::Vector2(16), 0. ,FL::Vector2(1), "buttonBorder", "transparent");		
			FL::GuiCore::PushMenuStyles();
			if (ImGui::BeginPopupContextItem("##InspectorMoreContext", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
			{
				if (FL::GuiCore::MenuItem(" Delete GameObject"))
				{						
					objectQueuedForDeletion = focusedID;								
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
			FL::GuiCore::PopMenuStyles();

			ImGui::SameLine(0,3);
			FL::GuiCore::MoveScreenCursor(0, -1);
			static bool b_expandAll = true;
			if (b_expandAll)
			{
				if (FL::GuiCore::RenderImageButton("##ExpandCollapseAllComponents" + std::to_string(focusedID), FL::Assets::assetManager.GetTexture("expandFlipped"),FL::Vector2(16), 0 ,FL::Vector2(1), "buttonBorder", "transparent"))
				{
					for (int i = 1; i < FL::ComponentType_Size; i++)
					{
						FL::Component* component = focusedObject->GetComponent((FL::ComponentType)i);
						if (component != nullptr)
						{
							component->SetCollapsed(b_expandAll);
						}
					}
					b_expandAll = !b_expandAll;
				}
				if (ImGui::IsItemHovered())
				{
					FL::GuiCore::RenderTextToolTip("Collapse all");
				}
			}
			else
			{
				if (FL::GuiCore::RenderImageButton("##ExpandCollapseAllComponents" + std::to_string(focusedID), FL::Assets::assetManager.GetTexture("expand"),FL::Vector2(16), 0 ,FL::Vector2(1), "buttonBorder", "transparent", "white"))
				{
					for (int i = 1; i < FL::ComponentType_Size; i++)
					{
						FL::Component* component = focusedObject->GetComponent((FL::ComponentType)i);
						if (component != nullptr)
						{
							component->SetCollapsed(b_expandAll);
						}
					}
					b_expandAll = !b_expandAll;
				}
				if (ImGui::IsItemHovered())
				{
					FL::GuiCore::RenderTextToolTip("Expand all");
				}
			}		

			// For scrolling components section with background
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::Assets::assetManager.GetColor("componentsScrollingBg"));				
			ImGui::BeginChild("ComponentsSectionBg",FL::Vector2(0, ImGui::GetContentRegionAvail().y - 30), FL::GuiCore::childFlags);
			ImGui::PopStyleColor();
			// {			

				if (focusedObject != nullptr)
				{
					FL::Component* queuedForDelete = nullptr;
					
					for (int i = 1; i < FL::ComponentType_Size; i++)
					{
						FL::Component* component = focusedObject->GetComponent((FL::ComponentType)i);
						if (component != nullptr)
						{
							Inspector::BeginComponent(component, queuedForDelete);
							if (!component->IsCollapsed())
							{
								switch ((FL::ComponentType)i)
								{											
									case FL::ComponentType_Animation:           Inspector::RenderAnimationComponent(static_cast<FL::Animation*>(component)); break;
									case FL::ComponentType_Audio:			    Inspector::RenderAudioComponent(static_cast<FL::Audio*>(component)); break;
									// case FL::ComponentType_Body: 			    Inspector::RenderBodyComponent(static_cast<FL::Body*>(component)); break;
									case FL::ComponentType_Body2D: 			    Inspector::RenderBody2DComponent(static_cast<FL::Body2D*>(component)); break;
									case FL::ComponentType_Button: 			    Inspector::RenderButtonComponent(static_cast<FL::Button*>(component)); break;
									case FL::ComponentType_Camera:			    Inspector::RenderCameraComponent(static_cast<FL::Camera*>(component)); break;
									case FL::ComponentType_Canvas:			    Inspector::RenderCanvasComponent(static_cast<FL::Canvas*>(component)); break;
									case FL::ComponentType_CharacterController: Inspector::RenderCharacterControllerComponent(static_cast<FL::CharacterController*>(component)); break;										
									case FL::ComponentType_Light:			    Inspector::RenderLightComponent(static_cast<FL::Light*>(component)); break;
									case FL::ComponentType_Mesh:			    Inspector::RenderMeshComponent(static_cast<FL::Mesh*>(component)); break;
									case FL::ComponentType_Script:			    Inspector::RenderScriptComponent(static_cast<FL::Script*>(component)); break;
									case FL::ComponentType_Sprite:			    Inspector::RenderSpriteComponent(static_cast<FL::Sprite*>(component)); break;
									case FL::ComponentType_Text:			    Inspector::RenderTextComponent(static_cast<FL::Text*>(component)); break;
									case FL::ComponentType_TileMap:			    Inspector::RenderTileMapComponent(static_cast<FL::TileMap*>(component)); break;
									case FL::ComponentType_Transform:		    Inspector::RenderTransformComponent(static_cast<FL::Transform*>(component)); break;
								
									default: break;
								}
							}
							Inspector::EndComponent(component);
						}
					}

					if (queuedForDelete != nullptr)
					{
						focusedObject->RemoveComponent(queuedForDelete);
						queuedForDelete = nullptr;
					}
				}

			// }
			ImGui::EndChild(); // ComponentsSectionBg

			
			FL::GuiCore::PushMenuStyles();
			FL::GuiCore::RenderButton("Add Component",FL::Vector2(ImGui::GetContentRegionAvail().x, 0));
			if (ImGui::BeginPopupContextItem("##AddComponent", ImGuiPopupFlags_MouseButtonLeft))
			{
				// Add all the component types you can add to this GameObject
				for (int i = 1; i < FL::ComponentType_Size; i++)
				{
					if (!focusedObject->GetComponent((FL::ComponentType)i))
					{
						std::string componentTypeString = " " + FL::ComponentTypeStrings[i];
						if (FL::GuiCore::MenuItem(componentTypeString.c_str()))
						{
							focusedObject->AddComponent((FL::ComponentType)i);
							ImGui::CloseCurrentPopup();
						}
						if (i < FL::ComponentType_Size - 1)
							FL::GuiCore::RenderMenuSeparator();
					}
				}					
				ImGui::EndPopup();
			}
			FL::GuiCore::PopMenuStyles();
		}		
		
		if (objectQueuedForDeletion != -1)
		{
			FL::ProjectManager::loadedProject.RemoveFocusedObjectID(focusedID);			
			FL::SceneManager::loadedScene.DeleteGameObject(focusedID);
			objectQueuedForDeletion = -1;
		}
		
		FL::GuiCore::EndWindow(); // Inspector	
	}
}