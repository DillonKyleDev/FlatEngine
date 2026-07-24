#include "components/Component.h"
#include "GameObject.h"
#include "tools/JsonHelper.h"
#include "Logger.h"
#include "managers/Assets.h"
#include "managers/PrefabManager.h"
#include "managers/SceneManager.h"

#include <fstream>

namespace FL = FlatEngine;


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
        
        // json CreateJsonFromObject(GameObject* currentObject)
        // {		
            // if (currentObject == nullptr)
            //     return json::object();

            // json componentsArray = json::array();

            // for (int i = 1; i < FL::ComponentType_Size; i++)
            // {
            //     FL::Component* component = currentObject->GetComponent((FL::ComponentType)i);
            //     if (component != nullptr)
            //     {
            //         std::string typeString = component->GetTypeString();

            //         if (typeString != "Null")
            //         {                        
            //             componentsArray.push_back(component->GetData());
            //         }
            //     }
            // }
            
            // json childrenArray = json::array();

            // for (int c = 0; c < currentObject->GetChildren().size(); c++)
            // {
            //     childrenArray.push_back(currentObject->GetChildren()[c]);
            // }
            
            // std::map <std::string, bool> tagList = currentObject->GetTagList().GetTagsMap();
            // std::map <std::string, bool> collidesTagList = currentObject->GetTagList().GetCollidesTagsMap();

            // json tagsObjectArray = json::array();
            // for (std::map<std::string, bool>::iterator tagIter = tagList.begin(); tagIter != tagList.end(); tagIter++)
            // {
            //     // For making sure we don't save any stale tags that aren't available in the Tags.lua file
            //     for (std::string availableTag : Assets::assetManager.GetTags())
            //     {
            //         if (tagIter->first == availableTag)
            //         {
            //             json tag = json::object({ { tagIter->first, tagIter->second } });
            //             tagsObjectArray.push_back(tag);
            //         }
            //     }
            // }
            // json collidesTagsObjectArray = json::array();
            // for (std::map<std::string, bool>::iterator tagIter = collidesTagList.begin(); tagIter != collidesTagList.end(); tagIter++)
            // {
            //     // For making sure we don't save any stale collides tags that aren't available in the Tags.lua file
            //     for (std::string availableTag : Assets::assetManager.GetTags())
            //     {
            //         if (tagIter->first == availableTag)
            //         {
            //             json collides = json::object({ { tagIter->first, tagIter->second } });
            //             collidesTagsObjectArray.push_back(collides);
            //         }
            //     }
            // }
            
            // std::string objectName = currentObject->GetName();
            // Vector3 spawnLocation = currentObject->GetPrefabSpawnLocation();
            // if (currentObject->Get<Transform>())
            // {
            //     spawnLocation = currentObject->Get<Transform>()->GetPosition();
            // }
            
            // json gameObjectJson = json::object({
            //     { "b_isPrefab", currentObject->IsPrefab() },
            //     { "prefabName", currentObject->GetPrefabName() },
            //     { "spawnLocationX", spawnLocation.x },
            //     { "spawnLocationY", spawnLocation.y },
            //     { "spawnLocationZ", spawnLocation.z },
            //     { "name", objectName },
            //     { "id", currentObject->GetID() },
            //     { "b_isActive", currentObject->IsActive() },
            //     { "parent", currentObject->GetParentID() },
            //     { "children", childrenArray },
            //     { "components", componentsArray },
            //     { "tags", tagsObjectArray },
            //     { "collidesTags", collidesTagsObjectArray },
            // });

            // return gameObjectJson;
        // }

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

        // void CreateObjectFromJson(json objectJson, Scene* scene, GameObject* loadedObject)
        // {
        //     std::string objectName = CheckJsonString(objectJson, "name", "Name");
        //     bool b_isActive = CheckJsonBool(objectJson, "b_isActive", objectName);
        //     bool b_isPrefab = CheckJsonBool(objectJson, "b_isPrefab", objectName);            
        //     std::string prefabName = CheckJsonString(objectJson, "prefabName", objectName);
        //     Vector3 spawnLocation = Vector3(0, 0, 0);
        //     spawnLocation.x = CheckJsonFloat(objectJson, "spawnLocationX", objectName); // SetOrigin() is taken care of by Instantiate() using parentID
        //     spawnLocation.y = CheckJsonFloat(objectJson, "spawnLocationY", objectName);
        //     spawnLocation.z = CheckJsonFloat(objectJson, "spawnLocationZ", objectName);
        //     long loadedID = CheckJsonLong(objectJson, "id", objectName);
        //     long loadedParentID = CheckJsonLong(objectJson, "parent", objectName);
        //     std::vector<long> loadedChildrenIDs = std::vector<long>();
        //     TagList tags = TagList(loadedID);

        //     if (JsonContains(objectJson, "children", objectName))
        //     {
        //         for (int c = 0; c < objectJson["children"].size(); c++)
        //         {
        //             loadedChildrenIDs.push_back(objectJson["children"][c]);
        //         }
        //     }

        //     if (b_isPrefab)
        //     {
        //         loadedObject = PrefabManager::Instantiate(prefabName, spawnLocation, scene, loadedParentID, loadedID);
        //         if (loadedObject != nullptr)
        //         {
        //             loadedObject->SetName(objectName);
        //         }
        //     }
        //     else
        //     {			                
        //         GameObject* loadedObject = FL::SceneManager::loadedScene.CreateEmptyGameObject(loadedParentID, loadedID);                

        //         if (loadedObject != nullptr)
        //         {
        //             loadedObject->SetName(objectName);				
        //             loadedObject->SetActive(b_isActive);
                    
        //             tags.PutData(objectJson);
        //             loadedObject->SetTagList(tags);
                                    
        //             try
        //             {
        //                 if (objectJson.contains("components"))
        //                 {
        //                     for (int j = 0; j < objectJson.at("components").size(); j++)
        //                     {
        //                         json componentJson = objectJson.at("components").at(j);
        //                         std::string typeString = CheckJsonString(componentJson, "type", objectName);
        //                         long id = CheckJsonLong(componentJson, "id", objectName);
        //                         // ComponentType type = (ComponentType)CheckJsonInt(componentJson, "type", objectName); move to this later, change save process.
        //                         for (int i = 1; i < ComponentType_Size; i++)
        //                         {
        //                             if (typeString == ComponentTypeStrings[i])
        //                             {
        //                                 Component* component = loadedObject->AddComponent((ComponentType)i, id, componentJson);                                          
        //                             }
        //                         }                                                   
        //                     }
        //                 }
        //             } // Is this even doing anything??? vvv
        //             catch (const json::out_of_range& e)
        //             {
        //                 Logger::log.Err("{}", e.what());			
        //             }
        //             catch (const json::type_error& e)
        //             {
        //                 Logger::log.Err("{}", e.what());	
        //             }
        //         }
        //     }

        //     // Update the moment of inertia if applicable
        //     if (loadedObject != nullptr)
        //     {
        //         if (loadedObject->Get<Button>() != nullptr)
        //         {
        //             loadedObject->Get<Button>()->CalculateActiveEdges();
        //         }
        //     }
        // }
    }
}