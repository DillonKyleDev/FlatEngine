#include "FlatEngine.h"
#include "FlatGui.h"
#include "Project.h"
#include "Text.h"
#include "Texture.h"
#include "Vector2.h"
#include "WindowManager.h"

#include <ctime>
#include <memory>
#include <vector>
#include <map>
#include <filesystem>


namespace FL = FlatEngine;

namespace FlatGui
{
	std::map<std::string, Text> FG_projectNameTexts = std::map<std::string, Text>();

	std::vector<std::string> RetrieveProjectPaths()
	{
		std::vector<std::string> projectPaths = std::vector<std::string>();

		for (std::string projectPath : FL::FindAllFilesWithExtension("../projects", ".prj"))
		{
			projectPaths.push_back(projectPath);
		}

		return projectPaths;
	}

	void RecreateProjectNameTexts()
	{
		std::vector<std::string> projectPaths = RetrieveProjectPaths();
		FG_projectNameTexts.clear();

		for (int i = 0; i < projectPaths.size(); i++)
		{
			std::string projectName = FL::GetFilenameFromPath(projectPaths[i], true);
			Text projectNameText = Text();
			std::pair<std::string, Text> textPair = { projectName, projectNameText };
			FG_projectNameTexts.emplace(textPair);
			FG_projectNameTexts.at(projectName).SetFontPath("../engine/fonts/Karla-Regular.ttf");
			FG_projectNameTexts.at(projectName).SetFontSize(24);
			FG_projectNameTexts.at(projectName).SetText(projectName);
		}
	}

	bool ProjectTimeCustomComp(std::string path1, std::string path2)
	{
		tm timeStruct1 = FL::GetProjectTimeStruct(path1);
		tm timeStruct2 = FL::GetProjectTimeStruct(path2);
		time_t timeStamp1 = FL::CreateTimeStamp(timeStruct1);
		time_t timeStamp2 = FL::CreateTimeStamp(timeStruct2);

		return FL::CompareTimeStamps(timeStamp1, timeStamp2);		
	}

	void RenderProjectHub(bool& b_projectSelected, std::string& projectPath)
	{				
		static std::vector<std::string> projectPaths = RetrieveProjectPaths();

		bool b_isOpen = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(10, 10));
		FL::SetNextViewportToFillWindow();
		FL::BeginWindow("Project Hub", b_isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize, FL::GetColor("transparent"));		
		ImGui::PushStyleColor(ImGuiCol_WindowBg, FL::GetColor("transparent"));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("frameBg"));
		ImGui::PopStyleVar();
		// {

			// Get window dimensions for background image
			Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
			Vector2 canvas_sz = ImGui::GetContentRegionAvail();
			Vector2 dimensions = Vector2((float)FL::GetTextureObject("flatEngine")->GetWidth(), (float)FL::GetTextureObject("flatEngine")->GetHeight());
			float headerHeight = dimensions.y;

			// Draw window background gradient
			ImGui::Image(FL::GetTexture("projectHubBg"), canvas_sz);

			// Reset cursor to before drawing the bg image
			ImGui::SetCursorScreenPos(canvas_p0);

			// Draw header background gradient
			ImGui::Image(FL::GetTexture("flatEngineLogoGradient"), Vector2(canvas_sz.x, headerHeight + 10));
			ImGui::SetCursorScreenPos(Vector2(canvas_p0.x + 10, canvas_p0.y + 5));

			ImGui::Image(FL::GetTexture("flatEngine"), dimensions);

			ImGui::Separator();
			ImGui::Separator();

			FL::MoveScreenCursor(0, 5);			

			Vector2 startProjects = ImGui::GetCursorScreenPos();
			FL::BeginWindowChild("Projects", FL::GetColor("transparent"));
			// {

				FL::MoveScreenCursor(10, 0);
				ImGui::PushStyleColor(ImGuiCol_Text, FL::GetColor32("col_7"));
				ImGui::Text("Recent");

				ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, startProjects.y));
				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("projectSelectionTable"));
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2);
				FL::MoveScreenCursor(10, 20);
				ImGui::BeginChild("ProjectsTable", Vector2(ImGui::GetContentRegionAvail().x - 20, ImGui::GetContentRegionAvail().y - 100), FL::F_childFlags);
				ImGui::PopStyleVar();
			    ImGui::PopStyleColor();
				// {


					Vector2 startTable = ImGui::GetCursorScreenPos();
					float scrollY = 0;

					ImGui::PushStyleColor(ImGuiCol_TableRowBg, FL::GetColor32("transparent"));
					ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, FL::GetColor32("transparent"));
					ImGui::PushStyleColor(ImGuiCol_TableBorderLight, FL::GetColor32("transparent"));
					ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, FL::GetColor32("transparent"));
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, Vector2(40));
					if (FL::PushTable("#ProjectsTable", 1))
					{
						// {
						ImGui::TableNextRow();

						if (projectPaths.size() == 0)
						{
							ImGui::TableSetColumnIndex(0);
							ImGui::SetCursorPos(Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5));
							ImGui::Text("Nothing to see here...");
						}
						else
						{
							static bool b_projectNameTextsCreated = false;

							if (!b_projectNameTextsCreated)
							{
								RecreateProjectNameTexts();
								std::sort(projectPaths.begin(), projectPaths.end(), ProjectTimeCustomComp);
								b_projectNameTextsCreated = true;
							}


							for (int i = 0; i < projectPaths.size(); i++)
							{
								tm timeStruct = FL::GetProjectTimeStruct(projectPaths[i]);
								std::string formattedTime = FL::GetFormattedTime(timeStruct);								
								std::string dateModifiedString = "Modified:  " + formattedTime;
								std::string pathString = "Location: " + std::filesystem::canonical(projectPaths[i]).string();
								std::string projectName = FL::GetFilenameFromPath(projectPaths[i], true);

								std::shared_ptr<Texture> texture = FG_projectNameTexts.at(projectName).GetTexture();
								int width = texture->GetWidth();
								int height = texture->GetHeight();			
								float indent = 5;

								ImGui::TableSetColumnIndex(0);	
								Vector2 buttonStart = ImGui::GetCursorScreenPos();
								buttonStart = buttonStart - Vector2(3, 0);
								Vector2 renderStart = Vector2(ImGui::GetCursorScreenPos()) + Vector2(indent, 0);
								Vector2 renderEnd = renderStart + Vector2((float)width, (float)height);
								ImGui::GetWindowDrawList()->AddImage((void*)texture->GetTexture(), renderStart, renderEnd, Vector2(0), Vector2(1), FL::GetColor32("col_9"));
								FL::MoveScreenCursor(indent, 35);
								ImGui::Text(dateModifiedString.c_str());
								FL::MoveScreenCursor(indent, 0);
								ImGui::Text(pathString.c_str());
								FL::MoveScreenCursor(0, 2);

								Vector2 buttonSize = Vector2(ImGui::GetContentRegionAvail().x + 6, 75);
								FL::RenderInvisibleButton(pathString.c_str(), buttonStart, buttonSize);
								bool b_hovered = ImGui::IsItemHovered();
								bool b_clicked = ImGui::IsItemClicked();

								if (b_hovered)
								{
									ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
									ImGui::GetWindowDrawList()->AddRectFilled(buttonStart, buttonStart + buttonSize, FL::GetColor32("transparentLight"));
								}
								if (b_clicked)
								{
									b_projectSelected = true;
									projectPath = projectPaths[i];
								}

								if (i < projectPaths.size() - 1)
								{
									ImGui::TableNextRow();
								}
							}
						}

						// Save table scroll for table outline
						scrollY = ImGui::GetScrollY();
						FL::PopTable();
					}

					ImGui::PopStyleVar();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();


				// }
				FL::EndWindowChild(); // ProjectsTable
				

				Vector2 endTable = ImGui::GetCursorScreenPos();

				FL::MoveScreenCursor(0, 5);
				ImGui::Separator();				
				FL::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 120, 6);

				static std::string projectName = "";
				static bool b_openProjectModal = false;

				if (FL::RenderButton("New Project", Vector2(100, 30)))
				{
					b_openProjectModal = true;
				}

				if (FL::RenderInputModal("Create New Project", "Project name", projectName, b_openProjectModal))
				{
					CreateNewProject(projectName);
					projectPaths = RetrieveProjectPaths();
					std::sort(projectPaths.begin(), projectPaths.end(), ProjectTimeCustomComp);
					RecreateProjectNameTexts();
				}

			// }
			FL::EndWindowChild(); // Projects

			ImGui::GetWindowDrawList()->AddRect(Vector2(startTable.x - 6, startTable.y - 6 + scrollY), Vector2(startTable.x + ImGui::GetContentRegionAvail().x - 36, endTable.y - 3), FL::GetColor32("projectHubTableOutline"), 2);

		// }
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		FL::EndWindow(); // Project Hub
	}
}