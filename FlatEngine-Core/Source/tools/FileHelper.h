#pragma once

#include <string>
#include <vector>


namespace FlatEngine
{
    namespace FileHelper
    {
        extern std::string OpenSaveFileExplorer();
        extern std::string OpenLoadFileExplorer();
        extern std::string GetFilenameFromPath(std::string path, bool b_keepExtension = false);
        extern std::string MakePathRelative(std::string filePath);
        extern std::string GetCurrentDir();
        extern bool DoesFileExist(std::string filePath);
        extern bool FilepathHasExtension(std::string filePath, std::string extension);	
        extern void DeleteFileUsingPath(std::string filePath);
        extern std::vector<std::string> FindAllFilesWithExtension(std::string dirPath, std::string extension);
        extern std::string GetFilePathUsingFileName(std::string dirPath, std::string name);
        extern void WriteStringToFile(std::string path, std::string text);
        extern void CopyFileFL(std::string from, std::string to);
    }
}