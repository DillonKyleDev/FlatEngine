#include "Application.h"
#include "FlatEngine.h"
#include "GuiCore.h"
#include "managers/Assets.h"
#include "tools/Vector2.h"

#include <cstddef>

namespace FL = FlatEngine;


namespace FlatGui 
{
	int framesToSkip = 10;

	void RenderToolbar()
	{		
		ImGui::Begin("Gameloop Control Panel", NULL, ImGuiDockNodeFlags_::ImGuiDockNodeFlags_NoUndocking);
		// {

			ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 0.4f);

			float windowWidth = ImGui::GetWindowSize().x;
			FL::GuiCore::MoveScreenCursor(3, 3);

			ImGui::BeginDisabled(!FL::application->gameloop->IsStarted());
			if (FL::GuiCore::RenderImageButton("##RestartGameloopIcon", FL::Assets::assetManager.GetTexture("restart"), FL::Vector2(24, 24), 0, FL::Vector2(0)))
			{
				FL::application->gameloop->Stop();	
				FL::application->gameloop->Start();				
			}
			ImGui::EndDisabled();
			ImGui::SameLine(0, 5);

			ImGui::BeginDisabled(FL::application->gameloop->IsStarted());
			if (FL::GuiCore::RenderImageButton("##PlayGameloopIcon", FL::Assets::assetManager.GetTexture("play"), FL::Vector2(24, 24), 0, FL::Vector2(0)))
			{
				FL::application->gameloop->Start();				
			}
			ImGui::EndDisabled();
			ImGui::SameLine(0, 5);
		
			ImGui::BeginDisabled(!FL::application->gameloop->IsStarted());
			if (FL::GuiCore::RenderImageButton("##PauseGameloopIcon", FL::Assets::assetManager.GetTexture("pause"), FL::Vector2(24, 24), 0, FL::Vector2(0)))
			{
				FL::application->gameloop->TogglePauseGameLoop();				
			}
			ImGui::EndDisabled();
			ImGui::SameLine(0, 5);

			ImGui::BeginDisabled(!FL::application->gameloop->IsStarted());
			if (FL::GuiCore::RenderImageButton("##StopGameloopIcon", FL::Assets::assetManager.GetTexture("stop"), FL::Vector2(24, 24), 0, FL::Vector2(0)))
			{
				FL::application->gameloop->Stop();
			}
			ImGui::EndDisabled();
			ImGui::SameLine(0, 5);
		
			ImGui::BeginDisabled(!FL::application->gameloop->IsPaused());
			if (FL::GuiCore::RenderImageButton("##AdvanceGameloopIcon", FL::Assets::assetManager.GetTexture("nextFrame"), FL::Vector2(24, 24), 0, FL::Vector2(0)))
			{
				FL::application->gameloop->SkipFrames(1);
			}
			ImGui::EndDisabled();
			if (ImGui::IsItemHovered())
			{
				FL::GuiCore::RenderTextToolTip("Advance 1");
			}
			ImGui::SameLine(0, 5);

			ImGui::BeginDisabled(!FL::application->gameloop->IsPaused());
			if (FL::GuiCore::RenderImageButton("##SkipFramesIcon", FL::Assets::assetManager.GetTexture("skipFrames"), FL::Vector2(24, 24), 0, FL::Vector2(0)))
			{				
				FL::application->gameloop->SkipFrames(framesToSkip);
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
			FL::GuiCore::RenderDragInt("##NumberOfFramesToSkipDrag", 30, framesToSkip, 1, 1, 360);
		

			ImGui::PopStyleVar();

		// }
		ImGui::End(); // Gameloop Control Panel
	}
}