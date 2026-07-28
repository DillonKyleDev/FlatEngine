#include "GameObject.h"
#include "managers/AnimationManager.h"
#include "components/Animation.h"
#include "managers/Assets.h"
#include "managers/ProjectManager.h"
#include "tools/JsonHelper.h"

#include <memory>


namespace FlatEngine
{
    namespace AnimationManager
    {
        std::map<std::string, void (*)(GameObject*, LuaManager::LuaParameter)> cppAnimationEventFunctions = std::map<std::string, void (*)(GameObject*, LuaManager::LuaParameter)>();

        void AddCPPAnimationEventFunction(std::string functionName, void (*eventFunction)(GameObject*, LuaManager::LuaParameter))
        {
            if (functionName != "" && cppAnimationEventFunctions.count(functionName) == 0)
            {
                std::pair<std::string, void (*)(GameObject*, LuaManager::LuaParameter)> newFunctionPair = { functionName, eventFunction };
                cppAnimationEventFunctions.emplace(newFunctionPair);
            }
        }

        void CreateNewAnimationFile(std::string filename, std::string path)
        {		
            std::string filePath = "";

            if (path == "")
            {
                filePath = Assets::assetManager.GetDir("projectDir") + "/animations/" + filename + ".anm";
            }
            else
            {
                filePath = path + "/" + filename + ".anm";
            }

            AnimationData animData = AnimationData();
            animData.name = filename;
            animData.path = filePath;
            SaveAnimationFile(animData, filePath);

            if (FL::ProjectManager::loadedProject.loadedAnimationPath == "")
            {
                FL::ProjectManager::loadedProject.loadedAnimationPath = filePath;
                FL::ProjectManager::SaveCurrentProject();
            }
        }

        void SaveAnimationFile(const AnimationData& animData, std::string path)
        {		                    
            json propsArray = json::array();
            for (const auto& prop : animData.props)
            {		
                propsArray.push_back(prop.get()->GetData());
            }        

            json animationData = json::object({
                { "name", animData.name },
                { "length", animData.length },
                { "b_loop", animData.b_loop },
                { "b_startAtOrigin", animData.b_startAtOrigin },
                { "animationProperties", propsArray }
            });
            
            json animationJson = json::object({ {"animation", animationData } });
            JsonHelper::WriteJsonToFile(animationJson, path);
        }

        AnimationData LoadAnimationFile(std::string path)
        {
            json fileContentJson = JsonHelper::LoadFileData(path);
            AnimationData animProps = AnimationData();
            animProps.path = path;

            if (fileContentJson.contains("animation"))
            {				                            
                json animationJson = fileContentJson["animation"];
                std::string animName = JsonHelper::CheckJsonString(animationJson, "name", "name");
                animProps.name = animName;
                animProps.length = JsonHelper::CheckJsonFloat(animationJson, "length", animName);
                animProps.b_loop = JsonHelper::CheckJsonBool(animationJson, "b_loop", animName);
                animProps.b_startAtOrigin = JsonHelper::CheckJsonBool(animationJson, "b_startAtOrigin", animName);                              
                json propJson = animationJson["animationProperties"];
                
                for (int i = 0; i < propJson.size(); i++)
                {
                    PropertyType type = JsonHelper::CheckJsonInt(propJson[i], "type", animName) != -1 ? (PropertyType)JsonHelper::CheckJsonInt(propJson[i], "type", animName) : PropertyType_None;

                    switch (type)                   
                    {   
                        case PropertyType_Event:     { std::shared_ptr<EventProp> prop = std::make_shared<EventProp>(); prop->PutData(propJson[i], animName); animProps.props.push_back(prop); break; }
                        case PropertyType_Transform: { std::shared_ptr<TransformProp> prop = std::make_shared<TransformProp>(); prop->PutData(propJson[i], animName); animProps.props.push_back(prop); break; }
                        case PropertyType_Sprite:    { std::shared_ptr<SpriteProp> prop = std::make_shared<SpriteProp>(); prop->PutData(propJson[i], animName); animProps.props.push_back(prop); break; }
                        case PropertyType_Text:      { std::shared_ptr<TextProp> prop = std::make_shared<TextProp>(); prop->PutData(propJson[i], animName); animProps.props.push_back(prop); break; }
                        default: break;                    
                    }  
                }        
            }            

            return animProps;
        }
    }
}