#include "GuiCore.h"
#include "tools/Profiler.h"

#include <deque>

namespace FL = FlatEngine;


namespace FlatGui 
{ 
	void RenderProfiler(bool& b_show)
	{
		FL::GuiCore::BeginWindow("Profiler", b_show);
		// {

			static ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable;
			static bool b_animate = false;
			static int offset = 0;
			static bool b_showColliderPairs = true;

			FL::GuiCore::RenderCheckbox("Animate", b_animate);
			if (b_animate)
			{
				offset = (offset + 1) % 100;
			}


			int processCounter = 1;

			if (ImGui::BeginTable("##table", 3, flags, FL::Vector2(-1, 0))) 
			{
				ImGui::TableSetupColumn("Process Name", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Time (ms)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Visualization");
				ImGui::TableHeadersRow();
				//ImPlot::PushColormap(ImPlotColormap_Cool);

				if (FL::Profiler::processes.size() > 0)
				{
					for (std::vector<FL::Profiler::ProfilerProcess>::iterator iter = FL::Profiler::processes.begin(); iter != FL::Profiler::processes.end(); iter++)
					{
						std::string processName = (*iter).GetProcessName();
						std::vector<float> hangTimeVector = (*iter).GetHangTimeData();
						std::deque<float> rawDataVector = (*iter).GetRawData();

						float* dataArray;

						if (hangTimeVector.size() > 0)
						{
							dataArray = &hangTimeVector.front();
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::Text("%s", std::to_string(processCounter).c_str());
							ImGui::SameLine(0, 5);
							ImGui::Text("%s", processName.c_str());
							ImGui::TableSetColumnIndex(1);
							ImGui::Text("%.0f ms", rawDataVector.front());
							ImGui::TableSetColumnIndex(2);
							ImGui::PushID(processCounter);
							//Sparkline("##spark", dataArray, 100, 0, 10.0f, offset, ImPlot::GetColormapColor((int)rawDataVector.front()), Vector2(-1, 35));
							ImGui::PopID();
						}

						processCounter++;
					}
				}

				//ImPlot::PopColormap();
				ImGui::EndTable();
			}

		FL::GuiCore::EndWindow(); // Profiler
	}
}