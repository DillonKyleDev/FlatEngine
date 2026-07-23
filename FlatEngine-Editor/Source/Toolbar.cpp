#include "FlatEngine.h"
#include "GameLoop.h"
#include "GuiCore.h"
#include "managers/Assets.h"
#include "tools/Vector2.h"

#include <cstddef>

namespace FL = FlatEngine;


namespace FlatGui 
{
	void RenderToolbar()
	{		
		ImGui::Begin("Gameloop Control Panel", NULL, ImGuiDockNodeFlags_::ImGuiDockNodeFlags_NoUndocking);
		// {

			std::string playID = "##PlayGameloopIcon";
			std::string pauseID = "##PauseGameloopIcon";
			std::string stopID = "##StopGameloopIcon";
			std::string nextFrameID = "##AdvanceGameloopIcon";
			std::string skipFramesID = "##SkipFramesIcon";
			std::string framesToSkipDragID = "##NumberOfFramesToSkipDrag";
			static int framesToSkip = 10;
			ImVec4 gameloopControl_tint_col = ImVec4(1.0, 1.0, 1.0, 1.0f);
			ImVec4 gameloopControl_bg_col = ImVec4(.50f, .50f, .5f, 1.0f);


			ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 0.4f);

			float windowWidth = ImGui::GetWindowSize().x;
			FL::GuiCore::MoveScreenCursor(3, 3);
			ImGui::BeginDisabled(FL::GameLoopStarted());
			if (FL::GuiCore::RenderImageButton(playID.c_str(), FL::Assets::assetManager.GetTexture("play"), FL::Vector2(24, 24), 0, FL::Vector2(0)))
			{
				FL::StartGameLoop();				
			}
			ImGui::EndDisabled();
			ImGui::SameLine(0, 5);
		
			ImGui::BeginDisabled(!FL::GameLoopStarted());
			if (FL::GuiCore::RenderImageButton(pauseID.c_str(), FL::Assets::assetManager.GetTexture("pause"), FL::Vector2(24, 24), 0, FL::Vector2(0)))
			{
				FL::PauseGameLoop();				
			}
			ImGui::EndDisabled();
			ImGui::SameLine(0, 5);

			ImGui::BeginDisabled(!FL::GameLoopStarted());
			if (FL::GuiCore::RenderImageButton(stopID.c_str(), FL::Assets::assetManager.GetTexture("stop"), FL::Vector2(24, 24), 0, FL::Vector2(0)))
			{
				FL::StopGameLoop();
			}
			ImGui::EndDisabled();
			ImGui::SameLine(0, 5);
		
			ImGui::BeginDisabled(!FL::GameLoopPaused());
			if (FL::GuiCore::RenderImageButton(nextFrameID.c_str(), FL::Assets::assetManager.GetTexture("nextFrame"), FL::Vector2(24, 24), 0, FL::Vector2(0)))
			{
				FL::F_Application->GetGameLoop()->SkipFrames(1);
			}
			ImGui::EndDisabled();
			if (ImGui::IsItemHovered())
			{
				FL::GuiCore::RenderTextToolTip("Advance 1");
			}
			ImGui::SameLine(0, 5);

			ImGui::BeginDisabled(!FL::GameLoopPaused());
			if (FL::GuiCore::RenderImageButton(skipFramesID.c_str(), FL::Assets::assetManager.GetTexture("skipFrames"), FL::Vector2(24, 24), 0, FL::Vector2(0)))
			{				
				FL::F_Application->GetGameLoop()->SkipFrames(framesToSkip);
			}
			ImGui::EndDisabled();
			if (ImGui::IsItemHovered())
			{
				FL::GuiCore::RenderTextToolTip("Advance " + std::to_string(framesToSkip));
			}

			ImGui::SameLine(0, 5); 
			FL::GuiCore::MoveScreenCursor(3, 5);
			ImGui::PushStyleColor(ImGuiCol_Text, FL::Assets::assetManager.GetColor("logText"));
			ImGui::Text("Frames to advance ");
			ImGui::PopStyleColor();
			ImGui::SameLine();
			FL::GuiCore::MoveScreenCursor(-12, 2);
			FL::GuiCore::RenderDragInt(framesToSkipDragID.c_str(), 30, framesToSkip, 1, 1, 360);
		

			ImGui::PopStyleVar();

		// }
		ImGui::End(); // Gameloop Control Panel
	}
}