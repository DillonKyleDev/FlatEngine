#pragma once
#include "tools/Vector4.h"

#include <map>
#include <memory>
#include "SDL_ttf.h"
#include <string>
#include <vector>
#include "vulkan/vulkan_core.h"


namespace FlatEngine
{ 
    class Texture;

    namespace Assets
    {
        class AssetManager
        {
        public:
            AssetManager();

            void UpdateProjectDirs(std::string projectPath);
            std::string GetRootPath();
            void FindRootDir();
            void CollectDirectories();
            void CollectColors();
            void CollectTextures();
            void CollectTags();
            void LoadFonts();
            void FreeFonts();
            std::string GetFailedToLoadImagePath();
            std::string GetDir(std::string dirName);
            std::string GetFilePath(std::string fileName);
            Vector4 GetColor(std::string colorName);
            Uint32 GetColor32(std::string colorName);
            std::shared_ptr<Texture>& GetTextureObject(std::string textureName);            
            VkDescriptorSet GetTexture(std::string textureName);
            std::vector<std::string> GetTags();
            TTF_Font* GetMainFont();

        private:
            std::string m_rootPath;
            TTF_Font* m_mainFont;
            std::map<std::string, std::string> m_directories;
            std::map<std::string, std::string> m_files;
            std::map<std::string, Vector4> m_colors;
            std::map<std::string, std::shared_ptr<Texture>> m_textures;
            std::vector<std::string> m_tags;
            std::string m_resourceFailedToLoadImagePath;
            std::shared_ptr<Texture> m_errorTexture;
            std::string m_resourceNotPresentImagePath;
            std::shared_ptr<Texture> m_noResourceTexture;
            Vector4 m_errorColor;
        };

        extern AssetManager assetManager;
    }
}