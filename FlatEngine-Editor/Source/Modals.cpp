#include "FlatGui.h"
#include "GuiCore.h"
#include "imgui.h"
#include "managers/AnimationManager.h"
#include "managers/Assets.h"
#include "managers/Controls.h"
#include "managers/PrefabManager.h"
#include "managers/ProjectManager.h"
#include "managers/Scene.h"
#include "managers/SceneManager.h"
#include "Modals.h"
#include "render/Material.h"
#include "render/VulkanManager.h"
#include "scripting/CPPScriptMethods.h"
#include "tools/Vector2.h"

namespace FL = FlatEngine;


namespace FlatGui 
{
    namespace Modals
    {
        std::string newFileName = "";
        bool b_openProjectModal = false;
        bool b_openSceneModal = false;
        bool b_openLuaModal = false;
        bool b_openCPPModal = false;
        bool b_openAnimationModal = false;
        bool b_openMappingContextModal = false;
        bool b_openMaterialModal = false;
        bool b_openTileSetModal = false;
        bool b_openPrefabModal = false;
        long gameObjectToPrefab = -1;

        // Returns true if a valid input was retrieved. ***Remember to use static std::strings for inputValue***
        bool RenderInputModal(std::string label, std::string description, std::string& inputValue, bool& b_openModal)
        {		
            if (b_openModal)
            {
                ImGui::OpenPopup(label.c_str());
                b_openModal = false;
            }

            bool b_validInput = false;

            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));		
            ImGuiStyle& style = ImGui::GetStyle();    
            style.Colors[ImGuiCol_ModalWindowDimBg] = FL::Assets::assetManager.GetColor("modalWindowDimBg");  
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, FL::Vector2(10));
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, FL::Vector2(FL::GuiCore::childPadding));
            if (ImGui::BeginPopupModal(label.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {			
                ImGui::Text("%s", description.c_str());
                FL::GuiCore::MoveScreenCursor(0, 2);

                float inputWidth = ImGui::CalcTextSize(description.c_str()).x;
                if (inputWidth < 245)
                {
                    inputWidth = 245;
                }
                
                std::string inputLabel = "##" + label;
                FL::GuiCore::RenderInput(inputLabel.c_str(), "", inputValue, false, inputWidth);

                if (ImGui::IsKeyPressed(ImGuiKey_Enter))
                {
                    b_validInput = inputValue != "";
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::IsKeyPressed(ImGuiKey_Escape))
                {
                    b_validInput = false;
                    ImGui::CloseCurrentPopup();
                }

                FL::GuiCore::MoveScreenCursor(0, 5);

                if (FL::GuiCore::RenderButton("OK", FL::Vector2(50, 25)))
                { 
                    b_validInput = (inputValue != "");
                    ImGui::CloseCurrentPopup(); 
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine(0, 8);
                if (FL::GuiCore::RenderButton("Cancel", FL::Vector2(50, 25)))
                {
                    ImGui::CloseCurrentPopup();
                }			
                ImGui::EndPopup();
            }		
            
            ImGui::PopStyleVar(3);

            return b_validInput;
        }

        bool RenderConfirmModal(std::string label, std::string description, bool& b_openModal)
        {
            bool b_confirm = false;

            if (b_openModal)
            {
                ImGui::OpenPopup(label.c_str());
                b_openModal = false;
            }

            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal(label.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("%s", description.c_str());

                ImGui::Separator();

                float inputWidth = ImGui::CalcTextSize(description.c_str()).x;
                if (inputWidth < 250)
                {
                    inputWidth = 250;
                }

                if (ImGui::IsKeyPressed(ImGuiKey_Enter))
                {
                    b_confirm = true;
                    ImGui::CloseCurrentPopup();
                }

                if (ImGui::IsKeyPressed(ImGuiKey_Escape))
                {
                    b_confirm = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 145);
                if (ImGui::Button("Confirm", ImVec2(70, 0)))
                {
                    b_confirm = true;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine(0, 5);
                if (ImGui::Button("Cancel", ImVec2(70, 0)))
                {
                    b_confirm = false;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            return b_confirm;
        }
    }

    void RenderModals()
    {
        if (Modals::RenderInputModal("Create New Project", "Project name", Modals::newFileName, Modals::b_openProjectModal))
        {
            FL::ProjectManager::CreateNewProject(Modals::newFileName);
            Modals::newFileName = "";
        }
        if (Modals::RenderInputModal("Create Scene", "Enter a name for the Scene", Modals::newFileName, Modals::b_openSceneModal))
        {
            if (FL::SceneManager::CreateNewSceneFile(Modals::newFileName) != "")
            {
                FL::SceneManager::SaveTempScene();
                FL::SceneManager::SaveScene(FL::SceneManager::CreateAndLoadNewScene(), Modals::newFileName);
                FL::SceneManager::LoadScene(Modals::newFileName);
                FL::ProjectManager::loadedProject.loadedScenePath = Modals::newFileName;
                FL::ProjectManager::SaveCurrentProject();
            }

            Modals::newFileName = "";
        }

        if (Modals::RenderInputModal("Create Lua Script", "Enter a name for the Lua script", Modals::newFileName, Modals::b_openLuaModal))
        {
            FL::LuaManager::CreateNewLuaScript(Modals::newFileName);
            Modals::newFileName = "";
        }
        if (Modals::RenderInputModal("Create C++ Script", "Enter a name for the C++ script", Modals::newFileName, Modals::b_openCPPModal))
        {
            FL::CreateNewCPPScript(Modals::newFileName);
            Modals::newFileName = "";
        }
        if (Modals::RenderInputModal("Create Animation", "Enter a name for the Animation", Modals::newFileName, Modals::b_openAnimationModal))
        {
            FL::AnimationManager::CreateNewAnimationFile(Modals::newFileName);
            Modals::newFileName = "";
        }
        if (Modals::RenderInputModal("Create Mapping Context", "Enter a name for the Mapping Context", Modals::newFileName, Modals::b_openMappingContextModal))
        {
            FL::Controls::CreateNewMappingContextFile(Modals::newFileName);
            Modals::newFileName = "";
        }
        if (Modals::RenderInputModal("Create Material", "Enter a name for the Material", Modals::newFileName, Modals::b_openMaterialModal))
        {
            std::shared_ptr<FL::Material> newMaterial = FL::VulkanManager::vulkan.CreateNewMaterialFile(Modals::newFileName);
            FL::VulkanManager::vulkan.AddSceneViewMaterial(newMaterial);
            FL::VulkanManager::vulkan.AddGameViewMaterial(newMaterial);
            FL::VulkanManager::selectedMaterialName = Modals::newFileName;            
            Modals::newFileName = "";
        }
        if (Modals::RenderInputModal("Create TileSet", "Enter a name for the TileSet", Modals::newFileName, Modals::b_openTileSetModal))
        {
            // FL::CreateNewTileSetFile(newFileName);
            Modals::newFileName = "";
        }
        if (Modals::gameObjectToPrefab != -1)
		{
			if (Modals::RenderInputModal("Create Prefab", "Enter a name for the Prefab:", Modals::newFileName, Modals::b_openPrefabModal))
			{
                FL::GameObject* gameObject = FL::SceneManager::loadedScene.GetObjectByID(Modals::gameObjectToPrefab);				
				FL::PrefabManager::CreatePrefab(FL::Assets::assetManager.GetDir("prefabs") + "/" + Modals::newFileName + ".prf", gameObject);
				gameObject->SetIsPrefab(true);
				gameObject->SetPrefabName(Modals::newFileName);
				//currentObject.SetPrefabSpawnLocation(currentObject.GetTransform()->GetPosition());
                Modals::newFileName = "";
			}
		}
    }
}