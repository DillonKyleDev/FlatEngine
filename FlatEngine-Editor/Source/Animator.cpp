#include "Animator.h"
#include "components/Animation.h"
#include "GameObject.h"
#include "GuiCore.h"
#include "managers/AnimationManager.h"
#include "managers/Assets.h"
#include "managers/ProjectManager.h"
#include "managers/SceneManager.h"
#include "Modals.h"
#include "render/RenderWindow.h"
#include "tools/Logger.h"
#include "tools/Vector2.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"

#include "imgui.h"
#include "math.h"
#include <cstddef>
#include <cstring>
#include <string>

namespace FL = FlatEngine;


namespace FlatGui 
{	
	namespace Animator
	{
		FL::AnimationData loadedAnimation = FL::AnimationData();
		FL::AnimationProperty* selectedKeyframe = nullptr;
		FL::AnimationProperty* keyframeQueuedForDelete = nullptr;
		float gridStep = 50.0f;
	}

	void AddAnimatorMouseControls(std::string buttonID, FL::Vector2 startPos, FL::Vector2 size, FL::Vector2 &scrolling, FL::Vector2 centerPoint, FL::Vector2 &gridStep, uint32_t rectColor, bool b_filled, ImGuiButtonFlags buttonFlags, bool b_allowOverlap, bool b_weightedScroll, float zoomMultiplier, float minGridStep, float maxGridStep)
	{
		FL::Vector2 mouseDelta = FL::Vector2();
		FL::Vector2 mousePos = ImGui::GetIO().MousePos;
		static FL::Vector2 lastMousePos = ImGui::GetIO().MousePos;

		if (size.x > 0 && size.y > 0)
		{
			ImGuiIO& inputOutput = ImGui::GetIO();
			FL::Vector2 endPos = FL::Vector2(startPos.x + size.x, startPos.y + size.y);

			// For calculating scrolling mouse position and what vector to zoom to
			static float DYNAMIC_ANIMATOR_WIDTH = trunc(endPos.x - startPos.x);
			static float DYNAMIC_ANIMATOR_HEIGHT = trunc(endPos.y - startPos.y);

			if (b_filled)
			{
				ImGui::GetWindowDrawList()->AddRectFilled(startPos, FL::Vector2(startPos.x + size.x, startPos.y + size.y), rectColor);
			}
			else
			{
				ImGui::GetWindowDrawList()->AddRect(startPos, FL::Vector2(startPos.x + size.x, startPos.y + size.y), rectColor);
			}

			FL::GuiCore::RenderInvisibleButton(buttonID.c_str(), startPos, size, b_allowOverlap, false, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			const bool b_isHovered = ImGui::IsItemHovered();
			const bool b_isActive = ImGui::IsItemActive();
			const bool b_isClicked = ImGui::IsItemClicked();

			const float mouse_threshold_for_pan = 0.0f;
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				lastMousePos = mousePos;
			}
			if (b_isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
			{
				mouseDelta = FL::Vector2(mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y);
				scrolling.x += mouseDelta.x;
				scrolling.y += mouseDelta.y;
				lastMousePos = mousePos;	

				FL::Vector2 extent = FL::Vector2((float)FL::RenderWindow::window.GetExtent().width, (float)FL::RenderWindow::window.GetExtent().height);
				if (mousePos.x > extent.x - 2)
				{
					SDL_WarpMouseInWindow(FL::RenderWindow::window.GetWindow(), 1, (int)mousePos.y);
					lastMousePos = FL::Vector2(0, mousePos.y);
				}
				else if (mousePos.x < 1)
				{
					SDL_WarpMouseInWindow(FL::RenderWindow::window.GetWindow(), (int)extent.x - 2, (int)mousePos.y);
					lastMousePos = FL::Vector2(extent.x - 1, mousePos.y);
				}
				if (mousePos.y > extent.y - 1)
				{
					SDL_WarpMouseInWindow(FL::RenderWindow::window.GetWindow(), (int)mousePos.x, 1);
					lastMousePos = FL::Vector2(mousePos.x, 0);
				}
				else if (mousePos.y < 1)
				{
					SDL_WarpMouseInWindow(FL::RenderWindow::window.GetWindow(), (int)mousePos.x, (int)extent.y - 1);
					lastMousePos = FL::Vector2(mousePos.x, extent.y);
				}
			}

			// Get scroll amount for changing zoom level of scene view
			FL::Vector2 mousePos = FL::Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
			float scrollInput = inputOutput.MouseWheel;
			float weight = 0.01f;
			float signedMousePosX = mousePos.x - centerPoint.x - (DYNAMIC_ANIMATOR_WIDTH / 2);
			float signedMousePosY = mousePos.y - centerPoint.y - (DYNAMIC_ANIMATOR_HEIGHT / 2);
			float zoomSpeed = 1;
			float finalZoomSpeed = zoomSpeed * zoomMultiplier;

			if (inputOutput.KeyCtrl)
			{
				finalZoomSpeed += zoomMultiplier;
			}

			if (b_isHovered)
			{
				if (scrollInput > 0)
				{
					if (b_weightedScroll)
					{
						scrolling.x -= trunc(signedMousePosX * weight);
						scrolling.y -= trunc(signedMousePosY * weight);
					}
					if (gridStep.x + finalZoomSpeed < maxGridStep)
					{
						gridStep.x += finalZoomSpeed;
					}
					else
					{
						gridStep.x = maxGridStep;
					}
					if (gridStep.y + finalZoomSpeed < maxGridStep)
					{
						gridStep.y += finalZoomSpeed;
					}
					else
					{
						gridStep.y = maxGridStep;
					}
				}
				else if (scrollInput < 0)
				{
					if (b_weightedScroll)
					{
						scrolling.x += trunc(signedMousePosX * weight);
						scrolling.y += trunc(signedMousePosY * weight);
					}
					if (gridStep.x - finalZoomSpeed > minGridStep)
					{
						gridStep.x -= finalZoomSpeed;
					}
					else
					{
						gridStep.x = minGridStep;
					}
					if (gridStep.y - finalZoomSpeed > minGridStep)
					{
						gridStep.y -= finalZoomSpeed;
					}
					else
					{
						gridStep.y = minGridStep;
					}
				}				
			}
		}
	}

	void RenderAnimationTimelineGrid(FL::Vector2& zeroPoint, FL::Vector2 scrolling, FL::Vector2 canvasP0, FL::Vector2 canvasP1, FL::Vector2 canvasSize, float gridgridStep)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvasP0, canvasP1, FL::Assets::assetManager.GetColor32("timelineGridBg"));
		zeroPoint = FL::Vector2(scrolling.x + canvasP0.x, canvasP0.y + scrolling.y);
		
		// Draw vertical grid lines
		for (float x = trunc(fmodf(zeroPoint.x, gridgridStep)); x < canvasP0.x + canvasSize.x; x += gridgridStep)
		{
			if (x > canvasP0.x)
				FL::Logger::log.DrawLine(FL::Vector2(x, canvasP0.y), FL::Vector2(x, canvasP1.y), "timelineGridlinesV", 1.0f, draw_list);
		}
		for (float x = trunc(fmodf(zeroPoint.x, gridgridStep * 2)); x < canvasP0.x + canvasSize.x; x += gridgridStep * 2)
		{
			if (x > canvasP0.x)
				FL::Logger::log.DrawLine(FL::Vector2(x, canvasP0.y), FL::Vector2(x, canvasP1.y), "timelineGridlinesVBright", 1.0f, draw_list);
		}
		// Draw horizontal grid lines
		for (float y = trunc(fmodf(zeroPoint.y, gridgridStep)); y < canvasP0.y + canvasSize.y; y += gridgridStep / 2)
		{
			if (y > canvasP0.y)
			{
				FL::Logger::log.DrawLine(FL::Vector2(canvasP0.x, y), FL::Vector2(canvasP1.x, y), "timelineGridlinesH", 1.0f, draw_list);
			}
		}
		for (float y = trunc(fmodf(zeroPoint.y, gridgridStep * 2)); y < canvasP0.y + canvasSize.y; y += gridgridStep)
		{
			if (y > canvasP0.y)
			{
				FL::Logger::log.DrawLine(FL::Vector2(canvasP0.x, y), FL::Vector2(canvasP1.x, y), "timelineGridlinesHBright", 1.0f, draw_list);
			}
		}
	}

	void RenderAnimationTimelineKeyFrames(std::shared_ptr<FL::AnimationProperty>& keyFrame, FL::PropertyType& nodeClicked, int counter, FL::Vector2& pipPosition, FL::Vector2 zeroPoint, FL::Vector2 scrolling, FL::Vector2 canvasP0, FL::Vector2 canvasP1, FL::Vector2 canvasSize)
	{
		FL::PropertyType type = keyFrame->type;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();	
		float pipTextureWH = 20.0f;					
		std::string pipTextureString = "keyFrame";
		bool b_spriteScalesWithZoom = false;
		int renderOrder = 1;
		
		if (FL::Assets::assetManager.GetTexture("keyFrame") != nullptr)
		{
			bool b_isSelected = (Animator::selectedKeyframe == keyFrame.get());
			if (b_isSelected)
			{						
				pipTextureString = "keyFrameSelected";
			}

			FL::Vector2 pipImageOffset = FL::Vector2(pipTextureWH / 2);
			FL::Vector2 pipStartingPoint = zeroPoint + (FL::Vector2(pipPosition.x * Animator::gridStep, -pipPosition.y * Animator::gridStep) - pipImageOffset);																					

			if (pipStartingPoint.x + pipImageOffset.x >= canvasP0.x)
			{
				ImGui::SetCursorScreenPos(pipStartingPoint);
				std::string pipID = std::to_string((int)type) + std::to_string(counter) + "-KeyFramePip";
				ImGui::InvisibleButton(pipID.c_str(), FL::Vector2(pipTextureWH), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | 4096);
				const bool b_isClicked = ImGui::IsItemClicked();
				const bool b_isHovered = ImGui::IsItemHovered();
				const bool b_isActive = ImGui::IsItemActive();   // Held
				const bool b_isRightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);	
				
				
				// Right click menu
				if (ImGui::BeginPopupContextItem())
				{
					FL::GuiCore::PushMenuStyles();
					if (FL::GuiCore::MenuItem("Delete"))
					{
						Animator::keyframeQueuedForDelete = keyFrame.get();
						ImGui::CloseCurrentPopup();
					}
					FL::GuiCore::PopMenuStyles();

					ImGui::EndPopup();
				}
		
				if (b_isHovered)
				{
					pipTextureString = "keyFrameHovered";
				}

				if (b_isSelected)
				{
					FL::Logger::log.DrawLine(FL::Vector2((int)pipStartingPoint.x + pipImageOffset.x, canvasP0.y), FL::Vector2((int)pipStartingPoint.x + pipImageOffset.y, canvasP1.y), "Animator::selectedKeyframePipLine", 1.0f, draw_list);
				}	
				ImGui::SetCursorScreenPos(FL::Vector2((int)pipStartingPoint.x, (int)pipStartingPoint.y));	
				ImGui::Image(FL::Assets::assetManager.GetTexture(pipTextureString), FL::Vector2(pipTextureWH));


				if (b_isHovered)
				{
					ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Hand);
				}

				if (b_isActive || b_isHovered)
				{
					// Mouse Hover Tooltip - Mouse Over Tooltip
					std::string keyTimeText = "Time: " + std::to_string(keyFrame->time / 1000) + " sec";
					FL::Vector2 m = ImGui::GetIO().MousePos;
					ImGui::SetNextWindowPos(FL::Vector2(m.x + 15, m.y + 5));
					ImGui::Begin("1", NULL, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
					ImGui::Text("%s", keyTimeText.c_str());
					ImGui::End();
				}
		
				const float mouse_threshold_for_pan = 5.0f;
				if (b_isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, mouse_threshold_for_pan))
				{
					ImGuiIO& inputOutput = ImGui::GetIO();
					if (keyFrame->time + inputOutput.MouseDelta.x / Animator::gridStep * 1000 >= 0)
					{
						keyFrame->time += inputOutput.MouseDelta.x / Animator::gridStep * 1000;
					}
				}

				if (b_isClicked)
				{
					nodeClicked = type;
					Animator::selectedKeyframe = keyFrame.get();								
				}	
			}
		}
	}

	void RenderAnimator(bool& b_show)
	{
		if (!b_show)
			return;

		if (FL::GuiCore::BeginWindow("Animator", b_show))
		{
			FL::GuiCore::BeginResizeWindowChild("Animated Properties", "outerWindow");			
			// {

				std::string animationName = "None";
				if (Animator::loadedAnimation.path != FL::ProjectManager::loadedProject.loadedAnimationPath && FL::ProjectManager::loadedProject.loadedAnimationPath != "")
				{
					Animator::loadedAnimation = FL::AnimationManager::LoadAnimationFile(FL::ProjectManager::loadedProject.loadedAnimationPath);
				}
				if (Animator::loadedAnimation.name != "")
				{
					animationName = Animator::loadedAnimation.name + ".anm";
				}

				FL::GuiCore::BeginWindowChild("Manage Animation", "animationDetailsOuter");
				// {

					std::string animationHeaderString = "Loaded: " + animationName;
					FL::GuiCore::MoveScreenCursor(4, 4);
					ImGui::Text("%s", animationHeaderString.c_str());
										
					// Three dots
					ImGui::SameLine();
					FL::GuiCore::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 18, -2);
					FL::GuiCore::RenderImageButton("##AnimatorHamburgerMenu", FL::Assets::assetManager.GetTexture("threeDots"), FL::Vector2(16), 0, FL::Vector2(1), "buttonBorder", "transparent");
					FL::GuiCore::PushMenuStyles();
					if (ImGui::BeginPopupContextItem("##AnimationHamburgerMenu", ImGuiPopupFlags_MouseButtonLeft))
					{
						if (FL::GuiCore::MenuItem("Save Animation"))
						{
							if (Animator::loadedAnimation.path != "")
							{	
								FL::AnimationManager::SaveAnimationFile(Animator::loadedAnimation, Animator::loadedAnimation.path);
							}
							ImGui::CloseCurrentPopup();
						}
						FL::GuiCore::RenderSeparator(0,0);
						if (FL::GuiCore::MenuItem("New Animation"))
						{
							Modals::b_openAnimationModal = true;
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
					FL::GuiCore::PopMenuStyles();					


					auto animationSettingsScreenPos = ImGui::GetCursorScreenPos();
					FL::GuiCore::BeginWindowChild("Manage Animation Settings", "animationDetailsInner");
					// {

						auto animationSettingsWindowSize = ImGui::GetWindowSize();
						const char* properties[] = { "- property -", "Event", "Transform", "Sprite", "Camera", "Canvas", "Audio", "Text", "CharacterController" };
						static int current_property = 0;
						static FL::PropertyType nodeClicked = FL::PropertyType_None;

						if (Animator::loadedAnimation.name != "")
						{						
							FL::GuiCore::MoveScreenCursor(3,3);
							FL::GuiCore::RenderCheckbox("Loop Animation", Animator::loadedAnimation.b_loop);
							FL::GuiCore::RenderSeparator(3,3);

							static std::string selected_property = "";										
							FL::GuiCore::RenderSectionHeader("Animation Properties");																										
											
							FL::GuiCore::PushTreeStyles();							
							if (ImGui::BeginTable("##AnimationProperties", 1, FL::GuiCore::tableFlags))
							{
								ImGui::TableSetupColumn("##PROPERTY", 0, ImGui::GetContentRegionAvail().x + 1);
												
								for (int i = 1; i < FL::PropertyType_Size; i++)
								{
									ImGuiTreeNodeFlags node_flags;

									std::string treeID = animationName + "_" + FL::PropertyTypeStrings[i] + "_node";
									if (nodeClicked == (FL::PropertyType)i)									
										node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected;									
									else									
										node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;									

									ImGui::TableNextRow();
									ImGui::TableSetColumnIndex(0);
									ImGui::TreeNodeEx((void*)(intptr_t)treeID.c_str(), node_flags, "%s", FL::PropertyTypeStrings[i].c_str());
									if (ImGui::IsItemClicked())									
										nodeClicked = (FL::PropertyType)i;									

									ImGui::PushID(treeID.c_str());
									ImGui::PopID();									
								};
											
								ImGui::EndTable();
							}
							FL::GuiCore::PopTreeStyles();
						}
						
						// Border Animation Timeline
						ImGui::GetWindowDrawList()->AddRect({ animationSettingsScreenPos.x, animationSettingsScreenPos.y }, { animationSettingsScreenPos.x + animationSettingsWindowSize.x, animationSettingsScreenPos.y + animationSettingsWindowSize.y }, FL::Assets::assetManager.GetColor32("animationSettingsBorder"), 0);

					// }
					FL::GuiCore::EndWindowChild(); // Manage Animation Settings

				// }
				FL::GuiCore::EndWindowChild(); // Manage Animation

			// }
			FL::GuiCore::EndWindowChild(); // Animator Properties


			ImGui::SameLine(0, 0);
			

			// Timeline
			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::Assets::assetManager.GetColor("animationTimelineOuter"));
			FL::GuiCore::BeginWindowChild("Animation Timeline", "animationTimelineOuter", 0, FL::Vector2(0));
			ImGui::PopStyleColor();
			// {					
						
				auto timelineHeaderSize = ImGui::GetWindowSize();
				FL::Vector2 timelineHeaderP0 = ImGui::GetCursorScreenPos();
				ImGui::GetWindowDrawList()->AddRectFilled({ timelineHeaderP0.x + 10, timelineHeaderP0.y }, { timelineHeaderP0.x + timelineHeaderSize.x, timelineHeaderP0.y + 25 }, FL::Assets::assetManager.GetColor32("animationTimelineHeader"), 0);

				// Draw Property selected and background rect for it
				if (nodeClicked != FL::PropertyType_None)
				{							
					FL::GuiCore::MoveScreenCursor(20, 5);
					std::string nodeClickedString = FL::PropertyTypeStrings[(int)nodeClicked] + "keyframes";
					ImGui::Text("%s", nodeClickedString.c_str());

					ImGui::SameLine(0,0);		
					FL::GuiCore::MoveScreenCursor(6, -2);							
					if (FL::GuiCore::RenderImageButton("#AddKeyframeButton", FL::Assets::assetManager.GetTexture("plus"), FL::Vector2(16)))
					{
						Animator::loadedAnimation.AddKeyFrame(nodeClicked);
					}
					if (ImGui::IsItemHovered())
					{
						FL::GuiCore::RenderTextToolTip("Add new keyframe");
					}

					ImGui::SameLine();	
					FL::GuiCore::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 22, -2);
					if (FL::GuiCore::RenderImageButton("#RemoveAllKeyframesButton", FL::Assets::assetManager.GetTexture("trash"), FL::Vector2(16)))
					{
						Animator::loadedAnimation.RemoveAll(nodeClicked);
						nodeClicked = FL::PropertyType_None;
						Animator::selectedKeyframe = nullptr;
					}
					if (ImGui::IsItemHovered())
					{
						FL::GuiCore::RenderTextToolTip("Delete Animation Property");
					}
				}
				else 
				{
					FL::GuiCore::MoveScreenCursor(20, 5);
					ImGui::Text("No keyframe selected");
					FL::GuiCore::MoveScreenCursor(0, 3);
				}
									
				FL::Vector2 canvasP0 = ImGui::GetCursorScreenPos();					
				FL::Vector2 canvasSize = ImGui::GetContentRegionAvail();
				if (canvasSize.x < 50.0f) canvasSize.x = 50.0f;
				if (canvasSize.y < 50.0f) canvasSize.y = 50.0f;
				FL::Vector2 canvasP1 = FL::Vector2(canvasP0.x + canvasSize.x, canvasP0.y + canvasSize.y);
				static FL::Vector2 scrolling = FL::Vector2(0, 0);
				FL::Vector2 gridgridStep = FL::Vector2(Animator::gridStep);
				FL::Vector2 zeroPoint = FL::Vector2(0);
				float maxGridStep = 500;
				float minGridStep = 5;

				AddAnimatorMouseControls("##AnimatorTimelineGridButton", canvasP0, canvasSize, scrolling, FL::Vector2(), gridgridStep, FL::Assets::assetManager.GetColor32("transparent"), false, 0, true, false, 10, minGridStep, maxGridStep);
				Animator::gridStep = gridgridStep.x;
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
				
				canvasP0.x += 10;
				RenderAnimationTimelineGrid(zeroPoint, scrolling, canvasP0, canvasP1, canvasSize, Animator::gridStep);
				// Border Animation Timeline					
				auto timelineWindowSize = ImGui::GetWindowSize();
				ImGui::GetWindowDrawList()->AddRect({ canvasP0.x, canvasP0.y }, { canvasP0.x + timelineWindowSize.x, canvasP0.y + timelineWindowSize.y }, FL::Assets::assetManager.GetColor32("animationTimelineBorder"), 0);

				const float PIP_PROP_Y_POS_START = -0.5f; // Value in grid space
				float animationLength = animationLength = Animator::loadedAnimation.length;			
				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				int IDCounter = 0;								
				for (int i = 1; i < (int)FL::PropertyType_Size; i++)
				{
					FL::Vector4 rectColor = FL::Assets::assetManager.GetColor("rectColor" + std::to_string(i));
					std::vector<float> keyFrameTimes = std::vector<float>();
				
					float topYPos = canvasP0.y + scrolling.y + ((i - 1) * Animator::gridStep);
					float bottomYPos = topYPos + Animator::gridStep;		

					if (nodeClicked != (FL::PropertyType)i)					
						rectColor.w = 0.1;					

					rectColor *= 255;
					ImU32 color = IM_COL32(rectColor.x, rectColor.y, rectColor.z, rectColor.w);

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

					FL::Vector2 topLeftCorner = FL::Vector2(canvasP0.x, topYPos);
					FL::Vector2 bottomRightCorner = FL::Vector2(canvasP1.x, bottomYPos);
					draw_list->AddRectFilled(topLeftCorner, bottomRightCorner, color);

					for (auto& frame : Animator::loadedAnimation.props)
					{
						if (frame->type == (FL::PropertyType)i)
						{
							std::string ID = std::to_string(i);					
							float keyFrameX = frame->time / 1000;
							float propertyYPos = PIP_PROP_Y_POS_START - (i - 1);
							FL::Vector2 keyFramePos = FL::Vector2(keyFrameX, propertyYPos);		
							if (zeroPoint.y + (propertyYPos * Animator::gridStep * -1) < canvasP1.y && zeroPoint.y + (propertyYPos * Animator::gridStep * -1) + 6 < canvasP1.y && zeroPoint.y + (propertyYPos * Animator::gridStep * -1) > canvasP0.y)
							{
								RenderAnimationTimelineKeyFrames(frame, nodeClicked, IDCounter, keyFramePos, zeroPoint, scrolling, canvasP0, canvasP1, canvasSize);
							}
							IDCounter++;
						}
					}
				}					

				if (Animator::keyframeQueuedForDelete != nullptr)
				{
					if (Animator::keyframeQueuedForDelete == Animator::selectedKeyframe)
					{						
						Animator::selectedKeyframe = nullptr;
					}
					Animator::loadedAnimation.RemoveKeyFrame(Animator::keyframeQueuedForDelete);
				}

			// }
			FL::GuiCore::EndWindowChild(); // Animation Timeline			
		}

		FL::GuiCore::EndWindow(); // Animator
	}

	void RenderKeyFrameEditor(bool& b_show)
	{
		if (!b_show)
			return;

		if (FL::GuiCore::BeginWindow("Keyframe Editor", b_show))
		{
			// Border around object
			auto propsWindowPos = ImGui::GetWindowPos();
			auto propsWindowSize = ImGui::GetWindowSize();  // This is the size of the current box, perfect for getting the exact dimensions for a border
			ImGui::GetWindowDrawList()->AddRect({ propsWindowPos.x + 2, propsWindowPos.y + 2 }, { propsWindowPos.x + propsWindowSize.x - 2, propsWindowPos.y + propsWindowSize.y - 2 }, FL::Assets::assetManager.GetColor32("componentBorder"), 0);


			FL::GuiCore::BeginWindowChild("Animated Properties", "keyframeEditorBg", 0, FL::Vector2(0));
			// {

				std::string keyFrameProperty = "No keyFrame selected";
				if (Animator::selectedKeyframe != nullptr)
				{
					keyFrameProperty = Animator::selectedKeyframe->name + "Frame";
				}

				if (Animator::selectedKeyframe != nullptr)
				{
					float frameTime = Animator::selectedKeyframe->time / 1000;
					if (FL::GuiCore::RenderDragFloat("Keyframe time (sec)", 100, frameTime, 0.001f, 0, 10000000))
					{
						Animator::selectedKeyframe->time = frameTime * 1000;
					}

					ImGui::SameLine();

					// Three dots
					FL::GuiCore::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 18, 0);					
					FL::GuiCore::RenderImageButton("##KeyframeEditorHamburgerMenu", FL::Assets::assetManager.GetTexture("threeDots"), FL::Vector2(16), 0, FL::Vector2(1), "buttonBorder", "transparent");					
					// Popup context
					FL::GuiCore::PushMenuStyles();
					if (ImGui::BeginPopupContextItem("##KeyframeEditorHamburgerMenu", ImGuiPopupFlags_MouseButtonLeft))
					{
						if (FL::GuiCore::MenuItem("Delete keyframe"))
						{
							Animator::loadedAnimation.RemoveKeyFrame(Animator::selectedKeyframe);
							Animator::selectedKeyframe = nullptr;
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
					FL::GuiCore::PopMenuStyles();	
										
					FL::GuiCore::RenderSectionHeader("", 3.0f, 3.0f, "sectionHeaderEmptyBg", "separator");	

					if (Animator::selectedKeyframe->type == FL::PropertyType_Event)
					{
						FL::EventProp* event = static_cast<FL::EventProp*>(Animator::selectedKeyframe);
						std::string functionName = event->functionName;
						bool b_cppEvent = event->b_cppEvent;
						bool b_luaEvent = event->b_luaEvent;

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

						if (ImGui::RadioButton("C++ Function", currentChoice == choices[0]))
						{
							currentChoice = choices[0];
							event->b_cppEvent = true;
							event->b_luaEvent = false;
						}
						if (ImGui::RadioButton("Lua Function", currentChoice == choices[1]))
						{
							currentChoice = choices[1];
							event->b_cppEvent = false;
							event->b_luaEvent = true;
						}

						FL::GuiCore::RenderSeparator(1, 1);

						if (event->b_cppEvent)
						{
							int currentEventFunction = 0;
							std::vector<std::string> eventFunctions = { "- none -" };

							for (std::map<std::string, void (*)(FL::GameObject*, FL::LuaManager::LuaParameter)>::iterator iter = FL::AnimationManager::cppAnimationEventFunctions.begin(); iter != FL::AnimationManager::cppAnimationEventFunctions.end(); iter++)
							{
								eventFunctions.push_back(iter->first);
							}

							for (int i = 0; i < eventFunctions.size(); i++)
							{
								if (event->functionName == eventFunctions[i])
								{
									currentEventFunction = i;
								}
							}

							if (eventFunctions.size())
							{
								std::string comboID = "##EventFunctionName";
								if (FL::GuiCore::RenderCombo(comboID, eventFunctions[currentEventFunction], eventFunctions, currentEventFunction))
								{
									event->functionName = eventFunctions[currentEventFunction];
								}
							}
							else
							{
								ImGui::Text("Add event functions using AddCPPAnimationEventFunction() in attached C++ script.");
							}
						}

						if (b_luaEvent)
						{
							if (FL::GuiCore::RenderInput("##AnimationEventName", "Function Name", functionName))
							{
								event->functionName = functionName;
							}
						}

						FL::GuiCore::RenderLuaParametersTable("##EventPropLuaParams", "Event Property Parameters", event->eventParamContainer);
					}
					else if (Animator::selectedKeyframe->type == FL::PropertyType_Transform)
					{
						FL::TransformProp* transformProp = static_cast<FL::TransformProp*>(Animator::selectedKeyframe);					
						static ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
						
						FL::GuiCore::RenderSectionHeader("Transform Property Settings", 0.0f, 5.0f, "sectionHeaderKeyframeEditor", "sectionHeaderSeparatorKeyframeEditor");
						FL::GuiCore::RenderCheckbox("Start At Origin##startAtOrigin", Animator::loadedAnimation.b_startAtOrigin);							
						FL::GuiCore::RenderSectionHeader("Keyframe Property Settings", 5.0f, 5.0f, "sectionHeaderKeyframeEditor", "sectionHeaderSeparatorKeyframeEditor");											
						FL::GuiCore::RenderCheckbox("POSITION##b_posAnimated", transformProp->b_posAnimated); ImGui::SameLine();
						FL::GuiCore::RenderCheckbox("ROTATION##b_rotationAnimated", transformProp->b_rotationAnimated); ImGui::SameLine();
						FL::GuiCore::RenderCheckbox("SCALE##b_scaleAnimated", transformProp->b_scaleAnimated);
						FL::GuiCore::RenderSeparator(5, 5);						

						float labelWidth = 68;					
						FL::GuiCore::RenderVector3Table(FL::GuiCore::TableProps("##TransformComponentTable", "Position", FL::Vector2(), labelWidth), transformProp->position);
						FL::GuiCore::RenderVector3Table(FL::GuiCore::TableProps("##TransformComponentTable", "Rotation", FL::Vector2(), labelWidth), transformProp->eulerRotation);
						FL::GuiCore::RenderVector3Table(FL::GuiCore::TableProps("##TransformComponentTable", "Scale", FL::Vector2(), labelWidth),    transformProp->scale);

						FL::GuiCore::RenderSeparator(5, 5);
						static int current_transform_interp = 0;
						current_transform_interp = (int)transformProp->positionInterpType;
						static int current_scale_interp = 0;	
						current_scale_interp = (int)transformProp->scaleInterpType;									
						static int current_rotation_interp = 0;		
						current_rotation_interp = (int)transformProp->rotationInterpType;	
						
						ImGui::Text("INTERPOLATION");												
						std::vector<std::string> interpTypes = 
						{ 
							"Linear", "Ease-In-Sine", "Ease-Out-Sine", 
							"Ease-In-Out-Sine", "Ease-In-Elastic", 
							"Ease-Out-Elastic", "Ease-In-Out-Elastic", 
							"Ease-In-Back", "Ease-Out-Back", "Ease-In-Out-Back", 
							"Ease-In-Out-Quart", "Ease-In-Out-Cubic" 
						};

						float comboWidths = ImGui::GetContentRegionAvail().x / 3.0f;
						float startXPos = ImGui::GetCursorScreenPos().x;
						ImGui::Text("Position"); 
						ImGui::SameLine(0,0); 
						FL::GuiCore::MoveScreenCursor(comboWidths - (ImGui::GetCursorScreenPos().x - startXPos),0);
						float startXRot = ImGui::GetCursorScreenPos().x;
						ImGui::Text("Rotation"); ImGui::SameLine(0,0); 
						ImGui::SameLine(0,0); 
						FL::GuiCore::MoveScreenCursor(comboWidths - (ImGui::GetCursorScreenPos().x - startXRot),0);
						ImGui::Text("Scale");

						FL::GuiCore::PushComboStyles();

						ImGui::SetNextItemWidth(comboWidths);
						if (ImGui::BeginCombo("##AnimationKeyframePositionInterpTypes", interpTypes[current_transform_interp].c_str()))
						{
							for (int n = 0; n < interpTypes.size(); n++)
							{
								bool is_selected = (interpTypes[current_transform_interp] == interpTypes[n]);
								if (ImGui::Selectable(interpTypes[n].c_str(), is_selected))
								{
									current_transform_interp = n;
									transformProp->positionInterpType = (FL::Easing::InterpType)n;
								}
								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}

						ImGui::SameLine(0,0);
						
						ImGui::SetNextItemWidth(comboWidths);
						if (ImGui::BeginCombo("##AnimationKeyframeRotationInterpTypes", interpTypes[current_rotation_interp].c_str()))
						{
							for (int n = 0; n < interpTypes.size(); n++)
							{
								bool is_selected = (interpTypes[current_rotation_interp] == interpTypes[n]);
								if (ImGui::Selectable(interpTypes[n].c_str(), is_selected))
								{
									current_rotation_interp = n;
									transformProp->rotationInterpType = (FL::Easing::InterpType)n;										
								}
								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
						
						ImGui::SameLine(0,0);

						ImGui::SetNextItemWidth(comboWidths);
						if (ImGui::BeginCombo("##AnimationKeyframeScaleInterpTypes", interpTypes[current_scale_interp].c_str()))
						{
							for (int n = 0; n < interpTypes.size(); n++)
							{
								bool is_selected = (interpTypes[current_scale_interp] == interpTypes[n]);
								if (ImGui::Selectable(interpTypes[n].c_str(), is_selected))
								{
									current_scale_interp = n;
									transformProp->scaleInterpType = (FL::Easing::InterpType)n;
								}
								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
						FL::GuiCore::PopComboStyles();
					}
					else if (Animator::selectedKeyframe->name == "Sprite")
					{
						FL::SpriteProp* sprite = static_cast<FL::SpriteProp*>(Animator::selectedKeyframe);
						std::string path = sprite->path;
						float xOffset = sprite->xOffset;
						float yOffset = sprite->yOffset;
						ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
						FL::Vector4 tintColor = sprite->tintColor;
						bool b_instantlyChangeTint = sprite->b_instantTintChange;
				
						FL::GuiCore::RenderCheckbox("##SpritePathAnimated", sprite->b_pathAnimated);
						ImGui::SameLine();

						// Sprite path
						ImGui::BeginDisabled(!sprite->b_pathAnimated);
						int droppedValue = -1;
						std::string openedPath = "";
						if (FL::GuiCore::DropInputCanOpenFiles("##spritePathKeyFrameEditor", "Path", path, FL::GuiCore::fileExplorerTarget, droppedValue, openedPath, "Drop image files here from the File Explorer"))
						{
							if (openedPath != "")
							{
								sprite->path = openedPath;
							}
							else if (droppedValue != -1)
							{
								std::filesystem::path fs_path(FL::GuiCore::selectedFiles[droppedValue - 1]);
								if (fs_path.extension() == ".png")
								{
									sprite->path = fs_path.string();
									FL::Texture texture = FL::Texture();
									texture.LoadFromFile(fs_path.string());
									if (texture.GetTexture() != nullptr)
									{
										sprite->xOffset = (float)(texture.GetWidth() / 2.0f);
										sprite->yOffset = (float)(texture.GetHeight() / 2.0f);
									}
								}
								else
								{
									FL::Logger::log.Err("File must be of type .png to drop here.");
								}
							}
						}
						ImGui::EndDisabled();

						FL::GuiCore::MoveScreenCursor(0, 8);

						FL::GuiCore::RenderCheckbox("##SpriteOffsetAnimated", sprite->b_offsetAnimated);
						ImGui::SameLine();

						// Offset
						ImGui::BeginDisabled(!sprite->b_offsetAnimated);	
						// if (FL::GuiCore::PushTable("##AnimatedSpriteProperties", 2))
						// {
						// 	if (FL::GuiCore::RenderFloatDragTableRow("##AnimatedxSpriteOffsetDrag", "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
						// 	{
						// 		sprite->xOffset = xOffset;
						// 	}
						// 	if (FL::GuiCore::RenderFloatDragTableRow("##AnimatedySpriteOffsetDrag", "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
						// 	{
						// 		sprite->yOffset = yOffset;
						// 	}
						// 	FL::GuiCore::PopTable();
						// }
						ImGui::EndDisabled();

						FL::GuiCore::MoveScreenCursor(0, 8);

						FL::GuiCore::RenderCheckbox("##SpriteTintColorAnimated", sprite->b_tintColorAnimated);
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
						if (FL::GuiCore::RenderCheckbox("Instantly change tint", b_instantlyChangeTint))
						{
							sprite->b_instantTintChange = b_instantlyChangeTint;
						}
						ImGui::EndDisabled();
					}
					else if (Animator::selectedKeyframe->name == "Text")
					{
						FL::TextProp* text = static_cast<FL::TextProp*>(Animator::selectedKeyframe);
						std::string path = text->fontPath;
						std::string textString = text->text;
						float xOffset = text->xOffset;
						float yOffset = text->yOffset;
						ImGuiSliderFlags flags = ImGuiSliderFlags_::ImGuiSliderFlags_None;
						FL::Vector4 tintColor = text->tintColor;
						bool b_instantlyChangeTint = text->b_instantTintChange;

						FL::GuiCore::RenderCheckbox("##textAnimated", text->b_textAnimated);
						ImGui::SameLine();

						ImGui::BeginDisabled(!text->b_textAnimated);
						if (FL::GuiCore::RenderInput("##AnimationTextStringKeyFrameEditor", "Text", textString, false))
						{
							text->text = textString;
						}
						ImGui::EndDisabled();

						FL::GuiCore::MoveScreenCursor(0, 4);

						FL::GuiCore::RenderCheckbox("##fontPathAnimated", text->b_fontPathAnimated);
						ImGui::SameLine();

						ImGui::BeginDisabled(!text->b_fontPathAnimated);
						int droppedValue = -1;
						std::string openedPath = "";
						if (FL::GuiCore::DropInputCanOpenFiles("##TextPathKeyFrameEditor", "File", path, FL::GuiCore::fileExplorerTarget, droppedValue, openedPath, "Drop font files here from the File Explorer"))
						{
							if (openedPath != "")
							{
								text->fontPath = openedPath;
							}
							else if (droppedValue != -1)
							{
								std::filesystem::path fs_path(FL::GuiCore::selectedFiles[droppedValue - 1]);
								if (fs_path.extension() == ".ttf")
								{
									text->fontPath = fs_path.string();
									FL::Text tempText = FL::Text();
									tempText.SetFontPath(fs_path.string());
									tempText.SetText(text->text);

									if (tempText.GetTexture() != nullptr)
									{
										text->xOffset = (float)(tempText.GetTexture()->GetWidth() / 2.0f);
										text->yOffset = (float)(tempText.GetTexture()->GetHeight() / 2.0f);
									}
								}
								else
								{
									FL::Logger::log.Err("File must be a font file (.ttf) to drop here.");
								}
							}
						}
						ImGui::EndDisabled();

						FL::GuiCore::MoveScreenCursor(0, 8);

						// Render Table
						FL::GuiCore::RenderCheckbox("##offsetAnimated", text->b_offsetAnimated);
						ImGui::SameLine();
						ImGui::BeginDisabled(!text->b_offsetAnimated);
						// if (FL::GuiCore::PushTable("##AnimatedTextProperties", 2))
						// {
						// 	if (FL::GuiCore::RenderFloatDragTableRow("##AnimatedxTextOffsetDrag", "X Offset", xOffset, 0.1f, -FLT_MAX, -FLT_MAX))
						// 	{
						// 		text->xOffset = xOffset;
						// 	}
						// 	if (FL::GuiCore::RenderFloatDragTableRow("##AnimatedyTextOffsetDrag", "Y Offset", yOffset, 0.1f, -FLT_MAX, -FLT_MAX))
						// 	{
						// 		text->yOffset = yOffset;
						// 	}
						// 	FL::GuiCore::PopTable();
						// }
						ImGui::EndDisabled();

						FL::GuiCore::MoveScreenCursor(0, 8);

						// Tint color picker
						FL::GuiCore::RenderCheckbox("##tintColorAnimated", text->b_tintColorAnimated);
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
						if (FL::GuiCore::RenderCheckbox("Instantly change tint", b_instantlyChangeTint))
						{
							text->b_instantTintChange = b_instantlyChangeTint;
						}
						ImGui::EndDisabled();
					}
				}
				else
				{	
					FL::GuiCore::MoveScreenCursor(5, 3);
					ImGui::TextWrapped("Select a keyframe to edit from the Animation Timeline.");
				}

			// }
			FL::GuiCore::EndWindowChild(); // Animated Properties			
		}

		FL::GuiCore::EndWindow(); // Keyframe Editor
	}
}