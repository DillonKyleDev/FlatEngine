#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameObject.h"
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
#include "Physics.h"
#include "Body.h"
#include "Shape.h"
#include "Box.h"
#include "Circle.h"
#include "Capsule.h"
#include "Polygon.h"
#include "Chain.h"
#include "Project.h"
#include "TileMap.h"
#include "TileSet.h"
#include "Joint.h"
#include "DistanceJoint.h"
#include "PrismaticJoint.h"
#include "RevoluteJoint.h"
#include "MouseJoint.h"
#include "MotorJoint.h"
#include "WheelJoint.h"
#include "WeldJoint.h"
#include "JointMaker.h"

#include "box2D.h"
#include "imgui.h"
#include "imgui_internal.h"

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
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1);
		ImGui::SetNextWindowScroll(Vector2(0, 0));
		ImGui::BeginChild(componentID.c_str(), Vector2(0, 0), FL::F_autoResizeChildFlags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		// {
		
			// Border around each component
			Vector2 windowPos = ImGui::GetWindowPos();
			Vector2 windowSize = ImGui::GetWindowSize();

			// Tooltip for Component meta data
			FL::RenderInvisibleButton("ComponentToolTipButton-" + std::to_string(ID), { windowPos.x, windowPos.y }, { windowSize.x, 27 });
			if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt)
			{
				FL::BeginToolTip("Component Data");
				FL::MoveScreenCursor(0, -1);
				FL::RenderSeparator(3, 3);
				FL::MoveScreenCursor(0, -1);
				FL::RenderToolTipLong("ID       ", ID);
				FL::MoveScreenCursor(0, -1);
				FL::RenderToolTipLong("ParentID ", component->GetParentID());
				FL::EndToolTip();
			}
			ImGui::SetCursorScreenPos({ windowPos.x + 5, windowPos.y + 5 });
		
			// Component Name			
			FL::MoveScreenCursor(4, 5);
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

			// Border around Components Section
			ImGui::GetWindowDrawList()->AddRect({ windowPos.x, windowPos.y - 1 }, { windowPos.x + windowSize.x, windowPos.y + windowSize.y + 1 }, FL::GetColor32("componentSectionBorder"), 0);
	}

	void EndComponent(Component* component)
	{
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			if (!component->IsCollapsed())
			{
				ImGui::PopStyleColor();
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
		float rotation = transform->GetRotation();
		if (transform->GetParent()->GetBody() != nullptr)
		{
			position = transform->GetParent()->GetBody()->GetPosition();
			rotation = transform->GetParent()->GetBody()->GetRotation();
		}
		float xPos = position.x;
		float yPos = position.y;
		Vector2 scale = transform->GetScale();
		float xScale = scale.x;
		float yScale = scale.y;
		
		bool b_isActive = transform->IsActive();
		long ID = transform->GetID();

		if (RenderIsActiveCheckbox(b_isActive))
		{
			transform->SetActive(b_isActive);
		}
		
		if (FL::PushTable("##TransformProperties" + std::to_string(ID), 2))
		{
			if (FL::RenderFloatDragTableRow("##xPosition" + std::to_string(ID), "X Position", xPos, 0.1f, -FLT_MAX, FLT_MAX))
			{
				transform->SetPosition(Vector2(xPos, yPos));
			}
			if (FL::RenderFloatDragTableRow("##yPosition" + std::to_string(ID), "Y Position", yPos, 0.1f, -FLT_MAX, FLT_MAX))
			{
				transform->SetPosition(Vector2(xPos, yPos));
			}	
			if (FL::RenderFloatDragTableRow("##rotation" + std::to_string(ID), "Rotation", rotation, 0.5f, -FLT_MAX, FLT_MAX))
			{
				transform->SetRotation(rotation);
			}
			if (FL::RenderFloatDragTableRow("##xScaleDrag" + std::to_string(ID), "X Scale", xScale, 0.1f, 0.001f, 1000))
			{
				if (xScale > 0)
				{
					transform->SetScale(Vector2(xScale, yScale));
				}
			}
			if (FL::RenderFloatDragTableRow("##yScaleDrag" + std::to_string(ID), "Y Scale", yScale, 0.1f, 0.001f, 1000))
			{
				if (yScale > 0)
				{
					transform->SetScale(Vector2(xScale, yScale));
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
			}
			if (FL::RenderFloatDragTableRow("##ySpriteScaleDrag" + std::to_string(ID), "Y Scale", yScale, 0.1f, 0.001f, 1000))
			{
				if (yScale > 0)
				{
					sprite->SetScale(Vector2(xScale, yScale));
				}
			}
			if (FL::RenderFloatDragTableRow("##xSpriteOffsetDrag" + std::to_string(ID), "X Offset", xOffset, 0.1f, -FLT_MAX, FLT_MAX))
			{
				sprite->SetOffset(Vector2(xOffset, yOffset));
			}
			if (FL::RenderFloatDragTableRow("##ySpriteOffsetDrag" + std::to_string(ID), "Y Offset", yOffset, 0.1f, -FLT_MAX, FLT_MAX))
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
		if (FL::RenderImageButton("##" + componentType + "PivotTopLeftButton", FL::GetTexture("upLeft"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotTopLeft;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotTopButton", FL::GetTexture("up"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotTop;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotTopRightButton", FL::GetTexture("upRight"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotTopRight;
			b_pivotChanged = true;
		}

		// Left, Center, Right
		FL::MoveScreenCursor(5, 0);
		if (FL::RenderImageButton("##" + componentType + "PivotLeftButton", FL::GetTexture("left"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotLeft;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotCenterButton", FL::GetTexture("center"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotCenter;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotRightButton", FL::GetTexture("right"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotRight;
			b_pivotChanged = true;
		}

		// BottomLeft, Bottom, BottomRight		
		FL::MoveScreenCursor(5, 0);
		if (FL::RenderImageButton("##" + componentType + "PivotBottomLeftButton", FL::GetTexture("downLeft"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotBottomLeft;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotBottomButton", FL::GetTexture("down"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
		{
			pivot = FL::Pivot::PivotBottom;
			b_pivotChanged = true;
		}
		ImGui::SameLine(0, 3);
		if (FL::RenderImageButton("##" + componentType + "PivotBottomRightButton", FL::GetTexture("downRight"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("imageButtonDark"), FL::GetColor("imageButtonTint"), FL::GetColor("pivotButtonHovered")))
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
		std::map<std::string, Script::S_ScriptParam> &scriptParams = script->GetScriptParams();
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

		if (currentScript < allScriptNames.size() && currentScript != 0)
		{
			// Set the values for a new parameter
			static std::string newParamName = "";
			int currentNewParamType = 0;
			std::vector<std::string> types = { "string", "int", "float", "double", "long", "bool", "Vector2" };
			static  Script::S_ScriptParam newParam = Script::S_ScriptParam();

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
			for (std::map<std::string, Script::S_ScriptParam>::iterator paramIter = scriptParams.begin(); paramIter != scriptParams.end(); paramIter++)
			{
				int currentParamType = 0;
				Script::S_ScriptParam& param = paramIter->second;

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
		bool b_leftClick = button->GetLeftClick();
		bool b_rightClick = button->GetRightClick();
		std::shared_ptr<Animation::S_Event> functionParams = button->GetFunctionParams();
		long ID = button->GetID();
		std::string functionName = functionParams->functionName;
		bool b_cppEvent = functionParams->b_cppEvent;
		bool b_luaEvent = functionParams->b_luaEvent;

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
			if (FL::RenderFloatDragTableRow("##activeoffsetx" + std::to_string(ID), "X Offset", activeOffset.x, 0.1f, -FLT_MAX, FLT_MAX))
			{
				button->SetActiveOffset(activeOffset);
			}
			if (FL::RenderFloatDragTableRow("##activeoffsety" + std::to_string(ID), "Y Offset", activeOffset.y, 0.1f, -FLT_MAX, FLT_MAX))
			{
				button->SetActiveOffset(activeOffset);
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

			std::string choices[2] = { "C++", "Lua" };
			std::string currentChoice = "";

			if (b_cppEvent)
			{
				currentChoice = "C++";
			}
			else if (b_luaEvent)
			{
				currentChoice = "Lua";
			}

			std::string cppRadioID = "C++ Function##" + std::to_string(ID);
			std::string luaRadioID = "Lua Function##" + std::to_string(ID);

			if (ImGui::RadioButton(cppRadioID.c_str(), currentChoice == choices[0]))
			{
				currentChoice = choices[0];
				functionParams->b_cppEvent = true;
				functionParams->b_luaEvent = false;
			}
			if (ImGui::RadioButton(luaRadioID.c_str(), currentChoice == choices[1]))
			{
				currentChoice = choices[1];
				functionParams->b_cppEvent = false;
				functionParams->b_luaEvent = true;
			}

			FL::RenderSeparator(1, 1);

			if (functionParams->b_cppEvent)
			{
				int currentEventFunction = 0;
				std::vector<std::string> eventFunctions = { "- none -" };

				for (std::map<std::string, void (*)(GameObject*, Animation::S_EventFunctionParam)>::iterator iter = FL::F_CPPAnimationEventFunctions.begin(); iter != FL::F_CPPAnimationEventFunctions.end(); iter++)
				{
					eventFunctions.push_back(iter->first);
				}

				for (int i = 0; i < eventFunctions.size(); i++)
				{
					if (functionParams->functionName == eventFunctions[i])
					{
						currentEventFunction = i;
					}
				}

				if (eventFunctions.size())
				{
					FL::MoveScreenCursor(0, 3);
					ImGui::Text("Callback Function:");
					ImGui::SameLine();
					FL::MoveScreenCursor(0, -3);
					std::string comboID = "##EventFunctionName";
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (FL::RenderCombo(comboID, eventFunctions[currentEventFunction], eventFunctions, currentEventFunction))
					{
						functionParams->functionName = eventFunctions[currentEventFunction];
					}		
					FL::MoveScreenCursor(0, -6);
				}
				else
				{
					ImGui::TextWrapped("Add C++ callback functions using AddCPPAnimationEventFunction() in attached C++ script.");
				}
			}

			if (functionParams->b_luaEvent)
			{
				if (FL::RenderInput("##ButtonEventName", "Callback Function", functionName))
				{
					functionParams->functionName = functionName;
				}
			}

			std::string stringValue = functionParams->parameters.e_string;
			int intValue = functionParams->parameters.e_int;
			float floatValue = functionParams->parameters.e_float;
			int longValue = (int)functionParams->parameters.e_long;
			bool b_boolean = functionParams->parameters.e_boolean;
			Vector2 vector2 = functionParams->parameters.e_Vector2;

			if (FL::PushTable("##ButtonEventParameters", 2))
			{
				if (FL::RenderInputTableRow("##ButtonEventParamString" + std::to_string(ID), "String", stringValue, false))
				{
					functionParams->parameters.e_string = stringValue;
				}
				if (FL::RenderIntDragTableRow("##ButtonEventParamInt" + std::to_string(ID), "Int", intValue, 1, -INT_MAX, INT_MAX))
				{
					functionParams->parameters.e_int = intValue;
				}
				if (FL::RenderIntDragTableRow("##ButtonEventParamLong" + std::to_string(ID), "Long", longValue, 1, -LONG_MAX, LONG_MAX))
				{
					functionParams->parameters.e_int = intValue;
				}
				if (FL::RenderFloatDragTableRow("##ButtonEventParamFloat" + std::to_string(ID), "Float", floatValue, 0.001f, -FLT_MAX, FLT_MAX))
				{
					functionParams->parameters.e_float = floatValue;
				}
				if (FL::RenderFloatDragTableRow("##ButtonEventParamVector2X" + std::to_string(ID), "Vector2 X", vector2.x, 0.001f, -FLT_MAX, FLT_MAX))
				{
					functionParams->parameters.e_Vector2 = vector2;
				}
				if (FL::RenderFloatDragTableRow("##ButtonEventParamVector2Y" + std::to_string(ID), "Vector2 Y", vector2.y, 0.001f, -FLT_MAX, FLT_MAX))
				{
					functionParams->parameters.e_Vector2 = vector2;
				}
				if (FL::RenderCheckboxTableRow("##ButtonEventParamBoolean" + std::to_string(ID), "Boolean", b_boolean))
				{
					functionParams->parameters.e_boolean = b_boolean;
				}
				FL::PopTable();
			}

			FL::MoveScreenCursor(0, 3);								
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
			if (FL::RenderFloatDragTableRow("##xTextOffset" + std::to_string(ID), "X offset", xOffset, 0.1f, -FLT_MAX, FLT_MAX))
			{
				text->SetOffset(Vector2(xOffset, yOffset));
			}
			if (FL::RenderFloatDragTableRow("##yTextOffset" + std::to_string(ID), "Y offset", yOffset, 0.1f, -FLT_MAX, FLT_MAX))
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

	void RenderBodyComponent(Body* body)
	{		
		bool b_isActive = body->IsActive();
		FL::Physics::BodyProps bodyProps = body->GetBodyProps();
		long ID = body->GetID();
		Vector2 linearVelocity = body->GetLinearVelocity();
		float angularVelocity = body->GetAngularVelocity();

		if (RenderIsActiveCheckbox(b_isActive))
		{
			body->SetActive(b_isActive);
		}

		int currentType = bodyProps.type;
		std::vector<std::string> types = { "static", "kinematic", "dynamic" };
		std::string comboID = "##BoxBodyTypeCombo";
		if (FL::RenderCombo(comboID, types[bodyProps.type], types, currentType, 100))
		{
			body->SetBodyType((b2BodyType)currentType);
		}

		if (FL::PushTable("##BodyProps" + std::to_string(ID), 2))
		{
			if (FL::RenderFloatDragTableRow("##BodyGravityScale" + std::to_string(ID), "Gravity Scale", bodyProps.gravityScale, 0.01f, -FLT_MAX, FLT_MAX))
			{
				body->SetGravityScale(bodyProps.gravityScale);
			}
			if (FL::RenderFloatDragTableRow("##BodyLinearDamping" + std::to_string(ID), "Linear Damping", bodyProps.linearDamping, 0.001f, 0.0f, -FLT_MAX))
			{
				body->SetLinearDamping(bodyProps.linearDamping);
			}
			if (FL::RenderFloatDragTableRow("##BodyAngularDamping" + std::to_string(ID), "Angular Damping", bodyProps.angularDamping, 0.001f, 0.0f, -FLT_MAX))
			{
				body->SetAngularDamping(bodyProps.angularDamping);
			}

			FL::RenderTextTableRow("##VelocityX" + std::to_string(ID), "X Velocity", std::to_string(linearVelocity.x));
			FL::RenderTextTableRow("##VelocityY" + std::to_string(ID), "Y Velocity", std::to_string(linearVelocity.y));
			FL::RenderTextTableRow("##AngularVelocity" + std::to_string(ID), "Angular Velocity (deg)", std::to_string(angularVelocity));
			FL::PopTable();
		}

		FL::MoveScreenCursor(0, 3);

		if (FL::RenderCheckbox(" Lock Rotation", bodyProps.b_lockedRotation))
		{
			body->SetLockedRotation(bodyProps.b_lockedRotation);
		}
		if (FL::RenderCheckbox(" Lock x-axis", bodyProps.b_lockedXAxis))
		{
			body->SetLockedXAxis(bodyProps.b_lockedXAxis);
		}
		if (FL::RenderCheckbox(" Lock y-axis", bodyProps.b_lockedYAxis))
		{
			body->SetLockedYAxis(bodyProps.b_lockedYAxis);
		}

		FL::MoveScreenCursor(0, 3);

		FL::RenderButton("Add Shape", Vector2(ImGui::GetContentRegionAvail().x, 0));
		if (ImGui::BeginPopupContextItem("##AddShape", ImGuiPopupFlags_MouseButtonLeft))
		{
			FL::PushMenuStyles();

			if (ImGui::MenuItem("Box"))
			{
				body->AddBox();
				ImGui::CloseCurrentPopup();
			}
			
			if (ImGui::MenuItem("Circle"))
			{
				body->AddCircle();
				ImGui::CloseCurrentPopup();
			}
		
			if (ImGui::MenuItem("Capsule"))
			{
				body->AddCapsule();
				ImGui::CloseCurrentPopup();
			}
			
			if (ImGui::MenuItem("Polygon"))
			{
				body->AddPolygon();
				ImGui::CloseCurrentPopup();
			}				

			if (ImGui::MenuItem("Chain"))
			{
				body->AddChain();
				ImGui::CloseCurrentPopup();
			}			

			FL::PopMenuStyles();
			ImGui::EndMenu();
		}

		if (body->GetShapes().size() > 0)
		{
			FL::MoveScreenCursor(0, 3);
			ImGui::Text("Body Shapes");
			FL::MoveScreenCursor(0, 2);
			ImGui::Separator();
			FL::MoveScreenCursor(0, -3);

			// For scrolling shapes section with background
			std::string childID = "Shapes_" + std::to_string(ID);
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("shapesScrollingBg"));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 5));
			ImGui::BeginChild(childID.c_str(), Vector2(0, 300), FL::F_childFlags);
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();

			// Border around components section
			auto wPos = ImGui::GetWindowPos();
			auto wSize = ImGui::GetWindowSize();
			b2ShapeId shapeToDelete = b2_nullShapeId;
			b2ChainId chainToDelete = b2_nullChainId;

			std::vector<Shape*> shapes = body->GetShapes();

			for (int i = 0; i < shapes.size(); i++)
			{
				RenderShapeComponentProps(shapes[i], shapeToDelete, chainToDelete);
				
				if (i != shapes.size() - 1)
				{
					FL::MoveScreenCursor(0, 3);
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


			ImGui::EndChild();

			// Border around Shapes Section
			ImGui::GetWindowDrawList()->AddRect({wPos.x, wPos.y - 1}, {wPos.x + wSize.x, wPos.y + wSize.y + 1}, FL::GetColor32("componentSectionBorder"), 0);
		}
	}
	
	void RenderShapeComponentProps(Shape* shape, b2ShapeId& shapeToDelete, b2ChainId& chainToDelete)
	{
		Shape::ShapeProps shapeProps = shape->GetShapeProps();
		b2ShapeId shapeID = shape->GetShapeID();
		b2ChainId chainID = shape->GetChainID();
		Shape::ShapeType shapeType = shape->GetShapeType();
		std::string ID = "";
		if (shapeType != Shape::ShapeType::BS_Chain)
		{
			ID = "shape_" + std::to_string(shapeID.index1) + "_" + std::to_string(shapeID.world0);
		}
		else
		{
			ID = "chain_" + std::to_string(chainID.index1) + "_" + std::to_string(chainID.world0);
		}
		std::string shapeString = shape->GetShapeString() + " ID: " + ID;
		bool b_isSensor = shapeProps.b_isSensor;
		float restitution = shapeProps.restitution;
		float friction = shapeProps.friction;
		float density = shapeProps.density;
		Vector2 dimensions = shapeProps.dimensions;
		float cornerRadius = shapeProps.cornerRadius;
		float radius = shapeProps.radius;
		float capsuleLength = shapeProps.capsuleLength;
		bool b_horizontal = shapeProps.b_horizontal;
		std::vector<Vector2> points = shapeProps.points;
		int pointCount = (int)points.size();
		bool b_showPoints = shape->ShowPoints();
		bool b_editingPoints = shape->IsEditingPoints();
		bool b_isLoop = shapeProps.b_isLoop;
		float tangentSpeed = shapeProps.tangentSpeed;
		float rollingResistance = shapeProps.rollingResistance;
		bool b_enableSensorEvents = shapeProps.b_enableSensorEvents;
		bool b_enableContactEvents = shapeProps.b_enableContactEvents;

		std::string childID = "Shape_" + ID;
		ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("shapeBg"));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 3));
		ImGui::BeginChild(childID.c_str(), Vector2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize);
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		// Border around each shape
		auto shapeWindowPos = ImGui::GetWindowPos();
		auto shapeWindowSize = ImGui::GetWindowSize();	

		FL::RenderSectionHeader(shapeString);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20, 0);
		FL::MoveScreenCursor(0, -3);

		std::string trashcanID = "##trashIcon-" + ID;

		if (FL::RenderImageButton(trashcanID.c_str(), FL::GetTexture("trash")))
		{
			if (shapeType != Shape::ShapeType::BS_Chain)
			{
				shapeToDelete = shapeID;
			}
			else
			{
				chainToDelete = chainID;
			}
		}			

		if (FL::PushTable("##" + shapeString + "ShapeProps" + ID, 2))
		{
			if (FL::RenderFloatDragTableRow("##" + shapeString + "Density" + ID, "Density", density, 0.001f, 0.001f, FLT_MAX))
			{
				shape->SetDensity(density);
			}
			if (FL::RenderFloatDragTableRow("##" + shapeString + "Friction" + ID, "Friction", friction, 0.001f, 0.0f, FLT_MAX))
			{
				shape->SetFriction(friction);
			}
			if (FL::RenderFloatDragTableRow("##" + shapeString + "Restitution" + ID, "Restitution", restitution, 0.001f, 0.0f, FLT_MAX))
			{
				shape->SetRestitution(restitution);
			}
			if (shapeType != Shape::ShapeType::BS_Polygon && shapeType != Shape::ShapeType::BS_Chain && shapeType)
			{
				Vector2 positionOffset = shapeProps.positionOffset;
				float rotationOffset = FL::RadiansToDegrees(b2Rot_GetAngle(shapeProps.rotationOffset));				

				if (FL::RenderFloatDragTableRow("##PositionXOffset" + ID, "X Offset", positionOffset.x, 0.01f, -FLT_MAX, FLT_MAX))
				{
					shape->SetPositionOffset(positionOffset);
				}
				if (FL::RenderFloatDragTableRow("##PositionYOffset" + ID, "Y Offset", positionOffset.y, 0.01f, -FLT_MAX, FLT_MAX))
				{
					shape->SetPositionOffset(positionOffset);
				}
				if (FL::RenderFloatDragTableRow("##RotationOffset" + ID, "Rotation Offset", rotationOffset, 0.5f, -FLT_MAX, FLT_MAX))
				{
					shape->SetRotationOffset(rotationOffset);
				}
			}
			if (shapeType == Shape::ShapeType::BS_Box)
			{
				if (FL::RenderFloatDragTableRow("##BoxWidth" + ID, "Width", dimensions.x, 0.01f, 0.01f, FLT_MAX))
				{
					static_cast<Box*>(shape)->SetDimensions(dimensions);
				}
				if (FL::RenderFloatDragTableRow("##BoxHeight" + ID, "Height", dimensions.y, 0.01f, 0.01f, FLT_MAX))
				{
					static_cast<Box*>(shape)->SetDimensions(dimensions);
				}
				if (FL::RenderFloatDragTableRow("##ShapeCornerRadius" + ID, "Corner Radius", cornerRadius, 0.001f, 0.0f, FLT_MAX))
				{
					if (cornerRadius >= 0)
					{
						static_cast<Box*>(shape)->SetCornerRadius(cornerRadius);
					}
					else
					{
						cornerRadius = 0;
					}
				}
			}
			else if (shapeType == Shape::ShapeType::BS_Polygon)
			{
				if (FL::RenderFloatDragTableRow("##ShapeCornerRadius" + ID, "Corner Radius", cornerRadius, 0.001f, 0.0f, FLT_MAX))
				{
					if (cornerRadius >= 0)
					{
						static_cast<FL::Polygon*>(shape)->SetCornerRadius(cornerRadius);
					}
					else
					{
						cornerRadius = 0;
					}
				}
			}
			if (shapeType == Shape::ShapeType::BS_Circle)
			{
				if (FL::RenderFloatDragTableRow("##ShapeRadius" + ID, "Radius", radius, 0.01f, 0.01f, FLT_MAX))
				{
					static_cast<Circle*>(shape)->SetRadius(radius);
				}
			}
			if (shapeType == Shape::ShapeType::BS_Capsule)
			{
				if (FL::RenderFloatDragTableRow("##ShapeRadius" + ID, "Radius", radius, 0.01f, 0.01f, FLT_MAX))
				{
					static_cast<Capsule*>(shape)->SetRadius(radius);
				}
				if (FL::RenderFloatDragTableRow("##CapsuleLength" + ID, "Length", capsuleLength, 0.01f, 0.01f, FLT_MAX))
				{
					static_cast<Capsule*>(shape)->SetCapsuleLength(capsuleLength);
				}
				if (FL::RenderFloatDragTableRow("##CapsuleHeight" + ID, "Radii", radius, 0.01f, 0.01f, FLT_MAX))
				{
					static_cast<Capsule*>(shape)->SetRadius(radius);
				}
			}
			if (shapeType == Shape::ShapeType::BS_Chain)
			{
				if (FL::RenderFloatDragTableRow("##ChainTangentSpeed" + ID, "Tangent Speed", tangentSpeed, 0.001f, -FLT_MAX, FLT_MAX))
				{
					static_cast<Chain*>(shape)->SetTangentSpeed(tangentSpeed);
				}
				if (FL::RenderFloatDragTableRow("##ChainRollingResistance" + ID, "Rolling Resistance", rollingResistance, 0.001f, -FLT_MAX, FLT_MAX))
				{
					static_cast<Chain*>(shape)->SetRollingResistance(rollingResistance);
				}
			}
			FL::PopTable();
		}

		FL::MoveScreenCursor(0, 3);
		if (shapeType != Shape::ShapeType::BS_Chain)
		{
			if (FL::RenderCheckbox(" Is Sensor##Chain_" + ID, b_isSensor))
			{
				shape->SetIsSensor(b_isSensor);
			}
		}
		if (FL::RenderCheckbox(" Enable Sensor Events##Chain_" + ID, b_enableSensorEvents))
		{
			shape->SetEnableSensorEvents(b_enableSensorEvents);
		}
		if (FL::RenderCheckbox(" Enable Contact Events##_Chain" + ID, b_enableContactEvents))
		{
			shape->SetEnableContactEvents(b_enableContactEvents);
		}

		if (shapeType == Shape::ShapeType::BS_Capsule)
		{
			if (FL::RenderCheckbox(" Horizontal##Capsule_" + ID, b_horizontal))
			{
				static_cast<Capsule*>(shape)->SetHorizontal(b_horizontal);
			}
		}

		if (shapeType == Shape::ShapeType::BS_Chain)
		{			
			if (FL::RenderCheckbox(" Loop endpoints##Chain_" + ID, b_isLoop))
			{
				static_cast<Chain*>(shape)->SetIsLoop(b_isLoop);
			}
			
			FL::MoveScreenCursor(0, 3);
			ImGui::Separator();
			FL::MoveScreenCursor(0, 3);

			if (!b_editingPoints)
			{
				if (FL::RenderButton("Edit Points##Chain_" + ID))
				{
					shape->SetEditingPoints(true);
				}
			}
			else
			{
				if (FL::RenderButton("Stop editing##Chain_" + ID))
				{
					shape->SetEditingPoints(false);
				}
			}

			ImGui::SameLine();
			FL::MoveScreenCursor(0, 3);
			if (FL::RenderCheckbox(" Show points##Chain_" + ID, b_showPoints))
			{
				shape->SetShowPoints(b_showPoints);
			}

			if (b_showPoints)
			{
				FL::MoveScreenCursor(0, 3);
				if (FL::PushTable("##ShapePointPositions" + ID, 2))
				{
					for (int i = 0; i < pointCount; i++)
					{
						if (FL::RenderFloatDragTableRow("##ShapePointXPos" + ID + std::to_string(i), std::to_string(i) + ": X Position", points[i].x, 0.001f, -FLT_MAX, FLT_MAX))
						{
							static_cast<Chain*>(shape)->SetPoints(points);
						}
						if (FL::RenderFloatDragTableRow("##ShapePointYPos" + ID + std::to_string(i), std::to_string(i) + ": Y Position", points[i].y, 0.001f, -FLT_MAX, FLT_MAX))
						{
							static_cast<Chain*>(shape)->SetPoints(points);
						}
					}
					FL::PopTable();
				}
			}
		}
		else if (shapeType == Shape::ShapeType::BS_Polygon)
		{			
			FL::MoveScreenCursor(0, 3);
			ImGui::Separator();
			FL::MoveScreenCursor(0, 3);

			if (!b_editingPoints)
			{
				if (FL::RenderButton("Edit Points##Polygon_" + ID))
				{
					shape->SetEditingPoints(true);
				}
			}
			else
			{
				if (FL::RenderButton("Stop editing##Polygon_" + ID))
				{
					shape->SetEditingPoints(false);
				}
			}

			ImGui::SameLine();	
			FL::MoveScreenCursor(0, 3);
			if (FL::RenderCheckbox(" Show points##Polygon_" + ID, b_showPoints))
			{
				shape->SetShowPoints(b_showPoints);
			}

			if (b_showPoints)
			{
				FL::MoveScreenCursor(0, 3);				
				if (FL::PushTable("##ShapePointPositions" + ID, 2))
				{
					for (int i = 0; i < pointCount; i++)
					{
						if (FL::RenderFloatDragTableRow("##ShapePointXPos" + ID + std::to_string(i), std::to_string(i) + ": X Position", points[i].x, 0.001f, -FLT_MAX, FLT_MAX))
						{
							static_cast<FL::Polygon*>(shape)->SetPoints(points);
						}
						if (FL::RenderFloatDragTableRow("##ShapePointYPos" + ID + std::to_string(i), std::to_string(i) + ": Y Position", points[i].y, 0.001f, -FLT_MAX, FLT_MAX))
						{
							static_cast<FL::Polygon*>(shape)->SetPoints(points);
						}
					}
					FL::PopTable();
				}
			}
		}


		FL::MoveScreenCursor(0, 3);

		ImGui::EndChild();

		ImGui::GetWindowDrawList()->AddRect({ shapeWindowPos.x , shapeWindowPos.y }, { shapeWindowPos.x + shapeWindowSize.x, shapeWindowPos.y + shapeWindowSize.y }, FL::GetColor32("componentBorder"), 0);
	}

	void RenderJointMakerComponent(JointMaker* jointMaker)
	{
		long ID = jointMaker->GetID();
		bool b_isActive = jointMaker->IsActive();
		
		std::vector<Joint*> joints = jointMaker->GetJoints();

		if (RenderIsActiveCheckbox(b_isActive))
		{
			jointMaker->SetActive(b_isActive);
		}

		FL::RenderButton("Add Joint", Vector2(ImGui::GetContentRegionAvail().x, 0));
		if (ImGui::BeginPopupContextItem("##AddJoint", ImGuiPopupFlags_MouseButtonLeft))
		{
			FL::PushMenuStyles();

			if (ImGui::MenuItem("Distance Joint"))
			{				
				jointMaker->AddDistanceJoint();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Prismatic Joint"))
			{
				jointMaker->AddPrismaticJoint();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Revolute Joint"))
			{
				jointMaker->AddRevoluteJoint();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Mouse Joint"))
			{
				jointMaker->AddMouseJoint();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Wheel Joint"))
			{
				jointMaker->AddWheelJoint();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Motor Joint"))
			{
				jointMaker->AddMotorJoint();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Wheel Joint"))
			{
				jointMaker->AddWheelJoint();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Weld Joint"))
			{
				jointMaker->AddWeldJoint();
				ImGui::CloseCurrentPopup();
			}

			FL::PopMenuStyles();
			ImGui::EndMenu();
		}

		if (jointMaker->GetJoints().size() > 0)
		{
			FL::MoveScreenCursor(0, 3);
			ImGui::Text("Joints");
			FL::MoveScreenCursor(0, 2);
			ImGui::Separator();
			FL::MoveScreenCursor(0, -3);

			// For scrolling shapes section with background
			std::string childID = "Joints_" + std::to_string(ID);
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("jointsScrollingBg"));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 5));
			ImGui::BeginChild(childID.c_str(), Vector2(0, 300), FL::F_childFlags);
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();

			// Border around components section
			auto wPos = ImGui::GetWindowPos();
			auto wSize = ImGui::GetWindowSize();
			long jointToDelete = -1;			

			std::vector<Joint*> joints = jointMaker->GetJoints();

			for (int i = 0; i < joints.size(); i++)
			{
				RenderJointComponentProps(joints[i], jointToDelete);

				if (i != joints.size() - 1)
				{
					FL::MoveScreenCursor(0, 3);
				}
			}

			if (jointToDelete != -1)
			{
				jointMaker->RemoveJoint(jointToDelete);
			}


			ImGui::EndChild();

			// Border around Shapes Section
			ImGui::GetWindowDrawList()->AddRect({ wPos.x, wPos.y - 1 }, { wPos.x + wSize.x, wPos.y + wSize.y + 1 }, FL::GetColor32("componentSectionBorder"), 0);
		}

		//if (FL::PushTable("##JointMakerProps" + std::to_string(ID), 2))
		//{
		//	

		//	FL::PopTable();
		//}
	}

	void RenderJointComponentProps(Joint* joint, long& jointToDelete)
	{		
		long jointID = joint->GetJointID();		
		Joint::JointType jointType = joint->GetJointType();
		std::string jointTypeString = joint->GetJointString();
		std::string ID = jointTypeString + " ID: " + std::to_string(jointID);
		Body* bodyA = joint->GetBodyA();
		Body* bodyB = joint->GetBodyB();	
		bool b_collideConnected = joint->CollideConnected();
		Vector2 anchorA = joint->GetAnchorA();
		Vector2 anchorB = joint->GetAnchorB();

		std::string childID = "Joint_" + ID;
		ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("jointBg"));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 3));
		ImGui::BeginChild(childID.c_str(), Vector2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize);
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		// Border around each joint
		auto shapeWindowPos = ImGui::GetWindowPos();
		auto shapeWindowSize = ImGui::GetWindowSize();

		FL::RenderSectionHeader(ID);
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20, 0);
		FL::MoveScreenCursor(0, -3);

		std::string trashcanID = "##trashIcon-" + ID;

		if (FL::RenderImageButton(trashcanID.c_str(), FL::GetTexture("trash")))
		{
			jointToDelete = jointID;
		}

		FL::MoveScreenCursor(0, 3);

		int droppedObjectID = -1;		
		std::string bodyAName = "";
		std::string bodyBName = "";

		if (bodyA != nullptr)
		{
			bodyAName = bodyA->GetParent()->GetName();
		}
		if (bodyB != nullptr)
		{
			bodyBName = bodyB->GetParent()->GetName();
		}

		FL::MoveScreenCursor(0, 3.0f);

		if (FL::DropInput("##InputBodyA" + ID, "BodyA", bodyAName, FL::F_hierarchyTarget, droppedObjectID, "Drag and drop GameObjects from the Hierarchy to assign it's Body component."))
		{
			if (droppedObjectID >= 0)
			{
				joint->SetBodyAID(droppedObjectID);
			}
		}
		if (FL::DropInput("##InputBodyB" + ID, "BodyB", bodyBName, FL::F_hierarchyTarget, droppedObjectID, "Drag and drop GameObjects from the Hierarchy to assign it's Body component."))
		{
			if (droppedObjectID >= 0)
			{
				joint->SetBodyBID(droppedObjectID);
			}
		}

		if (FL::PushTable("##JointProps" + ID, 2))
		{
			if (FL::RenderFloatDragTableRow("##AnchorAX" + ID, "Anchor A x-pos", anchorA.x, 0.1f, -FLT_MAX, FLT_MAX))
			{
				joint->SetAnchorA(anchorA);
			}
			if (FL::RenderFloatDragTableRow("##AnchorAY" + ID, "Anchor A y-pos", anchorA.y, 0.1f, -FLT_MAX, FLT_MAX))
			{
				joint->SetAnchorA(anchorA);
			}
			if (FL::RenderFloatDragTableRow("##AnchorBX" + ID, "Anchor B x-pos", anchorB.x, 0.1f, -FLT_MAX, FLT_MAX))
			{
				joint->SetAnchorB(anchorB);
			}
			if (FL::RenderFloatDragTableRow("##AnchorBY" + ID, "Anchor B y-pos", anchorB.y, 0.1f, -FLT_MAX, FLT_MAX))
			{
				joint->SetAnchorB(anchorB);
			}
			FL::PopTable();
		}

		switch (jointType)
		{
		case Joint::JT_Distance:
			RenderDistanceJointProps(static_cast<DistanceJoint*>(joint));
			break;
		case Joint::JT_Prismatic:
			RenderPrismaticJointProps(static_cast<PrismaticJoint*>(joint));
			break;
		case Joint::JT_Revolute:
			RenderRevoluteJointProps(static_cast<RevoluteJoint*>(joint));
			break;
		case Joint::JT_Mouse:
			RenderMouseJointProps(static_cast<MouseJoint*>(joint));
			break;
		case Joint::JT_Wheel:
			RenderWheelJointProps(static_cast<WheelJoint*>(joint));
			break;
		case Joint::JT_Weld:
			RenderWeldJointProps(static_cast<WeldJoint*>(joint));
			break;
		case Joint::JT_Motor:
			RenderMotorJointProps(static_cast<MotorJoint*>(joint));
			break;
		default:
			break;
		}

		ImGui::EndChild();
	}

	void RenderDistanceJointProps(DistanceJoint* joint)
	{
		long ID = joint->GetJointID();
		DistanceJoint::DistanceJointProps jointProps = joint->GetJointProps();
		float dampingRatio = jointProps.dampingRatio;
		bool b_enableLimit = jointProps.b_enableLimit;
		bool b_enableMotor = jointProps.b_enableMotor;
		bool b_enableSpring = jointProps.b_enableSpring;
		float hertz = jointProps.hertz;
		float minLength = jointProps.minLength;
		float maxLength = jointProps.maxLength;
		float length = jointProps.length;
		float maxMotorForce = jointProps.maxMotorForce;
		float motorSpeed = jointProps.motorSpeed;

		if (FL::RenderCheckbox("Enable Spring##" + ID, b_enableSpring))
		{
			joint->SetEnableSpring(b_enableSpring);
		}
		if (FL::RenderCheckbox("Enable Motor##" + ID, b_enableMotor))
		{
			joint->SetEnableMotor(b_enableMotor);
		}
		if (FL::RenderCheckbox("Enable Limit##" + ID, b_enableLimit))
		{
			joint->SetEnableLimit(b_enableLimit);			
		}

		if (FL::PushTable("##DistanceJointProps" + ID, 2))
		{
			if (FL::RenderFloatDragTableRow("##Length" + ID, "Length", length, 0.1f, 0, FLT_MAX))
			{
				joint->SetLength(length);
			}
			if (FL::RenderFloatDragTableRow("##MinLength" + ID, "Min Length", minLength, 0.1f, 0, FLT_MAX))
			{
				joint->SetLengthRange(minLength, maxLength);
			}
			if (FL::RenderFloatDragTableRow("##MaxLength" + ID, "Max Length", maxLength, 0.1f, 0, FLT_MAX))
			{
				joint->SetLengthRange(minLength, maxLength);
			}

			// Spring
			if (b_enableSpring)
			{
				if (FL::RenderFloatDragTableRow("##DampingRatio" + ID, "Spring Damping Ratio", dampingRatio, 0.1f, 0, FLT_MAX))
				{
					joint->SetSpringDampingRatio(dampingRatio);
				}
				if (FL::RenderFloatDragTableRow("##SpringHertz" + ID, "Spring Hertz", hertz, 0.1f, 0, FLT_MAX))
				{
					joint->SetSpringHertz(hertz);
				}
			}
			// Motor
			if (b_enableMotor)
			{
				if (FL::RenderFloatDragTableRow("##MotorSpeed" + ID, "Motor Speed", motorSpeed, 0.1f, 0, FLT_MAX))
				{
					joint->SetMotorSpeed(motorSpeed);
				}
				if (FL::RenderFloatDragTableRow("##MaxMotorForce" + ID, "Max Motor Force", maxMotorForce, 0.1f, 0, FLT_MAX))
				{
					joint->SetMaxMotorForce(maxMotorForce);
				}
			}
			FL::PopTable();
		}

		FL::MoveScreenCursor(0, 3.0f);
	}

	void RenderPrismaticJointProps(PrismaticJoint* joint)
	{
		long ID = joint->GetJointID();
		PrismaticJoint::PrismaticJointProps jointProps = joint->GetJointProps();
		float dampingRatio = jointProps.dampingRatio;
		bool b_enableLimit = jointProps.b_enableLimit;
		bool b_enableMotor = jointProps.b_enableMotor;
		bool b_enableSpring = jointProps.b_enableSpring;
		float hertz = jointProps.hertz;
		Vector2 localAxisA = jointProps.localAxisA;
		float lowerTranslation = jointProps.lowerTranslation;
		float upperTranslation = jointProps.upperTranslation;
		float maxMotorForce = jointProps.maxMotorForce;
		float motorSpeed = jointProps.motorSpeed;
		float targetTranslation = jointProps.targetTranslation;


		if (FL::RenderCheckbox("Enable Spring##" + ID, b_enableSpring))
		{
			joint->SetEnableSpring(b_enableSpring);
		}
		if (FL::RenderCheckbox("Enable Motor##" + ID, b_enableMotor))
		{
			joint->SetEnableMotor(b_enableMotor);
		}
		if (FL::RenderCheckbox("Enable Limit##" + ID, b_enableLimit))
		{
			joint->SetEnableLimit(b_enableLimit);
		}

		if (FL::PushTable("##DistanceJointProps" + ID, 2))
		{
			// if (b_enableLimits) <-- Probably
			if (FL::RenderFloatDragTableRow("##LowerTranslation" + ID, "Lower Translation", lowerTranslation, 0.1f, -FLT_MAX, FLT_MAX))
			{
				joint->SetTranslationRange(lowerTranslation, upperTranslation);
			}
			if (FL::RenderFloatDragTableRow("##UpperTranslation" + ID, "Upper Translation", upperTranslation, 0.1f, -FLT_MAX, FLT_MAX))
			{
				joint->SetTranslationRange(lowerTranslation, upperTranslation);
			}
			//if (FL::RenderFloatDragTableRow("##TranslationTarget" + ID, "Local Axis X", localAxisA.x, 0.1f, -FLT_MAX, FLT_MAX))
			//{				
			//	//joint->SetLocalAxisA(localAxisA);
			//}
			//if (FL::RenderFloatDragTableRow("##LocalAxisAY" + ID, "Local Axis Y", localAxisA.y, 0.1f, -FLT_MAX, FLT_MAX))
			//{
			//	//joint->SetLocalAxisA(localAxisA);
			//}

			// Spring
			if (b_enableSpring)
			{
				if (FL::RenderFloatDragTableRow("##DampingRatio" + ID, "Spring Damping Ratio", dampingRatio, 0.1f, 0, FLT_MAX))
				{
					joint->SetSpringDampingRatio(dampingRatio);
				}
				if (FL::RenderFloatDragTableRow("##SpringHertz" + ID, "Spring Hertz", hertz, 0.1f, 0, FLT_MAX))
				{
					joint->SetSpringHertz(hertz);
				}
			}
			// Motor
			if (b_enableMotor)
			{
				if (FL::RenderFloatDragTableRow("##MotorSpeed" + ID, "Motor Speed", motorSpeed, 0.1f, 0, FLT_MAX))
				{
					joint->SetMotorSpeed(motorSpeed);
				}
				if (FL::RenderFloatDragTableRow("##MaxMotorForce" + ID, "Max Motor Force", maxMotorForce, 0.1f, 0, FLT_MAX))
				{
					joint->SetMaxMotorForce(maxMotorForce);
				}
			}
			FL::PopTable();
		}

		FL::MoveScreenCursor(0, 3.0f);
	}

	void RenderRevoluteJointProps(RevoluteJoint* joint)
	{

	}

	void RenderMouseJointProps(MouseJoint* joint)
	{

	}

	void RenderWeldJointProps(WeldJoint* joint)
	{

	}

	void RenderMotorJointProps(MotorJoint* joint)
	{

	}

	void RenderWheelJointProps(WheelJoint* joint)
	{

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
						if (FL::RenderImageButton(tileButtonID, texture, Vector2(iconSize, iconSize), 0, Vector2(1, 1), FL::GetColor("imageButton"), FL::GetColor("imageButtonTint"), FL::GetColor("tileSetIconHovered"), FL::GetColor("imageButtonActive"), uvStart, uvEnd))
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



		//FL::RenderSubTitle("Collision Areas");

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
		//FL::MoveScreenCursor(0, -3);
		//ImGui::BeginDisabled(collisionAreaLabel == "");
		//if (FL::RenderButton("Create"))
		//{
		//	if (!tileMap->ContainsCollisionAreaLabel(collisionAreaLabel))
		//	{
		//		std::vector<FL::CollisionAreaData> newData = std::vector<FL::CollisionAreaData>();
		//		collisionAreas.emplace(collisionAreaLabel, newData);
		//		collisionAreaLabel = "";
		//	}
		//	else
		//	{
		//		FL::LogError("Collision area label already being used in this TileMap, please choose a different one.");
		//	}
		//}
		//ImGui::EndDisabled();

		//FL::RenderInput("##CollisionAreaLabel" + std::to_string(ID), "", collisionAreaLabel, false);


		//FL::RenderSeparator(3, 3);


		//if (areaNames.size() > 0)
		//{
		//	ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("innerWindow"));
		//	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
		//	FL::PushMenuStyles();
		//	if (ImGui::BeginTable("#CollisionAreasTable", 1, FL::F_tableFlags))
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
		//	FL::PopMenuStyles();
		//	ImGui::PopStyleVar();
		//	ImGui::PopStyleColor();
		//}

		//if (selectedCollisionArea != "")
		//{
		//	if (FL::RenderButton("Draw Collision Areas"))
		//	{
		//		FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
		//		FG_collisionAreasBuffer.clear();
		//	}
		//	ImGui::SameLine(0, 5);
		//	ImGui::BeginDisabled(FG_collisionAreasBuffer.size() == 0);
		//	if (FL::RenderButton("Generate Colliders"))
		//	{
		//		FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
		//		tileMap->SetCollisionAreaValues(selectedCollisionArea, FG_collisionAreasBuffer);
		//		FG_collisionAreasBuffer.clear();
		//	}
		//	ImGui::EndDisabled();
		//}

		//if (areaNames.size() > 0)
		//{			
		//	FL::MoveScreenCursor(0, 5);
		//}
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