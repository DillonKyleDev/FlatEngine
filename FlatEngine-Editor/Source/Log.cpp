#include "FlatGui.h"
#include "GuiCore.h"
#include "managers/Assets.h"
#include "managers/Settings.h"
#include "tools/Logger.h"

#include "imgui.h"

namespace FL = FlatEngine;


namespace FlatGui 
{
	void RenderLog(bool& b_show)
	{
		if (!b_show)
			return;

		if (FL::GuiCore::BeginWindow("Log", b_show))
		{										
			auto windowPos = ImGui::GetWindowPos();
			auto windowSize = ImGui::GetWindowSize();
			//ImGui::GetWindowDrawList()->AddRect({ windowPos.x + 2, windowPos.y + 2 }, { windowSize.x + windowPos.x - 2, windowPos.y + windowSize.y - 2 }, FL::Assets::assetManager.GetColor32("componentBorder"), 0);


			FL::GuiCore::MoveScreenCursor(2, 1);
			
			static int lines = 0;
			if (FL::GuiCore::RenderImageButton("clearLogButton", FL::Assets::assetManager.GetTexture("clear"), FL::Vector2(16), 0))
			{
				FL::Logger::log.ClearBuffer();
				lines = 0;
			}

			ImGui::SameLine(0, 3);

			if (FL::GuiCore::RenderButton("copy", FL::Vector2(40, 18), 0, "button", "buttonHovered","buttonActive", FL::Vector2(5, 1)))
			{
				FL::Logger::log.CopyToClipboard();
			}

			ImGui::SameLine();			
			
			FL::GuiCore::MoveScreenCursor(0, 3); if (FL::GuiCore::RenderCheckbox("Trace", FL::Settings::settings.b_showTrace))       { FL::Settings::settings.SaveSettings(); } ImGui::SameLine();
			FL::GuiCore::MoveScreenCursor(0, 3); if (FL::GuiCore::RenderCheckbox("Debug", FL::Settings::settings.b_showDebug))       { FL::Settings::settings.SaveSettings(); } ImGui::SameLine();
			FL::GuiCore::MoveScreenCursor(0, 3); if (FL::GuiCore::RenderCheckbox("Info", FL::Settings::settings.b_showInfo))         { FL::Settings::settings.SaveSettings(); } ImGui::SameLine();
			FL::GuiCore::MoveScreenCursor(0, 3); if (FL::GuiCore::RenderCheckbox("Warn", FL::Settings::settings.b_showWarn))         { FL::Settings::settings.SaveSettings(); } ImGui::SameLine();
			FL::GuiCore::MoveScreenCursor(0, 3); if (FL::GuiCore::RenderCheckbox("Error", FL::Settings::settings.b_showError))       { FL::Settings::settings.SaveSettings(); } ImGui::SameLine();
			FL::GuiCore::MoveScreenCursor(0, 3); if (FL::GuiCore::RenderCheckbox("Critical", FL::Settings::settings.b_showCritical)) { FL::Settings::settings.SaveSettings(); } 

			FL::Vector2 cursorPos = FL::Vector2(ImGui::GetCursorScreenPos().x - 1, ImGui::GetCursorScreenPos().y - 1);
			FL::Vector2 availSpace = ImGui::GetContentRegionAvail();

			// Draw Border around log
			ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, FL::Vector2(cursorPos.x + availSpace.x + 2, cursorPos.y + availSpace.y + 2), FL::Assets::assetManager.GetColor32("logOutline"));


			ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::Assets::assetManager.GetColor("logBg"));
			FL::GuiCore::BeginWindowChild("Log", "logBg", FL::GuiCore::childFlags);
			ImGui::PopStyleColor();
			// {
				
				bool b_triggerCopy = false;
				std::lock_guard<std::mutex> lock(FL::Logger::log.m_mutex);
				const char* bufStart = FL::Logger::log.m_buffer.begin();

				std::vector<int> filteredLogIndices = std::vector<int>();
				filteredLogIndices.reserve(FL::Logger::log.m_lineInfos.size());
				for (int i = 0; i < FL::Logger::log.m_lineInfos.size(); i++)
				{
					auto& info = FL::Logger::log.m_lineInfos[i];
					switch (info.level)
					{
						case spdlog::level::trace:    if (!FL::Settings::settings.b_showTrace)    continue; break;
						case spdlog::level::debug:    if (!FL::Settings::settings.b_showDebug)    continue; break;
						case spdlog::level::info:     if (!FL::Settings::settings.b_showInfo)     continue; break;
						case spdlog::level::warn:     if (!FL::Settings::settings.b_showWarn)     continue; break;
						case spdlog::level::err:      if (!FL::Settings::settings.b_showError)    continue; break;
						case spdlog::level::critical: if (!FL::Settings::settings.b_showCritical) continue; break;
						default: break;
					}

					filteredLogIndices.push_back(i);
				}

				ImGuiListClipper clipper;
				clipper.Begin((int)filteredLogIndices.size());
				while (clipper.Step()) 
				{
					for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
					{
						int i = filteredLogIndices[row]; // map the clipped row back to the real line index
						auto& info = FL::Logger::log.m_lineInfos[i];
						const char* lineStart = bufStart + info.startOffset;
						const char* lineEnd = (i + 1 < FL::Logger::log.m_lineInfos.size())
								? bufStart + FL::Logger::log.m_lineInfos[i + 1].startOffset - 1 // -1 to exclude the '\n'
								: FL::Logger::log.m_buffer.end();

						ImGui::PushID(i);
						ImGui::PushStyleColor(ImGuiCol_Text, info.color);
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1,1,1,0.1f));
						ImGui::PushStyleColor(ImGuiCol_HeaderActive,  ImVec4(1,1,1,0.15f));

						std::string lineStr(lineStart, lineEnd); // Selectable needs a null-terminated label
						if (ImGui::Selectable(lineStr.c_str(), info.selected)) 
						{
							if (!ImGui::GetIO().KeyCtrl) 
							{
								for (auto& l : FL::Logger::log.m_lineInfos) l.selected = false; // clear others unless ctrl-clicking
							}
							info.selected = !info.selected;
						}
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{							
							b_triggerCopy = true;
						}

						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::PopID();
					}
				}
				clipper.End();

				if (b_triggerCopy)
				{
					FL::Logger::log.CopyToClipboard();
					b_triggerCopy = false;
				}
			
			// }	
			FL::GuiCore::EndWindowChild(); // Log Child			
		}			

		FL::GuiCore::EndWindow(); // Log
	}
}