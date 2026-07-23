#pragma once
#include "tools/Vector2.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"

#include <imgui.h>
#include <memory>
#include <string>
#include "spdlog/common.h"
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>  // if using stdout_color_sink_mt
#include <spdlog/sinks/basic_file_sink.h>     // if using basic_file_sink_mt
#include <spdlog/spdlog.h>
#include <spdlog/spdlog.h>
#include <thread.hpp>


namespace FlatEngine
{	
	namespace Logger
	{
		struct LogLineInfo 
		{
			int startOffset;  // byte offset into buffer where this line starts
			ImVec4 color;
			bool selected = false;
			spdlog::level::level_enum level;
		};

		class ImGuiLogSink;

		class Log
		{
		public:
			template <typename... Args>
			void Trace(spdlog::format_string_t<Args...> fmt, Args &&...args)    
			{			
				std::string prefixedFmt = "] [trace] " + std::string(fmt.get().data(), fmt.get().size());
				m_logger->trace(fmt::runtime(prefixedFmt), std::forward<Args>(args)...);
			}

			template <typename... Args>
			void Debug(spdlog::format_string_t<Args...> fmt, Args &&...args) 
			{
				std::string prefixedFmt = "] [debug] " + std::string(fmt.get().data(), fmt.get().size());
				m_logger->debug(fmt::runtime(prefixedFmt), std::forward<Args>(args)...);
			}

			template <typename... Args>
			void LuaDebug(std::string message, std::string callingScript, long my_id) 
			{
				std::string prefixedFmt = "] [" + callingScript + " ID:" + std::to_string(my_id) + "] " + message;
				m_logger->debug(fmt::runtime(prefixedFmt));				
			}

			template <typename... Args>
			void Info(spdlog::format_string_t<Args...> fmt, Args &&...args)
			{			
				std::string prefixedFmt = "] " + std::string(fmt.get().data(), fmt.get().size());
				m_logger->info(fmt::runtime(prefixedFmt), std::forward<Args>(args)...);
			}

			template <typename... Args>
			void Succ(spdlog::format_string_t<Args...> fmt, Args &&...args)
			{			
				std::string prefixedFmt = "] [success] " + std::string(fmt.get().data(), fmt.get().size());
				m_logger->debug(fmt::runtime(prefixedFmt), std::forward<Args>(args)...);
			}

			template <typename... Args>
			void Warn(spdlog::format_string_t<Args...> fmt, Args &&...args)
			{			
				std::string prefixedFmt = "] [warning] " + std::string(fmt.get().data(), fmt.get().size());				
				m_logger->warn(fmt::runtime(prefixedFmt), std::forward<Args>(args)...);
			}

			template <typename... Args>
			void Err(spdlog::format_string_t<Args...> fmt, Args &&...args)
			{			
				std::string prefixedFmt = "] [error] " + std::string(fmt.get().data(), fmt.get().size());
				m_logger->error(fmt::runtime(prefixedFmt), std::forward<Args>(args)...);
			}

			template <typename... Args>
			void Critical(spdlog::format_string_t<Args...> fmt, Args &&...args)
			{			
				std::string prefixedFmt = "] [critical] " + std::string(fmt.get().data(), fmt.get().size());
				m_logger->critical(fmt::runtime(prefixedFmt), std::forward<Args>(args)...);
			}		

			Log();
			~Log();
			void LogVector2(Vector2 vector, std::string line = "", std::string from = "[C++]");
			void LogVector3(Vector3 vector, std::string line = "", std::string from = "[C++]");
			void LogVector4(Vector4 vector, std::string line = "", std::string from = "[C++]");
			void LogSeparator();
			void DrawRectangle(Vector2 startingPoint, Vector2 endingPoint, std::string color, float thickness, ImDrawList* drawList);
			void DrawLine(Vector2 startingPoint, Vector2 endingPoint, std::string color, float thickness, ImDrawList* drawList);
			void DrawPoint(Vector2 point, std::string color, ImDrawList* drawlist);
			ImGuiTextBuffer &GetBuffer();
			void ClearBuffer();
			void AddLog(const std::string& message,  spdlog::level::level_enum level);
			void CopyToClipboard();

			std::mutex m_mutex;
			ImGuiTextBuffer m_buffer;			
			std::vector<LogLineInfo> m_lineInfos;	

		private:
			void TrimOldest();
			ImVec4 GetColorForLevel(spdlog::level::level_enum level);

			std::shared_ptr<spdlog::logger> m_logger;
			std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> m_consoleSink;
			std::shared_ptr<spdlog::sinks::basic_file_sink_mt> m_fileSink;
			std::shared_ptr<ImGuiLogSink> m_imguiSink;
			const int m_MAX_LOG_LINES = 1000;
		};

		class ImGuiLogSink : public spdlog::sinks::base_sink<std::mutex> 
		{
			public:
				Log* logger;

			protected:
				void sink_it_(const spdlog::details::log_msg& msg) override 
				{
					spdlog::memory_buf_t formatted;
					formatter_->format(msg, formatted);
					std::string text = fmt::to_string(formatted);					
					if (!text.empty() && text.back() == '\n') text.pop_back();

					logger->AddLog(text, msg.level);
				}
				void flush_() override {}
		};

		extern Log log;
	}
}

