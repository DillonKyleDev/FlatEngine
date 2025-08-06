///////////////////////
//// Global Assets ////
///////////////////////
#include "Application.h"

#include <map>
#include <string>
#include <memory>
#include <string>



namespace FlatEngine
{
	class Vector4;
	class Texture;

	class AssetManager
	{
	public:
		AssetManager();
		~AssetManager();	

		void UpdateProjectDirs(std::string projectPath);
		std::string GetRootPath();
		void FindRootDir();
		void CollectDirectories();
		void CollectColors();
		void CollectTextures();
		void CollectTags();
		std::string GetFailedToLoadImagePath();
		std::string GetDir(std::string dirName);
		std::string GetFilePath(std::string fileName);
		Vector4 GetColor(std::string colorName);
		Uint32 GetColor32(std::string colorName);
		std::shared_ptr<Texture>& GetTextureObject(std::string textureName);
		SDL_Texture* GetTexture(std::string textureName);		
	private:
		std::string m_rootPath;
		std::map<std::string, std::string> m_directories;
		std::map<std::string, std::string> m_files;
		std::map<std::string, Vector4> m_colors;
		std::map<std::string, std::shared_ptr<Texture>> m_textures;
		std::vector<std::string> m_tags;
		std::string m_resourceFailedToLoadImagePath;
		std::shared_ptr<Texture> m_errorTexture;
		Vector4 m_errorColor;
	};
}