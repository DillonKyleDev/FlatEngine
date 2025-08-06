#pragma once
#include "AssetManager.h"
#include "FlatEngine.h"
#include "Texture.h"
#include "Vector4.h"
#include "Project.h"

#include "imgui.h"
#include <fstream>

namespace FlatEngine
{
	AssetManager::AssetManager()
	{
		m_directories = std::map<std::string, std::string>();
		m_files = std::map<std::string, std::string>();
		m_colors = std::map<std::string, Vector4>();
		m_textures = std::map<std::string, std::shared_ptr<Texture>>();
		m_tags = std::vector<std::string>();
		m_errorTexture = std::make_shared<Texture>();
		m_errorColor = Vector4(1, 0, 0, 1);
		m_resourceFailedToLoadImagePath = "";	
	}

	AssetManager::~AssetManager()
	{
	}

	void AssetManager::UpdateProjectDirs(std::string projectPath)
	{
		std::string projectName = GetFilenameFromPath(projectPath);

		m_directories.emplace("projectDir", "..\\projects\\" + projectName + "\\");
		m_directories.emplace("mappingContexts", "..\\projects\\" + projectName + "\\mappingContexts\\");
		m_directories.emplace("prefabs", "..\\projects\\" + projectName + "\\prefabs\\");
		m_directories.emplace("animations", "..\\projects\\" + projectName + "\\animations\\");
		m_directories.emplace("scenes", "..\\projects\\" + projectName + "\\scenes\\");
		m_directories.emplace("scripts", "..\\projects\\" + projectName + "\\scripts\\");
		m_directories.emplace("audio", "..\\projects\\" + projectName + "\\audio\\");
		m_directories.emplace("images", "..\\projects\\" + projectName + "\\images\\");
		m_directories.emplace("tileSets", "..\\projects\\" + projectName + "\\tileSets\\");
		m_directories.emplace("tileTextures", "..\\projects\\" + projectName + "\\imagese\\tileTextures\\");
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
					m_rootPath += "\\";
					timeout = 0;
					b_rootFound = true;
				}
			}
			
			if (!b_rootFound)
			{
				currentDir._Remove_filename_and_separator();
				timeout--;
			}
		}

		if (!b_rootFound)
		{
			m_rootPath = "../";
			LogError("Timeout: could not find directory FlatEngine2D.");
		}
	}

	void AssetManager::CollectDirectories()
	{
		m_directories.clear();
		m_directories.emplace("projects", "../projects/");
		m_files.clear();

		m_files.emplace("colors", m_rootPath + "engine\\scripts\\Colors.lua");
		m_files.emplace("textures", m_rootPath + "engine\\scripts\\Textures.lua");
		m_files.emplace("cinzelBlack", m_rootPath + "engine\\fonts\\Cinzel\\Cinzel-Black.ttf");
	}

	void AssetManager::CollectTags()
	{
		F_TagsAvailable.clear();

		std::string tagsPath = "../engine/scripts/Tags.lua";

		// Load in lua script
		if (DoesFileExist(tagsPath))
		{
			try
			{
				auto script = F_Lua.safe_script_file(tagsPath);

				std::optional<sol::table> tagsTable = F_Lua["F_Tags"];

				if (tagsTable)
				{		
					int counter = 1;
					for (const auto& entry : tagsTable.value())
					{
						F_TagsAvailable.push_back(entry.second.as<std::string>());
					}
				}
			}
			catch (sol::error err)
			{
				LogError("Something went wrong in Tags.lua");
				LogError(err.what());
			}
		}
	}

	void AssetManager::CollectColors()
	{
		if (DoesFileExist(GetFilePath("colors")))
		{
			if (CheckLuaScriptFile(GetFilePath("colors")))
			{
				try
				{
					auto script = F_Lua.safe_script_file(GetFilePath("colors"));
					std::optional<sol::table> colorTable = F_Lua["F_Colors"];
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
					LogError("Something went wrong in lua file Colors.lua");
					LogError(err.what());
				}
			}
			else
			{
				LogError("Lua Colors file failed to load.");
			}
		}
	}	

	void AssetManager::CollectTextures()
	{		
		if (DoesFileExist(GetFilePath("textures")))
		{
			if (CheckLuaScriptFile(GetFilePath("textures")))
			{
				m_textures.clear();
				try
				{
					auto script = F_Lua.safe_script_file(GetFilePath("textures"));
					sol::object errPath = F_Lua["F_ResourceFailedToLoadImagePath"];
					m_resourceFailedToLoadImagePath = errPath.as<std::string>();
					m_errorTexture->LoadFromFile(m_resourceFailedToLoadImagePath);

					std::optional<sol::table> textureTable = F_Lua["F_Textures"];
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
					LogError("Something went wrong in lua Textures.lua file");
					LogError(err.what());
				}
			}
			else
			{
				LogError("Lua Textures file failed to load.");
			}
		}

		// Load error texture
		if (m_resourceFailedToLoadImagePath != "")
		{
			m_errorTexture->LoadFromFile(m_resourceFailedToLoadImagePath);
		}
	}

	std::string AssetManager::GetFailedToLoadImagePath()
	{
		return m_resourceFailedToLoadImagePath;
	}

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

	SDL_Texture* AssetManager::GetTexture(std::string textureName)
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
}