#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameObject.h"
#include "Project.h"
#include "Camera.h"
#include "Transform.h"
#include "Sprite.h"
#include "Text.h"
#include "Texture.h"
#include "Scene.h"
#include "Audio.h"

#include "imgui.h"
#include "math.h"
#include <fstream>


namespace FL = FlatEngine;

namespace FlatGui 
{	
	void RenderAnimator()
	{
		FL::BeginWindow("Animator");
		// {
			
			FL::BeginResizeWindowChild("Animated Properties", FL::GetColor("outerWindow"), 0, Vector2(0));			
			// {
			
				std::string animationName = "- No Animation Selected -";
				if (GetFocusedAnimation() != nullptr && GetFocusedAnimation()->animationName != "")
				{
					animationName = GetFocusedAnimation()->animationName;
				}

				FL::BeginWindowChild("Manage Animation", FL::GetColor("outerWindow"), 0, Vector2(0));
				// {

					static std::string animationFilePath;
					if (GetFocusedAnimation() != nullptr)
					{
						animationFilePath = GetFocusedAnimation()->animationPath;
					}

					static std::string newFileName = "";
					static bool b_openAnimationModal = false;
					if (FL::RenderInputModal("Create Animation", "Enter a name for the Animation", newFileName, b_openAnimationModal))
					{
						FL::CreateNewAnimationFile(newFileName, FL::GetDir("animations"));
						SetFocusedAnimation(FL::LoadAnimationFile(animationFilePath));
					}

					FL::RenderSectionHeader(animationName);

					// Three dots
					FL::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 24, -27);
					FL::RenderImageButton("##AnimatorHamburgerMenu", FL::GetTexture("threeDots"), Vector2(16, 16), 1, Vector2(1,1), FL::GetColor("transparent"));
					FL::PushMenuStyles();
					if (ImGui::BeginPopupContextItem("##AnimationHamburgerMenu", ImGuiPopupFlags_MouseButtonLeft))
					{
						if (ImGui::MenuItem("Save Animation"))
						{
							if (animationFilePath != "")
								FL::SaveAnimationFile(GetFocusedAnimation(), animationFilePath);
							ImGui::CloseCurrentPopup();
						}
						ImGui::Separator();
						if (ImGui::MenuItem("Load Animation"))
						{
							animationFilePath = FL::OpenLoadFileExplorer();
							if (animationFilePath != "")
							{
								SetFocusedAnimation(FL::LoadAnimationFile(animationFilePath));
								FL::F_LoadedProject.SetLoadedPreviewAnimationPath(animationFilePath);
							}
							ImGui::CloseCurrentPopup();
						}
						ImGui::Separator();
						if (ImGui::MenuItem("New Animation"))
						{
							b_openAnimationModal = true;
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
					FL::PopMenuStyles();					


					FL::MoveScreenCursor(0, 5);
					std::shared_ptr<Animation::S_AnimationProperties> animProps = GetFocusedAnimation();

					const char* properties[] = { "- select property -", "Event", "Transform", "Sprite", "Camera", "Script", "Button", "Canvas", "Audio", "Text", "CharacterController" };
					static int current_property = 0;
					static std::string nodeClicked = "";

					// Lambda
					auto L_PushBackKeyFrame = [&](std::string property)
					{
						// Add property to animation object
						if (property == "Event")
						{
							std::shared_ptr<Animation::S_Event> eventProperties = std::make_shared<Animation::S_Event>();
							eventProperties->name = "Event";
							animProps->eventProps.push_back(eventProperties);
						}
						else if (property == "Transform")
						{
							std::shared_ptr<Animation::S_Transform> transformProperties = std::make_shared<Animation::S_Transform>();
							transformProperties->name = "Transform";
							animProps->transformProps.push_back(transformProperties);
						}
						else if (property == "Sprite")
						{
							std::shared_ptr<Animation::S_Sprite> spriteProperties = std::make_shared<Animation::S_Sprite>();
							spriteProperties->name = "Sprite";
							animProps->spriteProps.push_back(spriteProperties);
						}
						else if (property == "Camera")
						{
							std::shared_ptr<Animation::S_Camera> cameraProperties = std::make_shared<Animation::S_Camera>();
							cameraProperties->name = "Camera";
							animProps->cameraProps.push_back(cameraProperties);
						}
						else if (property == "Script")
						{
							std::shared_ptr<Animation::S_Script> scriptProperties = std::make_shared<Animation::S_Script>();
							scriptProperties->name = "Script";
							animProps->scriptProps.push_back(scriptProperties);
						}
						else if (property == "Button")
						{
							std::shared_ptr<Animation::S_Button> buttonProperties = std::make_shared<Animation::S_Button>();
							buttonProperties->name = "Button";
							animProps->buttonProps.push_back(buttonProperties);
						}
						else if (property == "Canvas")
						{
							std::shared_ptr<Animation::S_Canvas> canvasProperties = std::make_shared<Animation::S_Canvas>();
							canvasProperties->name = "Canvas";
							animProps->canvasProps.push_back(canvasProperties);
						}
						else if (property == "Audio")
						{
							std::shared_ptr<Animation::S_Audio> audioProperties = std::make_shared<Animation::S_Audio>();
							audioProperties->name = "Audio";
							animProps->audioProps.push_back(audioProperties);
						}
						else if (property == "Text")
						{
							std::shared_ptr<Animation::S_Text> textProperties = std::make_shared<Animation::S_Text>();
							textProperties->name = "Text";
							animProps->textProps.push_back(textProperties);
						}
						else if (property == "CharacterController")
						{
							std::shared_ptr<Animation::S_CharacterController> characterControllerProperties = std::make_shared<Animation::S_CharacterController>();
							characterControllerProperties->name = "CharacterController";
							animProps->characterControllerProps.push_back(characterControllerProperties);
						}
					};
					// Lambda
					auto L_RemoveKeyFrame = [&](std::string property)
						{
							// Add property to animation object
							if (property == "Event")
							{
								animProps->eventProps.clear();
							}
							else if (property == "Transform")
							{
								animProps->transformProps.clear();
							}
							else if (property == "Sprite")
							{
								animProps->spriteProps.clear();
							}
							else if (property == "Camera")
							{
								animProps->cameraProps.clear();
							}
							else if (property == "Script")
							{
								animProps->scriptProps.clear();
							}
							else if (property == "Button")
							{
								animProps->buttonProps.clear();
							}
							else if (property == "Canvas")
							{
								animProps->canvasProps.clear();
							}
							else if (property == "Audio")
							{
								animProps->audioProps.clear();
							}
							else if (property == "Text")
							{
								animProps->textProps.clear();
							}
							else if (property == "CharacterController")
							{
								animProps->characterControllerProps.clear();
							}
						};

					if (animProps != nullptr && animProps->animationName != "")
					{						
						Animation* animation = nullptr;
						if (objectForFocusedAnimation != nullptr)
						{
							animation = objectForFocusedAnimation->GetAnimation();
						}

						FL::RenderCheckbox("Loop Animation", animProps->b_loop);

						// Copy of above
						//if (FL::RenderCheckbox("Loop Animation", animProps->b_loop) && animation != nullptr && animation->IsPlaying())
						//{
						//	//animation->Stop();
						//	//animation->Play(FL::GetEngineTime());
						//}

						static std::string selected_property = "";			
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
						ImGui::Text("Add Properties");
						FL::MoveScreenCursor(0, 3);
						std::vector<std::string> props = std::vector<std::string>();
						FL::PushComboStyles();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 34);
						if (ImGui::BeginCombo("##properties", properties[current_property]))
						{
							for (int n = 0; n < IM_ARRAYSIZE(properties); n++)
							{
								if (animProps->eventProps.size() == 0 && properties[n] == "Event" ||
									animProps->transformProps.size() == 0 && properties[n] == "Transform" ||
									animProps->spriteProps.size() == 0 && properties[n] == "Sprite" ||
									animProps->cameraProps.size() == 0 && properties[n] == "Camera" ||
									animProps->scriptProps.size() == 0 && properties[n] == "Script" ||
									animProps->buttonProps.size() == 0 && properties[n] == "Button" ||
									animProps->canvasProps.size() == 0 && properties[n] == "Canvas" ||
									animProps->audioProps.size() == 0 && properties[n] == "Audio" ||
									animProps->textProps.size() == 0 && properties[n] == "Text" ||
									animProps->characterControllerProps.size() == 0 && properties[n] == "CharacterController"
									)
								{
									bool b_isSelected = (properties[current_property] == properties[n]);
									if (ImGui::Selectable(properties[n], b_isSelected))
									{
										current_property = n;
									}
									if (b_isSelected)
									{
										ImGui::SetItemDefaultFocus();
									}
								}
							}
							ImGui::EndCombo();
						}
						FL::PopComboStyles();
						ImGui::SameLine(0, 5);
						if (FL::RenderButton("Add"))
						{
							L_PushBackKeyFrame(properties[current_property]);
							// Reset selector box to default
							current_property = 0;
						}

						//FL::RenderSeparator(3, 3);

						// List properties in this animation
						ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("innerWindow"));
						ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, Vector2(0, 0));
						FL::PushMenuStyles();

						// Conditionally begin the table
						if (animProps->eventProps.size() > 0 ||
							animProps->transformProps.size() > 0 ||
							animProps->spriteProps.size() > 0 ||
							animProps->cameraProps.size() > 0 ||
							animProps->scriptProps.size() > 0 ||
							animProps->buttonProps.size() > 0 ||
							animProps->canvasProps.size() > 0 ||
							animProps->audioProps.size() > 0 ||
							animProps->textProps.size() > 0 ||
							animProps->characterControllerProps.size() > 0)
						if (ImGui::BeginTable("##AnimationProperties", 1, FL::F_tableFlags))
						{
							ImGui::TableSetupColumn("##PROPERTY", 0, ImGui::GetContentRegionAvail().x + 1);
		

							auto RenderPropertyButton = [&](std::string property, int size, std::string& nodeClicked)
							{
								ImGuiTreeNodeFlags node_flags;

								std::string treeID = property + "_node";
								if (nodeClicked == property)
								{
									node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;
								}
								else
								{
									node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
								}

								//// TreeNode Opener - No TreePop because it's a leaf
								if (size > 0)
								{
									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									//ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);

									ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), node_flags, property.c_str());
									if (ImGui::IsItemClicked())
									{
										nodeClicked = property;
									}

									ImGui::PushID(treeID.c_str());
									ImGui::PopID();
								}
							};


							RenderPropertyButton("Event", (int)animProps->eventProps.size(), nodeClicked);
							RenderPropertyButton("Transform", (int)animProps->transformProps.size(), nodeClicked);
							RenderPropertyButton("Sprite", (int)animProps->spriteProps.size(), nodeClicked);
							RenderPropertyButton("Camera", (int)animProps->cameraProps.size(), nodeClicked);
							RenderPropertyButton("Script", (int)animProps->scriptProps.size(), nodeClicked);
							RenderPropertyButton("Button", (int)animProps->buttonProps.size(), nodeClicked);
							RenderPropertyButton("Canvas", (int)animProps->canvasProps.size(), nodeClicked);
							RenderPropertyButton("Audio", (int)animProps->audioProps.size(), nodeClicked);
							RenderPropertyButton("Text", (int)animProps->textProps.size(), nodeClicked);
							RenderPropertyButton("CharacterController", (int)animProps->characterControllerProps.size(), nodeClicked);

			
							ImGui::EndTable();
						}
						FL::PopMenuStyles();
						ImGui::PopStyleVar();
						ImGui::PopStyleColor();
					}

				// }
				FL::EndWindowChild(); // Manage Animation

			// }
			FL::EndWindowChild(); // Animator Properties


			ImGui::SameLine(0, 5);
			

			// Timeline Events
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("outerWindow"));
			FL::BeginWindowChild("Animation Timeline");
			ImGui::PopStyleColor();
			// {

				// Border Animation Timeline
				auto propsWindowPos = ImGui::GetWindowPos();
				auto propsWindowSize = ImGui::GetWindowSize();
				//ImGui::GetWindowDrawList()->AddRect({ propsWindowPos.x + 2, propsWindowPos.y + 2 }, { propsWindowPos.x + propsWindowSize.x - 2, propsWindowPos.y + propsWindowSize.y - 2 }, FL::GetColor32("componentBorder"), 0);

				ImGui::BeginChild("Property Header", Vector2(0,0), FL::F_headerFlags);
				// {

					FL::RenderSectionHeader("Animation Timeline");					

					float availableSpace = ImGui::GetContentRegionAvail().x / 2;
					ImGui::SetNextItemWidth(availableSpace);
					ImGui::Text("Property Selected: ");

					// Draw Property selected and background rect for it
					if (nodeClicked != "")
					{
						ImGui::SameLine(0, 5);
						FL::MoveScreenCursor(0, -3);						
						Vector2 textSize = ImGui::CalcTextSize(nodeClicked.c_str());
						Vector2 cursorScreen = ImGui::GetCursorScreenPos();
						ImGui::GetWindowDrawList()->AddRectFilled(cursorScreen, Vector2(cursorScreen.x + textSize.x + 6, cursorScreen.y + textSize.y + 6), ImGui::GetColorU32(FL::GetColor("tableCellLight")), 2);						
						FL::MoveScreenCursor(3, 3);
						ImGui::Text(nodeClicked.c_str());
						
						FL::MoveScreenCursor(0, 6);
						if (FL::RenderButton("Add Keyframe"))
						{
							L_PushBackKeyFrame(nodeClicked);
						}
						ImGui::SameLine(0, 5);			
						if (FL::RenderButton("Remove Property"))
						{
							L_RemoveKeyFrame(nodeClicked);
						}
					}

					if (objectForFocusedAnimation != nullptr)
					{
						// Animate the focused object
						Animation* animation = objectForFocusedAnimation->GetAnimation();
						std::string playID = "##playAnimationPreview";
						std::string stopID = "##StopGameloopIcon";
						bool b_isPreviewing = false;
			
						//if (animation != nullptr)
						//{
						//	b_isPreviewing = animation->IsPlaying();
						//}

						// Play Button
						ImGui::BeginDisabled(b_isPreviewing);
						if (FL::RenderImageButton(playID.c_str(), FL::GetTexture("play"), Vector2(14, 14), 0, Vector2(1,1), FL::GetColor("button"), FL::GetColor("white"), FL::GetColor("buttonHovered"), FL::GetColor("buttonActive")))
						{
							//if (animation != nullptr)
							//{
							//	FG_previewAnimationStartTime = FL::GetEllapsedGameTimeInMs();
							//	FG_previewAnimationTime = FL::GetEllapsedGameTimeInMs();
							//	animation->Play(FG_previewAnimationStartTime);
							//	b_isPreviewing = true;
							//}
						}
						ImGui::EndDisabled();
						ImGui::SameLine(0, 5);

						// Stop button	
						ImGui::BeginDisabled(!b_isPreviewing);
						if (FL::RenderImageButton(stopID.c_str(), FL::GetTexture("stop"), Vector2(14, 14), 0, Vector2(1, 1), FL::GetColor("button"), FL::GetColor("white"), FL::GetColor("buttonHovered"), FL::GetColor("buttonActive")))
						{
							//animation->Stop();
							b_isPreviewing = false;
							FG_previewAnimationTime = 0;
						}
						ImGui::EndDisabled();			
						ImGui::SameLine(0, 5);


						ImGui::PushStyleColor(ImGuiCol_Text, FL::GetColor("col_1"));
						FL::MoveScreenCursor(0, 4);
						ImGui::Text("Preview Animation");
						ImGui::PopStyleColor();
					}

				// }
				ImGui::EndChild(); // Properties Header

				static float animatorGridgridStep = 50;				
				Vector2 canvasP0 = ImGui::GetCursorScreenPos();
				Vector2 canvasSize = ImGui::GetContentRegionAvail();
				if (canvasSize.x < 50.0f) canvasSize.x = 50.0f;
				if (canvasSize.y < 50.0f) canvasSize.y = 50.0f;
				Vector2 canvasP1 = Vector2(canvasP0.x + canvasSize.x, canvasP0.y + canvasSize.y);
				static Vector2 scrolling = Vector2(0, 0);
				Vector2 gridgridStep = Vector2(animatorGridgridStep);
				Vector2 zeroPoint = Vector2(0, 0);
				float maxGridStep = 500;
				float minGridStep = 5;

				AddSceneViewMouseControls("##AnimatorTimelineGridButton", ImGui::GetCursorScreenPos(), canvasSize, scrolling, Vector2(0, 0), gridgridStep, FL::GetColor32("transparent"), false, 0, true, false, 10, minGridStep, maxGridStep);
				animatorGridgridStep = gridgridStep.x;
				if (scrolling.x > 0)
				{
					scrolling.x = 0;
				}
				if (scrolling.y > 0)
				{
					scrolling.y = 0;
				}
				if (scrolling.y < -1500)
				{
					scrolling.y = -1500;
				}

				RenderAnimationTimelineGrid(zeroPoint, scrolling, canvasP0, canvasP1, canvasSize, animatorGridgridStep);

				// Get all keyFramePip positions
				float propertyYPos = -0.5f; // Value in grid space
				int propertyCounter = 0;    // For values in screenspace below
				float animationLength = 0.0f;
				if (animProps != nullptr)
				{
					animationLength = animProps->animationLength;
				}
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				static std::shared_ptr<Animation::S_Property> keyframeQueuedForDelete = nullptr;

				// Lambda
				auto L_RenderPropertyInTimeline = [&](std::string property, std::vector<float> keyFrameTimes, Vector4 rectColor)
				{
					float topYPos = canvasP0.y + scrolling.y + (propertyCounter * animatorGridgridStep);
					float bottomYPos = topYPos + animatorGridgridStep;
					ImU32 color;

					if (nodeClicked == property)
					{
						color = IM_COL32(rectColor.x, rectColor.y, rectColor.z, rectColor.w);
					}
					else
					{
						color = IM_COL32(rectColor.x, rectColor.y, rectColor.z, 20);
					}
		
					// prevents being drawn off screen and introducing scrollbar
					if (topYPos < canvasP0.y)
					{
						topYPos = canvasP0.y;
					}
					if (bottomYPos < canvasP0.y)
					{
						bottomYPos = canvasP0.y;
					}

					if (topYPos > canvasP1.y)
					{
						topYPos = canvasP1.y;
					}
					if (bottomYPos > canvasP1.y)
					{
						bottomYPos = canvasP1.y;
					}

					Vector2 topLeftCorner = Vector2(canvasP0.x, topYPos);
					Vector2 bottomRightCorner = Vector2(canvasP1.x, bottomYPos);
					draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, color);
				};
				// Lambda
				auto L_RenderAnimationTimelineKeyFrames = [](std::shared_ptr<Animation::S_Property> keyFrame, int counter, Vector2& pipPosition, Vector2 zeroPoint, Vector2 scrolling, Vector2 canvasP0, Vector2 canvasP1, Vector2 canvasSize, float gridgridStep)
				{
					std::string ID = keyFrame->name;
					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					float spriteTextureWidth = 10;
					float spriteTextureHeight = 10;
					Vector2 spriteOffset = Vector2(5, 5);
					bool b_spriteScalesWithZoom = false;
					int renderOrder = 1;
					
					if (FL::GetTexture("keyFrame") != nullptr)
					{
						Vector2 pipStartingPoint = FL::AddImageToDrawList(FL::GetTexture("keyFrame"), pipPosition, zeroPoint, 12, 12, Vector2(6, 6), Vector2(1, 1), b_spriteScalesWithZoom, animatorGridgridStep, draw_list);

						ImGui::SetCursorScreenPos(pipStartingPoint);
						std::string pipID = ID + std::to_string(counter) + "-KeyFramePip";
						ImGui::InvisibleButton(pipID.c_str(), Vector2(12, 12), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
						const bool b_isClicked = ImGui::IsItemClicked();
						const bool b_isHovered = ImGui::IsItemHovered();
						const bool b_isActive = ImGui::IsItemActive();   // Held
						const bool b_isRightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);						

						if (b_isHovered)
						{
							ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
						}
	
						if (b_isActive || b_isHovered)
						{
							// Mouse Hover Tooltip - Mouse Over Tooltip
							std::string keyTimeText = "Time: " + std::to_string(keyFrame->time / 1000) + " sec";
							Vector2 m = ImGui::GetIO().MousePos;
							ImGui::SetNextWindowPos(Vector2(m.x + 15, m.y + 5));
							ImGui::Begin("1", NULL, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
							ImGui::Text(keyTimeText.c_str());
							ImGui::End();
						}
				
						if (b_isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0))
						{
							ImGuiIO& inputOutput = ImGui::GetIO();
							if (keyFrame->time + inputOutput.MouseDelta.x / animatorGridgridStep * 1000 >= 0)
							{
								keyFrame->time += inputOutput.MouseDelta.x / animatorGridgridStep * 1000;
							}
						}

						if (b_isClicked)
						{
							nodeClicked = ID;
							FG_SelectedKeyFrameToEdit = keyFrame;
							//ImGui::SetWindowFocus("Keyframe Editor");
						}	

						// Right click menu
						if (ImGui::BeginPopupContextItem())
						{
							FL::PushMenuStyles();
							if (ImGui::MenuItem("Delete"))
							{
								keyframeQueuedForDelete = keyFrame;
								ImGui::CloseCurrentPopup();
							}
							FL::PopMenuStyles();

							ImGui::EndPopup();
						}
					}
				};

				Vector4 rectColor;
				int IDCounter = 0;

				// Draw colored box for transform keyframes
				if (animProps != nullptr)
				{
					if (animProps->eventProps.size() > 0)
					{
						rectColor = Vector4(255, 255, 255, 100);
						std::vector<float> keyFrameTimes = std::vector<float>();
						L_RenderPropertyInTimeline("Event", keyFrameTimes, rectColor);

						for (std::shared_ptr<Animation::S_Event> frame : animProps->eventProps)
						{					
							std::string ID = "Event";
							// Get keyFrame time and convert to seconds
							float keyFrameX = frame->time / 1000;
							Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);					
							if (zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) + 6 < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) > canvasP0.y)
								L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvasP0, canvasP1, canvasSize, animatorGridgridStep);
							IDCounter++;
						}
						propertyYPos--;
						propertyCounter++;
					}
					if (animProps->transformProps.size() > 0)
					{
						rectColor = Vector4(214, 8, 118, 100);
						std::vector<float> keyFrameTimes = std::vector<float>();
						L_RenderPropertyInTimeline("Transform", keyFrameTimes, rectColor);

						for (std::shared_ptr<Animation::S_Transform> frame : animProps->transformProps)
						{
							std::string ID = "Transform";					
							float keyFrameX = frame->time / 1000;
							Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);		
							if (zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) + 6 < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) > canvasP0.y)
								L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvasP0, canvasP1, canvasSize, animatorGridgridStep);
							IDCounter++;
						}
						propertyYPos--;
						propertyCounter++;
					}
					if (animProps->spriteProps.size() > 0)
					{
						rectColor = Vector4(83, 214, 8, 100);
						std::vector<float> keyFrameTimes = std::vector<float>();
						L_RenderPropertyInTimeline("Sprite", keyFrameTimes, rectColor);

						for (std::shared_ptr<Animation::S_Sprite> frame : animProps->spriteProps)
						{
							std::string ID = "Sprite";					
							float keyFrameX = frame->time / 1000;
							Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
							if (zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) + 6 < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) > canvasP0.y)
								L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvasP0, canvasP1, canvasSize, animatorGridgridStep);
							IDCounter++;
						}
						propertyYPos--;
						propertyCounter++;
					}
					if (animProps->cameraProps.size() > 0)
					{
						rectColor = Vector4(206, 108, 4, 100);
						std::vector<float> keyFrameTimes = std::vector<float>();
						L_RenderPropertyInTimeline("Camera", keyFrameTimes, rectColor);

						for (std::shared_ptr<Animation::S_Camera> frame : animProps->cameraProps)
						{
							std::string ID = "Camera";					
							float keyFrameX = frame->time / 1000;
							Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
							if (zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) + 6 < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) > canvasP0.y)
								L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvasP0, canvasP1, canvasSize, animatorGridgridStep);
							IDCounter++;
						}
						propertyYPos--;
						propertyCounter++;
					}
					if (animProps->scriptProps.size() > 0)
					{
						rectColor = Vector4(4, 159, 206, 100);
						std::vector<float> keyFrameTimes = std::vector<float>();
						L_RenderPropertyInTimeline("Script", keyFrameTimes, rectColor);

						for (std::shared_ptr<Animation::S_Script> frame : animProps->scriptProps)
						{
							std::string ID = "Script";					
							float keyFrameX = frame->time / 1000;
							Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
							if (zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) + 6 < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) > canvasP0.y)
								L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvasP0, canvasP1, canvasSize, animatorGridgridStep);
							IDCounter++;
						}
						propertyYPos--;
						propertyCounter++;
					}
					if (animProps->buttonProps.size() > 0)
					{
						rectColor = Vector4(152, 16, 198, 100);
						std::vector<float> keyFrameTimes = std::vector<float>();
						L_RenderPropertyInTimeline("Button", keyFrameTimes, rectColor);

						for (std::shared_ptr<Animation::S_Button> frame : animProps->buttonProps)
						{
							std::string ID = "Button";					
							float keyFrameX = frame->time / 1000;
							Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
							if (zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) + 6 < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) > canvasP0.y)
								L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvasP0, canvasP1, canvasSize, animatorGridgridStep);
							IDCounter++;
						}
						propertyYPos--;
						propertyCounter++;
					}
					if (animProps->canvasProps.size() > 0)
					{
						rectColor = Vector4(224, 81, 15, 100);
						std::vector<float> keyFrameTimes = std::vector<float>();
						L_RenderPropertyInTimeline("Canvas", keyFrameTimes, rectColor);

						for (std::shared_ptr<Animation::S_Canvas> frame : animProps->canvasProps)
						{
							std::string ID = "Canvas";					
							float keyFrameX = frame->time / 1000;
							Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
							if (zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) + 6 < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) > canvasP0.y)
								L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvasP0, canvasP1, canvasSize, animatorGridgridStep);
							IDCounter++;
						}
						propertyYPos--;
						propertyCounter++;
					}
					if (animProps->audioProps.size() > 0)
					{
						rectColor = Vector4(237, 244, 14, 100);
						std::vector<float> keyFrameTimes = std::vector<float>();
						L_RenderPropertyInTimeline("Audio", keyFrameTimes, rectColor);

						for (std::shared_ptr<Animation::S_Audio> frame : animProps->audioProps)
						{
							std::string ID = "Audio";					
							float keyFrameX = frame->time / 1000;
							Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
							if (zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) + 6 < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) > canvasP0.y)
								L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvasP0, canvasP1, canvasSize, animatorGridgridStep);
							IDCounter++;
						}
						propertyYPos--;
						propertyCounter++;
					}
					if (animProps->textProps.size() > 0)
					{
						rectColor = Vector4(15, 224, 200, 100);
						std::vector<float> keyFrameTimes = std::vector<float>();
						L_RenderPropertyInTimeline("Text", keyFrameTimes, rectColor);

						for (std::shared_ptr<Animation::S_Text> frame : animProps->textProps)
						{
							std::string ID = "Text";					
							float keyFrameX = frame->time / 1000;
							Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
							if (zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) + 6 < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) > canvasP0.y)
								L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvasP0, canvasP1, canvasSize, animatorGridgridStep);
							IDCounter++;
						}
						propertyYPos--;
						propertyCounter++;
					}
					if (animProps->characterControllerProps.size() > 0)
					{
						rectColor = Vector4(85, 183, 11, 100);
						std::vector<float> keyFrameTimes = std::vector<float>();
						L_RenderPropertyInTimeline("CharacterController", keyFrameTimes, rectColor);

						for (std::shared_ptr<Animation::S_CharacterController> frame : animProps->characterControllerProps)
						{
							std::string ID = "CharacterController";					
							float keyFrameX = frame->time / 1000;
							Vector2 keyFramePos = Vector2(keyFrameX, propertyYPos);
							if (zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) + 6 < canvasP1.y && zeroPoint.y + (propertyYPos * animatorGridgridStep * -1) > canvasP0.y)
								L_RenderAnimationTimelineKeyFrames(frame, IDCounter, keyFramePos, zeroPoint, scrolling, canvasP0, canvasP1, canvasSize, animatorGridgridStep);
							IDCounter++;
						}
						propertyYPos--;
						propertyCounter++;
					}
				}

				if (keyframeQueuedForDelete != nullptr)
				{
					if (keyframeQueuedForDelete == FG_SelectedKeyFrameToEdit)
					{						
						FG_SelectedKeyFrameToEdit = nullptr;
					}
					FG_FocusedAnimation->RemoveKeyFrame(keyframeQueuedForDelete);
				}

			// }
			FL::EndWindowChild(); // Animation Timeline
		
		// }
		FL::EndWindow(); // Animator
	}

	void RenderAnimationPreview()
	{
		static Vector2 gridStep = Vector2(50, 50);

		FL::BeginWindow("Animator Preview", FG_b_showAnimationPreview);
		// {
		
			Vector2 canvasP0 = ImGui::GetCursorScreenPos();
			Vector2 canvasSize = ImGui::GetContentRegionAvail();
			if (canvasSize.x < 50.0f) canvasSize.x = 50.0f;
			if (canvasSize.y < 50.0f) canvasSize.y = 50.0f;
			Vector2 canvasP1 = Vector2(canvasP0.x + canvasSize.x, canvasP0.y + canvasSize.y);
			static Vector2 scrolling = Vector2(0, 0);
			static Vector2 viewPortDimensions = Vector2(0, 0);
			static bool b_firstRenderPassDone = false;
			static bool b_viewportSizeTaken = false;

			// Set initial viewport dimensions once
			if (b_firstRenderPassDone)
			{
				if (!b_viewportSizeTaken)
				{
					viewPortDimensions = Vector2(canvasSize.x, canvasSize.y);
				}
				b_viewportSizeTaken = true;
			}
			b_firstRenderPassDone = true;
			
			ImGuiIO& inputOutput = ImGui::GetIO();
			
			ImGui::InvisibleButton("AnimationPreview", canvasSize, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);			
			const bool b_isActive = ImGui::IsItemActive();
						
			if (b_isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
			{
				scrolling.x += inputOutput.MouseDelta.x;
				scrolling.y += inputOutput.MouseDelta.y;
			}
		
			Vector2 centerPoint = Vector2(0, 0);

			bool b_weightedScroll = false;
			RenderGridView(centerPoint, scrolling, b_weightedScroll, canvasP0, canvasP1, canvasSize, gridStep, Vector2(viewPortDimensions.x / 2, viewPortDimensions.y / 2));


			if (objectForFocusedAnimation != nullptr)
			{
				std::vector<GameObject> focusedObjectVector;
				focusedObjectVector.push_back(*objectForFocusedAnimation);
	
				// Animate the focused object
				if (FG_b_playPreviewAnimation)
				{				
					Animation* animation = objectForFocusedAnimation->GetAnimation();

					// If animation component is playing, play the animation
					//if (animation != nullptr && animation->IsPlaying())
					//{
					//	FG_previewAnimationTime = FL::GetEngineTime();
					//	animation->PlayAnimation(FG_previewAnimationTime);
					//}
				}

				//RenderViewObjects(focusedObjectVector, centerPoint, canvasP0, canvasSize, gridStep.x);
			}

		// }
		FL::EndWindow(); // Animator Preview
	}

	void RenderAnimationTimelineGrid(Vector2& zeroPoint, Vector2 scrolling, Vector2 canvasP0, Vector2 canvasP1, Vector2 canvasSize, float gridgridStep)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvasP0, canvasP1, FL::GetColor32("timelineGridBg"));
		zeroPoint = Vector2(scrolling.x + canvasP0.x, canvasP0.y + scrolling.y);

		// Draw vertical grid lines
		for (float x = trunc(fmodf(zeroPoint.x, gridgridStep)); x < canvasP0.x + canvasSize.x; x += gridgridStep)
		{
			FL::DrawLine(Vector2(x, canvasP0.y), Vector2(x, canvasP1.y), FL::GetColor("col_4"), 1.0f, draw_list);
		}
		for (float x = trunc(fmodf(zeroPoint.x, gridgridStep * 2)); x < canvasP0.x + canvasSize.x; x += gridgridStep * 2)
		{
			FL::DrawLine(Vector2(x, canvasP0.y), Vector2(x, canvasP1.y), FL::GetColor("col_8"), 1.0f, draw_list);
		}
		// Draw horizontal grid lines
		for (float y = trunc(fmodf(zeroPoint.y, gridgridStep)); y < canvasP0.y + canvasSize.y; y += gridgridStep / 2)
		{
			if (y > canvasP0.y)
			{
				FL::DrawLine(Vector2(canvasP0.x, y), Vector2(canvasP1.x, y), FL::GetColor("col_3"), 1.0f, draw_list);
			}
		}
	}

	void RenderKeyFrameEditor()
	{
		FL::BeginWindow("Keyframe Editor", FG_b_showKeyFrameEditor);
		// {
		
			// Border around object
			auto propsWindowPos = ImGui::GetWindowPos();
			auto propsWindowSize = ImGui::GetWindowSize();  // This is the size of the current box, perfect for getting the exact dimensions for a border
			ImGui::GetWindowDrawList()->AddRect({ propsWindowPos.x + 2, propsWindowPos.y + 2 }, { propsWindowPos.x + propsWindowSize.x - 2, propsWindowPos.y + propsWindowSize.y - 2 }, FL::GetColor32("componentBorder"), 0);

			FL::BeginWindowChild("Animated Properties");
			// {

				std::string keyFrameProperty = "No KeyFrame Selected";
				if (FG_SelectedKeyFrameToEdit != nullptr)
				{
					keyFrameProperty = FG_SelectedKeyFrameToEdit->name + " Frame";
				}

				FL::RenderSectionHeader(keyFrameProperty);

				// Three dots
				FL::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 29, -31);
				ImGui::BeginDisabled(FG_FocusedAnimation == nullptr || FG_SelectedKeyFrameToEdit == nullptr);
				FL::RenderImageButton("##KeyframeEditorHamburgerMenu", FL::GetTexture("threeDots"), Vector2(16, 16), 1, Vector2(1, 1), FL::GetColor("transparent"));
				ImGui::EndDisabled();
				// Popup context
				FL::PushMenuStyles();
				if (ImGui::BeginPopupContextItem("##KeyframeEditorHamburgerMenu", ImGuiPopupFlags_MouseButtonLeft))
				{
					if (ImGui::MenuItem("Delete keyframe"))
					{
						FG_FocusedAnimation->RemoveKeyFrame(FG_SelectedKeyFrameToEdit);
						FG_SelectedKeyFrameToEdit = nullptr;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
				FL::PopMenuStyles();

				FL::RenderSeparator(6, 6);

				if (FG_SelectedKeyFrameToEdit != nullptr)
				{
					float frameTime = FG_SelectedKeyFrameToEdit->time / 1000;
					if (FL::RenderDragFloat("Keyframe time (seconds)", 100, frameTime, 0.001f, 0, 10000000))
					{
						FG_SelectedKeyFrameToEdit->time = frameTime * 1000;
					}

					if (FG_SelectedKeyFrameToEdit->name == "Event")
					{
						std::shared_ptr<Animation::S_Event> event = std::static_pointer_cast<Animation::S_Event>(FG_SelectedKeyFrameToEdit);
						std::string functionName = event->functionName;
						if (FL::RenderInput("##AnimationEventName", "Function Name", functionName))
						{
							event->functionName = functionName;
						}

						FL::MoveScreenCursor(0, 10);

						ImGui::BeginDisabled(event->parameters.size() >= 5);
						if (FL::RenderButton("Add parameter"))
						{
							Animation::S_EventFunctionParam param = Animation::S_EventFunctionParam();
							param.type = "string";
							event->parameters.push_back(param);
						}
						ImGui::EndDisabled();

						ImGui::SameLine(0, 5);

						if (event->parameters.size() >= 5)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, FL::GetColor32("col_5"));
						}
						std::string paramCountString = std::to_string(event->parameters.size()) + " / 5";
						ImGui::Text(paramCountString.c_str());
						if (event->parameters.size() >= 5)
						{
							ImGui::PopStyleColor();
						}

						FL::MoveScreenCursor(0, 5);

						if (event->parameters.size() > 0)
						{
							//FL::MoveScreenCursor(30, 0);
							ImGui::Text("Type:");
							ImGui::SameLine(0, 75);
							ImGui::Text("Value:");
						}

						FL::MoveScreenCursor(0, 5);

						int paramCounter = 0;
						int paramQueuedForDelete = -1;
						for (Animation::S_EventFunctionParam& param : event->parameters)
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
							event->parameters.erase(std::next(event->parameters.begin(), paramQueuedForDelete));
						}
					}
					else if (FG_SelectedKeyFrameToEdit->name == "Transform")
					{
						std::shared_ptr<Animation::S_Transform> transform = std::static_pointer_cast<Animation::S_Transform>(FG_SelectedKeyFrameToEdit);
						float xPos = transform->xPos;
						float yPos = transform->yPos;
						float xScale = transform->xScale;
						float yScale = transform->yScale;
						static ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;

						ImGui::PushItemWidth(ImGui::GetContentRegionMax().x / 3 - 5);

						FL::RenderCheckbox("##transformPosAnimated", transform->b_posAnimated);
						ImGui::SameLine();

						ImGui::BeginDisabled(!transform->b_posAnimated);
						if (FL::PushTable("##TransformAnimationKeyframe", 2))
						{
							if (FL::RenderFloatDragTableRow("##xPositionKeyframeDrag", "X Position", xPos, 0.1f, -FLT_MAX, -FLT_MAX))
							{
								transform->xPos = xPos;
							}
							if (FL::RenderFloatDragTableRow("##yPositionKKeyFrameDrag", "Y Position", yPos, 0.1f, -FLT_MAX, -FLT_MAX))
							{
								transform->yPos = yPos;
							}
							FL::PopTable();
						}
						ImGui::EndDisabled();

						FL::RenderCheckbox("##transformScaleAnimated", transform->b_scaleAnimated);
						ImGui::SameLine();

						ImGui::BeginDisabled(!transform->b_scaleAnimated);
						if (FL::PushTable("##TransformAnimationKeyframe", 2))
						{
							if (FL::RenderFloatDragTableRow("##xScaleDragKeyframeDrag", "X Scale", xScale, 0.1f, 0.001f, 1000))
							{
								transform->xScale = xScale;
							}
							if (FL::RenderFloatDragTableRow("##yScaleDragKeyframeDrag", "Y Scale", yScale, 0.1f, 0.001f, 1000))
							{
								transform->yScale = yScale;
							}
							FL::PopTable();
						}
						ImGui::EndDisabled();
						ImGui::Separator();


						static int current_interp = 0;						
						if (transform->transformInterpType == Animation::I_Lerp)
							current_interp = 0;
						else if (transform->transformInterpType == Animation::I_Slerp)
							current_interp = 1;
				
						ImGui::Text("Interpolation Type");
						std::vector<std::string> interpTypes = { "Lerp", "Slerp" };
						FL::PushComboStyles();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::BeginCombo("##AnimationKeyframeInterpTypes", interpTypes[current_interp].c_str()))
						{
							for (int n = 0; n < interpTypes.size(); n++)
							{
								bool is_selected = (interpTypes[current_interp] == interpTypes[n]);
								if (ImGui::Selectable(interpTypes[n].c_str(), is_selected))
								{
									current_interp = n;
									if (interpTypes[n] == "Lerp")
										transform->transformInterpType = Animation::I_Lerp;
									else if (interpTypes[n] == "Slerp")
										transform->transformInterpType = Animation::I_Slerp;
								}
								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
						FL::PopComboStyles();
					}
					else if (FG_SelectedKeyFrameToEdit->name == "Sprite")
					{
						std::shared_ptr<Animation::S_Sprite> sprite = std::static_pointer_cast<Animation::S_Sprite>(FG_SelectedKeyFrameToEdit);
						std::string path = sprite->path;
						float xOffset = sprite->xOffset;
						float yOffset = sprite->yOffset;
						ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
						Vector4 tintColor = sprite->tintColor;
						bool b_instantlyChangeTint = sprite->b_instantTintChange;
				
						FL::RenderCheckbox("##SpritePathAnimated", sprite->b_pathAnimated);
						ImGui::SameLine();

						// Sprite path
						ImGui::BeginDisabled(!sprite->b_pathAnimated);
						int droppedValue = -1;
						std::string openedPath = "";
						if (FL::DropInputCanOpenFiles("##spritePathKeyFrameEditor", "Path", path, FL::F_fileExplorerTarget, droppedValue, openedPath, "Drop image files here from the File Explorer"))
						{
							if (openedPath != "")
							{
								sprite->path = openedPath;
							}
							else if (droppedValue != -1)
							{
								std::filesystem::path fs_path(FL::F_selectedFiles[droppedValue - 1]);
								if (fs_path.extension() == ".png")
								{
									sprite->path = fs_path.string();
									Texture texture = Texture();
									texture.LoadFromFile(fs_path.string());
									if (texture.GetTexture() != nullptr)
									{
										sprite->xOffset = (float)(texture.GetWidth() / 2);
										sprite->yOffset = (float)(texture.GetHeight() / 2);
									}
								}
								else
								{
									FL::LogError("File must be of type .png to drop here.");
								}
							}
						}
						ImGui::EndDisabled();

						FL::MoveScreenCursor(0, 8);

						FL::RenderCheckbox("##SpriteOffsetAnimated", sprite->b_offsetAnimated);
						ImGui::SameLine();

						// Offset
						ImGui::BeginDisabled(!sprite->b_offsetAnimated);	
						if (FL::PushTable("##AnimatedSpriteProperties", 2))
						{
							if (FL::RenderFloatDragTableRow("##AnimatedxSpriteOffsetDrag", "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
							{
								sprite->xOffset = xOffset;
							}
							if (FL::RenderFloatDragTableRow("##AnimatedySpriteOffsetDrag", "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
							{
								sprite->yOffset = yOffset;
							}
							FL::PopTable();
						}
						ImGui::EndDisabled();

						FL::MoveScreenCursor(0, 8);

						FL::RenderCheckbox("##SpriteTintColorAnimated", sprite->b_tintColorAnimated);
						ImGui::SameLine();

						// Tint color picker
						ImGui::BeginDisabled(!sprite->b_tintColorAnimated);
						std::string tintID = "##AnimationFrameSpriteTintColorPicker";
						ImVec4 color = ImVec4(tintColor.x * 255.0f, tintColor.y * 255.0f, tintColor.z * 255.0f, tintColor.w * 255.0f);
						if (ImGui::ColorEdit4(tintID.c_str(), (float*)&tintColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
						{
							sprite->tintColor = tintColor;
						}
						ImGui::SameLine(0, 5);
						ImGui::Text("Tint color");
						ImGui::SameLine(0, 5);
						if (FL::RenderCheckbox("Instantly change tint", b_instantlyChangeTint))
						{
							sprite->b_instantTintChange = b_instantlyChangeTint;
						}
						ImGui::EndDisabled();
					}
					else if (FG_SelectedKeyFrameToEdit->name == "Text")
					{
						std::shared_ptr<Animation::S_Text> text = std::static_pointer_cast<Animation::S_Text>(FG_SelectedKeyFrameToEdit);
						std::string path = text->fontPath;
						std::string textString = text->text;
						float xOffset = text->xOffset;
						float yOffset = text->yOffset;
						ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
						Vector4 tintColor = text->tintColor;
						bool b_instantlyChangeTint = text->b_instantTintChange;

						FL::RenderCheckbox("##textAnimated", text->b_textAnimated);
						ImGui::SameLine();

						ImGui::BeginDisabled(!text->b_textAnimated);
						if (FL::RenderInput("##AnimationTextStringKeyFrameEditor", "Text", textString, false))
						{
							text->text = textString;
						}
						ImGui::EndDisabled();

						FL::MoveScreenCursor(0, 4);

						FL::RenderCheckbox("##fontPathAnimated", text->b_fontPathAnimated);
						ImGui::SameLine();

						ImGui::BeginDisabled(!text->b_fontPathAnimated);
						int droppedValue = -1;
						std::string openedPath = "";
						if (FL::DropInputCanOpenFiles("##TextPathKeyFrameEditor", "File", path, FL::F_fileExplorerTarget, droppedValue, openedPath, "Drop font files here from the File Explorer"))
						{
							if (openedPath != "")
							{
								text->fontPath = openedPath;
							}
							else if (droppedValue != -1)
							{
								std::filesystem::path fs_path(FL::F_selectedFiles[droppedValue - 1]);
								if (fs_path.extension() == ".ttf")
								{
									text->fontPath = fs_path.string();
									Text tempText = Text();
									tempText.SetFontPath(fs_path.string());
									tempText.SetText(text->text);

									if (tempText.GetTexture() != nullptr)
									{
										text->xOffset = (float)(tempText.GetTexture()->GetWidth() / 2);
										text->yOffset = (float)(tempText.GetTexture()->GetHeight() / 2);
									}
								}
								else
								{
									FL::LogError("File must be a font file (.ttf) to drop here.");
								}
							}
						}
						ImGui::EndDisabled();

						FL::MoveScreenCursor(0, 8);

						// Render Table
						FL::RenderCheckbox("##offsetAnimated", text->b_offsetAnimated);
						ImGui::SameLine();
						ImGui::BeginDisabled(!text->b_offsetAnimated);
						if (FL::PushTable("##AnimatedTextProperties", 2))
						{
							if (FL::RenderFloatDragTableRow("##AnimatedxTextOffsetDrag", "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
							{
								text->xOffset = xOffset;
							}
							if (FL::RenderFloatDragTableRow("##AnimatedyTextOffsetDrag", "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
							{
								text->yOffset = yOffset;
							}
							FL::PopTable();
						}
						ImGui::EndDisabled();

						FL::MoveScreenCursor(0, 8);

						// Tint color picker
						FL::RenderCheckbox("##tintColorAnimated", text->b_tintColorAnimated);
						ImGui::SameLine();

						ImGui::BeginDisabled(!text->b_tintColorAnimated);
						std::string tintID = "##AnimationFrameTextTintColorPicker";
						ImVec4 color = ImVec4(tintColor.x * 255.0f, tintColor.y * 255.0f, tintColor.z * 255.0f, tintColor.w * 255.0f);
						if (ImGui::ColorEdit4(tintID.c_str(), (float*)&tintColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
						{
							text->tintColor = tintColor;
						}
						ImGui::SameLine(0, 5);
						ImGui::Text("Tint color");
						ImGui::SameLine();
						if (FL::RenderCheckbox("Instantly change tint", b_instantlyChangeTint))
						{
							text->b_instantTintChange = b_instantlyChangeTint;
						}
						ImGui::EndDisabled();
					}
					else if (FG_SelectedKeyFrameToEdit->name == "Audio")
					{
						std::shared_ptr<Animation::S_Audio> audio = std::static_pointer_cast<Animation::S_Audio>(FG_SelectedKeyFrameToEdit);			

						FL::RenderCheckbox("Stop All Other Sounds", audio->b_stopAllOtherSounds);				
				
						static int current_audio = 0;
						FL::MoveScreenCursor(0, 5);
						ImGui::Text("Audio to play");
						FL::MoveScreenCursor(0, 3);
						std::vector<std::string> audios = { "- None -" };	
						// Check the Animation components in the scene to see if they have this Animation attached to it, then get all the Audio component audios those have attached to them.  Those are the available sounds here.
						for (std::pair<long, FL::Animation> animationPair : FL::GetLoadedScene()->GetAnimations())
						{
							if (animationPair.second.HasAnimation(FG_FocusedAnimation->animationName))
							{
								Audio* audioComponent = FL::GetLoadedScene()->GetAudioByOwner(animationPair.first);
								if (audioComponent != nullptr && audioComponent->GetSounds().size() > 0)
								{
									for (int i = 0; i < audioComponent->GetSounds().size(); i++)
									{
										std::string componentSoundName = audioComponent->GetSounds()[i].name;
										audios.push_back(componentSoundName);
										if (audio->soundName == componentSoundName)
										{
											current_audio = i + 1;
										}
									}
								}
							}
						}

						if (current_audio + 1 > audios.size() || audio->soundName == "")
						{
							current_audio = 0;
						}

						FL::PushComboStyles();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::BeginCombo("##AnimationKeyframeAudiosCombo", audios[current_audio].c_str()))
						{
							for (int n = 0; n < audios.size(); n++)
							{
								bool is_selected = (audios[current_audio] == audios[n]);
								if (ImGui::Selectable(audios[n].c_str(), is_selected))
								{
									current_audio = n;
									if (audios[current_audio] != "- None -")
									{
										audio->soundName = audios[current_audio];
									}
									else
									{
										audio->soundName = "";
									}
								}
								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
						FL::PopComboStyles();

						FL::MoveScreenCursor(0, 8);

						if (audios.size() == 1)
						{
							ImGui::TextWrapped("Add this Animation to a GameObject with an Audio component to see its available Sounds.");
						}
					}
				}
				else
				{	
					ImGui::TextWrapped("Select a keyframe to edit from the Animation Timeline...");
				}

			// }
			FL::EndWindowChild(); // Animated Properties

		// }
		FL::EndWindow(); // Keyframe Editor
	}

	void SetFocusedAnimation(std::shared_ptr<FL::Animation::S_AnimationProperties> animation)
	{
		FG_FocusedAnimation = animation;
		FL::GetLoadedProject().SetLoadedPreviewAnimationPath(animation->animationPath);
		SaveCurrentProject();
	}

	std::shared_ptr<FL::Animation::S_AnimationProperties> GetFocusedAnimation()
	{
		return FG_FocusedAnimation;
	}
}