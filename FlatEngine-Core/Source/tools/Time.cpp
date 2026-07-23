
#include "Logger.h"
#include "tools/JsonHelper.h"
#include "tools/FileHelper.h"
#include "tools/Time.h"

#include <fstream>
#include <SDL_timer.h>


namespace FlatEngine
{
    namespace Time
    {
        // return SDL_GetTicks64();
        Uint64 Time()
        {
            return SDL_GetTicks64();
        }

        time_t GetCurrentTimeAndDateStamp()
        {
            time_t timeStamp;
            time(&timeStamp);
            return timeStamp;
        }

        time_t CreateTimeStamp(tm timeStruct)
        {		
            return mktime(&timeStruct);
        }

        tm GetTMStructFromTimeStamp(time_t timeStamp)
        {
            tm timeStruct;
            #ifdef _WINDOWS
                localtime_s(&timeStruct, &timeStamp);
            #else
                localtime_r(&timeStamp, &timeStruct);
            #endif

            return timeStruct;
        }

        // Returns true if timeStamp1 is more recent than timeStamp2
        bool CompareTimeStamps(time_t timeStamp1, time_t timeStamp2)
        {
            return difftime(timeStamp1, timeStamp2) > 0;
        }

        tm GetProjectTimeStruct(std::string projectPath)
        {
            tm dateSaved = tm();

            std::ofstream fileObject;
            std::ifstream ifstream(projectPath);

            fileObject.open(projectPath, std::ios::in);
            std::string fileContent = "";

            if (fileObject.good())
            {
                std::string line;
                while (!ifstream.eof())
                {
                    std::getline(ifstream, line);
                    fileContent.append(line + "\n");
                }
            }

            fileObject.close();

            if (fileObject.good())
            {
                json projectJson = json::parse(fileContent);

                if (projectJson["Project Properties"][0] != "nullptr")
                {
                    for (int i = 0; i < projectJson["Project Properties"].size(); i++)
                    {
                        try
                        {
                            json projectData = projectJson["Project Properties"][i];
                            std::string projectName = FileHelper::GetFilenameFromPath(projectPath);

                            dateSaved.tm_year = JsonHelper::CheckJsonInt(projectData, "yearsSinceSave", projectName);
                            dateSaved.tm_mon = JsonHelper::CheckJsonInt(projectData, "monthsSinceSave", projectName);
                            dateSaved.tm_mday = JsonHelper::CheckJsonInt(projectData, "daysSinceSave", projectName);
                            dateSaved.tm_hour = JsonHelper::CheckJsonInt(projectData, "hoursSinceSave", projectName);
                            dateSaved.tm_min = JsonHelper::CheckJsonInt(projectData, "minutesSinceSave", projectName);
                            dateSaved.tm_sec = JsonHelper::CheckJsonInt(projectData, "secondsSinceSave", projectName);
                        }
                        catch (const json::out_of_range& e)
                        {
                            Logger::log.Err("{}", e.what());
                        }
                    }
                }
            }

            return dateSaved;
        }

        std::string GetFormattedTime(tm& timeStruct)
        {
            char formatedTimeStamp[50];
            strftime(formatedTimeStamp, 50, "%b %e, %Y - %I:%M:%S %p", &timeStruct);
            return formatedTimeStamp;
        }

    }
}