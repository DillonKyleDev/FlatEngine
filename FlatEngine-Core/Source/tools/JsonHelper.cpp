#include "tools/JsonHelper.h"
#include "Logger.h"

#include <fstream>


namespace FlatEngine
{
    namespace JsonHelper
    {
        json LoadFileData(std::string filepath)
        {		
            std::ofstream fileObject;
            std::ifstream ifstream(filepath);

            // Open file in in mode
            fileObject.open(filepath, std::ios::in);		
            std::string fileContent = "";

            if (fileObject.good())
            {
                std::string line;
                while (!ifstream.eof()) {
                    std::getline(ifstream, line);
                    fileContent.append(line + "\n");
                }
            }
            
            fileObject.close();

            if (fileObject.good() && fileContent != "\n")
            {			
                return json::parse(fileContent);
            }
            else
            {
                return nullptr;
            }
        }

        void WriteJsonToFile(json jsonData, std::string filePath)
        {
            std::ofstream fileObject;
            std::ifstream ifstream(filePath);

            // Delete old contents of the file
            fileObject.open(filePath, std::ofstream::out | std::ofstream::trunc);
            fileObject.close();

            // Opening file in append mode
            fileObject.open(filePath, std::ios::app);            		
            fileObject << jsonData.dump(4).c_str() << std::endl;
        }

        bool JsonContains(json obj, std::string checkFor, std::string loadedName)
        {
            bool contains = false;
            if (obj.contains(checkFor))
            {
                contains = true;
            }
            else
            {
                Logger::log.Err("JsonContains() - {} does not contain a value for {}.", loadedName, checkFor);
            }
            return contains;
        }

        float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName)
        {
            float value = -1;
            if (obj.contains(checkFor))
            {
                value = obj.at(checkFor);
            }
            else
            {
                Logger::log.Err("CheckJsonFloat() - {} object does not contain a value for {}.", loadedName, checkFor);
            }
            return value;
        }

        float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage)
        {
            float value = -1;
            if (obj.contains(checkFor))
            {
                value = obj.at(checkFor);
            }
            else
            {
                errorMessage = "CheckJsonFloat() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".";
            }
            return value;
        }

        int CheckJsonInt(json obj, std::string checkFor, std::string loadedName)
        {
            int value = -1;
            if (obj.contains(checkFor))
            {
                value = obj.at(checkFor);
            }
            else
            {
                Logger::log.Err("CheckJsonInt() - {} object does not contain a value for {}.", loadedName, checkFor);
            }
            return value;
        }

        int CheckJsonInt(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage)
        {
            int value = -1;
            if (obj.contains(checkFor))
            {
                value = obj.at(checkFor);
            }
            else
            {
                errorMessage = "CheckJsonInt() - {} object does not contain a value for {}." + loadedName + checkFor;
            }
            return value;
        }

        long CheckJsonLong(json obj, std::string checkFor, std::string loadedName)
        {
            long value = -1;
            if (obj.contains(checkFor))
            {
                value = obj.at(checkFor);
            }
            else
            {
                Logger::log.Err("CheckJsonLong() - {} object does not contain a value for {}.", loadedName, checkFor);
            }
            return value;
        }

        long CheckJsonLong(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage)
        {
            long value = -1;
            if (obj.contains(checkFor))
            {
                value = obj.at(checkFor);
            }
            else
            {
                errorMessage = "CheckJsonLong() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".";
            }
            return value;
        }

        double CheckJsonDouble(json obj, std::string checkFor, std::string loadedName)
        {
            double value = -1;
            if (obj.contains(checkFor))
            {
                value = obj.at(checkFor);
            }
            else
            {
                Logger::log.Err("CheckJsonLong() - {} object does not contain a value for {}.", loadedName, checkFor);
            }
            return value;
        }

        double CheckJsonDouble(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage)
        {
            double value = -1;
            if (obj.contains(checkFor))
            {
                value = obj.at(checkFor);
            }
            else
            {
                errorMessage = "CheckJsonLong() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".";
            }
            return value;
        }
        bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName)
        {
            bool value = false;
            if (obj.contains(checkFor))
            {
                value = obj.at(checkFor);
            }
            else
            {
                Logger::log.Err("CheckJsonBool() - {} object does not contain a value for {}.", loadedName, checkFor);
            }
            return value;
        }

        bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage)
        {
            bool value = false;
            if (obj.contains(checkFor))
            {
                value = obj.at(checkFor);
            }
            else
            {
                errorMessage = "CheckJsonBool() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".";
            }
            return value;
        }

        std::string CheckJsonString(json obj, std::string checkFor, std::string loadedName)
        {		
            std::string value = "";
            if (obj.contains(checkFor))
            {
                value = obj.at(checkFor);
            }
            else
            {
                Logger::log.Err("CheckJsonString() - {} object does not contain a value for {}.", loadedName, checkFor);
            }
            return value;
        }

        std::string CheckJsonString(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage)
        {
            std::string value = "";
            if (obj.contains(checkFor))
            {
                value = obj.at(checkFor);
            }
            else
            {
                errorMessage = "CheckJsonString() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".";
            }
            return value;
        }
    }
}