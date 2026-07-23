#pragma once
#include "Profiler.h"
#include "tools/Vector2.h"
#include "tools/Vector4.h"

#include <deque>
#include <string>
#include <vector>


namespace FlatEngine
{
    namespace Profiler
    {
        class ProfilerProcess
		{
		public:
			ProfilerProcess(std::string name);

			std::string GetProcessName();
			void AddHangTimeData(float hangTime);
			std::deque<float> GetRawData();
			std::vector<float> GetHangTimeData();

		private:
			std::string m_processName;
			std::deque<float> m_rawHangTimeData;
			std::vector<float> m_snapShotData;
			int m_tickCounter;
		};

        extern std::vector<ProfilerProcess> processes;

        extern void AddProfilerProcess(std::string name);
        extern void AddProcessData(std::string processName, float data);
        extern void RemoveProfilerProcess(std::string toRemove);
        extern void SetupProfilerProcesses();
        extern void CleanupProfilerProcesses();
        extern void Sparkline(const char* ID, const float* values, int count, float min, float max, int offset, const FlatEngine::Vector4& color, const FlatEngine::Vector2& size);
    }
}