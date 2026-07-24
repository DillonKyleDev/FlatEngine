#pragma once

#include "json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;
#include <string>


namespace FlatEngine
{
    namespace JsonHelper
    {
        extern json LoadFileData(std::string filePath);
        extern void WriteJsonToFile(std::string filePath, json jsonData);
        extern std::string CheckJsonString(json obj, std::string checkFor, std::string loadedName);
        extern std::string CheckJsonString(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage);
        extern float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName);
        extern float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage);
        extern int CheckJsonInt(json obj, std::string checkFor, std::string loadedName);
        extern int CheckJsonInt(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage);
        extern long CheckJsonLong(json obj, std::string checkFor, std::string loadedName);
        extern long CheckJsonLong(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage);
        extern double CheckJsonDouble(json obj, std::string checkFor, std::string loadedName);
        extern double CheckJsonDouble(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage);
        extern bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName);
        extern bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage);
        extern bool JsonContains(json obj, std::string checkFor, std::string loadedName);
    }
}