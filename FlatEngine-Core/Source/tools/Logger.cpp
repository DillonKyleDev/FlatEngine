#include "managers/Assets.h"
#include "tools/Vector4.h"
#include "tools/Logger.h"

#include <memory>
#include <spdlog/spdlog.h>


namespace FlatEngine
{
	namespace Logger 
	{
		Log log;

		std::string GetTimestampedLogPath()
		{
			auto now = std::chrono::system_clock::now();
			std::time_t t = std::chrono::system_clock::to_time_t(now);
			std::tm tm = *std::localtime(&t);

			std::ostringstream oss;
			oss << "../logs/log_output_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".log";
			return oss.str();
		}

		Log::Log()
		{
			m_buffer = ImGuiTextBuffer();

			// Create sinks for each output we will be logging to
			m_consoleSink       = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			m_fileSink          = std::make_shared<spdlog::sinks::basic_file_sink_mt>(GetTimestampedLogPath(), true);
			m_imguiSink         = std::make_shared<ImGuiLogSink>();
			m_imguiSink->logger = this;
			// m_imguiSink->set_pattern("[%l] %v");
			m_imguiSink->set_pattern("[%H:%M:%S.%2e%v");
			
			// Create a new logger and give it all the sinks we need
			std::vector<spdlog::sink_ptr> sinks { m_consoleSink, m_fileSink, m_imguiSink };
			auto logger = std::make_shared<spdlog::logger>("Engine", sinks.begin(), sinks.end());

			// register new logger as the default
			// spdlog::set_default_logger(logger);
			
			spdlog::register_logger(logger);
			m_logger = spdlog::get("Engine");
			m_logger->set_level(spdlog::level::trace);
		}

		void Log::LogVector2(Vector2 vector, std::string line, std::string from)
		{
			line = from + " " + line + " x: " + std::to_string(vector.x) + " y: " + std::to_string(vector.y) + "\n";
			m_logger->info("{}", line.c_str());
		}

		void Log::LogVector3(Vector3 vector, std::string line, std::string from)
		{
			line = from + " " + line + " x: " + std::to_string(vector.x) + " y: " + std::to_string(vector.y) + " z: " + std::to_string(vector.z) + "\n";
			m_logger->info("{}", line.c_str());
		}

		void Log::LogVector4(Vector4 vector, std::string line, std::string from)
		{
			line = from + " " + line + " x: " + std::to_string(vector.x) + " y: " + std::to_string(vector.y) + " z: " + std::to_string(vector.z) + " w: " + std::to_string(vector.w) + "\n";
			m_logger->info("{}", line.c_str());
		}

		void Log::LogSeparator()
		{
			std::string line = "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
			m_logger->info("{}", line.c_str());
		}

		void Log::DrawRectangle(Vector2 startingPoint, Vector2 endingPoint, std::string color, float thickness, ImDrawList* drawList)
		{
			drawList->AddRect(Vector2(startingPoint.x, startingPoint.y), Vector2(endingPoint.x, endingPoint.y), Assets::assetManager.GetColor32(color), 0.0f, 0, thickness);
		}

		void Log::DrawLine(Vector2 startingPoint, Vector2 endingPoint, std::string color, float thickness, ImDrawList* drawList)
		{
			drawList->AddLine(Vector2(startingPoint.x, startingPoint.y), Vector2(endingPoint.x, endingPoint.y), Assets::assetManager.GetColor32(color), thickness);
		}

		void Log::DrawPoint(Vector2 point, std::string color, ImDrawList* drawList)
		{
			drawList->AddLine(Vector2(point.x, point.y), Vector2(point.x + 1, point.y), Assets::assetManager.GetColor32(color));
			drawList->AddLine(Vector2(point.x, point.y), Vector2(point.x, point.y - 1), Assets::assetManager.GetColor32(color));
			drawList->AddLine(Vector2(point.x + 1, point.y + 1), Vector2(point.x + 1, point.y + 1), Assets::assetManager.GetColor32(color));
		}

		ImGuiTextBuffer &Log::GetBuffer()
		{
			return m_buffer;
		}
		
		void Log::ClearBuffer()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_buffer.clear();
			m_lineInfos.clear();
		}

		void Log::LuaDebugImpl(const std::string& message, const std::string& callingScript, const std::string& my_id, const std::string& tag)
		{
			std::string prefixedFmt = "[" + tag + "] [" + callingScript + " ID:" + my_id + "] " + message;
			m_logger->debug(fmt::runtime(prefixedFmt));
		}

		Vector4 Log::GetColorForLevel(spdlog::level::level_enum level)
		{
			switch (level) {
				case spdlog::level::trace:    return Assets::assetManager.GetColor("trace");
				case spdlog::level::debug:    return Assets::assetManager.GetColor("debug");
				case spdlog::level::info:     return Assets::assetManager.GetColor("info");
				case spdlog::level::warn:     return Assets::assetManager.GetColor("warn");
				case spdlog::level::err:      return Assets::assetManager.GetColor("err");
				case spdlog::level::critical: return Assets::assetManager.GetColor("critical");
				default:                      return Assets::assetManager.GetColor("info");
			}
		}

		Vector4 Log::GetColorForMessage(const std::string& message, spdlog::level::level_enum level)
		{
			if (message.find("[pst]", 0) != std::string::npos)
			{
				return Assets::assetManager.GetColor("persistent");
			}
			if (message.find("[lua]", 0) != std::string::npos)
			{
				return Assets::assetManager.GetColor("debug");
			}
			return GetColorForLevel(level);
		}

		void Log::AddLog(const std::string& message, spdlog::level::level_enum level)
		{			
			Vector4 color = GetColorForMessage(message, level);
			std::lock_guard<std::mutex> lock(m_mutex);
			int startOffset = m_buffer.size();
			m_buffer.append(message.c_str());
			m_buffer.append("\n");
			m_lineInfos.push_back({ startOffset, color, false, level });

			if ((int)m_lineInfos.size() > m_MAX_LOG_LINES)
			{
				TrimOldest();
			}
		}

		void Log::TrimOldest()
		{		
			int trimCount = m_MAX_LOG_LINES * 0.001f;
			int cutoffOffset = m_lineInfos[trimCount].startOffset;

			// Rebuild the buffer starting from cutoffOffset
			std::string remaining(m_buffer.begin() + cutoffOffset, m_buffer.end());
			m_buffer.clear();
			m_buffer.append(remaining.c_str());

			// Shift remaining line offsets down and drop the trimmed entries
			m_lineInfos.erase(m_lineInfos.begin(), m_lineInfos.begin() + trimCount);
			for (auto& info : m_lineInfos)
			{
				info.startOffset -= cutoffOffset;
			}
		}

		void Log::CopyToClipboard()
		{
			const char* bufStart = m_buffer.begin();

			std::string clipboardText;
			for (int i = 0; i < (int)m_lineInfos.size(); i++) {
				if (!m_lineInfos[i].selected) continue;
				const char* lineStart = bufStart + m_lineInfos[i].startOffset;
				const char* lineEnd = (i + 1 < m_lineInfos.size())
									? bufStart + m_lineInfos[i + 1].startOffset - 1 // -1 to exclude the '\n'
									: m_buffer.end();
				clipboardText.append(lineStart, lineEnd);
				clipboardText += "\n";
			}

			if (clipboardText != "") 
			{
				ImGui::SetClipboardText(clipboardText.c_str());			
			}
		}
	}
}