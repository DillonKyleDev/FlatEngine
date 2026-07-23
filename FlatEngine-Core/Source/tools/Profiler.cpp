#include "Profiler.h"


namespace FlatEngine
{
	namespace Profiler
    {
		// Profiler use
		// float startTime = (float)FL::GetEngineTime();
		// startTime = (float)FL::GetEngineTime();
		// <Process runs here>
		// FL::AddProcessData("TileSet Editor", (float)FL::GetEngineTime() - startTime);

		std::vector<ProfilerProcess> processes = std::vector<ProfilerProcess>();

		
		ProfilerProcess::ProfilerProcess(std::string name)
		{
			m_processName = name;
			m_rawHangTimeData = std::deque<float>();
			m_tickCounter = 0;
		}

		std::string ProfilerProcess::GetProcessName()
		{
			return m_processName;
		}

		void ProfilerProcess::AddHangTimeData(float hangTime)
		{
			if (m_rawHangTimeData.size() < 100)
			{
				m_rawHangTimeData.push_front(hangTime);
			}
			else if (m_rawHangTimeData.size() >= 100)
			{
				m_rawHangTimeData.pop_front();
				m_rawHangTimeData.push_back(hangTime);
			}

			if (m_tickCounter < 100)
			{
				m_tickCounter++;
			}
			else if (m_tickCounter == 100)
			{
				m_snapShotData.clear();
				for (int i = 0; i < 100; i++)
				{
					m_snapShotData.push_back(m_rawHangTimeData[i]);
				}
				m_tickCounter++;
			}
			else if (m_tickCounter > 100 && m_tickCounter < 200)
			{
				m_tickCounter++;
			}
			else if (m_tickCounter == 200)
			{
				m_tickCounter = 0;
			}

		}

		std::deque<float> ProfilerProcess::GetRawData()
		{
			return m_rawHangTimeData;
		}

		std::vector<float> ProfilerProcess::GetHangTimeData()
		{
			return m_snapShotData;
		}

		void AddProfilerProcess(std::string name)
		{
			ProfilerProcess process = ProfilerProcess(name);
			processes.push_back(process);
		}

		void AddProcessData(std::string processName, float data)
		{
			for (ProfilerProcess &process : processes)
			{
				if (process.GetProcessName() == processName)
				{
					process.AddHangTimeData(data);
				}
			}
		}

		void RemoveProfilerProcess(std::string toRemove)
		{
			std::vector<ProfilerProcess>::iterator iter = processes.begin();

			while (iter != processes.end())
			{
				if ((*iter).GetProcessName() == toRemove)
				{
					processes.erase(iter);
					return;
				}
				iter++;
			}
		}

		void SetupProfilerProcesses()
		{
			// if (FG_b_showProfiler)
			// {					
			// 	FL::AddProfilerProcess("Render");
			// 	FL::AddProfilerProcess("Render Present");

			// 	if (FG_b_showFileExplorer)
			// 	{
			// 		FL::AddProfilerProcess("File Explorer");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("File Explorer");
			// 	}


			// 	if (FG_b_showTileSetEditor)
			// 	{
			// 		FL::AddProfilerProcess("TileSet Editor");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("TileSet Editor");
			// 	}

			// 	if (FG_b_showScriptEditor)
			// 	{
			// 		FL::AddProfilerProcess("Script Editor");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Script Editor");
			// 	}

			// 	if (FG_b_showHierarchy)
			// 	{
			// 		FL::AddProfilerProcess("Scene Hierarchy");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Scene Hierarchy");
			// 	}

			// 	if (FG_b_showPersistantHierarchy)
			// 	{
			// 		FL::AddProfilerProcess("Persistant Hierarchy");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Persistant Hierarchy");
			// 	}

			// 	if (FG_b_showInspector)
			// 	{
			// 		FL::AddProfilerProcess("Inspector");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Inspector");
			// 	}

			// 	if (FG_b_showGameView)
			// 	{
			// 		FL::AddProfilerProcess("Game View");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Game View");
			// 	}

			// 	if (FG_b_showSceneView)
			// 	{
			// 		FL::AddProfilerProcess("Scene View");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Scene View");
			// 	}

			// 	if (FG_b_showAnimator)
			// 	{
			// 		FL::AddProfilerProcess("Animator");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Animator");
			// 	}

			// 	if (FG_b_showAnimationPreview)
			// 	{
			// 		FL::AddProfilerProcess("Animation Preview");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Animation Preview");
			// 	}

			// 	if (FG_b_showKeyFrameEditor)
			// 	{
			// 		FL::AddProfilerProcess("Key Frame Editor");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Key Frame Editor");
			// 	}

			// 	if (FG_b_showLogger)
			// 	{
			// 		FL::AddProfilerProcess("Log");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Log");
			// 	}

			// 	if (FG_b_showProfiler)
			// 	{
			// 		FL::AddProfilerProcess("Profiler");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Profiler");
			// 	}

			// 	if (FG_b_showMappingContextEditor)
			// 	{
			// 		FL::AddProfilerProcess("Mapping Context Editor");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Mapping Context Editor");
			// 	}
			
			// 	if (FG_b_showSettings)
			// 	{
			// 		FL::AddProfilerProcess("Settings");
			// 	}
			// 	else
			// 	{
			// 		FL::RemoveProfilerProcess("Settings");
			// 	}

			// 	FL::AddProfilerProcess("Collision Testing");
			// }
		}

		void RunOnceAfterInitialization()
		{
			SetupProfilerProcesses();
		}

		void CleanupProfilerProcesses()
		{		
			RemoveProfilerProcess("Render");
			RemoveProfilerProcess("Render Present");
		}

		void Sparkline(const char* ID, const float* values, int count, float min, float max, int offset, const FlatEngine::Vector4& color, const FlatEngine::Vector2& size) 
		{
			//ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, Vector2(0, 0));
			//if (ImPlot::BeginPlot(ID, size, ImPlotFlags_CanvasOnly)) 
			//{
			//	ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
			//	ImPlot::SetupAxesLimits(0, count - 1, min, max, ImGuiCond_Always);
			//	ImPlot::SetNextLineStyle(color);
			//	ImPlot::SetNextFillStyle(color, 0.25);
			//	ImPlot::PlotLine(ID, values, count, 1, 0, ImPlotLineFlags_Shaded, offset);
			//	ImPlot::EndPlot();
			//}
			//ImPlot::PopStyleVar();
		}
	}
}