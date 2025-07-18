#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameObject.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "TagList.h"
#include "Component.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Scene.h"
#include "Script.h"
#include "Button.h"
#include "Canvas.h"
#include "Animation.h"
#include "Audio.h"
#include "Text.h"
#include "CharacterController.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "Project.h"
#include "TileMap.h"
#include "TileSet.h"

#include "imgui.h"

namespace FL = FlatEngine;


namespace FlatGui 
{
	// ImGui Wrappers
	void BeginComponent(FL::Component* component, FL::Component*& queuedForDelete, std::string typeNameOverride)
	{
		bool b_isCollapsed = component->IsCollapsed();
		long ID = component->GetID();

		std::string componentType = component->GetTypeString();
		if (typeNameOverride != "")
		{
			componentType = typeNameOverride;
		}
		std::string componentID = component->GetTypeString() + std::to_string(component->GetID());

		// Begin Component
		ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("innerWindow"));
		ImGui::PushStyleColor(ImGuiCol_Border, FL::GetColor("componentBorder"));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2);
		ImGui::SetNextWindowScroll(Vector2(0, 0));
		ImGui::BeginChild(componentID.c_str(), Vector2(0, 0), FL::F_autoResizeChildFlags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		// {
		
			// Border around each component
			auto componentWindowPos = ImGui::GetWindowPos();
			auto componentWindowSize = ImGui::GetWindowSize();		
			ImGui::GetWindowDrawList()->AddRect({ componentWindowPos.x + 2, componentWindowPos.y + 2 }, { componentWindowPos.x + componentWindowSize.x - 2, componentWindowPos.y + componentWindowSize.y - 2 }, FL::GetColor32("componentBorder"), 2);

			// Tooltip for Component meta data
			FL::RenderInvisibleButton("ComponentToolTipButton-" + std::to_string(ID), { componentWindowPos.x, componentWindowPos.y }, { componentWindowSize.x, 27 });
			if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt)
			{
				FL::BeginToolTip("Component Data");
				FL::RenderToolTipLong("ID", ID);
				FL::RenderToolTipLong("ParentID", component->GetParentID());
				FL::EndToolTip();
			}
			ImGui::SetCursorScreenPos({ componentWindowPos.x + 5, componentWindowPos.y + 5 });
		
			// Component Name			
			FL::MoveScreenCursor(5, 5);
			ImGui::Text(componentType.c_str());

			if (component->GetType() != FL::T_Transform)
			{
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - (FL::F_childPadding + 36), 5);
			}
			else
			{
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - (FL::F_childPadding + 13), 5);
			}

			ImGui::PushItemWidth(-1.0f);
			ImGui::PushStyleColor(ImGuiCol_Border, FL::GetColor("componentBorder"));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(1.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0));

			std::string expandID = "##expandIcon-" + std::to_string(ID);
			std::string trashcanID = "##trashIcon-" + std::to_string(ID);
			std::string openFileID = "##openFileIcon-" + std::to_string(ID);

			if (component->GetType() != FL::T_Transform)
			{			
				FL::MoveScreenCursor(0, -3);
				if (FL::RenderImageButton(trashcanID.c_str(), FL::GetTexture("trash")))
				{
					queuedForDelete = component;
				}
				ImGui::SameLine(0, 5);
			}

			FL::MoveScreenCursor(0, -3);
			if (b_isCollapsed)
			{
				if (FL::RenderImageButton(expandID.c_str(), FL::GetTexture("expand")))
				{
					component->SetCollapsed(!b_isCollapsed);
				}
			}
			else
			{
				if (FL::RenderImageButton(expandID.c_str(), FL::GetTexture("expandFlipped")))
				{
					component->SetCollapsed(!b_isCollapsed);
				}
			}

			if (!b_isCollapsed)
			{
				ImGui::Separator();
			}
		
			if (!component->IsCollapsed())
			{			
				std::string componentItemID = "##ComponentItem-" + component->GetTypeString();
				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("componentBg"));
				ImGui::BeginChild(componentItemID.c_str(), Vector2(0, 0), FL::F_autoResizeChildFlags);
			}
	}

	void EndComponent(Component* component)
	{
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			// Add some space to the bottom of each component
			if (!component->IsCollapsed())
			{
				ImGui::PopStyleColor();
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
				ImGui::EndChild();
			}

		// }
		ImGui::EndChild(); // Component
	}

	bool RenderIsActiveCheckbox(bool& b_isActive)
	{
		FL::MoveScreenCursor(1, 3);
		bool b_checked = FL::RenderCheckbox("Active", b_isActive);
		FL::MoveScreenCursor(0, 3);
		ImGui::Separator();
		FL::MoveScreenCursor(0, 3);

		return b_checked;
	}

	void RenderTransformComponent(Transform* transform)
	{
		Vector2 position = transform->GetPosition();
		float xPos = position.x;
		float yPos = position.y;
		Vector2 scale = transform->GetScale();
		float xScale = scale.x;
		float yScale = scale.y;
		float rotation = transform->GetRotation();
		bool b_isActive = transform->IsActive();
		long ID = transform->GetID();

		if (RenderIsActiveCheckbox(b_isActive))
		{
			transform->SetActive(b_isActive);
		}
		
		if (FL::PushTable("##TransformProperties" + std::to_string(ID), 2))
		{
			if (FL::RenderFloatDragTableRow("##xPosition" + std::to_string(ID), "X Position", xPos, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				transform->SetPosition(Vector2(xPos, yPos));
			}
			if (FL::RenderFloatDragTableRow("##yPosition" + std::to_string(ID), "Y Position", yPos, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				transform->SetPosition(Vector2(xPos, yPos));
			}	
			if (FL::RenderFloatDragTableRow("##rotation" + std::to_string(ID), "Rotation", rotation, 0.1f, -360, 360))
			{
				transform->SetRotation(rotation);
			}
			if (FL::RenderFloatDragTableRow("##xScaleDrag" + std::to_string(ID), "X Scale", xScale, 0.1f, 0.001f, 1000))
			{
				if (xScale > 0)
				{
					transform->SetScale(Vector2(xScale, yScale));
				}
				else
				{
					FL::LogError("Scale must be greater than 0.");
				}
			}
			if (FL::RenderFloatDragTableRow("##yScaleDrag" + std::to_string(ID), "Y Scale", yScale, 0.1f, 0.001f, 1000))
			{
				if (yScale > 0)
				{
					transform->SetScale(Vector2(xScale, yScale));
				}
				else
				{
					FL::LogError("Scale must be greater than 0.");
				}
			}
			FL::PopTable();
		}
	}

	void RenderSpriteComponent(Sprite* sprite)
	{
		std::string path = FL::GetFilenameFromPath(sprite->GetPath(), true);
		int textureWidth = sprite->GetTextureWidth();
		int textureHeight = sprite->GetTextureHeight();
		Vector2 textureScale = sprite->GetScale();
		FL::Pivot pivotPoint = sprite->GetPivotPoint();
		std::string pivotString = sprite->GetPivotPointString();
		float xScale = textureScale.x;
		float yScale = textureScale.y;
		int renderOrder = sprite->GetRenderOrder();
		bool b_isActive = sprite->IsActive();
		Vector2 offset = sprite->GetOffset();
		float xOffset = offset.x;
		float yOffset = offset.y;
		std::string pathString = "Path: ";
		std::string textureWidthString = std::to_string(textureWidth) + "px";
		std::string textureHeightString = std::to_string(textureHeight) + "px";
		Vector4 tintColor = sprite->GetTintColor();
		long ID = sprite->GetID();		
		
		if (RenderIsActiveCheckbox(b_isActive))
		{
			sprite->SetActive(b_isActive);
		}

		int droppedValue = -1;
		std::string openedPath = "";
		if (FL::DropInputCanOpenFiles("##InputSpritePath", "File", FL::GetFilenameFromPath(path, true), FL::F_fileExplorerTarget, droppedValue, openedPath, "Drop images here from File Explorer"))
		{
			if (droppedValue >= 0)
			{
				std::filesystem::path fsPath(FL::F_selectedFiles[droppedValue - 1]);
				if (fsPath.extension() == ".png" || fsPath.extension() == ".jpg" || fsPath.extension() == ".tif" || fsPath.extension() == ".webp" || fsPath.extension() == ".jxl")
				{
					sprite->SetTexture(fsPath.string());
				}
				else
				{
					FL::LogError("File must be of type .png to drop here.");
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

		FL::RenderSeparator(2, 3);
		
		if (FL::PushTable("##SpriteProperties" + std::to_string(ID), 2))
		{
			if (FL::RenderFloatDragTableRow("##xSpriteScaleDrag" + std::to_string(ID), "X Scale", xScale, 0.1f, 0.001f, 1000))
			{
				if (xScale > 0)
				{
					sprite->SetScale(Vector2(xScale, yScale));
				}
				else
				{
					FL::LogError("Scale must be greater than 0.");
				}
			}
			if (FL::RenderFloatDragTableRow("##ySpriteScaleDrag" + std::to_string(ID), "Y Scale", yScale, 0.1f, 0.001f, 1000))
			{
				if (yScale > 0)
				{
					sprite->SetScale(Vector2(xScale, yScale));
				}
				else
				{
					FL::LogError("Scale must be greater than 0.");
				}
			}
			if (FL::RenderFloatDragTableRow("##xSpriteOffsetDrag" + std::to_string(ID), "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				sprite->SetOffset(Vector2(xOffset, yOffset));
			}
			if (FL::RenderFloatDragTableRow("##ySpriteOffsetDrag" + std::to_string(ID), "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				sprite->SetOffset(Vector2(xOffset, yOffset));
			}
			if (FL::RenderIntDragTableRow("##renderOrder" + std::to_string(ID), "Render Order", renderOrder, 1, 0, (int)FL::F_maxSpriteLayers))
			{
				sprite->SetRenderOrder(renderOrder);
			}
			FL::RenderTextTableRow("##textureWidth" + std::to_string(ID), "Texture width", textureWidthString);
			FL::RenderTextTableRow("##textureHeight" + std::to_string(ID), "Texture height", textureHeightString);
			FL::PopTable();
		}

		FL::RenderSeparator(3, 3);

		if (RenderPivotSelectionButtons("Sprite", pivotPoint))
		{
			sprite->SetPivotPoint(pivotPoint);
		}

		FL::RenderSeparator(6, 3);

		// Tint color picker
		std::string tintID = "##SpriteTintColor" + std::to_string(ID) + "-" + std::to_string(ID);		
		if (ImGui::ColorEdit4(tintID.c_str(), (float*)&tintColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
		{
			sprite->SetTintColor(tintColor);
		}
		ImGui::SameLine(0, 5);
		ImGui::Text("Tint color");
	}

	bool RenderPivotSelectionButtons(std::string componentType, FL::Pivot& pivot)
	{				
		ImGui::Text("Pivot Point: ");
		ImGui::SameLine();
		ImGui::Text(FL::F_PivotStrings[pivot].c_str());
		FL::MoveScreenCursor(0, 1);		

		bool b_pivotChanged = false;
		Vector2 cellSize = Vector2(ImGui::GetContentRegionAvail().x, 72);
		Vector2 cursorScreen = ImGui::GetCursorScreenPos();

		// TopLeft, Top, TopRight
		ImGui::GetWindowDrawList()->AddRectFilled(cursorScreen, Vector2(cursorScreen.x + cellSize.x, cursorScreen.y + cellSize.y), FL::GetColor32("pivotSelectionBg"));
		FL::MoveScreenCursor(5, 5);
		if (FL::RenderImageButton("##" + componentType + "PivotTopLeftButton", FL::GetTexture("upLeft"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotTopLeft;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotTopButton", FL::GetTexture("up"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotTop;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotTopRightButton", FL::GetTexture("upRight"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotTopRight;
			b_pivotChanged = true;
		}

		// Left, Center, Right
		FL::MoveScreenCursor(5, 0);
		if (FL::RenderImageButton("##" + componentType + "PivotLeftButton", FL::GetTexture("left"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotLeft;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotCenterButton", FL::GetTexture("center"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotCenter;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotRightButton", FL::GetTexture("right"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotRight;
			b_pivotChanged = true;
		}

		// BottomLeft, Bottom, BottomRight		
		FL::MoveScreenCursor(5, 0);
		if (FL::RenderImageButton("##" + componentType + "PivotBottomLeftButton", FL::GetTexture("downLeft"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotBottomLeft;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotBottomButton", FL::GetTexture("down"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotBottom;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotBottomRightButton", FL::GetTexture("downRight"), Vector2(16, 16), 1, FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotBottomRight;
			b_pivotChanged = true;
		}
				
		return b_pivotChanged;
	}

	void RenderCameraComponent(Camera* camera)
	{
		float width = camera->GetWidth();
		float height = camera->GetHeight();
		bool b_isPrimary = camera->IsPrimary();
		float zoom = camera->GetZoom();
		ImVec4 frustrumColor = camera->GetFrustrumColor();
		bool b_isActive = camera->IsActive();
		long ID = camera->GetID();
		bool b_follow = camera->GetShouldFollow();
		std::string followingName = "";
		long toFollowID = camera->GetToFollowID();
		GameObject* followingObject = FL::GetObjectByID(toFollowID);

		if (toFollowID != -1 && followingObject != nullptr)
		{
			followingName = FL::GetObjectByID(toFollowID)->GetName();
		}
		else if (followingObject == nullptr)
		{
			camera->SetToFollowID(-1);
		}

		float followSmoothing = camera->GetFollowSmoothing();
		
		if (RenderIsActiveCheckbox(b_isActive))
		{
			camera->SetActive(b_isActive);
		}
		
		if (FL::PushTable("##CameraProperties" + std::to_string(ID), 2))
		{
			if (FL::RenderFloatDragTableRow("##cameraWidth" + std::to_string(ID), "Camera width", width, 0.1f, 0, 1000))
			{
				camera->SetDimensions(width, height);
			}
			if (FL::RenderFloatDragTableRow("##cameraHeight" + std::to_string(ID), "Camera height", height, 0.1f, 0, 1000))
			{
				camera->SetDimensions(width, height);
			}
			if (FL::RenderFloatDragTableRow("##cameraZoom" + std::to_string(ID), "Camera zoom", zoom, 0.1f, 1, 100))
			{
				camera->SetZoom(zoom);
			}
			if (FL::RenderFloatDragTableRow("##cameraFollowSmoothing" + std::to_string(ID), "Follow smoothing", followSmoothing, 0.01f, 0, 1))
			{
				camera->SetFollowSmoothing(followSmoothing);
			}
			FL::PopTable();
		}

		FL::RenderSeparator(3, 3);

		int droppedValue = -1;
		if (FL::DropInput("##CameraFollowObject", "Following", followingName, "DND_HIERARCHY_OBJECT", droppedValue, "Drag a GameObject here from the Hierarchy"))
		{
			if (FL::GetObjectByID(droppedValue) != nullptr || droppedValue == -1)
			{
				camera->SetToFollowID(droppedValue);
			}
		}

		ImGui::BeginDisabled(toFollowID == -1);
		if (FL::RenderCheckbox("Follow", b_follow))
		{
			camera->SetShouldFollow(b_follow);
		}
		ImGui::EndDisabled();

		FL::RenderSeparator(3, 3);
			
		if (FL::RenderCheckbox("Is Primary Camera", b_isPrimary))
		{
			camera->SetPrimaryCamera(b_isPrimary);
			if (b_isPrimary)
			{
				FL::SetPrimaryCamera(camera);
			}
			else
			{
				FL::RemovePrimaryCamera();
			}
		}		

		// Frustrum color picker
		std::string frustrumID = "##FrustrumColor" + std::to_string(ID);
		ImVec4 color = ImVec4(frustrumColor.x / 255.0f, frustrumColor.y / 255.0f, frustrumColor.z / 255.0f, frustrumColor.w / 255.0f);
		ImGui::ColorEdit4(frustrumID.c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::SameLine(0, 5);
		ImGui::Text("Frustrum color");
		camera->SetFrustrumColor(ImVec4(color.x * 255.0f, color.y * 255.0f, color.z * 255.0f, color.w * 255.0f));
	}

	void RenderScriptComponent(Script* script)
	{
		std::vector<std::string> allScriptNames = FL::F_luaScriptNames;
		static int currentScript = 0;
		std::string attachedScriptName = script->GetAttachedScript();
		bool b_isActive = script->IsActive();
		std::map<std::string, Animation::S_EventFunctionParam> &scriptParams = script->GetScriptParams();
		long ID = script->GetID();
		int luaScriptCount = (int)FL::F_luaScriptNames.size();
		for (std::string CPPScript : FL::F_CPPScriptNames)
		{
			allScriptNames.push_back(CPPScript);
		}
			

		if (RenderIsActiveCheckbox(b_isActive))
		{
			script->SetActive(b_isActive);
		}

		for (int i = 0; i < allScriptNames.size(); i++)
		{
			if (attachedScriptName == allScriptNames[i])
			{
				currentScript = i;
				break;
			}
		}

		std::string newLuaScriptModalLabel = "Create Lua Script";
		std::string newLCPPScriptModalLabel = "Create C++ Script";
		std::string newScriptName = "";

		if (FL::RenderSelectable("##SelectLuaScript", allScriptNames, currentScript))
		{
			script->SetAttachedScript(allScriptNames[currentScript]);
		}

		bool b_openLuaModal = false;
		if (FL::RenderButton("New Lua Script", Vector2(110, 20)))
		{
			b_openLuaModal = true;
		}

		ImGui::SameLine(0,5);

		bool b_openCPPModal = false;
		if (FL::RenderButton("New C++ Script", Vector2(110, 20)))
		{
			b_openCPPModal = true;
		}

		if (FL::RenderInputModal(newLuaScriptModalLabel.c_str(), "Enter a name for the Lua script", newScriptName, b_openLuaModal))
		{
			FL::CreateNewLuaScript(newScriptName);
		}

		if (FL::RenderInputModal(newLCPPScriptModalLabel.c_str(), "Enter a name for the C++ script", newScriptName, b_openCPPModal))
		{
			FL::CreateNewCPPScript(newScriptName);
		}

		FL::RenderSeparator(3, 3);
		FL::RenderSectionHeader("Parameters");

		if (currentScript < luaScriptCount && currentScript != 0)
		{
			// Set the values for a new parameter
			static std::string newParamName = "";
			int currentNewParamType = 0;
			std::vector<std::string> types = { "string", "int", "float", "double", "long", "bool", "Vector2" };
			static Animation::S_EventFunctionParam newParam = Animation::S_EventFunctionParam();

			for (int i = 0; i < types.size(); i++)
			{
				if (newParam.type == types[i])
				{
					currentNewParamType = i;
				}
			}

			std::string newParamComboID = "##EventFunctionParameterType-createNew";
			if (FL::RenderCombo(newParamComboID, types[currentNewParamType], types, currentNewParamType, 85))
			{
				newParam.type = types[currentNewParamType];
			}

			ImGui::SameLine();
			FL::RenderInput("##newScriptParamInput", "", newParamName);

			float inputWidth = ImGui::GetContentRegionAvail().x - 36;

			ImGui::BeginDisabled(newParamName == "");
			FL::MoveScreenCursor(0, 3);
			if (FL::RenderButton("Add parameter"))
			{
				newParam.type = types[currentNewParamType];
				script->AddScriptParam(newParamName, newParam);
				newParamName = "";
				currentNewParamType = 0;
			}
			ImGui::EndDisabled();

			if (scriptParams.size() > 0)
			{
				FL::RenderSeparator(3, 3);

				// Display existing parameters
				FL::MoveScreenCursor(0, 5);

				if (scriptParams.size() > 0)
				{
					ImGui::Text("Type:");
					ImGui::SameLine(0, 60);
					ImGui::Text("Name:");
					ImGui::SameLine(0, 75);
					ImGui::Text("Value:");
				}

				FL::MoveScreenCursor(0, 5);
			}

			int paramCounter = 0;
			std::string paramQueuedForDelete = "";
			for (std::map<std::string, Animation::S_EventFunctionParam>::iterator paramIter = scriptParams.begin(); paramIter != scriptParams.end(); paramIter++)
			{
				int currentParamType = 0;
				Animation::S_EventFunctionParam& param = paramIter->second;

				for (int i = 0; i < types.size(); i++)
				{
					if (param.type == types[i])
					{
						currentParamType = i;
					}
				}

				std::string currentParamComboID = "##EventFunctionParameterType" + std::to_string(paramCounter);
				if (FL::RenderCombo(currentParamComboID, types[currentParamType], types, currentParamType, 85))
				{
					param.type = types[currentParamType];
				}

				ImGui::SameLine();
				ImGui::Text(paramIter->first.c_str());
				ImGui::SameLine();



				float inputWidth = ImGui::GetContentRegionAvail().x - 36;

				if (param.type == "string")
				{
					std::string stringValue = param.e_string;
					if (FL::RenderInput("##EventParamString" + std::to_string(paramCounter), "", stringValue, false, inputWidth))
					{
						param.e_string = stringValue;
					}
				}
				else if (param.type == "int")
				{
					int intValue = param.e_int;
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
					if (FL::RenderDragInt("##EventParamInt" + std::to_string(paramCounter), inputWidth, intValue, 1, INT_MIN, INT_MAX, 0, "input"))
					{
						param.e_int = intValue;
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}
				else if (param.type == "long")
				{
					int longValue = (int)param.e_long;
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
					if (FL::RenderDragInt("##EventParamLong" + std::to_string(paramCounter), inputWidth, longValue, 1, INT_MIN, INT_MAX, 0, "input"))
					{
						param.e_long = longValue;
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}
				else if (param.type == "float")
				{
					float floatValue = param.e_float;
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
					if (FL::RenderDragFloat("##EventParamFloat" + std::to_string(paramCounter), inputWidth, floatValue, 0.01f, -FLT_MAX, FLT_MAX, 0, "input"))
					{
						param.e_float = floatValue;
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}
				else if (param.type == "double")
				{
					float doubleValue = (float)param.e_double;
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
					if (FL::RenderDragFloat("##EventParamFloat" + std::to_string(paramCounter), inputWidth, doubleValue, 0.01f, -FLT_MAX, FLT_MAX, 0, "input"))
					{
						param.e_double = doubleValue;
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}
				else if (param.type == "bool")
				{
					std::vector<std::string> trueFalse = { "true", "false" };
					int currentBool = 0;
					if (param.e_boolean)
					{
						currentBool = 0;
					}
					else
					{
						currentBool = 1;
					}
					if (FL::RenderCombo("##EventParamBooleanDropdown" + std::to_string(paramCounter), param.e_boolean ? "true" : "false", trueFalse, currentBool, inputWidth))
					{
						param.e_boolean = trueFalse[currentBool] == "true";
					}
				}
				else if (param.type == "Vector2")
				{
					inputWidth = (inputWidth / 2) - 3;
					Vector2 vector2Value = param.e_Vector2;

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
					if (FL::RenderDragFloat("##EventParamVector2X" + std::to_string(paramCounter), inputWidth, vector2Value.x, 0.01f, -FLT_MAX, FLT_MAX, 0, "input"))
					{
						param.e_Vector2.x = vector2Value.x;
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();

					ImGui::SameLine(0, 6);

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
					if (FL::RenderDragFloat("##EventParamVector2Y" + std::to_string(paramCounter), inputWidth, vector2Value.y, 0.01f, -FLT_MAX, FLT_MAX, 0, "input"))
					{
						param.e_Vector2.y = vector2Value.y;
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}

				ImGui::SameLine(0, 5);

				std::string trashcanID = "##EventParamtrashIcon-" + std::to_string(paramCounter);
				if (FL::RenderImageButton(trashcanID.c_str(), FL::GetTexture("trash")))
				{
					paramQueuedForDelete = paramIter->first;
				}

				paramCounter++;
			}

			if (paramQueuedForDelete != "")
			{
				scriptParams.erase(paramQueuedForDelete);
			}
		}
	}

	void RenderButtonComponent(Button* button)
	{
		bool b_isActive = button->IsActive();
		float activeWidth = button->GetActiveWidth();
		float activeHeight = button->GetActiveHeight();
		Vector2 activeOffset = button->GetActiveOffset();
		int activeLayer = button->GetActiveLayer();	
		std::string luaFunctionName = button->GetLuaFunctionName();
		bool b_leftClick = button->GetLeftClick();
		bool b_rightClick = button->GetRightClick();
		std::shared_ptr<Animation::S_Event> luaFunctionParams = button->GetLuaFunctionParams();
		long ID = button->GetID();

		if (RenderIsActiveCheckbox(b_isActive))
		{
			button->SetActive(b_isActive);
		}

		if (FL::PushTable("##ButtonProperties" + std::to_string(ID), 2))
		{
			if (FL::RenderIntDragTableRow("##activeLayer" + std::to_string(ID), "Active layer", activeLayer, 1, 20, 20))
			{
				button->SetActiveLayer(activeLayer);
			}
			if (FL::RenderFloatDragTableRow("##activeWidth" + std::to_string(ID), "Active width", activeWidth, 0.1f, 0, 1000))
			{
				button->SetActiveDimensions(activeWidth, activeHeight);
			}
			if (FL::RenderFloatDragTableRow("##activeHeight" + std::to_string(ID), "Active height", activeHeight, 0.1f, 0, 1000))
			{
				button->SetActiveDimensions(activeWidth, activeHeight);
			}
			if (FL::RenderFloatDragTableRow("##activeoffsetx" + std::to_string(ID), "X Offset", activeOffset.x, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				button->SetActiveOffset(activeOffset);
			}
			if (FL::RenderFloatDragTableRow("##activeoffsety" + std::to_string(ID), "Y Offset", activeOffset.y, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				button->SetActiveOffset(activeOffset);
			}
			if (FL::RenderInputTableRow("##luaFunctionName" + std::to_string(ID), "On Click", luaFunctionName))
			{
				button->SetLuaFunctionName(luaFunctionName);
			}
			if (FL::RenderCheckboxTableRow("##leftClickableCheckbox" + std::to_string(ID), "Left Click", b_leftClick))
			{
				button->SetLeftClick(b_leftClick);
			}
			if (FL::RenderCheckboxTableRow("##rightClickableCheckbox" + std::to_string(ID), "Right Click", b_rightClick))
			{
				button->SetRightClick(b_rightClick);
			}
			FL::PopTable();

			FL::RenderSectionHeader("On Click Function Parameters");

			ImGui::BeginDisabled(luaFunctionParams->parameters.size() >= 5);
			FL::MoveScreenCursor(0, 3);
			if (FL::RenderButton("Add parameter"))
			{
				Animation::S_EventFunctionParam param = Animation::S_EventFunctionParam();
				param.type = "string";
				luaFunctionParams->parameters.push_back(param);
			}
			ImGui::EndDisabled();

			ImGui::SameLine(0, 5);

			if (luaFunctionParams->parameters.size() >= 5)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, FL::GetColor32("col_5"));
			}
			std::string paramCountString = std::to_string(luaFunctionParams->parameters.size()) + " / 5";
			ImGui::Text(paramCountString.c_str());
			if (luaFunctionParams->parameters.size() >= 5)
			{
				ImGui::PopStyleColor();
			}

			FL::MoveScreenCursor(0, 5);

			if (luaFunctionParams->parameters.size() > 0)
			{
				//FL::MoveScreenCursor(30, 0);
				ImGui::Text("Type:");
				ImGui::SameLine(0, 75);
				ImGui::Text("Value:");
			}

			FL::MoveScreenCursor(0, 5);

			int paramCounter = 0;
			int paramQueuedForDelete = -1;
			for (Animation::S_EventFunctionParam& param : luaFunctionParams->parameters)
			{
				int currentType = 0;
				std::vector<std::string> types = { "string", "int", "float", "double", "long", "bool", "Vector2" };

				for (int i = 0; i < types.size(); i++)
				{
					if (param.type == types[i])
					{
						currentType = i;
					}
				}

				std::string comboID = "##EventFunctionParameterType" + std::to_string(paramCounter);
				if (FL::RenderCombo(comboID, types[currentType], types, currentType, 85))
				{
					param.type = types[currentType];
				}

				ImGui::SameLine();

				float inputWidth = ImGui::GetContentRegionAvail().x - 36;

				if (param.type == "string")
				{
					std::string stringValue = param.e_string;
					if (FL::RenderInput("##EventParamString" + std::to_string(paramCounter), "", stringValue, false, inputWidth))
					{
						param.e_string = stringValue;
					}
				}

				if (param.type == "int")
				{
					int intValue = param.e_int;
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
					if (FL::RenderDragInt("##EventParamInt" + std::to_string(paramCounter), inputWidth, intValue, 1, INT_MIN, INT_MAX, 0, "input"))
					{
						param.e_int = intValue;
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}
				if (param.type == "long")
				{
					int longValue = (int)param.e_long;
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
					if (FL::RenderDragInt("##EventParamLong" + std::to_string(paramCounter), inputWidth, longValue, 1, INT_MIN, INT_MAX, 0, "input"))
					{
						param.e_long = longValue;
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}
				if (param.type == "float")
				{
					float floatValue = param.e_float;
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
					if (FL::RenderDragFloat("##EventParamFloat" + std::to_string(paramCounter), inputWidth, floatValue, 0.01f, -FLT_MAX, FLT_MAX, 0, "input"))
					{
						param.e_float = floatValue;
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}
				if (param.type == "double")
				{
					float doubleValue = (float)param.e_double;
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
					if (FL::RenderDragFloat("##EventParamFloat" + std::to_string(paramCounter), inputWidth, doubleValue, 0.01f, -FLT_MAX, FLT_MAX, 0, "input"))
					{
						param.e_double = doubleValue;
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}
				if (param.type == "bool")
				{
					std::vector<std::string> trueFalse = { "true", "false" };
					int currentBool = 0;
					if (param.e_boolean)
					{
						currentBool = 0;
					}
					else
					{
						currentBool = 1;
					}
					if (FL::RenderCombo("##EventParamBooleanDropdown" + std::to_string(paramCounter), param.e_boolean ? "true" : "false", trueFalse, currentBool, inputWidth))
					{
						param.e_boolean = trueFalse[currentBool] == "true";
					}
				}
				if (param.type == "Vector2")
				{
					inputWidth = (inputWidth / 2) - 3;
					Vector2 vector2Value = param.e_Vector2;

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
					if (FL::RenderDragFloat("##EventParamVector2X" + std::to_string(paramCounter), inputWidth, vector2Value.x, 0.01f, -FLT_MAX, FLT_MAX, 0, "input"))
					{
						param.e_Vector2.x = vector2Value.x;
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();

					ImGui::SameLine(0, 6);

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(5, 4));
					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 1);
					if (FL::RenderDragFloat("##EventParamVector2Y" + std::to_string(paramCounter), inputWidth, vector2Value.y, 0.01f, -FLT_MAX, FLT_MAX, 0, "input"))
					{
						param.e_Vector2.y = vector2Value.y;
					}
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}

				ImGui::SameLine(0, 5);

				std::string trashcanID = "##EventParamtrashIcon-" + std::to_string(paramCounter);
				if (FL::RenderImageButton(trashcanID.c_str(), FL::GetTexture("trash")))
				{
					paramQueuedForDelete = paramCounter;
				}

				paramCounter++;
			}

			if (paramQueuedForDelete != -1)
			{
				luaFunctionParams->parameters.erase(std::next(luaFunctionParams->parameters.begin(), paramQueuedForDelete));
			}
		}
	}

	void RenderCanvasComponent(Canvas* canvas)
	{
		float canvasWidth = canvas->GetWidth();
		float canvasHeight = canvas->GetHeight();
		int layerNumber = canvas->GetLayerNumber();
		bool b_blocksLayers = canvas->GetBlocksLayers();
		bool b_isActive = canvas->IsActive();
		long ID = canvas->GetID();

		if (RenderIsActiveCheckbox(b_isActive))
		{
			canvas->SetActive(b_isActive);
		}

		if (FL::PushTable("##CanvasProperties" + std::to_string(ID), 2))
		{
			if (FL::RenderIntDragTableRow("##layerNumber" + std::to_string(ID), "Canvas layer", layerNumber, 1, 20, 20))
			{
				canvas->SetLayerNumber(layerNumber);
			}
			if (FL::RenderFloatDragTableRow("##Canvas width" + std::to_string(ID), "Width", canvasWidth, 0.1f, 0.1f, -FLT_MAX))
			{
				canvas->SetDimensions(canvasWidth, canvasHeight);
			}
			if (FL::RenderFloatDragTableRow("##Canvas height" + std::to_string(ID), "Height", canvasHeight, 0.1f, 0.1f, -FLT_MAX))
			{
				canvas->SetDimensions(canvasWidth, canvasHeight);
			}
			FL::PopTable();
		}

		FL::RenderSeparator(3, 3);

		if (FL::RenderCheckbox("Blocks Layers", b_blocksLayers))
		{
			canvas->SetBlocksLayers(b_blocksLayers);
		}
	}

	void RenderAnimationComponent(Animation* animation)
	{
		bool b_isActive = animation->IsActive();
		long ID = animation->GetID();
		std::vector<FL::Animation::AnimationData> &animations = animation->GetAnimations();

		if (RenderIsActiveCheckbox(b_isActive))
		{
			animation->SetActive(b_isActive);
		}

		int droppedAnimValue = -1;
		std::string openedAnimPath = "";
		static std::string newAnimationName = "";
		static std::string newAnimationPath = "";

		FL::RenderSubTitle("Add Animation");

		FL::RenderInput("##NewAnimationName", "Name", newAnimationName, false);

		FL::MoveScreenCursor(0, 3);

		if (FL::DropInputCanOpenFiles("##AnimationPathInspectorwindow-" + std::to_string(ID), "File", FL::GetFilenameFromPath(newAnimationPath, true), FL::F_fileExplorerTarget, droppedAnimValue, openedAnimPath, "Drop animation files here from the File Explorer"))
		{
			if (droppedAnimValue >= 0)
			{
				std::filesystem::path fsPath(FL::F_selectedFiles[droppedAnimValue - 1]);
				if (fsPath.extension() == ".anm")
				{
					newAnimationPath = fsPath.string();
				}
				else
				{
					FL::LogError("File must be of type .anm to drop here.");
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

		FL::MoveScreenCursor(0, 3);

		ImGui::BeginDisabled(newAnimationPath == "" || newAnimationName == "");
		if (FL::RenderButton("Add Animation"))
		{
			if (!animation->ContainsName(newAnimationName) && FL::DoesFileExist(newAnimationPath))
			{
				animation->AddAnimation(newAnimationName, newAnimationPath);
				newAnimationName = "";
				newAnimationPath = "";
			}
			else
			{
				if (animation->ContainsName(newAnimationName))
				{
					FL::LogError("Name already taken in this Animation component, please choose a different one.");
				}
			}
		}
		ImGui::EndDisabled();


		if (animations.size() > 0)
		{
			FL::RenderSeparator(4, 4);
			FL::RenderSubTitle("Attached Animations");
		}

		int IDCounter = 0;
		int queuedAnimationForDelete = -1;
		for (FL::Animation::AnimationData& animData : animations)
		{
			std::string currentAnimationName = animData.name;

			if (FL::RenderInput("##NewAnimationName" + std::to_string(IDCounter), "Name", currentAnimationName, false))
			{
				animData.name = currentAnimationName;
			}

			FL::MoveScreenCursor(0, 3);

			int droppedAnimDataValue = -1;
			std::string openedAnimDataPath = animData.path;
			if (FL::DropInputCanOpenFiles("##AnimationPathInspectorWindow-" + std::to_string(IDCounter), "File", FL::GetFilenameFromPath(openedAnimDataPath, true), FL::F_fileExplorerTarget, droppedAnimDataValue, openedAnimDataPath, "Drop animation files here from the File Explorer"))
			{
				if (droppedAnimDataValue >= 0)
				{
					std::filesystem::path fsPath(FL::F_selectedFiles[droppedAnimDataValue - 1]);
					if (fsPath.extension() == ".anm")
					{
						animData.path = fsPath.string();
					}
					else
					{
						FL::LogError("File must be of type .anm to drop here.");
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

			FL::MoveScreenCursor(0, 4);

			ImGui::BeginDisabled(animData.path == "");
			if (FL::RenderButton("Preview##" + std::to_string(IDCounter)))
			{
				animation->Play(animData.name);
			}
			ImGui::EndDisabled();

			if (animData.path != "")
			{
				ImGui::SameLine(0, 5);
			}

			ImGui::BeginDisabled(animData.path == "" || !animData.b_playing);
			if (FL::RenderButton("Stop##" + std::to_string(IDCounter)))
			{
				animation->Stop(animData.name);
			}
			ImGui::EndDisabled();

			if (animData.path != "")
			{
				ImGui::SameLine(0, 5);
			}

			ImGui::BeginDisabled(animData.path == "");
			if (FL::RenderButton("Edit##" + std::to_string(IDCounter)))
			{
				FG_b_showAnimator = true;

				SetFocusedAnimation(FL::LoadAnimationFile(animData.path));
				FL::F_LoadedProject.SetLoadedPreviewAnimationPath(animData.path);
			}
			ImGui::EndDisabled();

			ImGui::SameLine(0, 10);

			if (FL::RenderButton("Delete##" + std::to_string(IDCounter)))
			{
				queuedAnimationForDelete = IDCounter;
			}
	
			//if (FL::RenderCheckbox("Play on Start", animData.b_playing))
			//{
			//	animData.b_playing = !animData.b_playing;
			//}

			if (animData.name != animations.back().name)
			{
				FL::RenderSeparator(4, 6);
			}

			IDCounter++;
		}

		if (queuedAnimationForDelete != -1)
		{
			std::vector<Animation::AnimationData>::iterator index = animations.begin() + queuedAnimationForDelete;
			animations.erase(index);
		}
	}

	void RenderAudioComponent(Audio* audio)
	{
		long ID = audio->GetID();
		bool b_isActive = audio->IsActive();
		std::vector<FL::SoundData>& sounds = audio->GetSounds();

		if (RenderIsActiveCheckbox(b_isActive))
		{
			audio->SetActive(b_isActive);
		}

		static std::string path = "";
		static std::string name = "";
		static bool b_isNewAudioMusic = false;

		FL::RenderSubTitle("Add Audio");

		FL::RenderInput("##NameNewAudioDataObject", "Name", name, false);

		FL::MoveScreenCursor(0, 3);

		int droppedValue = -1;
		std::string openedPath = "";
		if (FL::DropInputCanOpenFiles("##AddAudioFile", "File", FL::GetFilenameFromPath(path, true), FL::F_fileExplorerTarget, droppedValue, openedPath, "Drop font files here from File Explorer"))
		{
			if (droppedValue >= 0)
			{
				std::filesystem::path fsPath(FL::F_selectedFiles[droppedValue - 1]);
				if (fsPath.extension() == ".wav" || fsPath.extension() == ".mp3")
				{
					path = FL::F_selectedFiles[droppedValue - 1];
				}
				else
				{
					FL::LogError("File must be of type audio to drop here.");
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
		FL::MoveScreenCursor(0, 4);

		ImGui::BeginDisabled(path == "" || name == "");
		if (FL::RenderButton("Add Audio"))
		{
			if (!audio->ContainsName(name) && FL::DoesFileExist(path))
			{
				audio->AddSound(name, path);
				path = "";
				name = "";
			}
			else
			{
				if (audio->ContainsName(name))
				{
					FL::LogError("Name already taken in this Audio component, please choose a different one.");
				}
			}
		}
		ImGui::EndDisabled();

		if (sounds.size() > 0)
		{
			FL::RenderSeparator(4, 4);
			FL::RenderSubTitle("Attached Audio Files");
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
			std::string inputId = "##audioPath_" + std::to_string(ID) + sound.name + std::to_string(IDCounter);

			if (FL::RenderInput("##NameExistingAudioDataObject" + std::to_string(IDCounter), "Name", audioName, false))
			{				
				sound.name = audioName;
			}
			FL::MoveScreenCursor(0, 4);

			if (FL::DropInput(inputId, "File", FL::GetFilenameFromPath(audioPath, true), FL::F_fileExplorerTarget, newDroppedValue, "Drop audio files here from the Explorer window"))
			{
				if (newDroppedValue != -1 && FL::F_selectedFiles.size() >= newDroppedValue)
				{
					std::filesystem::path fsPath(FL::F_selectedFiles[newDroppedValue - 1]);
					if (fsPath.extension() == ".wav" || fsPath.extension() == ".mp3")
					{
						audioPath = FL::F_selectedFiles[newDroppedValue - 1];
						sound.path = audioPath;
						audio->LoadAudio(sound);
					}
					else
					{
						FL::LogError("File must be of type audio to drop here.");
					}
				}
				else if (newDroppedValue == -1)
				{
					audioPath = "";
					sound.path = "";					
				}
			}
			FL::MoveScreenCursor(0, 4);
			
			ImGui::BeginDisabled(sound.name == "" || sound.path == "");
			// {

				// Play Audio
				ImGui::BeginDisabled(!(!sound.b_isMusic || !audio->IsMusicPlaying(sound.name)));
				if (FL::RenderImageButton("##ImageButtonPlay" + sound.name, FL::GetTexture("play")))
				{
					audio->Play(sound.name);
				}
				ImGui::SameLine(0, 5);
				ImGui::EndDisabled();

				// Pause Audio
				ImGui::BeginDisabled((!(!sound.b_isMusic || audio->IsMusicPlaying(sound.name)) || !sound.b_isMusic));
				if (FL::RenderImageButton("##ImageButtonPause" + sound.name, FL::GetTexture("pause")))
				{
					audio->Pause(sound.name);
				}
				ImGui::EndDisabled();
				ImGui::SameLine(0, 5);

				// Stop Audio
				ImGui::BeginDisabled((sound.b_isMusic && !audio->IsMusicPlaying(sound.name)));
				if (FL::RenderImageButton("##ImageButtonStop" + sound.name, FL::GetTexture("stop")))
				{
					audio->Stop(sound.name);
				}
				ImGui::EndDisabled();

			// }
			ImGui::EndDisabled();

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 90, 0);

			if (FL::RenderButton("Remove Sound##"+ sound.name))
			{
				sounds.erase(soundIter);
				break;
			}

			if (sound.name != sounds.back().name)
			{
				FL::RenderSeparator(4, 6);
			}

			IDCounter++;
		}
	}

	void RenderTextComponent(Text* text)
	{
		bool b_isActive = text->IsActive();
		std::shared_ptr<Texture> texture = text->GetTexture();
		float textureWidth = (float)texture->GetWidth();
		float textureHeight = (float)texture->GetHeight();
		int renderOrder = text->GetRenderOrder();
		int fontSize = text->GetFontSize();
		FL::Pivot pivotPoint = text->GetPivotPoint();
		Vector4 color = text->GetColor();
		Vector2 offset = text->GetOffset();
		float xOffset = offset.x;
		float yOffset = offset.y;
		long ID = text->GetID();

		if (RenderIsActiveCheckbox(b_isActive))
		{
			text->SetActive(b_isActive);
		}

		std::string textText = text->GetText();
		if (FL::RenderInput("##TextContent" + std::to_string(ID), "Text", textText))
		{
			text->SetText(textText);
			text->LoadText();
		}

		FL::MoveScreenCursor(0, 3);
		
		std::string fontPath = text->GetFontPath();
		int droppedValue = -1;		
		if (FL::DropInputCanOpenFiles("##InputFontPath", "Font", FL::GetFilenameFromPath(fontPath, true), FL::F_fileExplorerTarget, droppedValue, fontPath, "Drop font files here from File Explorer"))
		{
			if (droppedValue >= 0)
			{
				std::filesystem::path fsPath(FL::F_selectedFiles[droppedValue - 1]);
				if (fsPath.extension() == ".ttf")
				{
					text->SetFontPath(fsPath.string());
				}
				else
				{
					FL::LogError("File must be of type .ttf to drop here.");
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

		FL::RenderSeparator(3, 3);

		if (FL::PushTable("##TextProperties" + std::to_string(ID), 2))
		{
			FL::RenderTextTableRow("##textWidth" + std::to_string(ID), "Text width", std::to_string(textureWidth));
			FL::RenderTextTableRow("##textHeight" + std::to_string(ID), "Text height", std::to_string(textureHeight));
			if (FL::RenderIntDragTableRow("##textFontSize" + std::to_string(ID), "Font size", fontSize, 1, 0, 1000))
			{
				text->SetFontSize(fontSize);
			}
			if (FL::RenderFloatDragTableRow("##xTextOffset" + std::to_string(ID), "X offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				text->SetOffset(Vector2(xOffset, yOffset));
			}
			if (FL::RenderFloatDragTableRow("##yTextOffset" + std::to_string(ID), "Y offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
			{
				text->SetOffset(Vector2(xOffset, yOffset));
			}
			if (FL::RenderIntDragTableRow("##TextRenderOrder" + std::to_string(ID), "Render Order", renderOrder, 1, 0, (int)FL::F_maxSpriteLayers))
			{
				text->SetRenderOrder(renderOrder);
			}
			FL::PopTable();
		}

		FL::RenderSeparator(3, 3);

		if (RenderPivotSelectionButtons("Text", pivotPoint))
		{
			text->SetPivotPoint(pivotPoint);
		}

		FL::RenderSeparator(6, 3);

		// Tint color picker
		std::string tintID = "##TextColor" + std::to_string(ID) + "-" + std::to_string(ID);
		if (ImGui::ColorEdit4(tintID.c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
		{
			text->SetColor(color);
		}
		ImGui::SameLine(0, 5);
		ImGui::Text("Text color");
	}

	void RenderCharacterControllerComponent(CharacterController* characterController)
	{
		bool b_isActive = characterController->IsActive();
		float maxAcceleration = characterController->GetMaxAcceleration();								
		float maxSpeed = characterController->GetMaxSpeed();
		float airControl = characterController->GetAirControl();
		bool b_isMoving = characterController->IsMoving();	
		long ID = characterController->GetID();
		std::string isMovingString = "false";
		if (characterController->IsMoving())
		{
			isMovingString = "true";
		}

		if (RenderIsActiveCheckbox(b_isActive))
			characterController->SetActive(b_isActive);

		if (FL::PushTable("##CharacterControllerProps" + std::to_string(ID), 2))
		{
			if (FL::RenderFloatDragTableRow("##MaxAccelerationDrag" + std::to_string(ID), "Max Acceleration", maxAcceleration, 0.01f, 0.0f, 20.0f))
			{
				characterController->SetMaxAcceleration(maxAcceleration);
			}
			if (FL::RenderFloatDragTableRow("##MaxSpeedDrag" + std::to_string(ID), "Max Speed", maxSpeed, 0.01f, 0.0f, 1000.0f))
			{
				characterController->SetMaxSpeed(maxSpeed);
			}
			if (FL::RenderFloatDragTableRow("##AirControlDrag" + std::to_string(ID), "Air Control", airControl, 0.01f, 0.0f, 1000.0f))
			{
				characterController->SetAirControl(airControl);
			}
			FL::RenderTextTableRow("##IsMoving" + std::to_string(ID), "Is Moving", isMovingString);
			FL::PopTable();
		}
	}

	void RenderBoxColliderComponent(BoxCollider* boxCollider, TileMap* tileMap, std::string collisionAreaName)
	{
		long ID = boxCollider->GetID();
		bool b_isActive = boxCollider->IsActive();
		bool b_isColliding = boxCollider->IsColliding();
		bool b_isContinuous = boxCollider->IsContinuous();
		bool b_isStatic = boxCollider->IsStatic();
		bool b_isSolid = boxCollider->IsSolid();
		bool b_showActiveRadius = boxCollider->GetShowActiveRadius();
		//bool b_isComposite = boxCollider->IsComposite();
		float activeWidth = boxCollider->GetActiveWidth();
		float activeHeight = boxCollider->GetActiveHeight();
		ImVec4 activeEdges = boxCollider->GetActiveEdges();
		Vector2 activeOffset = boxCollider->GetActiveOffset();
		int activeLayer = boxCollider->GetActiveLayer();	
		std::string isCollidingString = "false";

		if (b_isColliding)
		{
			isCollidingString = "true";
		}

		if (RenderIsActiveCheckbox(b_isActive))
		{
			boxCollider->SetActive(b_isActive);
		}

		float widthIncrement = 0.01f;
		float heightIncrement = 0.01f;
		float offsetIncrementX = 0.01f;
		float offsetIncrementY = 0.01f;
		float maxWidth = 1000.0f;
		float maxHeight = 1000.0f;

		if (tileMap != nullptr && collisionAreaName != "")
		{
			float tileWidth = (float)tileMap->GetTileWidth();
			float tileHeight = (float)tileMap->GetTileHeight();
			float tileMapWidth = (float)tileMap->GetWidth();
			float tileMapHeight = (float)tileMap->GetHeight();

			widthIncrement = tileWidth / FL::F_pixelsPerGridSpace;
			heightIncrement = tileHeight / FL::F_pixelsPerGridSpace;
			offsetIncrementX = tileWidth / FL::F_pixelsPerGridSpace;
			offsetIncrementY = tileHeight / FL::F_pixelsPerGridSpace;
			maxWidth = tileMapWidth * tileWidth / FL::F_pixelsPerGridSpace;
			maxHeight = tileMapWidth * tileWidth / FL::F_pixelsPerGridSpace;
		}

		if (FL::PushTable("##BoxColliderProps" + std::to_string(ID), 2))
		{
			if (FL::RenderFloatDragTableRow("##BoxColliderWidth" + std::to_string(ID), "Width", activeWidth, widthIncrement, 0.0f, maxWidth))
			{
				boxCollider->SetActiveDimensions(activeWidth, activeHeight);
			}
			if (FL::RenderFloatDragTableRow("##BoxColliderHeight" + std::to_string(ID), "Height", activeHeight, heightIncrement, 0.0f, maxHeight))
			{
				boxCollider->SetActiveDimensions(activeWidth, activeHeight);
			}
			if (FL::RenderFloatDragTableRow("##ActiveOffsetBoxColliderX" + std::to_string(ID), "X Offset", activeOffset.x, offsetIncrementX, -FLT_MAX, -FLT_MAX))
			{
				boxCollider->SetActiveOffset(activeOffset);
			}
			if (FL::RenderFloatDragTableRow("##ActiveOffsetBoxColliderY" + std::to_string(ID), "Y Offset", activeOffset.y, offsetIncrementY, -FLT_MAX, -FLT_MAX))
			{
				boxCollider->SetActiveOffset(activeOffset);
			}
			if (FL::RenderIntDragTableRow("##BoxColliderActiveLayer" + std::to_string(ID), "Active layer", activeLayer, 1, 0, 100))
			{
				boxCollider->SetActiveLayer(activeLayer);
			}
			FL::RenderTextTableRow("##BoxColliderIsColliding" + std::to_string(ID), "Is Colliding", isCollidingString);
			FL::PopTable();
		}

		FL::MoveScreenCursor(0, 5);
		if (FL::RenderCheckbox(" Is Continuous", b_isContinuous))
		{
			boxCollider->SetIsContinuous(b_isContinuous);
		}
		if (FL::RenderCheckbox(" Is Static", b_isStatic))
		{
			boxCollider->SetIsStatic(b_isStatic);
		}
		if (FL::RenderCheckbox(" Is Solid", b_isSolid))
		{
			boxCollider->SetIsSolid(b_isSolid);
		}
		if (FL::RenderCheckbox(" Show Active Radius", b_showActiveRadius))
		{
			boxCollider->SetShowActiveRadius(b_showActiveRadius);
		}
		//if (FL::RenderCheckbox(" Is Composite", b_isComposite))
		//{
		//	boxCollider->SetIsComposite(b_isComposite);
		//}

		// Enter Collision Area draw mode for this BoxCollider
		if (tileMap != nullptr && collisionAreaName != "")
		{
			if (FL::RenderImageButton("##DrawCollisionArea" + std::to_string(ID), FL::GetTexture("tileColliderDraw")))
			{
				FG_currentSelectedColliderArea = collisionAreaName;
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
			}
			if (ImGui::IsItemHovered())
			{
				FL::RenderTextToolTip("Draw this collision area directly in the Scene View");
			}
		}
	}

	void RenderCircleColliderComponent(CircleCollider* circleCollider)
	{
		long ID = circleCollider->GetID();
		bool b_isActive = circleCollider->IsActive();
		bool b_isColliding = circleCollider->IsColliding();
		bool b_isContinuous = circleCollider->IsContinuous();
		bool b_isStatic = circleCollider->IsStatic();
		//bool b_isSolid = circleCollider->IsSolid();	disabled because circle physics is not sorted out yet		
		//bool b_isComposite = circleCollider->IsComposite();
		float activeRadius = circleCollider->GetActiveRadiusGrid();
		Vector2 activeOffset = circleCollider->GetActiveOffset();
		int activeLayer = circleCollider->GetActiveLayer();
		std::string isCollidingString = "false";

		if (b_isColliding)
		{
			isCollidingString = "true";
		}

		if (RenderIsActiveCheckbox(b_isActive))
		{
			circleCollider->SetActive(b_isActive);
		}

		if (FL::PushTable("##CircleColliderProps" + std::to_string(ID), 2))
		{
			if (FL::RenderFloatDragTableRow("##CircleColliderActiveRadius" + std::to_string(ID), "Radius", activeRadius, 0.01f, 0.0f, 20.0f))
			{
				circleCollider->SetActiveRadiusGrid(activeRadius);
			}
			if (FL::RenderFloatDragTableRow("##ActiveOffsetCircleColliderX" + std::to_string(ID), "X Offset", activeOffset.x, 0.01f, -FLT_MAX, -FLT_MAX))
			{
				circleCollider->SetActiveOffset(activeOffset);
			}
			if (FL::RenderFloatDragTableRow("##ActiveOffsetCircleColliderY" + std::to_string(ID), "Y Offset", activeOffset.y, 0.01f, -FLT_MAX, -FLT_MAX))
			{
				circleCollider->SetActiveOffset(activeOffset);
			}
			if (FL::RenderIntDragTableRow("##CircleColliderActiveLayer" + std::to_string(ID), "Active layer", activeLayer, 1, 0, 100))
			{
				circleCollider->SetActiveLayer(activeLayer);
			}
			FL::RenderTextTableRow("##CircleColliderIsColliding" + std::to_string(ID), "Is Colliding", isCollidingString);
			FL::PopTable();
		}
		
		FL::MoveScreenCursor(0, 5);
		if (FL::RenderCheckbox(" Is Continuous", b_isContinuous))
		{
			circleCollider->SetIsContinuous(b_isContinuous);
		}
		if (FL::RenderCheckbox(" Is Static", b_isStatic))
		{
			circleCollider->SetIsStatic(b_isStatic);
		}
		//if (FL::RenderCheckbox(" Is Solid", b_isSolid))
		//{
		//	circleCollider->SetIsSolid(b_isSolid);
		//}
		//if (ImGui::IsItemHovered())
		//{
		//	FL::RenderTextToolTip("SOLID circle colliders temporarily disabled.");
		//}
		//if (FL::RenderCheckbox(" Is Composite", b_isComposite))
		//{
		//	circleCollider->SetIsComposite(b_isComposite);
		//}
	}

	void RenderRigidBodyComponent(RigidBody* rigidBody)
	{
		long ID = rigidBody->GetID();
		bool b_isActive = rigidBody->IsActive();
		bool b_allowTorques = rigidBody->TorquesAllowed();
		bool b_isStatic = rigidBody->IsStatic();
		bool b_isGrounded = rigidBody->IsGrounded();
		float mass = rigidBody->GetMass();
		float gravity = rigidBody->GetGravity();
		float fallingGravity = rigidBody->GetFallingGravity();
		float terminalVelocity = rigidBody->GetTerminalVelocity();
		float angularDrag = rigidBody->GetAngularDrag();
		float windResistance = rigidBody->GetWindResistance();
		float friction = rigidBody->GetFriction();
		float equilibriumForce = rigidBody->GetEquilibriumForce();
		std::string isGroundedString = "false";

		if (b_isGrounded)
		{
			isGroundedString = "true";
		}

		// Read only		
		Vector2 velocity = rigidBody->GetVelocity();		
		Vector2 pendingForces = rigidBody->GetPendingForces();
		float angularVelocity = rigidBody->GetAngularVelocity();
		float pendingTorques = rigidBody->GetPendingTorques();

		if (RenderIsActiveCheckbox(b_isActive))
		{
			rigidBody->SetActive(b_isActive);
		}

		if (FL::PushTable("##RigidBodyProps" + std::to_string(ID), 2))
		{
			if (FL::RenderFloatDragTableRow("##Mass" + std::to_string(ID), "Mass", mass, 0.01f, 0.0f, -FLT_MAX))
			{
				rigidBody->SetMass(mass);
			}
			if (FL::RenderFloatDragTableRow("##GravityScale" + std::to_string(ID), "Gravity Scale", gravity, 0.01f, -FLT_MAX, -FLT_MAX))
			{
				rigidBody->SetGravity(gravity);
			}
			if (FL::RenderFloatDragTableRow("##FallingGravityScale" + std::to_string(ID), "Falling Gravity", fallingGravity, 0.01f, -FLT_MAX, -FLT_MAX))
			{
				rigidBody->SetFallingGravity(fallingGravity);
			}
			if (FL::RenderFloatDragTableRow("##TerminalVelocity" + std::to_string(ID), "Terminal Velocity", terminalVelocity, 0.01f, 0.001f, 1000))
			{
				rigidBody->SetTerminalVelocity(terminalVelocity);
			}
			if (FL::RenderFloatDragTableRow("##WindResistance" + std::to_string(ID), "Wind Resistance", windResistance, 0.01f, 0, 1))
			{
				rigidBody->SetWindResistance(windResistance);
			}
			if (FL::RenderFloatDragTableRow("##Friction" + std::to_string(ID), "Friction", friction, 0.01f, 0, 1))
			{
				rigidBody->SetFriction(friction);
			}
			if (FL::RenderFloatDragTableRow("##AngularDrag" + std::to_string(ID), "Angular Drag", angularDrag, 0.01f, 0, 1))
			{
				rigidBody->SetAngularDrag(angularDrag);
			}
			if (FL::RenderFloatDragTableRow("##EquilibriumForce" + std::to_string(ID), "Equilibrium Force", equilibriumForce, 0.01f, 0, 1000))
			{
				rigidBody->SetEquilibriumForce(equilibriumForce);
			}			
			FL::RenderTextTableRow("##VelocityX" + std::to_string(ID), "X Velocity", std::to_string(velocity.x));
			FL::RenderTextTableRow("##VelocityY" + std::to_string(ID), "Y Velocity", std::to_string(velocity.y));
			FL::RenderTextTableRow("##PendingForcesX" + std::to_string(ID), "X Pending Forces", std::to_string(pendingForces.x));
			FL::RenderTextTableRow("##PendingForcesY" + std::to_string(ID), "Y Pending Forces", std::to_string(pendingForces.y));
			FL::RenderTextTableRow("##AngularVelocity" + std::to_string(ID), "Angular Velocity (deg)", std::to_string(angularVelocity));
			FL::RenderTextTableRow("##PendingTorques" + std::to_string(ID), "Pending Torques", std::to_string(pendingTorques));
			FL::RenderTextTableRow("##RigidBodyGrounded" + std::to_string(ID), "Is Grounded", isGroundedString);
			FL::PopTable();
		}								

		FL::MoveScreenCursor(0, 5);
		if (FL::RenderCheckbox(" Is Static", b_isStatic))
		{
			rigidBody->SetIsStatic(b_isStatic);
		}
		if (FL::RenderCheckbox(" Allow Torques", b_allowTorques))
		{
			rigidBody->SetTorquesAllowed(b_allowTorques);
		}
	}

	void RenderTileMapComponent(TileMap* tileMap)
	{
		long ID = tileMap->GetID();
		bool b_isActive = tileMap->IsActive();
		int width = tileMap->GetWidth();
		int height = tileMap->GetHeight();
		int tileWidth = tileMap->GetTileWidth();
		int tileHeight = tileMap->GetTileHeight();
		int renderOrder = tileMap->GetRenderOrder();
		std::vector<std::string> tileSets = tileMap->GetTileSets();

		if (RenderIsActiveCheckbox(b_isActive))
		{
			tileMap->SetActive(b_isActive);
		}

		if (FL::PushTable("##tileMapProps" + std::to_string(ID), 2))
		{
			if (FL::RenderIntDragTableRow("##Width" + std::to_string(ID), "Width", width, 1, 1, INT_MAX))
			{
				tileMap->SetWidth(width);
				//tileMap->CalculateColliderValues();
			}
			if (FL::RenderIntDragTableRow("##Height" + std::to_string(ID), "Height", height, 1, 1, INT_MAX))
			{
				tileMap->SetHeight(height);
				//tileMap->RecalcCollisionAreaValues();
			}
			//if (FL::RenderIntDragTableRow("##TileWidth" + std::to_string(ID), "Tile Width", tileWidth, 1, 1, INT_MAX))
			//{
			//	tileMap->SetTileWidth(tileWidth);
			//	//tileMap->RecalcCollisionAreaValues();
			//}
			//if (FL::RenderIntDragTableRow("##TileHeight" + std::to_string(ID), "Tile Height", tileHeight, 1, 1, INT_MAX))
			//{
			//	tileMap->SetTileHeight(tileHeight);
			//	//tileMap->RecalcCollisionAreaValues();
			//}
			if (FL::RenderIntDragTableRow("##RenderOrder" + std::to_string(ID), "Render Order", renderOrder, 1, 0, FL::F_maxSpriteLayers))
			{
				tileMap->SetRenderOrder(renderOrder);
			}
			FL::PopTable();
		}

		FL::RenderSeparator(4, 4);

		FL::RenderSubTitle("Add TileSets");

		static int currentSelectableTileSet = 0;
		static int currentSelectableCollisionArea = 0;
		std::string activeTileSet = tileMap->GetSelectedTileSet();
		std::vector<std::string> tileSetNames;

		// Collect TileSets not already in this TileMap that are available to add to it
		for (int i = 0; i < FL::F_TileSets.size(); i++)
		{
			bool b_alreadyInTileMap = false;

			for (int j = 0; j < tileSets.size(); j++)
			{
				if (FL::F_TileSets[i].GetName() == tileSets[j])
				{
					b_alreadyInTileMap = true;
				}
			}

			if (!b_alreadyInTileMap)
			{
				tileSetNames.push_back(FL::F_TileSets[i].GetName());
			}
		}
		

		FL::RenderSelectable("##SelectTileSet", tileSetNames, currentSelectableTileSet);
		ImGui::BeginDisabled(tileSetNames.size() == 0);
		if (FL::RenderButton("Add to Palettes", Vector2(120, 20)))
		{
			if (tileSetNames.size() >= currentSelectableTileSet + 1)
			{
				tileMap->AddTileSet(tileSetNames[currentSelectableTileSet]);
			}
		}
		ImGui::EndDisabled();
	

		FL::RenderSeparator(4, 4);

		FL::RenderSubTitle("Tile Palettes");

		if (tileSets.size() > 0)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("innerWindow"));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
			FL::PushMenuStyles();
			if (ImGui::BeginTable("##TileSetsTable", 1, FL::F_tableFlags))
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
					ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, tileSetName.c_str());
					if (ImGui::IsItemClicked())
					{
						tileMap->SetSelectedTileSet(tileSetName);
					}
				}

				ImGui::EndTable();
			}
			FL::PopMenuStyles();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::TextWrapped("No TileSets attached to this TileMap.");
			FL::MoveScreenCursor(0, 3);
		}

		Vector2 tileSetTilesStart = ImGui::GetCursorScreenPos();
		Vector2 regionAvailable = ImGui::GetContentRegionMax();

		// Render Tiles within selected TileSet
		TileSet* selectedTileSet = FL::GetTileSet(activeTileSet);
		if (selectedTileSet != nullptr)
		{
			// Tile icon size slider
			FL::MoveScreenCursor(10, 10);
			ImGui::Text("Icon Size:");			
			static float iconSize = 50;
			FL::MoveScreenCursor(10, 2);
			FL::RenderSliderFloat("##TileMapIconSizeSlider", iconSize, 0.5f, 16, 128, ImGui::GetContentRegionAvail().x - 10, 0);
			FL::RenderSeparator(3, 3);

			// Render Palette Tile Icons
			float availableWidth = ImGui::GetWindowSize().x;
			int iconsThisRow = 0;
			std::map<int, std::pair<Vector2, Vector2>> allTiles;

			allTiles = selectedTileSet->GetTileSet();

			float horizontalSpacing = 5;
			float verticalSpacing = 2;
			int maxIconsPerRow = (int)(((regionAvailable.x - 20) / (iconSize + (horizontalSpacing))) - 1);

			for (int index : selectedTileSet->GetTileSetIndices())
			{
				// Drawing the first button in the row
				if (iconsThisRow == 0)
				{
					FL::MoveScreenCursor(horizontalSpacing + 5, verticalSpacing);
				}
					
				if (selectedTileSet->GetTexture() != nullptr)
				{
					Vector2 currentPos = ImGui::GetCursorScreenPos();
					SDL_Texture* texture = selectedTileSet->GetTexture()->GetTexture();
					int textureWidth = selectedTileSet->GetTexture()->GetWidth();
					int textureHeight = selectedTileSet->GetTexture()->GetHeight();
					Vector2 uvStart;
					Vector2 uvEnd;

					if (allTiles.count(index) > 0)
					{
						uvStart = allTiles.at(index).first;
						uvEnd = allTiles.at(index).second;
						uvStart = Vector2(uvStart.x / textureWidth, uvStart.y / textureHeight);
						uvEnd = Vector2(uvEnd.x / textureWidth, uvEnd.y / textureHeight);

						std::string tileButtonID = "##TileSelect" + std::to_string(index);
						if (FL::RenderImageButton(tileButtonID, texture, Vector2(iconSize, iconSize), 0, FL::GetColor("imageButton"), FL::GetColor("imageButtonTint"), FL::GetColor("tileSetIconHovered"), FL::GetColor("imageButtonActive"), uvStart, uvEnd))
						{
							std::pair<std::string, int> tileBrushPair = { selectedTileSet->GetName(), index };
							FL::F_tileSetAndIndexOnBrush = tileBrushPair;
							FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_BRUSH;
						}

						if (iconsThisRow < maxIconsPerRow)
						{
							ImGui::SetCursorScreenPos(Vector2(currentPos.x + iconSize + horizontalSpacing, currentPos.y)); // Ready to draw the next button
							iconsThisRow++;
						}
						else
						{
							iconsThisRow = 0;
						}
					}
				}
			}
			FL::MoveScreenCursor(0, iconSize + verticalSpacing + 10);
			ImGui::GetWindowDrawList()->AddRect(tileSetTilesStart, Vector2(tileSetTilesStart.x + regionAvailable.x - 5, ImGui::GetCursorScreenPos().y), FL::GetColor32("componentSectionBorder"), 0, 0, 1);
		}
		
		ImGui::SetCursorScreenPos(Vector2(tileSetTilesStart.x, ImGui::GetCursorScreenPos().y + 2));
		FL::RenderSeparator(6, 3);



		FL::RenderSubTitle("Collision Areas");

		std::map<std::string, std::vector<FL::CollisionAreaData>> &collisionAreas = tileMap->GetCollisionAreas();
		static std::string selectedCollisionArea = "";
		static int currentSelectedColArea = 0;
		static std::string collisionAreaLabel = "";
		std::vector<std::string> areaNames;
		for (std::pair<std::string, std::vector<FL::CollisionAreaData>> collisionArea : collisionAreas)
		{
			areaNames.push_back(collisionArea.first);
		}

		// Create new Collision Area
		ImGui::TextWrapped("New collision area name:");		
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 48, 0);
		
		FL::MoveScreenCursor(0, -3);
		ImGui::BeginDisabled(collisionAreaLabel == "");
		if (FL::RenderButton("Create"))
		{
			if (!tileMap->ContainsCollisionAreaLabel(collisionAreaLabel))
			{
				std::vector<FL::CollisionAreaData> newData = std::vector<FL::CollisionAreaData>();
				collisionAreas.emplace(collisionAreaLabel, newData);
				collisionAreaLabel = "";
			}
			else
			{
				FL::LogError("Collision area label already being used in this TileMap, please choose a different one.");
			}
		}
		ImGui::EndDisabled();

		FL::RenderInput("##CollisionAreaLabel" + std::to_string(ID), "", collisionAreaLabel, false);


		FL::RenderSeparator(3, 3);


		if (areaNames.size() > 0)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("innerWindow"));
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
			FL::PushMenuStyles();
			if (ImGui::BeginTable("#CollisionAreasTable", 1, FL::F_tableFlags))
			{
				ImGui::TableSetupColumn("##CollisionArea", 0, ImGui::GetContentRegionAvail().x);

				for (std::string areaName : areaNames)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					ImGuiTreeNodeFlags nodeFlags;
					std::string treeID = "##SelectActiveTileSetTree";

					// If node selected
					if (selectedCollisionArea == areaName)
					{
						nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
					}
					else
					{
						nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
					}

					// render a leaf
					ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), nodeFlags, areaName.c_str());
					if (ImGui::IsItemClicked())
					{
						tileMap->SetSelectedCollisionArea(areaName);
						selectedCollisionArea = areaName;
					}
				}

				ImGui::EndTable();
			}
			FL::PopMenuStyles();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}

		if (selectedCollisionArea != "")
		{
			if (FL::RenderButton("Draw Collision Areas"))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
				FG_collisionAreasBuffer.clear();
			}
			ImGui::SameLine(0, 5);
			ImGui::BeginDisabled(FG_collisionAreasBuffer.size() == 0);
			if (FL::RenderButton("Generate Colliders"))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
				tileMap->SetCollisionAreaValues(selectedCollisionArea, FG_collisionAreasBuffer);
				FG_collisionAreasBuffer.clear();
			}
			ImGui::EndDisabled();
		}

		if (areaNames.size() > 0)
		{			
			FL::MoveScreenCursor(0, 5);
		}
	}

	void BeginToolTip(std::string title)
	{		
		ImGui::BeginTooltip();
		ImGui::Text(title.c_str());
		FL::RenderSeparator(5, 5);
	}

	void EndToolTip()
	{		
		ImGui::EndTooltip();
	}

	void RenderToolTipText(std::string label, std::string text)
	{
		std::string newLabel = label + "  |  ";
		ImGui::Text(newLabel.c_str());
		ImGui::SameLine();
		ImGui::Text(text.c_str());
		FL::RenderSeparator(5, 5);
	}

	void RenderToolTipFloat(std::string label, float data)
	{
		std::string newLabel = label + "  |  ";
		ImGui::Text(newLabel.c_str());
		ImGui::SameLine();
		ImGui::Text(std::to_string(data).c_str());
		FL::RenderSeparator(5, 5);
	}

	void RenderToolTipLong(std::string label, long data)
	{
		std::string newLabel = label + "  |  ";
		ImGui::Text(newLabel.c_str());
		ImGui::SameLine();
		ImGui::Text(std::to_string(data).c_str());
		FL::RenderSeparator(5, 5);
	}

	void RenderToolTipLongVector(std::string label, std::vector<long> data)
	{
		std::string newLabel = label + "  |  ";
		ImGui::Text(newLabel.c_str());
		for (int i = 0; i < data.size(); i++)
		{
			std::string dataString = std::to_string(data[i]);
			if (i < data.size() - 1)
				dataString += ",";
			ImGui::SameLine();
			ImGui::Text(dataString.c_str());
		}
		FL::MoveScreenCursor(0, 5);
	}
}