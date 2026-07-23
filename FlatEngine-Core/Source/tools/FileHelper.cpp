#include "managers/Assets.h"
#include "tools/FileHelper.h"

#include <filesystem>
#include <fstream>


namespace FlatEngine
{
    namespace FileHelper
    {
        std::string OpenSaveFileExplorer()
        {
            // TODO: Understand how this function should work and implement it correctly
            std::string sSelectedFile = "";
            std::string sFilePath = "";
            //HRESULT hr = 0;

            ////  CREATE FILE OBJECT INSTANCE
            //HRESULT f_SysHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            //if (FAILED(f_SysHr))
            //{
            //	return "";
            //}

            //// CREATE FileSaveDialog OBJECT
            //IFileSaveDialog* f_FileSystem = nullptr;
            //hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_IFileSaveDialog, (void**)(&f_FileSystem));
            //if (FAILED(f_SysHr)) 
            //{
            //	CoUninitialize();
            //	return "";
            //}

            ////  SHOW OPEN FILE DIALOG WINDOW
            //f_SysHr = f_FileSystem->Show(nullptr);
            //if (FAILED(f_SysHr)) 
            //{
            //	f_FileSystem->Release();
            //	CoUninitialize();
            //	return "";
            //}

            ////  RETRIEVE FILE NAME FROM THE SELECTED ITEM
            //IShellItem* f_Files;
            //f_SysHr = f_FileSystem->GetResult(&f_Files);
            //if (FAILED(f_SysHr)) 
            //{
            //	f_FileSystem->Release();
            //	CoUninitialize();
            //	return "";
            //}

            ////  STORE AND CONVERT THE FILE NAME
            //PWSTR f_Path;
            //f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
            //if (FAILED(f_SysHr)) 
            //{
            //	f_Files->Release();
            //	f_FileSystem->Release();
            //	CoUninitialize();
            //	return "";
            //}

            ////  FORMAT AND STORE THE FILE PATH
            //std::wstring path(f_Path);
            //std::string c(path.begin(), path.end());
            //sFilePath = c;

            ////  FORMAT STRING FOR EXECUTABLE NAME
            //const size_t slash = sFilePath.find_last_of("//");
            //sSelectedFile = sFilePath.substr(slash + 1);

            ////  SUCCESS, CLEAN UP
            //CoTaskMemFree(f_Path);
            //f_Files->Release();
            //f_FileSystem->Release();
            //CoUninitialize();

            return sFilePath;
        }

        std::string OpenLoadFileExplorer()
        {
            // TODO: Understand how this function should work and implement it correctly
            std::string sSelectedFile = "";
            std::string sFilePath = "";
            //HRESULT hr = 0;
            //wchar_t* pSaveFileName = nullptr;
            //IShellItem* pShellItem = nullptr;
            //wchar_t* ppszName = nullptr;

            ////  CREATE FILE OBJECT INSTANCE
            //HRESULT f_SysHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            //if (FAILED(f_SysHr))
            //{
            //	return "";
            //}

            //// CREATE FileOpenDialog OBJECT
            //IFileOpenDialog* f_FileSystem;
            //f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
            //if (FAILED(f_SysHr)) 
            //{
            //	CoUninitialize();
            //	return "";
            //}

            ////  SHOW OPEN FILE DIALOG WINDOW
            //f_SysHr = f_FileSystem->Show(nullptr);
            //if (FAILED(f_SysHr)) 
            //{
            //	f_FileSystem->Release();
            //	CoUninitialize();
            //	return "";
            //}

            ////  RETRIEVE FILE NAME FROM THE SELECTED ITEM
            //IShellItem* f_Files;
            //f_SysHr = f_FileSystem->GetResult(&f_Files);
            //if (FAILED(f_SysHr)) 
            //{
            //	f_FileSystem->Release();
            //	CoUninitialize();
            //	return "";
            //}

            ////  STORE AND CONVERT THE FILE NAME
            //PWSTR f_Path;
            //f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
            //if (FAILED(f_SysHr)) 
            //{
            //	f_Files->Release();
            //	f_FileSystem->Release();
            //	CoUninitialize();
            //	return "";
            //}

            ////  FORMAT AND STORE THE FILE PATH
            //std::wstring path(f_Path);
            //std::string c(path.begin(), path.end());
            //sFilePath = c;

            ////  FORMAT STRING FOR EXECUTABLE NAME
            //const size_t slash = sFilePath.find_last_of("//");
            //sSelectedFile = sFilePath.substr(slash);
            ////  SUCCESS, CLEAN UP

            //std::string relativePath = MakePathRelative(sFilePath);

            //CoTaskMemFree(f_Path);
            //f_Files->Release();
            //f_FileSystem->Release();
            //CoUninitialize();

        
            return sFilePath;
        }

        std::string GetFilenameFromPath(std::string path, bool b_keepExtension)
        {
            std::string finalName = "";

            if (path != "")
            {
                const size_t slash = path.find_last_of("//");
                std::string wholeFilename = path.substr(slash + 1);
                const size_t dot1 = wholeFilename.find_last_of(".");
                std::string extension1 = "";

                if (dot1 < 100)
                {
                    extension1 = wholeFilename.substr(dot1);
                }

                if (!b_keepExtension)
                {
                    finalName = wholeFilename.substr(0, wholeFilename.size() - extension1.size());
                    // For scripting files because they have (.scp.lua) two extensions
                    const size_t dot2 = finalName.find_last_of(".");
                    if (dot2 < 100)
                    {
                        std::string extension2 = finalName.substr(dot2);
                        finalName = finalName.substr(0, finalName.size() - extension2.size());
                    }
                }
                else
                {
                    finalName = wholeFilename;
                }
            }

            return finalName;
        }

        //  Removes absolute path from the beginning of the selected filepath up to just after "FlatEngine"
        std::string MakePathRelative(std::string filepath)
        {
            std::string relativePath = "";
            std::string root = Assets::assetManager.GetDir("projectDir");
            size_t rootDirIndex;

            if (filepath != "")
            {
                if (root != "")
                {
                    rootDirIndex = filepath.find(root) + 10;
                }
                else
                {
                    rootDirIndex = 0;
                }

                if (rootDirIndex < 1000 && rootDirIndex != 0)
                {
                    relativePath = ".." + filepath.substr(rootDirIndex);
                }
                else
                {
                    relativePath = filepath;
                }
            }

            return relativePath;
        }

        std::string GetCurrentDir()
        {
            #ifdef _WINDOWS
                TCHAR buffer[MAX_PATH] = { 0 };
                GetModuleFileName(nullptr, buffer, MAX_PATH);
                std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"//");
                std::wstring ws = std::wstring(buffer).substr(0, pos);
                std::string dir(ws.begin(), ws.end());
                return dir;
            #elif _LINUX
                return std::filesystem::canonical("/proc/self/exe").parent_path().string() + "/";
            #endif
        }


        bool DoesFileExist(std::string filepath)
        {
            return (filepath != "" && std::filesystem::exists(filepath));
        }

        bool FilepathHasExtension(std::string filepath, std::string extension)
        {
            if (extension.substr(0, 1) == ".")
            {
                extension = extension.substr(1);
            }
            const size_t dot = filepath.find_last_of(".");
            std::string actualExtension = filepath.substr(dot + 1);

            return actualExtension == extension;
        }

        void DeleteFileUsingPath(std::string filepath)
        {
            if (filepath != "" && DoesFileExist(filepath))
            {
                std::filesystem::remove_all(filepath);
            }
        }

        std::string GetFilePathUsingFileName(std::string dirPath, std::string name)
        {
            std::string file = "";

            for (auto& p : std::filesystem::recursive_directory_iterator(dirPath))
            {
                if (p.path().stem().string() == name || p.path().string().find(name) != std::string::npos)
                {
                    file = p.path().string();
                }
            }

            return file;
        }

        std::vector<std::string> FindAllFilesWithExtension(std::string dirPath, std::string extension)
        {
            std::vector<std::string> files;

            for (auto& p : std::filesystem::recursive_directory_iterator(dirPath))
            {
                if (p.path().extension() == extension || p.path().string().find(extension) != std::string::npos)
                {
                    files.push_back(p.path().string());
                }
            }

            return files;
        }

        void WriteStringToFile(std::string path, std::string text)
        {
            std::ofstream fileObject;
            std::ifstream ifstream(path);

            // Delete old contents of the file
            fileObject.open(path, std::ofstream::out | std::ofstream::trunc);
            fileObject.close();

            // Opening file in append mode
            fileObject.open(path, std::ios::app);

            fileObject << text.c_str() << std::endl;
            fileObject.close();
        }

        void CopyFileFL(std::string from, std::string to)
        {
            std::filesystem::copy_file(from, to, std::filesystem::copy_options::overwrite_existing);
        }
    }
}