#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"
#include "Project.h"
#include "SceneManager.h"

#include "imgui.h"


namespace FL = FlatEngine;

namespace FlatGui 
{
	void RenderHierarchy()
	{
		FL::BeginWindow("Scene GameObjects");
		// {
			
			// Render Loaded Scene text and threeDots more menu button
			std::string sceneName = FL::GetLoadedScene()->GetName();
			std::string loadedSceneString = "Loaded Scene: " + sceneName;

			FL::RenderSectionHeader(loadedSceneString);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 22, 0);			

			static std::string newSceneName = "";
			static bool b_openSceneModal = false;
			if (FL::RenderInputModal("Create Scene", "Enter a name for the Scene", newSceneName, b_openSceneModal))
			{				
				std::string scenePath = "..\\projects\\" + FL::GetFilenameFromPath(FL::GetLoadedProject().GetPath()) + "\\scenes\\" + newSceneName + ".scn";
				FL::GetLoadedScene()->SetPath(scenePath);
				FL::GetLoadedScene()->SetName(newSceneName);
				FL::SaveScene(FL::GetLoadedScene(), scenePath);
				FL::F_SceneManager.SetLoadedScenePath(scenePath);
				FL::GetLoadedProject().SetLoadedScenePath(scenePath);	
				SaveCurrentProject();
				FL::SetProjectLoadedScenePath(scenePath);
			}

			// Three dots
			FL::MoveScreenCursor(0, -2);
			FL::RenderImageButton("##SaveLoadMenu", FL::GetTexture("threeDots"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("transparent"));
			FL::PushMenuStyles();
			if (ImGui::BeginPopupContextItem("##InspectorMoreContext", ImGuiPopupFlags_MouseButtonLeft))
			{
				if (ImGui::MenuItem("Save all"))
				{
					if (FL::GetLoadedScenePath() == "" && FL::GetLoadedScene() != nullptr)
					{						
						b_openSceneModal = true;
					}
					else
					{
						FL::SaveCurrentScene();
					}
					SaveCurrentProject();
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Save scene"))
				{
					if (FL::GetLoadedScenePath() == "" && FL::GetLoadedScene() != nullptr)
					{
						b_openSceneModal = true;
					}
					else
					{
						FL::SaveCurrentScene();
					}
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Save project"))
				{
					SaveCurrentProject();
					ImGui::CloseCurrentPopup();
				}			
				ImGui::Separator();
				ImGui::Separator();
				ImGui::BeginDisabled(FL::GetLoadedScenePath() == "");
				if (ImGui::MenuItem("Reload scene"))
				{
					SetFocusedGameObjectID(-1);
					FL::LoadScene(FL::GetLoadedScenePath());
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndDisabled();
				ImGui::EndPopup();
			}
			FL::PopMenuStyles();

			FL::MoveScreenCursor(0, 1);

			// Table for Scene Objects in Hierarchy
			float objectColumnWidth = ImGui::GetContentRegionAvail().x;
			float visibleIconColumnWidth = 25;
			float isPrefabIconColumnWidth = 25;
			static float currentIndent = 10;
			static bool b_allAreVisible = false;
			std::vector<GameObject*> sceneObjects = FL::GetLoadedScene()->GetSortedHierarchyObjects();
			std::map<long, GameObject>& persistantObjects = FL::GetPersistantObjects();

			static int node_clicked = -1;

			FL::BeginWindowChild("##ScrollingHierarchy", FL::GetColor("outerWindow"), 0, Vector2(0));
			// {

				long queuedForDelete = -1;
				long parentToUnparent = -1;
				long childToRemove = -1;
				ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("innerWindow"));
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, Vector2(0, 0));
				if (ImGui::BeginTable("##SceneHierarchyTable", 3, FL::F_tableFlags, Vector2(ImGui::GetContentRegionAvail().x - 1, ImGui::GetContentRegionAvail().y - 1)))
				{
					ImGui::TableSetupColumn("##VISIBLE", 0, visibleIconColumnWidth);
					ImGui::TableSetupColumn("##OBJECT", 0, objectColumnWidth);
					ImGui::TableSetupColumn("##ISPREFAB", 0, isPrefabIconColumnWidth);
					ImGui::TableNextRow();
					// {

						// Visible/Invisible all gameObjects at once
					ImGui::TableSetColumnIndex(0);
					if (b_allAreVisible)
					{
						FL::MoveScreenCursor(-1, 0);
						if (FL::RenderImageButton("##SetAllInvisible", FL::GetTexture("show"), Vector2(16, 16), 0, Vector2(4), FL::GetColor("button"), FL::GetColor("white"), FL::GetColor("buttonHovered"), FL::GetColor("buttonActive")))
						{
							for (GameObject* sceneObject : sceneObjects)
							{
								sceneObject->SetActive(false);
							}

							b_allAreVisible = false;
						}
					}
					else
					{
						FL::MoveScreenCursor(-1, 0);
						if (FL::RenderImageButton("##SetAllVisible", FL::GetTexture("hide"), Vector2(16, 16), 0, Vector2(4), FL::GetColor("button"), FL::GetColor("white"), FL::GetColor("buttonHovered"), FL::GetColor("buttonActive")))
						{
							for (GameObject* sceneObject : sceneObjects)
							{
								sceneObject->SetActive(false);
							}
							b_allAreVisible = true;

						}
					}

					ImGui::TableSetColumnIndex(1);
					ImGui::PushStyleColor(ImGuiCol_Text, FL::GetColor("logText"));
					ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 7, ImGui::GetCursorPosY() + 4)); // Indent the text
					ImGui::Text("SCENE OBJECTS");
					ImGui::PopStyleColor();

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(0, 0));
					ImGui::TableSetColumnIndex(2);
					ImGui::PopStyleVar();
					if (FL::RenderImageButton("##PrefabCubes", FL::GetTexture("prefabCube"), Vector2(16, 16), 0, Vector2(4), FL::GetColor("transparent"), FL::GetColor("white"), FL::GetColor("transparent"), FL::GetColor("transparent")))
					{
						// Doesn't do anything, should just be an icon
					}
					if (ImGui::IsItemHovered())
					{
						ImGui::SetMouseCursor(0);
					}

					for (GameObject* sceneObject : sceneObjects)
					{
						// If this object does not have a parent we render it and all of its children.
						if (sceneObject->GetParentID() == -1)
						{
							std::string name = sceneObject->GetName();
							const char* charName = name.c_str();
							float indent = 0;

							AddObjectToHierarchy(*sceneObject, charName, node_clicked, queuedForDelete, parentToUnparent, childToRemove, indent);
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
			FL::EndWindowChild();

			// Delete queued GameObject
			if (queuedForDelete != -1)
			{
				long focusedObjectID = GetFocusedGameObjectID();
				long saveFocusedObject = focusedObjectID;

				// Unfocus focused GameObject first
				SetFocusedGameObjectID(-1);
				FL::DeleteGameObject(queuedForDelete);

				// If previous focused object still exists, set it to focused object again
				if (FL::GetObjectByID(saveFocusedObject) != nullptr)
				{
					SetFocusedGameObjectID(saveFocusedObject);
				}
			}

			// Remove queued children
			if (parentToUnparent != -1 && childToRemove != -1)
			{
				FL::GetObjectByID(parentToUnparent)->RemoveChild(childToRemove);
				parentToUnparent = -1;
				childToRemove = -1;
			}

		// }
		FL::EndWindow(); // Hierarchy
	}

	// Add GameObject to Hierarchy viewport
	void AddObjectToHierarchy(GameObject& currentObject, const char* charName, int& node_clicked, long& queuedForDelete, long& parentToUnparent, long& childToRemove, float indent)
	{
		ImGuiTreeNodeFlags nodeFlags;
		long focusedObjectID = GetFocusedGameObjectID();
		bool b_objectFocused = focusedObjectID == currentObject.GetID();

		static bool b_openPrefabModal = false;
		std::string newPrefabName = "";
		static long modalOpenOn = -1;

		// Create new Prefab modal
		if (currentObject.GetID() == modalOpenOn)
		{
			if (FL::RenderInputModal("Create Prefab", "Enter a name for the Prefab:", newPrefabName, b_openPrefabModal))
			{
				b_openPrefabModal = false;
				FL::CreatePrefab(FL::GetDir("prefabs") + "/" + newPrefabName + ".prf", currentObject);
				currentObject.SetIsPrefab(true);
				currentObject.SetPrefabName(newPrefabName);
				currentObject.SetPrefabSpawnLocation(currentObject.GetTransform()->GetPosition());
			}
		}


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


		// Go to next row and column
		ImGui::TableNextRow();

		// Draw visibility eye icon
		ImGui::TableSetColumnIndex(0);
		std::string visibleID = "VisibleID" + std::to_string(currentObject.GetID());

		float extraIndent = 0;
		if (indent > 0)
		{
			extraIndent = 6;
		}
		float indentMultiplier = indent / 15;

		// Moves the cursor to account for the visible icon
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - indent - (extraIndent * indentMultiplier) - 1);
		indent += 15;

		// Show Visible/Invisible Icons
		if (currentObject.IsActive())
		{
			if (FL::RenderImageButton(visibleID.c_str(), FL::GetTexture("show"), Vector2(16), 0, Vector2(4), FL::GetColor("transparent")))
			{
				currentObject.SetActive(false);
			}
		}
		else
		{
			if (FL::RenderImageButton(visibleID.c_str(), FL::GetTexture("hide"), Vector2(16), 0, Vector2(4), FL::GetColor("transparent")))
			{
				currentObject.SetActive(true);
			}
		}

		// Render actual gameObject
		ImGui::TableSetColumnIndex(1);

		int index = -1;

		std::string id = "##SwapDropSource" + std::to_string(index);
		Vector2 cursorPos = Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 3);
		Vector2 availSpace = ImGui::GetContentRegionAvail();
		Vector2 size = Vector2(availSpace.x + 30 - cursorPos.x, 6);
		if (size.x < 30)
		{
			size.x = 30;
		}
		
		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, FL::GetColor("dropTarget"));
		FL::RenderInvisibleButton(id.c_str(), cursorPos, size);
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(FL::F_hierarchyTarget.c_str()))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				long ID = *(const long*)payload->Data;

				// Save Dropped Object
				GameObject* dropped = FL::GetObjectByID(ID);
				// Remove dropped object from its previous parents children
				if (dropped->GetParentID() != -1)
				{
					GameObject *parent = FL::GetObjectByID(dropped->GetParentID());
					parentToUnparent = dropped->GetParentID();
					childToRemove = dropped->GetID();
				}
				// Set parent ID of dropped object to -1
				dropped->SetParentID(-1);
			}
			ImGui::EndDragDropTarget();
		}

		bool b_nodeOpen = false;

		ImGui::PushStyleColor(ImGuiCol_Header, FL::GetColor("transparent"));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, FL::GetColor("transparent"));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, FL::GetColor("transparent"));

		// Indent for the GameObject name
		if (currentObject.GetParentID() != -1)
		{
			ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorPos().x + indent, ImGui::GetCursorScreenPos().y));
			// Set table cell bg color for child object		
			Vector4 childNodeColor = FL::GetColor("hierarchyChildObject");
			childNodeColor.w *= 0.03f * indent; // Gets darker the deeper the child object is nested in the hierarchy
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(childNodeColor));
		}

		if (currentObject.HasChildren())
		{
			if (FG_leafExpandedTracker.count(currentObject.GetID()) == 0)
			{
				FG_leafExpandedTracker.emplace(currentObject.GetID(), false);
			}

			ImGui::SetNextItemOpen(FG_leafExpandedTracker.at(currentObject.GetID()));
			b_nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)currentObject.GetID(), nodeFlags, charName);
			FG_leafExpandedTracker.at(currentObject.GetID()) = b_nodeOpen;
		}
		else
		{
			ImGui::TreeNodeEx((void*)(intptr_t)currentObject.GetID(), nodeFlags, charName);

			if (FG_leafExpandedTracker.count(currentObject.GetID()))
			{
				FG_leafExpandedTracker.erase(currentObject.GetID());
			}
		}

		// Don't change the background color of the tree node, change the background of the table row because it will fill the entire rect
		if (b_objectFocused)
		{
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::GetColor32("treeSelectableSelected"));
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::GetColor32("treeSelectableHovered"));
		}
		if (ImGui::IsItemActive())
		{
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, FL::GetColor32("treeSelectableActive"));
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();


			// Get Scene View Dimensions from its ImGui window
			Vector2 sceneViewDimensions;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));
			FL::PushWindowStyles();
			ImGui::Begin("Scene View", 0, 16 | 8);
			sceneViewDimensions = Vector2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
			ImGui::End();
			FL::PopWindowStyles();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();


		// Control click a Hierarchy item to focus on it in the Scene View
		if (ImGui::GetIO().KeyCtrl && ImGui::IsItemClicked())
		{
			FL::Transform* transform = currentObject.GetTransform();
			Vector2 position = transform->GetAbsolutePosition();
			FG_sceneViewScrolling = Vector2(position.x * -FG_sceneViewGridStep.x + (sceneViewDimensions.x / 2), position.y * FG_sceneViewGridStep.y + (sceneViewDimensions.y / 2));
		}

		// Hold Alt key and hover object in Hierarchy for ToolTip with information about that GameObject
		if (ImGui::IsItemHovered() && ImGui::GetIO().KeyAlt)
		{	
			FL::BeginToolTip("GameObject Data");
			FL::MoveScreenCursor(0, 3);
			FL::RenderSeparator(3, 3);
			FL::MoveScreenCursor(0, 3);
			FL::RenderToolTipLong("Object ID    ", currentObject.GetID());
			FL::MoveScreenCursor(0, 3);
			FL::RenderToolTipLong("Parent ID    ", currentObject.GetParentID());
			FL::MoveScreenCursor(0, 3);
			FL::RenderToolTipLongVector("Children IDs ", currentObject.GetChildren());
			FL::EndToolTip();
		}

		Vector2 savedCursorPos = ImGui::GetCursorPos();

		// Right click menu
		if (ImGui::BeginPopupContextItem())
		{
			FL::PushMenuStyles();
			if (ImGui::MenuItem("Create Child"))
			{
				GameObject* childObject = FL::CreateGameObject(currentObject.GetID());
				SetFocusedGameObjectID(childObject->GetID());
				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (currentObject.IsPrefab())
			{
				std::string prefabName = "Prefab: " + currentObject.GetPrefabName();
				ImGui::Text(prefabName.c_str());
				ImGui::Separator();
				if (ImGui::MenuItem("Save Prefab"))
				{
					FL::CreatePrefab(FL::GetDir("prefabs") + "/" + currentObject.GetPrefabName() + ".prf", currentObject);
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Unpack prefab"))
				{
					currentObject.SetIsPrefab(false);
					currentObject.SetPrefabName("");
					currentObject.SetPrefabSpawnLocation(Vector2(0, 0));
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Create New Prefab"))
				{
					b_openPrefabModal = true;
					modalOpenOn = currentObject.GetID();
					ImGui::CloseCurrentPopup();
				}
			}
			else
			{
				if (ImGui::MenuItem("Create Prefab"))
				{
					b_openPrefabModal = true;
					modalOpenOn = currentObject.GetID();
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Delete GameObject"))
			{
				queuedForDelete = currentObject.GetID();
				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Lock in view"))
			{
				if (FG_b_sceneViewLockedOnObject && FG_sceneViewLockedObjectID == currentObject.GetID())
				{
					FG_b_sceneViewLockedOnObject = false;
					FG_sceneViewLockedObjectID = currentObject.GetID();
				}
				else if (!FG_b_sceneViewLockedOnObject)
				{
					FG_sceneViewLockedObjectID = currentObject.GetID();
					FG_b_sceneViewLockedOnObject = true;
				}

				ImGui::CloseCurrentPopup();
			}
			FL::PopMenuStyles();

			ImGui::EndPopup();
		}
		
		if (ImGui::IsItemHovered() && ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_MouseLeft))
		{
			node_clicked = currentObject.GetID();
			SetFocusedGameObjectID(currentObject.GetID());
		}

		// Add As Child Drag and Drop
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
		{
			int ID = currentObject.GetID();
			ImGui::SetDragDropPayload(FL::F_hierarchyTarget.c_str(), &ID, sizeof(int));
			ImGui::Text("Set Parent");
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(FL::F_hierarchyTarget.c_str()))
			{
				IM_ASSERT(payload->DataSize == sizeof(int));
				int ID = *(const int*)payload->Data;

				// Save Dropped Object
				GameObject *dropped = FL::GetObjectByID(ID);
				// Remove dropped object from its previous parents children
				if (dropped->GetParentID() != -1)
				{
					GameObject *parent = dropped->GetParent();
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
			std::string prefabIDImageButton = "PrefabIDImage" + std::to_string(currentObject.GetID());
			std::string prefabIDContextMenu = "PrefabIDContext" + std::to_string(currentObject.GetID());
			ImGui::TableSetColumnIndex(2);
			ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() - 1, ImGui::GetCursorPosY()));
			FL::RenderImageButton(prefabIDImageButton.c_str(), FL::GetTexture("prefabCube"), Vector2(16), 0, Vector2(4), FL::GetColor("transparent"), FL::GetColor("white"), FL::GetColor("buttonHovered"), FL::GetColor("buttonActive"));
			FL::PushMenuStyles();
			if (ImGui::BeginPopupContextItem(prefabIDContextMenu.c_str(), ImGuiPopupFlags_MouseButtonLeft))
			{
				if (ImGui::MenuItem("Unpack prefab"))
				{
					currentObject.SetIsPrefab(false);
					currentObject.SetPrefabName("");
					currentObject.SetPrefabSpawnLocation(Vector2(0, 0));
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			FL::PopMenuStyles();
		}
		
		if (currentObject.HasChildren() && b_nodeOpen)
		{
			std::vector<long> childrenIDs = currentObject.GetChildren();
			
			for (long childID : childrenIDs)
			{
				GameObject* child = FL::GetObjectByID(childID);

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
}