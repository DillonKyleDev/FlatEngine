#include "managers/Assets.h"
#include "GuiCore.h"
#include "tools/FileHelper.h"
#include "managers/ProjectManager.h"
#include "Modals.h"
#include "tools/Time.h"
#include "render/Texture.h"
#include "tools/Vector2.h"

#include <ctime>
#include <memory>
#include <vector>
#include <map>
#include <filesystem>

namespace FL = FlatEngine;


namespace FlatGui
{
	void RenderProjectHub(bool& b_projectSelected, std::string& projectPath)
	{				
		static std::vector<std::string> projectPaths = FL::ProjectManager::RetrieveProjectPaths();

		// Skip project hub for debugging:
		b_projectSelected = true;
		projectPath = projectPaths[0];
		return;

		bool b_isOpen = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, FL::Vector2(10, 10));
		FL::GuiCore::SetNextViewportToFillWindow();
		FL::GuiCore::BeginWindow("Project Hub", b_isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);		
		ImGui::PushStyleColor(ImGuiCol_WindowBg, FL::Assets::assetManager.GetColor("transparent"));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::Assets::assetManager.GetColor("frameBg"));
		ImGui::PopStyleVar();
		// {

			// Get window dimensions for background image
			FL::Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
			FL::Vector2 canvas_sz = ImGui::GetContentRegionAvail();
			FL::Vector2 dimensions = FL::Vector2((float)FL::Assets::assetManager.GetTextureObject("flatEngine")->GetWidth(), (float)FL::Assets::assetManager.GetTextureObject("flatEngine")->GetHeight());
			float headerHeight = dimensions.y;

			// Draw window background gradient
			ImGui::Image(FL::Assets::assetManager.GetTexture("projectHubBg"), canvas_sz);

			// Reset cursor to before drawing the bg image
			ImGui::SetCursorScreenPos(canvas_p0);

			// Draw header background gradient
			ImGui::Image(FL::Assets::assetManager.GetTexture("flatEngineLogoGradient"), FL::Vector2(canvas_sz.x, headerHeight + 10));
			ImGui::SetCursorScreenPos(FL::Vector2(canvas_p0.x + 10, canvas_p0.y + 5));

			ImGui::Image(FL::Assets::assetManager.GetTexture("flatEngine"), dimensions);

			ImGui::Separator();
			ImGui::Separator();

			FL::GuiCore::MoveScreenCursor(0, 5);			

			FL::Vector2 startProjects = ImGui::GetCursorScreenPos();
			FL::GuiCore::BeginWindowChild("Projects", "transparent");
			// {

				FL::GuiCore::MoveScreenCursor(10, 0);
				ImGui::PushStyleColor(ImGuiCol_Text, FL::Assets::assetManager.GetColor32("col_7"));
				ImGui::Text("Recent");

				ImGui::SetCursorScreenPos(FL::Vector2(ImGui::GetCursorScreenPos().x, startProjects.y));
				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::Assets::assetManager.GetColor("projectSelectionTable"));
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2);
				FL::GuiCore::MoveScreenCursor(10, 20);
				ImGui::BeginChild("ProjectsTable", FL::Vector2(ImGui::GetContentRegionAvail().x - 20, ImGui::GetContentRegionAvail().y - 100), FL::GuiCore::childFlags);
				ImGui::PopStyleVar();
			    ImGui::PopStyleColor();
				// {


					FL::Vector2 startTable = ImGui::GetCursorScreenPos();
					float scrollY = 0;

					ImGui::PushStyleColor(ImGuiCol_TableRowBg, FL::Assets::assetManager.GetColor32("transparent"));
					ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, FL::Assets::assetManager.GetColor32("transparent"));
					ImGui::PushStyleColor(ImGuiCol_TableBorderLight, FL::Assets::assetManager.GetColor32("transparent"));
					ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, FL::Assets::assetManager.GetColor32("transparent"));
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, FL::Vector2(40));
					if (FL::GuiCore::PushTable("#ProjectsTable", 1))
					{
						// {
						ImGui::TableNextRow();

						if (projectPaths.size() == 0)
						{
							ImGui::TableSetColumnIndex(0);
							ImGui::SetCursorPos(FL::Vector2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5));
							ImGui::Text("Nothing to see here...");
						}
						else
						{
							static bool b_projectNameTextsCreated = false;

							if (!b_projectNameTextsCreated)
							{
								FL::ProjectManager::RecreateProjectNameTexts();
								std::sort(projectPaths.begin(), projectPaths.end(), FL::ProjectManager::ProjectTimeCustomComp);
								b_projectNameTextsCreated = true;
							}


							for (int i = 0; i < projectPaths.size(); i++)
							{
								tm timeStruct = FL::Time::GetProjectTimeStruct(projectPaths[i]);
								std::string formattedTime = FL::Time::GetFormattedTime(timeStruct);								
								std::string dateModifiedString = "Modified:  " + formattedTime;
								std::string pathString = "Location: " + std::filesystem::canonical(projectPaths[i]).string();
								std::string projectName = FL::FileHelper::GetFilenameFromPath(projectPaths[i], true);

								std::shared_ptr<FL::Texture> texture = FL::ProjectManager::projectNameTexts.at(projectName).GetTexture();
								int width = texture->GetWidth();
								int height = texture->GetHeight();			
								float indent = 5;

								ImGui::TableSetColumnIndex(0);	
								FL::Vector2 buttonStart = ImGui::GetCursorScreenPos();
								buttonStart = buttonStart - FL::Vector2(3, 0);
								FL::Vector2 renderStart = FL::Vector2(ImGui::GetCursorScreenPos()) + FL::Vector2(indent, 0);
								FL::Vector2 renderEnd = renderStart + FL::Vector2((float)width, (float)height);
								if (texture->GetTexture() != NULL)
								{
									ImGui::GetWindowDrawList()->AddImage((void*)texture->GetTexture(), renderStart, renderEnd, FL::Vector2(), FL::Vector2(1), FL::Assets::assetManager.GetColor32("col_9"));
								}
								FL::GuiCore::MoveScreenCursor(indent, 35);
								ImGui::Text("%s", dateModifiedString.c_str());
								FL::GuiCore::MoveScreenCursor(indent, 0);
								ImGui::Text("%s", pathString.c_str());
								FL::GuiCore::MoveScreenCursor(0, 2);

								FL::Vector2 buttonSize = FL::Vector2(ImGui::GetContentRegionAvail().x + 6, 75);
								FL::GuiCore::RenderInvisibleButton(pathString.c_str(), buttonStart, buttonSize);
								bool b_hovered = ImGui::IsItemHovered();
								bool b_clicked = ImGui::IsItemClicked();

								if (b_hovered)
								{
									ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
									ImGui::GetWindowDrawList()->AddRectFilled(buttonStart, buttonStart + buttonSize, FL::Assets::assetManager.GetColor32("transparentLight"));
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
						FL::GuiCore::PopTable();
					}

					ImGui::PopStyleVar();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();


				// }
				FL::GuiCore::EndWindowChild(); // ProjectsTable
				

				FL::Vector2 endTable = ImGui::GetCursorScreenPos();

				FL::GuiCore::MoveScreenCursor(0, 5);
				ImGui::Separator();				
				FL::GuiCore::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 120, 6);

				static std::string projectName = "";
				static bool b_openProjectModal = false;

				if (FL::GuiCore::RenderButton("New Project", FL::Vector2(100, 30)))
				{
					b_openProjectModal = true;
				}

				if (Modals::RenderInputModal("Create New Project", "Project name", projectName, b_openProjectModal))
				{
					FL::ProjectManager::CreateNewProject(projectName);
					projectPaths = FL::ProjectManager::RetrieveProjectPaths();
					std::sort(projectPaths.begin(), projectPaths.end(), FL::ProjectManager::ProjectTimeCustomComp);
					FL::ProjectManager::RecreateProjectNameTexts();
				}

			// }
			FL::GuiCore::EndWindowChild(); // Projects

			ImGui::GetWindowDrawList()->AddRect(FL::Vector2(startTable.x - 6, startTable.y - 6 + scrollY), FL::Vector2(startTable.x + ImGui::GetContentRegionAvail().x - 36, endTable.y - 3), FL::Assets::assetManager.GetColor32("projectHubTableOutline"), 2);

		// }
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		FL::GuiCore::EndWindow(); // Project Hub
	}
}