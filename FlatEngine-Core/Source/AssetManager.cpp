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

	void AssetManager::CollectDirectories(DirectoryType dirType)
	{
		m_directories.clear();
		m_files.clear();

		std::string dirPath = "";

		switch (dirType)
		{
		case RuntimeDir:
			dirPath = F_RuntimeDirectoriesLuaFilepath;
			break;
		case EditorDir:
			dirPath = F_EditorDirectoriesLuaFilepath;
			break;
		default:
			break;
		}

		// Load in lua script
		if (DoesFileExist(dirPath))
		{
			auto script = F_Lua.safe_script_file(dirPath);
			std::optional<sol::table> dirTable = F_Lua["F_Dirs"];
			std::optional<sol::table> pathTable = F_Lua["F_Paths"];

			// Directories
			if (dirTable)
			{
				for (const auto& entry : dirTable.value())
				{
					sol::object key = entry.first;
					sol::object value = entry.second;
					std::string sKey = key.as<std::string>();     // cast key as a string
					std::string sValue = value.as<std::string>(); // cast key as a string

					m_directories.emplace(sKey, sValue);

					// Create the directory if it doesn't yet exist
					if (!DoesFileExist(sValue) && sKey != "root")
					{
						//std::filesystem::create_directories(sValue);
					}
				}
			}

			// File Paths
			if (pathTable)
			{
				for (const auto& entry : pathTable.value())
				{
					sol::object key = entry.first;
					sol::object value = entry.second;
					std::string sKey = key.as<std::string>();     // cast key as a string
					std::string sValue = value.as<std::string>(); // cast key as a string

					m_files.emplace(sKey, sValue);
				}
			}
		}
	}

	void AssetManager::CollectColors()
	{
		// Load in lua script
		if (DoesFileExist(GetFilePath("colors")))
		{
			if (CheckLuaScriptFile(GetFilePath("colors")))
			{
				m_colors.clear();
				auto script = F_Lua.safe_script_file(GetFilePath("colors"));
				std::optional<sol::table> colorTable = F_Lua["F_Colors"];

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
			else
			{
				LogError("Lua Colors file failed to load.");
			}
		}
	}	

	void AssetManager::CollectTextures()
	{
		// Load in lua script
		if (DoesFileExist(GetFilePath("textures")))
		{
			if (CheckLuaScriptFile(GetFilePath("textures")))
			{
				m_textures.clear();
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
						std::string sKey = key.as<std::string>();     // cast key as a string
						std::string sValue = value.as<std::string>(); // cast key as a string

						std::shared_ptr<Texture> newTexture = std::make_shared<Texture>();
						m_textures.emplace(sKey, newTexture);
						m_textures.at(sKey)->LoadFromFile(sValue);
					}
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

	std::shared_ptr<Texture> AssetManager::GetTextureObject(std::string textureName)
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