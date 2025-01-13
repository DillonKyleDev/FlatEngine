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
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
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

					if (!focusedObject->HasComponent("CharacterController"))
					{
						if (ImGui::MenuItem("CharacterController"))
						{
							focusedObject->AddCharacterController();
							ImGui::CloseCurrentPopup();
						}
					}

					if (!focusedObject->HasComponent("RigidBody"))
					{
						if (ImGui::MenuItem("RigidBody"))
						{
							focusedObject->AddRigidBody();
							ImGui::CloseCurrentPopup();
						}
					}

					if (ImGui::MenuItem("BoxCollider"))
					{
						focusedObject->AddBoxCollider();
						ImGui::CloseCurrentPopup();
					}

					if (ImGui::MenuItem("CircleCollider"))
					{
						focusedObject->AddCircleCollider();
						ImGui::CloseCurrentPopup();
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
				ImGui::SameLine(ImGui::GetContentRegionAvail().x - 100, 5);


				static bool b_expandAll = true;
				if (b_expandAll)
				{
					if (FL::RenderImageButton("##ExpandCollapseAllComponents" + std::to_string(focusedObjectID), FL::GetTexture("expandFlipped")))
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
					if (FL::RenderImageButton("##ExpandCollapseAllComponents" + std::to_string(focusedObjectID), FL::GetTexture("expand")))
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
				ImGui::SameLine();

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
					std::string labels[2] = { "Tag", "Ignore" };
					if (FL::PushTable("TagsTable", 3, FL::F_resizeableTableFlags))
					{
						FL::RenderTextTableRow("TagsTableHeaders", "Tag", "Has", "Ignore");

						for (std::pair<std::string, bool> tag : tagList.GetTagsMap())
						{
							std::string tableRowId = tag.first + "TagCheckboxTableRow";
							RenderTagListTableRow(tableRowId.c_str(), tag.first, tagList);
						}

						FL::PopTable();
					}
					ImGui::EndPopup();
				}
				FL::PopMenuStyles();


				// Three Dots More Options Button
				ImGui::SameLine(0, 5);
				FL::RenderImageButton("##InspectorMoreButton", FL::GetTexture("threeDots"), Vector2(16, 16), 1, FL::GetColor("transparent"));
				FL::MoveScreenCursor(0, 4);
				FL::PushMenuStyles();
				if (ImGui::BeginPopupContextItem("##InspectorMoreContext", ImGuiPopupFlags_MouseButtonLeft)) // <-- use last item id as popup id
				{
					if (ImGui::BeginMenu("Add Component"))
					{
						L_ShowAddComponentsWindow();
						ImGui::EndMenu();
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Delete GameObject"))
					{
						SetFocusedGameObjectID(-1);
						FL::DeleteGameObject(focusedObject->GetID());
						ImGui::CloseCurrentPopup();
					}
	
					ImGui::EndPopup();
				}
				FL::PopMenuStyles();
		


				FL::RenderSectionHeader("Components");


				// For scrolling components section with background
				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("componentsScrollingBg"));
				ImGui::BeginChild("ComponentsSectionBg", Vector2(0,ImGui::GetContentRegionAvail().y - 30), FL::F_childFlags);
				ImGui::PopStyleColor();
				// {
				
					// Border around each component
					auto wPos = ImGui::GetWindowPos();
					auto wSize = ImGui::GetWindowSize();
		
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
						
						std::vector<BoxCollider*> boxColliders = focusedObject->GetBoxColliders();
						for (BoxCollider* boxCollider : boxColliders)
						{
							if (!boxCollider->IsTileMapCollider())
							{
								BeginComponent(boxCollider, queuedForDelete);
								if (!boxCollider->IsCollapsed())
								{
									RenderBoxColliderComponent(boxCollider);
								}
								EndComponent(boxCollider);
							}
						}
						
						std::vector<CircleCollider*> circleColliders = focusedObject->GetCircleColliders();
						for (CircleCollider* circleCollider : circleColliders)
						{
							BeginComponent(circleCollider, queuedForDelete);
							if (!circleCollider->IsCollapsed())
							{
								RenderCircleColliderComponent(circleCollider);
							}
							EndComponent(circleCollider);
						}
						
						RigidBody* rigidBody = focusedObject->GetRigidBody();
						if (rigidBody != nullptr)
						{
							BeginComponent(rigidBody, queuedForDelete);
							if (!rigidBody->IsCollapsed())
							{
								RenderRigidBodyComponent(rigidBody);
							}
							EndComponent(rigidBody);
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

				// Border around Components Section
				ImGui::GetWindowDrawList()->AddRect({ wPos.x, wPos.y - 1 }, { wPos.x + wSize.x, wPos.y + wSize.y + 1}, FL::GetColor32("componentSectionBorder"), 1);

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