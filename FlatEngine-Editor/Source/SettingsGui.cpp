#include "GuiCore.h"
#include "managers/AudioManager.h"
#include "managers/ProjectManager.h"

#include "imgui.h"
#include "SDL_mixer.h"

namespace FL = FlatEngine;


namespace FlatGui 
{
	void RenderSettings(bool& b_show)
	{
		if (!b_show)
			return;
		
		if (FL::GuiCore::BeginWindow("Settings", b_show))
		{			
			FL::GuiCore::BeginResizeWindowChild("EngineSettings");
			// {

				// List settings groups
				FL::GuiCore::RenderSectionHeader("Engine Setting");
				static std::string settingSelected = "Settings";
				if (FL::GuiCore::PushTreeList("EngineSettingSelect"))
				{
					FL::GuiCore::RenderTreeLeaf("Settings", settingSelected);
					//FL::RenderTreeLeaf("State", settingSelected);
					FL::GuiCore::PopTreeList();
				}

			// }
			ImGui::EndChild();



			// Vertical divider --------------------------------------------------------------------------
			ImGui::SameLine();




			FL::GuiCore::BeginWindowChild("Edit Settings");
			// {

				if (FL::GuiCore::PushTable("SettingsTable", 2))
				{
					// List settings per grouping
					if (settingSelected == "Settings")
					{
						// FL::GuiCore::RenderInputTableRow("##SceneLoadedOnStart", "Game start scene path", FL::ProjectManager::loadedProject.sceneToLoadAtRuntime, false);
						// if (ImGui::IsItemHovered())
						// {
						// 	FL::GuiCore::RenderTextToolTip("Enter as a relative path from inside the FlatEngine-Runtime folder (no quotation marks)\nie ../projects/project_name/scenes/scene_name.scn");
						// }

						// FL::GuiCore::RenderInputTableRow("Final Build Path", "Final project build path", FL::ProjectManager::loadedProject.buildPath);
						// if (ImGui::IsItemHovered())
						// {
						// 	FL::GuiCore::RenderTextToolTip("Absolute or relative path is allowed (no quotation marks)");
						// }

						int musicVolume = FL::ProjectManager::loadedProject.GetMusicVolume();
						int effectsVolume = FL::ProjectManager::loadedProject.GetEffectsVolume();
						// if (FL::GuiCore::RenderIntSliderTableRow("##MusicVolume", "Music Volume", musicVolume, 1, 0, MIX_MAX_VOLUME))
						// {
						// 	FL::AudioManager::SetMusicVolume(musicVolume);
						// }
						// if (FL::GuiCore::RenderIntSliderTableRow("##EffectVolume", "Effects Volume", effectsVolume, 1, 0, MIX_MAX_VOLUME))
						// {
						// 	FL::AudioManager::SetEffectsVolume(effectsVolume);
						// }
						
						// bool b_fullscreen = FL::ProjectManager::loadedProject.IsFullscreen();
						// if (FL::GuiCore::RenderCheckboxTableRow("##FullscreenCheckbox", "Fullscreen", b_fullscreen))
						// {
							// FL::ProjectManager::loadedProject.SetFullscreen(b_fullscreen);
						// }
						//bool b_vsyncEnabled = FL::ProjectManager::loadedProject.IsVsyncEnabled();
						//if (FL::RenderCheckboxTableRow("##VsyncCheckbox", "Vsync", b_vsyncEnabled))
						//{
						//	FL::ProjectManager::loadedProject.SetVsyncEnabled(b_vsyncEnabled);
						//}
					}
					else if (settingSelected == "State")
					{
						// FL::GuiCore::RenderCheckboxTableRow("##AutoSaveCheckbox", "Auto Save", FL::ProjectManager::loadedProject.b_autoSave);
					}

					FL::GuiCore::PopTable(); // SettingsTable
				}

			// }
			ImGui::EndChild(); // Edit Settings

			FL::GuiCore::EndWindow(); // Settings
		}		
	}
}