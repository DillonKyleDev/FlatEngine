#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameObject.h"
#include "Project.h"
#include "TagList.h"
#include "Scene.h"
#include "Transform.h"
#include "Sprite.h"
#include "Button.h"
#include "Camera.h"
#include "Canvas.h"
#include "Script.h"
#include "Text.h"
#include "Audio.h"
#include "CharacterController.h"
#include "Component.h"
#include "TileMap.h"

#include "imgui.h"

namespace FL = FlatEngine;


namespace FlatGui 
{
	void RenderInspector()
	{
		ImGui::SetNextWindowScroll(Vector2(0, 0));
		FL::BeginWindow("Inspector");
		// {

			// For border around components section
			Vector2 wPos;
			Vector2 wSize;

			long focusedObjectID = GetFocusedGameObjectID();

			if (focusedObjectID != -1 && FL::GetObjectByID(focusedObjectID) != nullptr)
			{
				// Get focused GameObject
				GameObject* focusedObject = FL::GetObjectByID(focusedObjectID);

				// Lambda
				auto L_ShowAddComponentsWindow = [&]()
				{
					FL::PushMenuStyles();

					// Add all the component types you can add to this GameObject
					if (!focusedObject->HasComponent("Sprite"))
					{
						if (ImGui::MenuItem("Sprite"))
						{
							focusedObject->AddSprite();
							ImGui::CloseCurrentPopup();
						}
					}

					if (!focusedObject->HasComponent("Button"))
					{
						if (ImGui::MenuItem("Button"))
						{
							focusedObject->AddButton();
							ImGui::CloseCurrentPopup();
						}
					}

					if (!focusedObject->HasComponent("Camera"))
					{
						if (ImGui::MenuItem("Camera"))
						{
							focusedObject->AddCamera();
							ImGui::CloseCurrentPopup();
						}
					}

					if (!focusedObject->HasComponent("Canvas"))
					{
						if (ImGui::MenuItem("Canvas"))
						{
							focusedObject->AddCanvas();
							ImGui::CloseCurrentPopup();
						}
					}

					if (!focusedObject->HasComponent("Animation"))
					{
						if (ImGui::MenuItem("Animation"))
						{
							focusedObject->AddAnimation();
							ImGui::CloseCurrentPopup();
						}
					}

					if (!focusedObject->HasComponent("Audio"))
					{
						if (ImGui::MenuItem("Audio"))
						{
							focusedObject->AddAudio();
							ImGui::CloseCurrentPopup();
						}
					}

					if (!focusedObject->HasComponent("Text"))
					{
						if (ImGui::MenuItem("Text"))
						{
							focusedObject->AddText();
							ImGui::CloseCurrentPopup();
						}
					}

					if (ImGui::MenuItem("Script"))
					{
						focusedObject->AddScript();
						ImGui::CloseCurrentPopup();
					}

					if (focusedObject->GetCharacterController() != nullptr)
					{
						if (ImGui::MenuItem("CharacterController"))
						{
							focusedObject->AddCharacterController();
							ImGui::CloseCurrentPopup();
						}
					}

					if (focusedObject->GetBody() == nullptr)
					{
						if (ImGui::MenuItem("Body"))
						{
							focusedObject->AddBody();
							ImGui::CloseCurrentPopup();
						}					
					}

					if (focusedObject->GetJointMaker() == nullptr)
					{
						if (ImGui::MenuItem("JointMaker"))
						{
							focusedObject->AddJointMaker();
							ImGui::CloseCurrentPopup();
						}						
					}

					if (!focusedObject->HasComponent("TileMap"))
					{
						if (ImGui::MenuItem("TileMap"))
						{
							focusedObject->AddTileMap();
							ImGui::CloseCurrentPopup();
						}
					}

					FL::PopMenuStyles();
				};
	
				std::string nameLabel = "Name";
				std::string objectName = focusedObject->GetName();
				if (FL::RenderInput("##GameObjectName", nameLabel, objectName))
				{
					focusedObject->SetName(objectName);
				}


				bool b_isActive = focusedObject->IsActive();
				FL::MoveScreenCursor(3, 2);
				if (FL::RenderCheckbox("Active", b_isActive))
				{
					focusedObject->SetActive(b_isActive);
				}
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - 90, 5);
				FL::MoveScreenCursor(0, -2);

				static Vector2 mousePos = ImGui::GetCursorScreenPos();
				TagList &tagList = focusedObject->GetTagList();			
				if (FL::RenderButton("Tags"))
				{
					mousePos = Vector2(ImGui::GetIO().MousePos.x - 200, ImGui::GetIO().MousePos.y);
					ImGui::SetNextWindowPos(mousePos);
				}

				FL::PushMenuStyles();
				if (ImGui::BeginPopupContextItem("TagsPopup", ImGuiPopupFlags_MouseButtonLeft))
				{
					std::string labels[2] = { "Is", "Collides" };
					if (FL::PushTable("TagsTable", 3, FL::F_resizeableTableFlags, Vector2(-1)))
					{
						FL::RenderTextTableRow("TagsTableHeaders", "Tag", "Has", "Collides");

						for (std::string tag : FL::F_TagsAvailable)
						{
							std::string tableRowId = tag + "TagCheckboxTableRow";
							RenderTagListTableRow(tableRowId.c_str(), tag, tagList);
						}

						FL::PopTable();
					}
					ImGui::EndPopup();
				}
				FL::PopMenuStyles();


				// Three Dots More Options Button
				ImGui::SameLine(0, 5);
				FL::MoveScreenCursor(0, -1);
				FL::RenderImageButton("##InspectorMoreButton", FL::GetTexture("threeDots"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("transparent"));		
				FL::PushMenuStyles();
				if (ImGui::BeginPopupContextItem("##InspectorMoreContext", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
				{
					if (ImGui::MenuItem("Delete GameObject"))
					{
						SetFocusedGameObjectID(-1);
						FL::DeleteGameObject(focusedObject->GetID());
						ImGui::CloseCurrentPopup();
					}
	
					ImGui::EndPopup();
				}
				FL::PopMenuStyles();

				ImGui::SameLine(0,2);
				FL::MoveScreenCursor(0, -1);
				static bool b_expandAll = true;
				if (b_expandAll)
				{
					if (FL::RenderImageButton("##ExpandCollapseAllComponents" + std::to_string(focusedObjectID), FL::GetTexture("expandFlipped"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("transparent")))
					{
						for (Component* component : focusedObject->GetComponents())
						{
							component->SetCollapsed(b_expandAll);
						}
						b_expandAll = !b_expandAll;
					}
					if (ImGui::IsItemHovered())
					{
						FL::RenderTextToolTip("Collapse all");
					}
				}
				else
				{
					if (FL::RenderImageButton("##ExpandCollapseAllComponents" + std::to_string(focusedObjectID), FL::GetTexture("expand"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("transparent"), FL::GetColor("white")))
					{
						for (Component* component : focusedObject->GetComponents())
						{
							component->SetCollapsed(b_expandAll);
						}
						b_expandAll = !b_expandAll;
					}
					if (ImGui::IsItemHovered())
					{
						FL::RenderTextToolTip("Expand all");
					}
				}


				FL::RenderSectionHeader("Components");				

				// For scrolling components section with background
				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("componentsScrollingBg"));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 5));
				ImGui::BeginChild("ComponentsSectionBg", Vector2(0,ImGui::GetContentRegionAvail().y - 30), FL::F_childFlags);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				// {
				
					// Border around components section
					wPos = ImGui::GetWindowPos();
					wSize = ImGui::GetWindowSize();

					Vector2 windowPos;
					Vector2 windowSize;
		
					if (focusedObject != nullptr)
					{
						FL::Component* queuedForDelete = nullptr;
						
						Transform* transform = focusedObject->GetTransform();
						if (transform != nullptr)
						{
							BeginComponent(transform, queuedForDelete);
							if (!transform->IsCollapsed())
							{
								RenderTransformComponent(transform);
							}
							EndComponent(transform);
						}

						Sprite* sprite = focusedObject->GetSprite();
						if (sprite != nullptr)
						{
							BeginComponent(sprite, queuedForDelete);
							if (!sprite->IsCollapsed())
							{
								RenderSpriteComponent(sprite);
							}
							EndComponent(sprite);
						}
						
						Camera* camera = focusedObject->GetCamera();
						if (camera != nullptr)
						{
							BeginComponent(camera, queuedForDelete);
							if (!camera->IsCollapsed())
							{
								RenderCameraComponent(camera);
							}
							EndComponent(camera);
						}
						
						std::vector<Script*> scripts = focusedObject->GetScripts();
						for (Script* script : scripts)
						{
							BeginComponent(script, queuedForDelete);
							if (!script->IsCollapsed())
							{
								RenderScriptComponent(script);
							}
							EndComponent(script);
						}
						
						Button* button = focusedObject->GetButton();
						if (button != nullptr)
						{
							BeginComponent(button, queuedForDelete);
							if (!button->IsCollapsed())
							{
								RenderButtonComponent(button);
							}
							EndComponent(button);
						}
						
						Canvas* canvas = focusedObject->GetCanvas();
						if (canvas != nullptr)
						{
							BeginComponent(canvas, queuedForDelete);
							if (!canvas->IsCollapsed())
							{
								RenderCanvasComponent(canvas);
							}
							EndComponent(canvas);
						}
						
						Animation* animation = focusedObject->GetAnimation();
						if (animation != nullptr)
						{
							BeginComponent(animation, queuedForDelete);
							if (!animation->IsCollapsed())
							{
								RenderAnimationComponent(animation);
							}
							EndComponent(animation);
						}

						Audio* audio = focusedObject->GetAudio();
						if (audio != nullptr)
						{
							BeginComponent(audio, queuedForDelete);
							if (!audio->IsCollapsed())
							{
								RenderAudioComponent(audio);
							}
							EndComponent(audio);
						}
						
						Text* text = focusedObject->GetText();
						if (text != nullptr)
						{
							BeginComponent(text, queuedForDelete);
							if (!text->IsCollapsed())
							{
								RenderTextComponent(text);
							}
							EndComponent(text);
						}
						
						CharacterController* characterController = focusedObject->GetCharacterController();
						if (characterController != nullptr)
						{
							BeginComponent(characterController, queuedForDelete);
							if (!characterController->IsCollapsed())
							{
								RenderCharacterControllerComponent(characterController);
							}
							EndComponent(characterController);
						}
						
						Body* body = focusedObject->GetBody();
						if (body != nullptr)
						{						
							BeginComponent(body, queuedForDelete);
							if (!body->IsCollapsed())
							{
								RenderBodyComponent(body);
							}
							EndComponent(body);
						}	

						JointMaker* jointMaker = focusedObject->GetJointMaker();
						if (jointMaker != nullptr)
						{							
							BeginComponent(jointMaker, queuedForDelete);
							if (!jointMaker->IsCollapsed())
							{
								RenderJointMakerComponent(jointMaker);
							}
							EndComponent(jointMaker);
						}
						
						TileMap* tileMap = focusedObject->GetTileMap();
						if (tileMap != nullptr)
						{
							BeginComponent(tileMap, queuedForDelete);
							if (!tileMap->IsCollapsed())
							{
								RenderTileMapComponent(tileMap);
							}
							EndComponent(tileMap);
						}

						if (queuedForDelete != nullptr)
						{
							focusedObject->RemoveComponent(queuedForDelete);
							queuedForDelete = nullptr;
						}
					}

				// }
				ImGui::EndChild(); // ComponentsSectionBg

				
				FL::RenderButton("Add Component", Vector2(ImGui::GetContentRegionAvail().x, 0));
				if (ImGui::BeginPopupContextItem("##AddComponent", ImGuiPopupFlags_MouseButtonLeft))
				{
					L_ShowAddComponentsWindow();
					ImGui::EndPopup();
				}
			}

		// }
		FL::EndWindow();
	}
}