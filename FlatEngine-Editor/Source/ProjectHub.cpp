#include "FlatEngine.h"
#include "FlatGui.h"
#include "Project.h"
#include "Text.h"


namespace FL = FlatEngine;

namespace FlatGui
{
	std::vector<std::string> RetrieveProjectPaths()
	{
		std::vector<std::string> projectPaths = std::vector<std::string>();

		for (std::string projectPath : FL::FindAllFilesWithExtension(FL::GetDir("projectDir"), ".prj"))
		{
			projectPaths.push_back(projectPath);
		}

		return projectPaths;
	}

	void RenderProjectHub(bool& b_projectSelected, std::string& projectPath)
	{		
		static std::vector<std::string> projectPaths = RetrieveProjectPaths();

		bool b_isOpen = true;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));
		FL::SetNextViewportToFillWindow();
		FL::BeginWindow("Project Hub", b_isOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize, FL::GetColor("transparent"));		
		ImGui::PushStyleColor(ImGuiCol_WindowBg, FL::GetColor("transparent"));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, FL::GetColor("frameBg"));
		ImGui::PopStyleVar();
		// {

			// Get window dimensions for background image
			Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
			Vector2 canvas_sz = ImGui::GetContentRegionAvail();
			Vector2 dimensions = Vector2((float)FL::GetTextureObject("selectProject")->GetWidth(), (float)FL::GetTextureObject("selectProject")->GetHeight());
			float headerHeight = dimensions.y;

			// Draw window background gradient
			ImGui::Image(FL::GetTexture("projectHubBg"), canvas_sz);

			// Reset cursor to before drawing the bg image
			ImGui::SetCursorScreenPos(canvas_p0);

			// Draw header background gradient
			ImGui::Image(FL::GetTexture("flatEngineLogoGradient"), Vector2(canvas_sz.x, headerHeight + 10));
			ImGui::SetCursorScreenPos(Vector2(canvas_p0.x + 10, canvas_p0.y + 5));

			ImGui::Image(FL::GetTexture("selectProject"), dimensions);

			ImGui::Separator();
			ImGui::Separator();

			FL::MoveScreenCursor(0, 5);			
			FL::MoveScreenCursor(50, 0);
			ImGui::Text("Choose a project:");
			FL::MoveScreenCursor(0, 5);

			Vector2 startProjects = ImGui::GetCursorScreenPos();
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(50, 0));
			FL::BeginWindowChild("Projects", FL::GetColor("transparent"));
			ImGui::PopStyleVar();
			// {

				ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, startProjects.y));
				ImGui::PushStyleColor(ImGuiCol_ChildBg, FL::GetColor("projectSelectionTable"));
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2);
				ImGui::BeginChild("ProjectsTable", Vector2(0, ImGui::GetContentRegionAvail().y - 100), FL::F_childFlags);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				// {

					Vector2 startTable = ImGui::GetCursorScreenPos();
					float scrollY = 0;

					ImGui::PushStyleColor(ImGuiCol_TableRowBg, FL::GetColor32("transparent"));
					ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, FL::GetColor32("transparent"));
					ImGui::PushStyleColor(ImGuiCol_TableBorderLight, FL::GetColor32("transparent"));
					ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, FL::GetColor32("transparent"));
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
							for (std::string path : projectPaths)
							{
								ImGui::TableSetColumnIndex(0);
								ImGui::SetCursorScreenPos(Vector2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y));
								if (FL::RenderButton(FL::GetFilenameFromPath(path), Vector2(ImGui::GetContentRegionAvail().x, 60), 1, FL::GetColor("projectHubButton"), FL::GetColor("projectHubButtonHovered"), FL::GetColor("projectHubButtonActive")))
								{
									b_projectSelected = true;
									projectPath = path;
								}
								ImGui::TableNextRow();
							}
						}

						// Save table scroll for table outline
						scrollY = ImGui::GetScrollY();
						FL::PopTable();
					}

					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();

				// }
				FL::EndWindowChild(); // ProjectsTable
				

				Vector2 endTable = ImGui::GetCursorScreenPos();

				FL::MoveScreenCursor(0, 5);
				ImGui::Separator();				
				FL::MoveScreenCursor(ImGui::GetContentRegionAvail().x - 110, 6);

				static std::string projectName = "";
				static bool b_openProjectModal = false;

				if (FL::RenderButton("New Project", Vector2(100, 30)))
				{
					b_openProjectModal = true;
				}

				if (FL::RenderInputModal("Create New Project", "Project name", projectName, b_openProjectModal))
				{
					/*Project newProject = Project();
					std::string projectPath = FL::GetDir("projects") + "/" + projectName + ".prj";
					SaveProject(newProject, projectPath);
					LoadProject(projectPath);
					*/
					CreateNewProject(projectName);
					projectPaths = RetrieveProjectPaths();
				}

			// }
			FL::EndWindowChild(); // Projects

			ImGui::GetWindowDrawList()->AddRect(Vector2(startTable.x - 6, startTable.y - 6 + scrollY), Vector2(startTable.x + ImGui::GetContentRegionAvail().x - 104, endTable.y - 3), FL::GetColor32("projectHubTableOutline"), 2);

		// }
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		FL::EndWindow(); // Project Hub
	}
}