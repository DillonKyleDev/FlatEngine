#pragma once

#include <ctime>
#include <SDL_syswm.h>
#include <string>


namespace FlatEngine
{
    namespace Time
    {
        extern Uint64 Time();
        extern time_t GetCurrentTimeAndDateStamp();
        extern time_t CreateTimeStamp(tm timeStruct);
        extern tm GetTMStructFromTimeStamp(time_t timeStamp);
        extern bool CompareTimeStamps(time_t timeStamp1, time_t timeStamp2);
        extern tm GetProjectTimeStruct(std::string projectPath);
        extern std::string GetFormattedTime(tm& timeStruct);
    }
}