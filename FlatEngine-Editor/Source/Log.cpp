#include "FlatGui.h"
#include "FlatEngine.h"
#include "Logger.h"
#include "Project.h"

#include "imgui.h"


namespace FL = FlatEngine;

namespace FlatGui 
{
	bool FG_b_clearBufferEveryFrame = false;

	void RenderLog()
	{
		FL::BeginWindow("Log", FG_b_showLogger);
		// {
		
			// Border around object
			auto windowPos = ImGui::GetWindowPos();
			auto windowSize = ImGui::GetWindowSize();
			//ImGui::GetWindowDrawList()->AddRect({ windowPos.x + 2, windowPos.y + 2 }, { windowSize.x + windowPos.x - 2, windowPos.y + windowSize.y - 2 }, FL::GetColor32("componentBorder"), 0);


			FL::MoveScreenCursor(2, 2);

			ImGuiTextBuffer log = FL::F_Logger.GetBuffer();
			static int lines = 0;
			if (FL::RenderButton("Clear"))
			{
				FL::F_Logger.ClearBuffer();
				lines = 0;
			}

			ImGui::SameLine();
			FL::MoveScreenCursor(0, 3);

			if (FL::RenderCheckbox("Clear every frame", FG_b_clearBufferEveryFrame))
			{
				FL::F_Logger.ClearBuffer();
				SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
			}






			//ImGui::PushStyleColor(ImGuiCol_Text, FL::GetColor("logText"));
			//ImGui::Text("Log buffer contents : % d bytes", log.size());
			//ImGui::PopStyleColor();

			FL::MoveScreenCursor(0, 2);

			Vector2 cursorPos = Vector2(ImGui::GetCursorScreenPos().x - 1, ImGui::GetCursorScreenPos().y - 1);
			Vector2 availSpace = ImGui::GetContentRegionAvail();

			// Draw Border around log
			ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, Vector2(cursorPos.x + availSpace.x + 2, cursorPos.y + availSpace.y + 2), FL::GetColor32("logOutline"));


			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("logBg"));
			FL::BeginWindowChild("Log", FL::GetColor("logBg"), FL::F_childFlags);
			ImGui::PopStyleColor();
			// {

				ImGui::PushStyleColor(ImGuiCol_Text, FL::GetColor("logText"));
				ImGui::TextUnformatted(log.begin(), log.end());
				ImGui::PopStyleColor();
			
			// }	
			FL::EndWindowChild(); // Log

		// }
		FL::EndWindow(); // Logger


		// For keeping the log from filling up when logging values every frame
		if (FG_b_clearBufferEveryFrame)
		{
			FL::F_Logger.ClearBuffer();
			FL::LogString("Log buffer is being cleared...");
		}
	}
}