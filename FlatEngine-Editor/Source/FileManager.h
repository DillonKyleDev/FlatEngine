#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <string>


namespace FlatEngine
{
    class Texture;
}

namespace FlatGui
{
    namespace FileManager
    {            
        extern std::string currentDirectory;

        extern void OpenFileContextually(std::filesystem::path fsPath);

        void RenderDirNodes(std::string dir);
        void RenderDirNode(std::filesystem::path fsPath, int IDCounter);
    }
}