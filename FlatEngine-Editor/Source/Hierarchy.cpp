#include "managers/Assets.h"
#include "GameObject.h"
#include "Hierarchy.h"
#include "managers/SceneManager.h"
#include "GuiCore.h"
#include "components/Transform.h"
#include "managers/Settings.h"
#include "managers/ProjectManager.h"
#include "managers/PrefabManager.h"
#include "Modals.h"

#include "imgui.h"
#include "render/SceneView.h"

namespace FL = FlatEngine;


namespace FlatGui 
{
	std::map<long, bool> leafExpandedTracker = std::map<long, bool>();


	void AddObjectToHierarchy(FL::GameObject& currentObject, const char* charName, long& node_clicked, long& queuedForDelete, long& parentToUnparent, long& childToRemove, float indent)
	{
		ImGuiTreeNodeFlags nodeFlags;		
		bool b_objectFocused = FL::ProjectManager::loadedProject.IsIDFocused(currentObject.GetID());

		// If this node is selected, use the nodeFlag_selected to highlight it
		if (b_objectFocused)
		{
			if (currentObject.HasChildren())
			{
				nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Selected;
			}
			else
			{
				nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
			}
		}
		// Not focused
		else
		{
			if (currentObject.HasChildren())
			{
				nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;
			}
			else
			{
				nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
			}
		}


		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);		
		if (FL::GuiCore::RenderImageButton("VisibleID"+ std::to_string(currentObject.GetID()), FL::Assets::assetManager.GetTexture(currentObject.IsActive() ? "show": "hide"), FL::Vector2(16), 0, FL::Vector2(4), "transparent", "transparent")) currentObject.SetActive(!currentObject.IsActive());

		ImGui::TableSetColumnIndex(1);
		static int index = 0;
		std::string id = "##SwapDropSourceBefore"+ std::to_string(currentObject.GetID());
		FL::Vector2 cursorPos = FL::Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 3);
		FL::Vector2 availSpace = ImGui::GetContentRegionAvail();
		FL::Vector2 size = FL::Vector2(availSpace.x + 30 - cursorPos.x, 6);
		if (size.x < 30)
		{
			size.x = 30;
		}
		
		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, FL::Assets::assetManager.GetColor("dropTarget"));
		FL::GuiCore::RenderInvisibleButton(id.c_str(), cursorPos, size, true);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(FL::GuiCore::hierarchyTarget.c_str()))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				long ID = *(const long*)payload->Data;

				FL::GameObject* dropped = FL::SceneManager::loadedScene.GetObjectByID(ID);
				if (dropped->GetParentID() != -1)
				{
					FL::GameObject *parent = FL::SceneManager::loadedScene.GetObjectByID(dropped->GetParentID());
					parentToUnparent = dropped->GetParentID();
					childToRemove = dropped->GetID();
				}
				dropped->SetParentID(-1);
			}
			ImGui::EndDragDropTarget();
		}

		bool b_nodeOpen = false;

		ImGui::PushStyleColor(ImGuiCol_Header, FL::Assets::assetManager.GetColor("transparent"));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, FL::Assets::assetManager.GetColor("transparent"));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, FL::Assets::assetManager.GetColor("transparent"));

		// Indent for the GameObject name
		if (currentObject.GetParentID() != -1)
		{
			ImGui::SetCursorScreenPos(FL::Vector2(ImGui::GetCursorPos().x + indent, ImGui::GetCursorScreenPos().y));
			FL::Vector4 childNodeColor = FL::Assets::assetManager.GetColor("hierarchyChildObject");
			childNodeColor.w *= 0.03f * indent; // Gets darker the deeper the child object is nested in the hierarchy
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(childNodeColor));
		}

		indent += 15;

		if (currentObject.HasChildren())
		{
			if (leafExpandedTracker.count(currentObject.GetID()) == 0)
			{
				leafExpandedTracker.emplace(currentObject.GetID(), false);
			}

			ImGui::SetNextItemOpen(leafExpandedTracker.at(currentObject.GetID()));			
			b_nodeOpen = ImGui::TreeNodeEx(std::to_string(currentObject.GetID()).c_str(), nodeFlags, "%s", charName);
			leafExpandedTracker.at(currentObject.GetID()) = b_nodeOpen;
		}
		else
		{
			ImGui::TreeNodeEx(std::to_string(currentObject.GetID()).c_str(), nodeFlags, "%s", charName);

			if (leafExpandedTracker.count(currentObject.GetID()))
			{
				leafExpandedTracker.erase(currentObject.GetID());
			}
		}

		if (b_objectFocused)
		{
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32("treeSelectableSelected"));
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32("treeSelectableHovered"));
		}
		if (ImGui::IsItemActive())
		{
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::Assets::assetManager.GetColor32("treeSelectableActive"));
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		// Control click a Hierarchy item to focus on it in the Scene View
		if (ImGui::GetIO().KeyCtrl && ImGui::IsItemClicked())
		{
			FL::Transform* transform = currentObject.Get<FL::Transform>();
			FL::Vector3 position = transform->GetAbsolutePosition();
			FL::SceneView::sceneViewCameraTransform.LookAt(position);			
		}

		// Hold Alt key and hover object in Hierarchy for ToolTip with information about that FL::GameObject
		if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt)
		{	
			FL::GuiCore::BeginToolTip("GameObject Data");
			FL::GuiCore::MoveScreenCursor(0, 3);
			FL::GuiCore::RenderSeparator(3, 3);
			FL::GuiCore::MoveScreenCursor(0, 3);
			FL::GuiCore::RenderToolTipLong("Object ID    ", currentObject.GetID());
			FL::GuiCore::MoveScreenCursor(0, 3);
			FL::GuiCore::RenderToolTipLong("Parent ID    ", currentObject.GetParentID());
			FL::GuiCore::MoveScreenCursor(0, 3);
			FL::GuiCore::RenderToolTipLongVector("Children IDs ", currentObject.GetChildren());
			FL::GuiCore::EndToolTip();
		}

		FL::Vector2 savedCursorPos = ImGui::GetCursorPos();

		// Right click menu
		FL::GuiCore::PushMenuStyles();
		if (ImGui::BeginPopupContextItem())
		{
			if (FL::GuiCore::MenuItem("Open In New Inspector"))
			{				
				FL::ProjectManager::loadedProject.AddFocusedObjectID(currentObject.GetID());
				ImGui::CloseCurrentPopup();
			}		
			FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
			if (FL::GuiCore::MenuItem("Create Child"))
			{
				FL::SceneManager::loadedScene.CreateGameObject(currentObject.GetID());								 
				ImGui::CloseCurrentPopup();
			}			
			FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
			if (currentObject.IsPrefab())
			{
				std::string prefabName = "PREFAB: "+ currentObject.GetPrefabName();
				ImGui::Text("%s", prefabName.c_str());	
				FL::GuiCore::RenderSeparator(0,0,"menuSeparator");			
				if (FL::GuiCore::MenuItem("Save Prefab"))
				{
					FL::PrefabManager::CreatePrefab(FL::Assets::assetManager.GetDir("prefabs") + "/"+ currentObject.GetPrefabName() + ".prf", &currentObject);
					ImGui::CloseCurrentPopup();
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (FL::GuiCore::MenuItem("Unpack prefab"))
				{
					currentObject.SetIsPrefab(false);
					currentObject.SetPrefabName("");
					currentObject.SetPrefabSpawnLocation(FL::Vector3());
					ImGui::CloseCurrentPopup();
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (FL::GuiCore::MenuItem("Create New Prefab"))
				{
					Modals::b_openPrefabModal = true;
					Modals::gameObjectToPrefab = currentObject.GetID();
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				if (FL::GuiCore::MenuItem("Create Prefab"))
				{
					Modals::b_openPrefabModal = true;
					Modals::gameObjectToPrefab = currentObject.GetID();
					ImGui::CloseCurrentPopup();
				}
			}			
			FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
			if (FL::GuiCore::MenuItem("Delete GameObject"))
			{
				queuedForDelete = currentObject.GetID();
				ImGui::CloseCurrentPopup();
			}			
			// FL::GuiCore::RenderSeparator(0,0,"menuSeparator");
			// if (FL::GuiCore::MenuItem("Lock in view"))
			// {
			// 	// if (FG_b_sceneViewLockedOnObject && FG_sceneViewLockedObjectID == currentObject.GetID())
			// 	// {
			// 	// 	FG_b_sceneViewLockedOnObject = false;
			// 	// 	FG_sceneViewLockedObjectID = currentObject.GetID();
			// 	// }
			// 	// else if (!FG_b_sceneViewLockedOnObject)
			// 	// {
			// 	// 	FG_sceneViewLockedObjectID = currentObject.GetID();
			// 	// 	FG_b_sceneViewLockedOnObject = true;
			// 	// }

			// 	ImGui::CloseCurrentPopup();
			// }
			ImGui::EndPopup();
		}
		FL::GuiCore::PopMenuStyles();
		
		if (ImGui::IsItemHovered() && ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_MouseLeft))
		{
			node_clicked = currentObject.GetID();
			FL::ProjectManager::loadedProject.RefocusID(currentObject.GetID());
		}

		// Add As Child Drag and Drop
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
		{
			int ID = currentObject.GetID();
			ImGui::SetDragDropPayload(FL::GuiCore::hierarchyTarget.c_str(), &ID, sizeof(int));
			ImGui::Text("Set Parent");
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(FL::GuiCore::hierarchyTarget.c_str()))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int ID = *(const int*)payload->Data;

				// Save Dropped Object
				FL::GameObject *dropped = FL::SceneManager::loadedScene.GetObjectByID(ID);
				// Remove dropped object from its previous parents children
				if (dropped->GetParentID() != -1)
				{
					FL::GameObject *parent = dropped->GetParent();
					parent->RemoveChild(dropped->GetID());
				}
				// Add dropped object to this object as a child
				currentObject.AddChild(dropped->GetID());
				dropped->SetParentID(currentObject.GetID());				
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopStyleColor();


		// Render Prefab Cube if it is a prefab object
		if (currentObject.IsPrefab())
		{
			std::string prefabIDImageButton = "PrefabIDImage"+ std::to_string(currentObject.GetID());
			std::string prefabIDContextMenu = "PrefabIDContext"+ std::to_string(currentObject.GetID());
			ImGui::TableSetColumnIndex(2);
			ImGui::SetCursorPos(FL::Vector2(ImGui::GetCursorPosX() - 1, ImGui::GetCursorPosY()));
			FL::GuiCore::RenderImageButton(prefabIDImageButton.c_str(), FL::Assets::assetManager.GetTexture("prefabCube"), FL::Vector2(16), 0, FL::Vector2(4), "buttonBorder", "transparent", "white", "buttonHovered", "buttonActive");
			FL::GuiCore::PushMenuStyles();
			if (ImGui::BeginPopupContextItem(prefabIDContextMenu.c_str(), ImGuiPopupFlags_MouseButtonLeft))
			{
				if (FL::GuiCore::MenuItem("Unpack prefab"))
				{
					currentObject.SetIsPrefab(false);
					currentObject.SetPrefabName("");
					currentObject.SetPrefabSpawnLocation(FL::Vector3());
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			FL::GuiCore::PopMenuStyles();
		}
		
		if (currentObject.HasChildren() && b_nodeOpen)
		{
			std::vector<long> childrenIDs = currentObject.GetChildren();
			
			for (long childID : childrenIDs)
			{
				FL::GameObject* child = FL::SceneManager::loadedScene.GetObjectByID(childID);

				if (child != nullptr)
				{
					std::string name = child->GetName();
					const char* childName = name.c_str();

					AddObjectToHierarchy(*child, childName, node_clicked, queuedForDelete, parentToUnparent, childToRemove, indent);
				}
			}

			ImGui::TreePop();
		}
	}

	void RenderHierarchy(bool& b_show)
	{
		if (!b_show)
			return;
		
		if (FL::GuiCore::BeginWindow("Hierarchy", b_show))
		{			
			// Render Loaded Scene text and threeDots more menu button
			std::string loadedSceneString = "Loaded Scene: "+ FL::SceneManager::loadedScene.name;

			FL::GuiCore::MoveScreenCursor(3,4);
			ImGui::Text("%s", loadedSceneString.c_str());			
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20, -2);			

			// Three dots
			FL::GuiCore::MoveScreenCursor(0, -2);
			FL::GuiCore::RenderImageButton("##SaveLoadMenu", FL::Assets::assetManager.GetTexture("threeDots"), FL::Vector2(16), 0, FL::Vector2(1), "buttonBorder", "transparent");
			FL::GuiCore::PushMenuStyles();
			if (ImGui::BeginPopupContextItem("##InspectorMoreContext", ImGuiPopupFlags_MouseButtonLeft))
			{
				if (FL::GuiCore::MenuItem("Save All"))
				{
					if (FL::SceneManager::loadedScene.path == "")
					{						
						Modals::b_openSceneModal = true;
					}
					else
					{
						FL::SceneManager::SaveCurrentScene();
					}
					FL::ProjectManager::SaveCurrentProject();
					FL::Settings::settings.SaveSettings();
					ImGui::CloseCurrentPopup();
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparator");
				if (FL::GuiCore::MenuItem("Save Scene"))
				{
					if (FL::SceneManager::loadedScene.path == "")
					{
						Modals::b_openSceneModal = true;
					}
					else
					{
						FL::SceneManager::SaveCurrentScene();
					}
					ImGui::CloseCurrentPopup();
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (FL::GuiCore::MenuItem("Save Project"))
				{
					FL::ProjectManager::SaveCurrentProject();
					ImGui::CloseCurrentPopup();
				}	
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (FL::GuiCore::MenuItem("Save Engine Settings"))
				{					
					FL::Settings::settings.SaveSettings();
					ImGui::CloseCurrentPopup();
				}	
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				ImGui::BeginDisabled(FL::SceneManager::loadedScene.path == "");
				if (FL::GuiCore::MenuItem("Reload Scene"))
				{
					FL::SceneManager::LoadScene(FL::SceneManager::loadedScene.path);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndDisabled();
				ImGui::EndPopup();
			}
			FL::GuiCore::PopMenuStyles();

			// Table for Scene Objects in Hierarchy			
			float visibleIconColumnWidth = 24;
			float isPrefabIconColumnWidth = 24;
			static float currentIndent = 10;
			static bool b_allAreVisible = false;
			std::vector<FL::GameObject>& sceneObjects = FL::SceneManager::loadedScene.GetSceneObjects();			

			static long node_clicked = -1;

			FL::GuiCore::BeginWindowChild("##ScrollingHierarchy", "outerWindow", 0, FL::Vector2());
			// {

				long queuedForDelete = -1;
				long parentToUnparent = -1;
				long childToRemove = -1;
				ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::Assets::assetManager.GetColor("innerWindow"));
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, FL::Vector2());
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, FL::Vector2());
				if (ImGui::BeginTable("##SceneHierarchyTable", 3, FL::GuiCore::tableFlagsBorders))
				{
					ImGui::TableSetupColumn("##VISIBLE", ImGuiTableColumnFlags_WidthFixed, visibleIconColumnWidth);
					ImGui::TableSetupColumn("##OBJECT", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("##ISPREFAB", ImGuiTableColumnFlags_WidthFixed, isPrefabIconColumnWidth);
					ImGui::TableNextRow();
					// {
					
					ImGui::TableSetColumnIndex(0);		
					if (FL::GuiCore::RenderImageButton("##SetAllVisible", FL::Assets::assetManager.GetTexture(b_allAreVisible ? "show": "hide"), FL::Vector2(16, 16), 0, FL::Vector2(4), "transparent", "button", "white", "buttonHovered", "buttonActive"))
					{
						for (FL::GameObject& sceneObject : sceneObjects)
						{
							sceneObject.SetActive(!b_allAreVisible);
						}

						b_allAreVisible = !b_allAreVisible;
					}					

					ImGui::TableSetColumnIndex(1);
					ImGui::PushStyleColor(ImGuiCol_Text, FL::Assets::assetManager.GetColor("logText"));
					ImGui::SetCursorPos(FL::Vector2(ImGui::GetCursorPosX() + 7, ImGui::GetCursorPosY() + 4)); // Indent the text
					ImGui::Text("SCENE OBJECTS");
					ImGui::PopStyleColor();

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, FL::Vector2(0));
					ImGui::TableSetColumnIndex(2);
					ImGui::PopStyleVar();
					if (FL::GuiCore::RenderImageButton("##PrefabCubes", FL::Assets::assetManager.GetTexture("prefabCube"), FL::Vector2(16, 16), 0, FL::Vector2(4), "transparent", "transparent", "white", "transparent", "transparent"))
					{
						// Doesn't do anything, should just be an icon
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetMouseCursor(0);
					}

					for (FL::GameObject& sceneObject : sceneObjects)
					{
						// If this object does not have a parent we render it and all of its children.
						if (sceneObject.GetParentID() == -1)
						{
							std::string name = sceneObject.GetName();
							const char* charName = name.c_str();
							float indent = 0;

							AddObjectToHierarchy(sceneObject, charName, node_clicked, queuedForDelete, parentToUnparent, childToRemove, indent);
						}
					}

					if (node_clicked != -1)
					{
						// Update selection state
						// (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
						if (ImGui::GetIO().KeyCtrl)
						{

						}
						else
						{

						}
					}

					// Add empty table rows so the table goes all the way to the bottom of the screen
					float availableVerticalSpace = ImGui::GetContentRegionAvail().y;
					while (availableVerticalSpace > 22)
					{			
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(1);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 9);
						ImGui::Text("");

						availableVerticalSpace = ImGui::GetContentRegionAvail().y;						
					}

					if (availableVerticalSpace > 1)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(1);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + availableVerticalSpace - 1);
					}

					// }
					ImGui::EndTable(); // Hierarchy Table
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();

			// }
			FL::GuiCore::EndWindowChild();

			// Delete queued GameObject
			if (queuedForDelete != -1)
			{				
				// long saveFocusedObject = FL::ProjectManager::loadedProject.focusedGameObjectID;

				// Unfocus focused GameObject first
				// if (FL::ProjectManager::loadedProject.IsIDFocused(queuedForDelete))
				FL::ProjectManager::loadedProject.RemoveFocusedObjectID(queuedForDelete);
				FL::SceneManager::loadedScene.DeleteGameObject(queuedForDelete);

				// // If previous focused object still exists, set it to focused object again
				// if (FL::SceneManager::loadedScene.GetObjectByID(saveFocusedObject) != nullptr)
				// {
				// 	FL::ProjectManager::loadedProject.focusedGameObjectID = saveFocusedObject;
				// }

				queuedForDelete = -1;
			}

			// Remove queued children
			if (parentToUnparent != -1 && childToRemove != -1)
			{
				FL::SceneManager::loadedScene.GetObjectByID(parentToUnparent)->RemoveChild(childToRemove);
				parentToUnparent = -1;
				childToRemove = -1;
			}			
		}

		FL::GuiCore::EndWindow(); // Hierarchy
	}
}