#include "components/Sprite.h"
#include "components/Transform.h"
#include "managers/Assets.h"
#include "managers/LuaManager.h"
#include "managers/PrefabManager.h"
#include "managers/SceneManager.h"
#include "render/Texture.h"
#include "tools/FileHelper.h"
#include "tools/Logger.h"

#include <filesystem>
#include "imgui.h"


namespace FlatEngine
{
    namespace Assets
    {
        AssetManager assetManager = AssetManager();

        AssetManager::AssetManager()
        {
            m_directories = std::map<std::string, std::string>();
            m_files = std::map<std::string, std::string>();
            m_colors = std::map<std::string, Vector4>();
            m_textures = std::map<std::string, std::shared_ptr<Texture>>();
            m_tags = std::vector<std::string>();
            m_errorTexture = std::make_shared<Texture>();
            m_noResourceTexture = std::make_shared<Texture>();
            m_errorColor = Vector4(1, 0, 0, 1);
            m_resourceFailedToLoadImagePath = "";	
            m_resourceNotPresentImagePath = "";
        }
        
        void AssetManager::UpdateProjectDirs(std::string projectPath)
        {
            std::string projectName = FileHelper::GetFilenameFromPath(projectPath);

            m_directories.emplace("projectDir",      "../projects/" + projectName + "/");
            m_directories.emplace("mappingContexts", "../projects/" + projectName + "/mappingContexts/");
            m_directories.emplace("prefabs",         "../projects/" + projectName + "/prefabs/");
            m_directories.emplace("animations",      "../projects/" + projectName + "/animations/");
            m_directories.emplace("scenes",          "../projects/" + projectName + "/scenes/");
            m_directories.emplace("scripts",         "../projects/" + projectName + "/scripts/");
            m_directories.emplace("audio",           "../projects/" + projectName + "/audio/");
            m_directories.emplace("images",          "../projects/" + projectName + "/images/");
            m_directories.emplace("tileSets",        "../projects/" + projectName + "/tileSets/");
            m_directories.emplace("tileTextures",    "../projects/" + projectName + "/images/tileTextures/");
        }

        std::string AssetManager::GetRootPath()
        {
            return m_rootPath;
        }

        void AssetManager::FindRootDir()
        {
            std::filesystem::path currentDir = std::filesystem::current_path();
            m_rootPath = "";
            int timeout = 10;
            bool b_rootFound = false;

            while (timeout)
            {
                for (const auto& entry : std::filesystem::directory_iterator(currentDir))
                {
                    if (entry.path().filename().string() == "FlatEngine.sln")
                    {
                        m_rootPath = currentDir.string();
                        m_rootPath += "/";
                        timeout = 0;
                        b_rootFound = true;
                    }
                }
                
                if (!b_rootFound)
                {
                    currentDir = currentDir.parent_path();
                    timeout--;
                }
            }

            if (!b_rootFound)
            {
                m_rootPath = "../";
            }
        }

        void AssetManager::CollectDirectories()
        {
            m_directories.clear();
            m_directories.emplace("projects", "../projects/");
            m_files.clear();

            m_files.emplace("colors", m_rootPath +   "engine/scripts/Colors.lua");
            m_files.emplace("textures", m_rootPath + "engine/scripts/Textures.lua");
            m_files.emplace("mainFont", m_rootPath + "engine/fonts/Cinzel/Cinzel-Black.ttf");
        }

        void AssetManager::CollectTags()
        {
            m_tags.clear();

            std::string tagsPath = "../engine/scripts/Tags.lua";

            // Load in lua script
            if (FileHelper::DoesFileExist(tagsPath))
            {
                try
                {
                    auto script = LuaManager::lua.safe_script_file(tagsPath);

                    std::optional<sol::table> tagsTable = LuaManager::lua["F_Tags"];

                    if (tagsTable)
                    {		
                        int counter = 1;
                        for (const auto& entry : tagsTable.value())
                        {
                            m_tags.push_back(entry.second.as<std::string>());
                        }
                    }
                }
                catch (sol::error err)
                {
                    Logger::log.Err("Something went wrong in Tags.lua {}", err.what());				
                }
            }
        }

        void AssetManager::CollectColors()
        {
            if (FileHelper::DoesFileExist(GetFilePath("colors")))
            {
                if (LuaManager::CheckLuaScriptFile(GetFilePath("colors")))
                {
                    try
                    {
                        auto script = LuaManager::lua.safe_script_file(GetFilePath("colors"));
                        std::optional<sol::table> colorTable = LuaManager::lua["F_Colors"];
                        m_colors.clear();

                        if (colorTable)
                        {
                            for (const auto& entry : colorTable.value())
                            {
                                sol::object key = entry.first;
                                sol::object value = entry.second;
                                std::string sKey = key.as<std::string>();     // cast key as a string
                                Vector4 sValue = value.as<Vector4>();         // cast key as a Vector4

                                m_colors.emplace(sKey, sValue);
                            }
                        }
                    }
                    catch (sol::error err)
                    {
                        Logger::log.Err("Something went wrong in lua file Colors.lua {}", err.what());					
                    }
                }
                else
                {
                    Logger::log.Err("Lua Colors file failed to load.");
                }
            }
        }	

        void AssetManager::CollectTextures()
        {		
            if (FileHelper::DoesFileExist(GetFilePath("textures")))
            {
                if (LuaManager::CheckLuaScriptFile(GetFilePath("textures")))
                {
                    m_textures.clear();
                    try
                    {
                        auto script = LuaManager::lua.safe_script_file(GetFilePath("textures"));

                        sol::object errPath = LuaManager::lua["F_ResourceFailedToLoadImagePath"];
                        m_resourceFailedToLoadImagePath = errPath.as<std::string>();
                        m_errorTexture->LoadFromFile(m_resourceFailedToLoadImagePath);
                        m_textures.emplace("resourceFailedToLoad", m_errorTexture);
        
                        sol::object noResourcePresentPath = LuaManager::lua["F_ResourceNotPresentImagePath"];
                        m_resourceNotPresentImagePath = noResourcePresentPath.as<std::string>();
                        m_noResourceTexture->LoadFromFile(m_resourceNotPresentImagePath);
                        m_textures.emplace("resourceNotPresent", m_noResourceTexture);

                        std::optional<sol::table> textureTable = LuaManager::lua["F_Textures"];
                        if (textureTable)
                        {
                            for (const auto& entry : textureTable.value())
                            {
                                sol::object key = entry.first;
                                sol::object value = entry.second;
                                std::string sKey = key.as<std::string>();
                                std::string sValue = value.as<std::string>();

                                std::shared_ptr<Texture> newTexture = std::make_shared<Texture>();

                                m_textures.emplace(sKey, newTexture);
                                m_textures.at(sKey)->LoadFromFile(sValue);
                            }
                        }
                    }
                    catch (sol::error err)
                    {
                        Logger::log.Err("Something went wrong in lua Textures.lua file {}", err.what());					
                    }
                }
                else
                {
                    Logger::log.Err("Lua Textures file failed to load.");
                }
            }

            // Load error texture
            if (m_resourceFailedToLoadImagePath != "")
            {
                m_errorTexture->LoadFromFile(m_resourceFailedToLoadImagePath);
            }
        }

        void AssetManager::LoadFonts()
        {
            m_mainFont = TTF_OpenFont(assetManager.GetFilePath("mainFont").c_str(), 40);
            if (m_mainFont == nullptr)
            {
                FlatEngine::Logger::log.Err("Failed to load lazy font! SDL_ttf Error: {}\n", TTF_GetError());
            }
        }

        void AssetManager::FreeFonts()
        {		
            TTF_CloseFont(m_mainFont);
            m_mainFont = nullptr;
        }

        std::string AssetManager::GetFailedToLoadImagePath()
        {
            return m_resourceFailedToLoadImagePath;
        }

        // Get directory path using name given in Directories.lua
        std::string AssetManager::GetDir(std::string dirName)
        {
            if (m_directories.count(dirName))
            {
                return m_directories.at(dirName);
            }
            else
            {
                return "";
            }
        }

        // Get file path using name given in Directories.lua
        std::string AssetManager::GetFilePath(std::string fileName)
        {
            if (m_files.count(fileName))
            {
                return m_files.at(fileName);
            }
            else
            {
                return "";
            }
        }

        // Get color using name given in Colors.lua file
        Vector4 AssetManager::GetColor(std::string colorName)
        {
            if (m_colors.count(colorName))
            {
                return m_colors.at(colorName);
            }
            else
            {
                return m_errorColor;
            }
        }

        // Get color using name given in Colors.lua file converted to Uint32 format
        Uint32 AssetManager::GetColor32(std::string colorName)
        {
            if (m_colors.count(colorName))
            {
                return ImGui::GetColorU32(m_colors.at(colorName));
            }
            else
            {
                return ImGui::GetColorU32(m_errorColor);
            }
        }

        // Get entire std::shared_ptr<Texture> object using name given in Textures.lua file
        std::shared_ptr<Texture>& AssetManager::GetTextureObject(std::string textureName)
        {
            if (m_textures.count(textureName))
            {
                return m_textures.at(textureName);
            }
            else
            {
                return m_errorTexture;
            }
        }

        VkDescriptorSet AssetManager::GetTexture(std::string textureName)
        {
            if (m_textures.count(textureName))
            {
                return m_textures.at(textureName)->GetTexture();
            }
            else
            {
                return m_errorTexture->GetTexture();
            }
        }

        std::vector<std::string> AssetManager::GetTags()
        {
            return m_tags;
        }

        TTF_Font* AssetManager::GetMainFont()
        {
            return m_mainFont;
        }

        GameObject* AssetManager::CreateAssetUsingFilePath(std::string filePath, Vector3 position)
        {
            std::string extension = std::filesystem::path(filePath).extension().string();

            if (extension == ".png" || extension == ".jpg" || extension == ".tif" || extension == ".webp" || extension == ".jxl")
            {
                GameObject* newObject = SceneManager::loadedScene.CreateGameObject();
                            
                newObject->SetName(FileHelper::GetFilenameFromPath(filePath) + "(" + std::to_string(newObject->GetID()) + ")");
                newObject->Get<Transform>()->SetPosition(Vector3(position.x, position.y, 0));
                newObject->Add<Sprite>()->SetTexture(filePath);
                return newObject;
            }
            else if (extension == ".prf")
            {
                return PrefabManager::Instantiate(FileHelper::GetFilenameFromPath(filePath), position);
            }
            else
            {
                return nullptr;
            }
            //else if (extension == ".scn")
            //{

            //}
            //// Mapping Context file
            //else if (extension == ".mpc")
            //{

            //}
            //// Animation file
            //else if (extension == ".anm")
            //{

            //}
            //// Lua file
            //else if (extension == ".scp")
            //{

            //}
        }
    }
}

