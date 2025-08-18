#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameObject.h"
#include "Scene.h"
#include "Project.h"
#include "Transform.h"

#include "imgui.h"


namespace FL = FlatEngine;

namespace FlatGui
{
	void RenderPersistantHierarchy()
	{
		FL::BeginWindow("Persistant GameObjects");
		// {

			// Render Loaded Project text and threeDots more menu button
			std::string projectName = "Loaded Project: " + FL::GetFilenameFromPath(FL::F_LoadedProject.GetPath());

			FL::RenderSectionHeader(projectName);
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 22, 0);

			static std::string newSceneName = "";
			static bool b_openSceneModal = false;
			if (FL::RenderInputModal("Create Scene", "Enter a name for the Scene", newSceneName, b_openSceneModal))
			{
				std::string scenePath = FL::CreateNewSceneFile(newSceneName);
				FL::SaveScene(FL::GetLoadedScene(), newSceneName);
				FL::LoadScene(scenePath);
				FL::SetProjectLoadedScenePath(scenePath);
			}

			// Three dots
			FL::MoveScreenCursor(0, -2);
			FL::RenderImageButton("##PersistantSaveLoadMenu", FL::GetTexture("threeDots"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("transparent"));
			FL::PushMenuStyles();
			if (ImGui::BeginPopupContextItem("##InspectorMoreContext", ImGuiPopupFlags_MouseButtonLeft))
			{
				if (ImGui::MenuItem("Save all"))
				{
					if (FL::GetLoadedScenePath() == "")
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
					if (FL::GetLoadedScenePath() == "")
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
			std::map<long, GameObject>& sceneObjects = FL::GetSceneObjects();
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
				if (ImGui::BeginTable("##PersistantHierarchyTable", 3, FL::F_tableFlags, Vector2(ImGui::GetContentRegionAvail().x - 1, ImGui::GetContentRegionAvail().y - 1)))
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
								for (std::map<long, GameObject>::iterator iter = persistantObjects.begin(); iter != persistantObjects.end(); iter++)
								{
									iter->second.SetActive(false);
								}
								b_allAreVisible = false;
							}
						}
						else
						{
							FL::MoveScreenCursor(-1, 0);
							if (FL::RenderImageButton("##SetAllVisible", FL::GetTexture("hide"), Vector2(16, 16), 0, Vector2(4), FL::GetColor("button"), FL::GetColor("white"), FL::GetColor("buttonHovered"), FL::GetColor("buttonActive")))
							{
								for (std::map<long, GameObject>::iterator iter = persistantObjects.begin(); iter != persistantObjects.end(); iter++)
								{
									iter->second.SetActive(true);
								}
								b_allAreVisible = true;
							}
						}

						ImGui::TableSetColumnIndex(1);
						ImGui::PushStyleColor(ImGuiCol_Text, FL::GetColor("logText"));
						ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX() + 7, ImGui::GetCursorPosY() + 4)); // Indent the text
						ImGui::Text("PERSISTANT GAMEOBJECTS");
						ImGui::PopStyleColor();

						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(0, 0));
						ImGui::TableSetColumnIndex(2);
						ImGui::PopStyleVar();

						for (std::map<long, GameObject>::iterator object = persistantObjects.begin(); object != persistantObjects.end(); object++)
						{
							// If this object does not have a parent we render it and all of its children.
							if (object->second.GetParentID() == -1)
							{
								GameObject& currentObject = object->second;
								std::string name = currentObject.GetName();
								const char* charName = name.c_str();
								float indent = 0;

								AddObjectToHierarchy(currentObject, charName, node_clicked, queuedForDelete, parentToUnparent, childToRemove, indent);
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
					ImGui::EndTable(); // Persistant Hierarchy Table
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

		// }
		FL::EndWindow(); // Hierarchy
	}
}