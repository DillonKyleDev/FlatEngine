#include "managers/Assets.h"
#include "GameObject.h"
#include "managers/SceneManager.h"
#include "GuiCore.h"
#include "components/Transform.h"
#include "managers/Settings.h"
#include "managers/ProjectManager.h"
#include "managers/PrefabManager.h"
#include "Modals.h"

#include "imgui.h"
#include "render/SceneView.h"
#include "tools/Vector2.h"

namespace FL = FlatEngine;


namespace FlatGui 
{
	long createChildParentID = -1;
	long queuedForDelete = -1;
	long parentToUnparent = -1;
	long droppedObjectID = -1;
	long newDroppedParent = -1;
	long insertDroppedObjectBefore = -1;


	void AddObjectToHierarchy(FL::GameObject* currentObject, const char* charName, long& node_clicked, float indent)
	{		
		bool b_objectFocused = FL::ProjectManager::loadedProject.IsIDFocused(currentObject->GetID());

		ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;		
		ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;				

		if (currentObject->HasChildren())		
			nodeFlags = treeFlags;		
		if (b_objectFocused)	
			nodeFlags |= ImGuiTreeNodeFlags_Selected;		
	
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);	
		FL::GuiCore::MoveScreenCursor(-(indent * 1.5), 0);	
		if (FL::GuiCore::RenderImageButton("VisibleID" + std::to_string(currentObject->GetID()), FL::Assets::assetManager.GetTexture(currentObject->IsActive() ? "show": "hide"), FL::Vector2(16), 0, FL::Vector2(4), "transparent", "transparent")) currentObject->SetActive(!currentObject->IsActive());

		ImGui::TableSetColumnIndex(1);
		static int index = 0;
		std::string id = "##SwapDropSourceBefore"+ std::to_string(currentObject->GetID());
		FL::Vector2 cursorPos = FL::Vector2(ImGui::GetCursorScreenPos().x + indent, ImGui::GetCursorScreenPos().y - 1);		
		FL::Vector2 size = FL::Vector2(ImGui::GetContentRegionAvail().x + 30 - cursorPos.x, 2);
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
				long currentParentID = currentObject->GetParentID();
				FL::GameObject* dropped = FL::SceneManager::loadedScene.GetObjectByID(ID);

				if (currentObject->GetID() != ID)
				{
					droppedObjectID = ID;	

					// Was parented before
					if (dropped->GetParentID() != -1)
					{
						parentToUnparent = dropped->GetParentID();												
					}
					// If object dropped in root of Hierarchy, save the new hierarchy position of dropped object
					if (currentParentID == -1)
					{
						insertDroppedObjectBefore = currentObject->hierarchyPosition;
					}
					// The object was dropped inside a parent objects internal hierarchy, add the dropped object as a new child to it.
					else
					{												
						insertDroppedObjectBefore = currentObject->parentedHierarchyPosition;
					}		
					newDroppedParent = currentParentID;					
				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::PushStyleColor(ImGuiCol_Header, FL::Assets::assetManager.GetColor("transparent"));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, FL::Assets::assetManager.GetColor("transparent"));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, FL::Assets::assetManager.GetColor("transparent"));

		// Indent for the GameObject name
		if (currentObject->GetParentID() != -1)
		{
			FL::GuiCore::MoveScreenCursor(indent, 0);			
			FL::Vector4 childNodeColor = FL::Assets::assetManager.GetColor("hierarchyChildObject");
			childNodeColor.w *= 0.03f * indent; // Gets darker the deeper the child object is nested in the hierarchy
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(childNodeColor));
		}

		indent += 14;

		bool b_nodeOpen = !currentObject->b_collapsed;
		if (currentObject->HasChildren())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, FL::Vector2(4, 5.0f));
			ImGui::SetNextItemOpen(b_nodeOpen);			
			b_nodeOpen = ImGui::TreeNodeEx(std::to_string(currentObject->GetID()).c_str(), nodeFlags, "%s", charName);
			currentObject->b_collapsed = !b_nodeOpen;
			ImGui::PopStyleVar();
		}
		else
		{			
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, FL::Vector2(4, 5.0f));
			ImGui::TreeNodeEx(std::to_string(currentObject->GetID()).c_str(), nodeFlags, "%s", charName);		
			ImGui::PopStyleVar();	
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		// Control click a Hierarchy item to focus on it in the Scene View
		if (ImGui::GetIO().KeyCtrl && ImGui::IsItemClicked())
		{
			FL::Transform* transform = currentObject->Get<FL::Transform>();
			FL::Vector3 position = transform->GetAbsolutePosition();
			FL::SceneView::sceneViewCameraTransform.LookAt(position);			
		}

		// Hold Alt key and hover object in Hierarchy for ToolTip with information about that FL::GameObject
		if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt)
		{	
			FL::GuiCore::BeginToolTip(           "GameObject Data");
			FL::GuiCore::RenderToolTipLong(      "Object ID    ", currentObject->GetID());
			FL::GuiCore::RenderToolTipLong(      "Parent ID    ", currentObject->GetParentID());
			FL::GuiCore::RenderToolTipLongVector("Children IDs ", currentObject->GetChildren());
			FL::GuiCore::RenderToolTipLong(      "Hierarchy Pos", currentObject->hierarchyPosition);
			FL::GuiCore::RenderToolTipLong(      "Parented Pos ", currentObject->parentedHierarchyPosition);
			FL::GuiCore::EndToolTip();
		}

		FL::Vector2 savedCursorPos = ImGui::GetCursorPos();

		// Right click menu
		FL::GuiCore::PushMenuStyles();
		if (ImGui::BeginPopupContextItem())
		{
			if (FL::GuiCore::MenuItem("Open In New Inspector"))
			{				
				FL::ProjectManager::loadedProject.AddFocusedObjectID(currentObject->GetID());
				ImGui::CloseCurrentPopup();
			}		
			FL::GuiCore::RenderMenuSeparator();
			if (FL::GuiCore::MenuItem("Create Child"))
			{
				createChildParentID = currentObject->GetID();										 
				ImGui::CloseCurrentPopup();
			}			
			
			if (currentObject->IsPrefab())
			{
				FL::GuiCore::RenderMenuSeparator(false);
				std::string prefabName = "PREFAB: "+ currentObject->GetPrefabName();
				FL::GuiCore::MenuItem(prefabName.c_str(), NULL, false, false);				
				FL::GuiCore::RenderMenuSeparator(false);			
				if (FL::GuiCore::MenuItem("Save Prefab"))
				{
					FL::PrefabManager::CreatePrefab(FL::Assets::assetManager.GetDir("prefabs") + "/"+ currentObject->GetPrefabName() + ".prf", currentObject);
					ImGui::CloseCurrentPopup();
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Unpack prefab"))
				{
					currentObject->SetIsPrefab(false);
					currentObject->SetPrefabName("");
					currentObject->SetPrefabSpawnLocation(FL::Vector3());
					ImGui::CloseCurrentPopup();
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Create New Prefab"))
				{
					Modals::b_openPrefabModal = true;
					Modals::gameObjectToPrefab = currentObject->GetID();
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Create Prefab"))
				{
					Modals::b_openPrefabModal = true;
					Modals::gameObjectToPrefab = currentObject->GetID();
					ImGui::CloseCurrentPopup();
				}
			}			
			FL::GuiCore::RenderMenuSeparator();
			if (FL::GuiCore::MenuItem("Delete GameObject"))
			{
				queuedForDelete = currentObject->GetID();
				ImGui::CloseCurrentPopup();
			}			
			ImGui::EndPopup();
		}
		FL::GuiCore::PopMenuStyles();
		
		if (ImGui::IsItemHovered() && ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_MouseLeft))
		{
			node_clicked = currentObject->GetID();
			FL::ProjectManager::loadedProject.RefocusID(currentObject->GetID());
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

		// Add As Child Drag and Drop
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
		{
			int ID = currentObject->GetID();
			ImGui::SetDragDropPayload(FL::GuiCore::hierarchyTarget.c_str(), &ID, sizeof(int));			
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(FL::GuiCore::hierarchyTarget.c_str()))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int ID = *(const int*)payload->Data;							
				FL::GameObject* dropped = FL::SceneManager::loadedScene.GetObjectByID(ID);
				long currentObjectID = currentObject->GetID();
				droppedObjectID = ID;	

				// Remove dropped object from its previous parents children
				if (dropped->GetParentID() != -1)
				{
					parentToUnparent = dropped->GetParentID();
				}			
				
				newDroppedParent = currentObjectID;											
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopStyleColor();


		// Render Prefab Cube if it is a prefab object
		if (currentObject->IsPrefab())
		{
			std::string prefabIDImageButton = "PrefabIDImage"+ std::to_string(currentObject->GetID());
			std::string prefabIDContextMenu = "PrefabIDContext"+ std::to_string(currentObject->GetID());
			ImGui::TableSetColumnIndex(2);			
			FL::GuiCore::RenderImageButton(prefabIDImageButton.c_str(), FL::Assets::assetManager.GetTexture("prefabCube"), FL::Vector2(16), 0, FL::Vector2(4), "buttonBorder", "transparent", "white", "buttonHovered", "buttonActive");
			FL::GuiCore::PushMenuStyles();
			if (ImGui::BeginPopupContextItem(prefabIDContextMenu.c_str(), ImGuiPopupFlags_MouseButtonLeft))
			{				
				std::string prefabName = "PREFAB: "+ currentObject->GetPrefabName();
				FL::GuiCore::MenuItem(prefabName.c_str(), NULL, false, false);				
				FL::GuiCore::RenderMenuSeparator(false);			
				if (FL::GuiCore::MenuItem("Save Prefab"))
				{
					FL::PrefabManager::CreatePrefab(FL::Assets::assetManager.GetDir("prefabs") + "/"+ currentObject->GetPrefabName() + ".prf", currentObject);
					ImGui::CloseCurrentPopup();
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Unpack prefab"))
				{
					currentObject->SetIsPrefab(false);
					currentObject->SetPrefabName("");
					currentObject->SetPrefabSpawnLocation(FL::Vector3());
					ImGui::CloseCurrentPopup();
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Create New Prefab"))
				{
					Modals::b_openPrefabModal = true;
					Modals::gameObjectToPrefab = currentObject->GetID();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			FL::GuiCore::PopMenuStyles();
		}
		
		if (currentObject->HasChildren() && b_nodeOpen)
		{
			std::vector<long> childrenIDs = currentObject->GetChildren();
			
			for (long childID : childrenIDs)
			{
				FL::GameObject* child = FL::SceneManager::loadedScene.GetObjectByID(childID);

				if (child != nullptr)
				{
					std::string name = child->GetName();
					const char* childName = name.c_str();

					AddObjectToHierarchy(child, childName, node_clicked, indent);
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
				FL::GuiCore::RenderMenuSeparator(false);
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
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Save Project"))
				{
					FL::ProjectManager::SaveCurrentProject();
					ImGui::CloseCurrentPopup();
				}	
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Save Engine Settings"))
				{					
					FL::Settings::settings.SaveSettings();
					ImGui::CloseCurrentPopup();
				}	
				FL::GuiCore::RenderMenuSeparator();
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
			static bool b_allAreVisible = true;
			static long node_clicked = -1;

			std::vector<long> sceneObjectIDs = FL::SceneManager::loadedScene.GetSortedHierarchyObjects();						

			FL::GuiCore::BeginWindowChild("##ScrollingHierarchy", "outerWindow", 0, FL::Vector2());
			// {
				
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
						for (long sceneObjectID : sceneObjectIDs)
						{
							FL::SceneManager::loadedScene.GetObjectByID(sceneObjectID)->SetActive(!b_allAreVisible);							
						}
						b_allAreVisible = !b_allAreVisible;
					}					

					ImGui::TableSetColumnIndex(1);
					ImGui::PushStyleColor(ImGuiCol_Text, FL::Assets::assetManager.GetColor("logText"));
					FL::GuiCore::MoveScreenCursor(7, 4);
					ImGui::Text("SCENE OBJECTS");
					ImGui::PopStyleColor();

					ImGui::TableSetColumnIndex(2);
					FL::GuiCore::MoveScreenCursor(4, 4);
					ImGui::Image(FL::Assets::assetManager.GetTexture("prefabCube"), FL::Vector2(16, 16));

					for (long sceneObjectID : sceneObjectIDs)
					{
						FL::GameObject* sceneObject  = FL::SceneManager::loadedScene.GetObjectByID(sceneObjectID);
						
						// If this object does not have a parent we render it and all of its children.
						if (sceneObject->GetParentID() == -1)
						{
							std::string name = sceneObject->GetName();
							const char* charName = name.c_str();
							float indent = 0;

							AddObjectToHierarchy(sceneObject, charName, node_clicked, indent);							
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
					while (availableVerticalSpace > 24)
					{			
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(1);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 11);
						ImGui::Text("");
						availableVerticalSpace = ImGui::GetContentRegionAvail().y;						
					}

					if (availableVerticalSpace > 1)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(1);
						FL::GuiCore::MoveScreenCursor(0, availableVerticalSpace);						
					}

					// }
					ImGui::EndTable(); // Hierarchy Table
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();

			// }
			FL::GuiCore::EndWindowChild();

			if (queuedForDelete != -1)
			{				
				FL::ProjectManager::loadedProject.RemoveFocusedObjectID(queuedForDelete);
				FL::SceneManager::loadedScene.DeleteGameObject(queuedForDelete);
				queuedForDelete = -1;
			}

			// Perform dropped object actions
			if (droppedObjectID != -1)
			{		
				FL::GameObject* dropped = FL::SceneManager::loadedScene.GetObjectByID(droppedObjectID);				

				if (parentToUnparent != -1)		
					FL::SceneManager::loadedScene.GetObjectByID(parentToUnparent)->RemoveChild(droppedObjectID);
				else if (dropped->GetParentID() == -1)
					FL::SceneManager::loadedScene.RemoveSortedHierarchyObject(dropped);
				
				dropped->SetParentID(newDroppedParent);

				// Reparent the dropped object to the parent tree it was dropped into
				if (newDroppedParent != -1)
				{
					FL::GameObject* newParent = FL::SceneManager::loadedScene.GetObjectByID(newDroppedParent);
					if (newParent != nullptr)
					{
						dropped->parentedHierarchyPosition = insertDroppedObjectBefore; // insertDroppedObjectBefore could be -1 (not set) which is accounted for in AddChild()
						newParent->AddChild(dropped);
						newParent->ReduceParentHierarchyPositions();
					}
				}
				// Reorder the root hierarchy to fit the dropped object into its new position
				else if (insertDroppedObjectBefore != -1)
				{						
					dropped->hierarchyPosition = insertDroppedObjectBefore;										
					FL::SceneManager::loadedScene.InsertSortedHierarchyObject(dropped);
					FL::SceneManager::loadedScene.ReduceHierarchyPositions();
				}

				parentToUnparent = -1;
				droppedObjectID = -1;
				newDroppedParent = -1;
				insertDroppedObjectBefore = -1;			
			}			
			if (createChildParentID != -1)
			{
				FL::GameObject* child = FL::SceneManager::loadedScene.CreateGameObject(createChildParentID);	
				FL::ProjectManager::loadedProject.RefocusID(child->GetID());
				createChildParentID = -1;
			}
		}

		FL::GuiCore::EndWindow(); // Hierarchy
	}
}