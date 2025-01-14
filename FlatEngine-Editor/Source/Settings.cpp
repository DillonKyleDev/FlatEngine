#include "FlatEngine.h"
#include "FlatGui.h"
#include "Project.h"

#include "imgui.h"
#include "SDL_mixer.h"


namespace FL = FlatEngine;

namespace FlatGui 
{
	void RenderSettings()
	{
		ImGui::SetNextItemWidth(400);
		FL::BeginWindow("Settings", FG_b_showSettings);
		// {
			
			FL::BeginResizeWindowChild("EngineSettings");
			// {

				// List settings groups
				FL::RenderSectionHeader("Engine Setting");
				static std::string settingSelected = "Settings";
				if (FL::PushTreeList("EngineSettingSelect"))
				{
					FL::RenderTreeLeaf("Settings", settingSelected);
					//FL::RenderTreeLeaf("State", settingSelected);
					FL::PopTreeList();
				}

			// }
			ImGui::EndChild();



			// Vertical divider --------------------------------------------------------------------------
			ImGui::SameLine();




			FL::BeginWindowChild("Edit Settings");
			// {

				if (FL::PushTable("SettingsTable", 2))
				{
					// List settings per grouping
					if (settingSelected == "Settings")
					{
						std::string startupScene = FL::F_LoadedProject.GetRuntimeScene();
						if (FL::RenderInputTableRow("##SceneLoadedOnStart", "Game start scene path", startupScene, false))
						{
							FL::F_LoadedProject.SetRuntimeScene(startupScene);
						}
						if (ImGui::IsItemHovered())
						{
							FL::RenderTextToolTip("Enter as a relative path from inside the FlatEngine-Runtime folder (no quotation marks)\nie ../projects/project_name/scenes/scene_name.scn");
						}

						std::string finalBuildPath = FL::F_LoadedProject.GetBuildPath();
						if (FL::RenderInputTableRow("Final Build Path", "Final project build path", finalBuildPath))
						{
							FL::F_LoadedProject.SetBuildPath(finalBuildPath);
						}
						if (ImGui::IsItemHovered())
						{
							FL::RenderTextToolTip("Absolute or relative path is allowed (no quotation marks)");
						}

						Vector2 currentResolution = FL::F_LoadedProject.GetResolution();
						std::string currentResString = std::to_string((int)currentResolution.x) + " x " + std::to_string((int)currentResolution.y);
						std::vector<std::string> resolutions = { "800 x 600", "1920 x 1080", "1920 x 1200" };
						static int currentResolutionIndex = 0;

						for (int r = 0; r < resolutions.size(); r++)
						{
							if (resolutions[r] == currentResString)
							{
								currentResolutionIndex = r;
							}
						}

						//FL::RenderSelectableTableRow("##ResolutionSelect", "Resolution", resolutions, currentResolutionIndex);

						//if (resolutions.at(currentResolutionIndex) == "800 x 600")
						//{
						//	FL::F_LoadedProject.SetResolution(Vector2(800, 600));
						//}
						//else if (resolutions.at(currentResolutionIndex) == "1920 x 1080")
						//{
						//	FL::F_LoadedProject.SetResolution(Vector2(1920, 1080));
						//}
						//else if (resolutions.at(currentResolutionIndex) == "1920 x 1200")
						//{
						//	FL::F_LoadedProject.SetResolution(Vector2(1920, 1200));
						//}

						int musicVolume = FL::GetLoadedProject().GetMusicVolume();
						int effectsVolume = FL::GetLoadedProject().GetEffectsVolume();
						if (FL::RenderIntSliderTableRow("##MusicVolume", "Music Volume", musicVolume, 1, 0, MIX_MAX_VOLUME))
						{
							FL::SetMusicVolume(musicVolume);
						}
						if (FL::RenderIntSliderTableRow("##EffectVolume", "Effects Volume", effectsVolume, 1, 0, MIX_MAX_VOLUME))
						{
							FL::SetEffectsVolume(effectsVolume);
						}
						
						bool b_fullscreen = FL::F_LoadedProject.IsFullscreen();
						if (FL::RenderCheckboxTableRow("##FullscreenCheckbox", "Fullscreen", b_fullscreen))
						{
							FL::F_LoadedProject.SetFullscreen(b_fullscreen);
						}
						//bool b_vsyncEnabled = FL::F_LoadedProject.IsVsyncEnabled();
						//if (FL::RenderCheckboxTableRow("##VsyncCheckbox", "Vsync", b_vsyncEnabled))
						//{
						//	FL::F_LoadedProject.SetVsyncEnabled(b_vsyncEnabled);
						//}
					}
					else if (settingSelected == "State")
					{
						bool b_autoSave = FL::F_LoadedProject.AutoSaveOn();
						if (FL::RenderCheckboxTableRow("##AutoSaveCheckbox", "Auto Save", b_autoSave))
						{
							FL::F_LoadedProject.SetAutoSave(b_autoSave);
						}
					}

					FL::PopTable(); // SettingsTable
				}

			// }
			ImGui::EndChild(); // Edit Settings

		// }
		FL::EndWindow(); // Settings
	}
}