#include "FlatGui.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "GameLoop.h"
#include "ProfilerProcess.h"
#include "Application.h"
#include "Scene.h"

#include <deque>
#include "implot.h"


namespace FL = FlatEngine;

using Process = FL::Process;
using Collider = FL::Collider;
using Scene = FL::Scene;

namespace FlatGui 
{ 
	void RenderProfiler()
	{
		FL::BeginWindow("Profiler", FG_b_showProfiler);
		// {

			static ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable;
			static bool b_animate = false;
			static int offset = 0;
			static bool b_showColliderPairs = true;

			FL::RenderCheckbox("Animate", b_animate);
			if (b_animate)
			{
				offset = (offset + 1) % 100;
			}


			int processCounter = 1;

			if (ImGui::BeginTable("##table", 3, flags, Vector2(-1, 0))) 
			{
				ImGui::TableSetupColumn("Process Name", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Time (ms)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Visualization");
				ImGui::TableHeadersRow();
				ImPlot::PushColormap(ImPlotColormap_Cool);

				if (FL::F_ProfilerProcesses.size() > 0)
				{
					for (std::vector<Process>::iterator iter = FL::F_ProfilerProcesses.begin(); iter != FL::F_ProfilerProcesses.end(); iter++)
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
							ImGui::Text(std::to_string(processCounter).c_str());
							ImGui::SameLine(0, 5);
							ImGui::Text(processName.c_str());
							ImGui::TableSetColumnIndex(1);
							ImGui::Text("%.0f ms", rawDataVector.front());
							ImGui::TableSetColumnIndex(2);
							ImGui::PushID(processCounter);
							Sparkline("##spark", dataArray, 100, 0, 10.0f, offset, ImPlot::GetColormapColor((int)rawDataVector.front()), Vector2(-1, 35));
							ImGui::PopID();
						}

						processCounter++;
					}
				}

				ImPlot::PopColormap();
				ImGui::EndTable();
			}

		FL::EndWindow(); // Profiler
	}

	void Sparkline(const char* ID, const float* values, int count, float min, float max, int offset, const Vector4& color, const Vector2& size) 
	{
		ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, Vector2(0, 0));
		if (ImPlot::BeginPlot(ID, size, ImPlotFlags_CanvasOnly)) 
		{
			ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
			ImPlot::SetupAxesLimits(0, count - 1, min, max, ImGuiCond_Always);
			ImPlot::SetNextLineStyle(color);
			ImPlot::SetNextFillStyle(color, 0.25);
			ImPlot::PlotLine(ID, values, count, 1, 0, ImPlotLineFlags_Shaded, offset);
			ImPlot::EndPlot();
		}
		ImPlot::PopStyleVar();
	}
}