#pragma once
#include "tools/JsonHelper.h"
#include "tools/Logger.h"
#include "tools/Vector2.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"

#include <map>
#include <sol.hpp>
#include <string>
#include <vector>

namespace FL = FlatEngine;


namespace FlatEngine
{
    class GameObject;
    struct ScriptData;
    class Body;
    class Body2D;

    namespace LuaManager
    {
        extern sol::state lua;
        extern std::vector<std::string> luaScriptPaths;
        extern std::vector<std::string> luaScriptNames;
        extern std::map<std::string, std::string> luaScriptsMap;
        extern std::map<std::string, sol::protected_function> loadedSceneScriptFiles;
        extern std::map<std::string, sol::protected_function> loadedPersistentScriptFiles;	

        enum ParameterType {
            ParameterType_None,
            ParameterType_String,
            ParameterType_Int32,
            ParameterType_Int64,
            ParameterType_Float,
            ParameterType_Double,
            ParameterType_Bool,
            ParameterType_Vec2,
            ParameterType_Vec3,
            ParameterType_Vec4
        };
        struct LuaParameter
        {		
            ParameterType type = ParameterType_None;
            std::string name = "";
            std::string p_string = "";
            int p_int32 = 0;
            int64_t p_int64 = 0;
            float p_float = 0.0f;
            double p_double = 0.0;
            bool p_bool = false;
            FL::Vector2 p_vec2 = FL::Vector2();
            FL::Vector3 p_vec3 = FL::Vector3();    
            FL::Vector4 p_vec4 = FL::Vector4();    
            
            json GetData()
            {
                json parameters = {		
                    { "name",   name },
                    { "type",   (int)type }	,
                    { "string", p_string },
                    { "int32",  p_int64 },
                    { "int64",  p_int32 },
                    { "float",  p_float },
                    { "double", p_double },			
                    { "bool",   p_bool },
                    { "vec2_x", p_vec2.x },
                    { "vec2_y", p_vec2.y },
                    { "vec3_x", p_vec3.x },
                    { "vec3_y", p_vec3.y },
                    { "vec3_z", p_vec3.z },
                    { "vec4_x", p_vec4.x },
                    { "vec4_y", p_vec4.y },
                    { "vec4_z", p_vec4.z },
                    { "vec4_w", p_vec4.w }			
                };

                return parameters;
            }
            void PutData(json parameterJson, std::string objectName)
            {
                name     = JsonHelper::CheckJsonString(parameterJson, "name", objectName);
                type     = JsonHelper::CheckJsonInt(parameterJson, "type", objectName) != -1 ? (ParameterType)JsonHelper::CheckJsonInt(parameterJson, "type", objectName) : ParameterType_None;	
                p_string = JsonHelper::CheckJsonString(parameterJson, "string", objectName);
                p_int32  = JsonHelper::CheckJsonInt(parameterJson, "int32", objectName);								
                p_int64  = JsonHelper::CheckJsonLong(parameterJson, "int64", objectName);
                p_float  = JsonHelper::CheckJsonFloat(parameterJson, "float", objectName);
                p_double = JsonHelper::CheckJsonDouble(parameterJson, "double", objectName);
                p_bool   = JsonHelper::CheckJsonBool(parameterJson, "bool", objectName);
                p_vec2   = Vector2(JsonHelper::CheckJsonFloat(parameterJson, "vec2_x", objectName), JsonHelper::CheckJsonFloat(parameterJson, "vec2_y", objectName));								
                p_vec3   = Vector3(JsonHelper::CheckJsonFloat(parameterJson, "vec3_x", objectName), JsonHelper::CheckJsonFloat(parameterJson, "vec3_y", objectName), JsonHelper::CheckJsonFloat(parameterJson, "vec3_z", objectName));								
                p_vec4   = Vector4(JsonHelper::CheckJsonFloat(parameterJson, "vec4_x", objectName), JsonHelper::CheckJsonFloat(parameterJson, "vec4_y", objectName), JsonHelper::CheckJsonFloat(parameterJson, "vec4_z", objectName), JsonHelper::CheckJsonFloat(parameterJson, "vec4_w", objectName));		
            }
        };
        struct LuaParameterContainer {
            std::string name;
            std::map<std::string, LuaParameter> parameters;
            std::string tempParameterName = "";
            int tempParameterType = 0;     
            
            json GetData()
            {
                json jsonData;

                for (auto parameter : parameters)
                {
                    jsonData.push_back(parameter.second.GetData());
                }

                return jsonData;
            }

            void PutData(json jsonData, std::string name)
            {
                if (JsonHelper::JsonContains(jsonData, "functionParameters", name))
                {
                    LuaParameter parameter;
                    parameter.PutData(jsonData.at("functionParameters"), name);
                    Add(parameter);
                }
            }

            void Add(LuaManager::LuaParameter parameter)
            {
                if (parameters.count(parameter.name) == 0)
                    parameters.emplace(parameter.name, parameter);            
                else                
                    Logger::log.Err("Script parameter already exists with that name.");            
            }

            LuaParameter Get(std::string paramName)
            {
                if (parameters.count(paramName))
                    return parameters.at(paramName);
 
                return LuaManager::LuaParameter();
            }

            void Remove(std::string paramName)
            {
                if (parameters.count(paramName))             
                    parameters.erase(paramName);                
                else                
                    Logger::log.Err("No Script parameter with that name was found.");                
            }
        };

        enum LuaEventFunction {
            OnBeginCollision,
            OnEndCollision,
            OnBeginSensorTouch,
            OnEndSensorTouch,
            OnButtonMouseOver,
            OnButtonMouseEnter,
            OnButtonMouseLeave,
            OnButtonLeftClick,
            OnButtonRightClick
        };
        const std::string luaEventNames[11] = {
            "OnBeginCollision",
            "OnEndCollision",
            "OnBeginSensorTouch",
            "OnEndSensorTouch",
            "OnButtonMouseOver",
            "OnButtonMouseEnter",
            "OnButtonMouseLeave",
            "OnButtonLeftClick",
            "OnButtonRightClick"
        };

        extern std::map<std::string, void (*)(FL::GameObject*, LuaParameter)> F_CPPAnimationEventFunctions;
        extern void InitLua();
        extern void RegisterLuaFunctions();
        extern void RegisterLuaTypes();
        extern void CreateNewLuaScript(std::string fileName, std::string path = "");
        extern bool InitLuaScript(FL::ScriptData script, GameObject* caller, std::map<std::string, sol::protected_function>& scriptTracker);
        extern bool ReadyScriptFile(std::string scriptToLoad, std::string& message);
        extern void RunLuaFuncOnAllScripts(std::string functionName);
        extern void RunLuaFuncOnSingleScript(FL::ScriptData scriptData, GameObject* caller, std::string functionName);
        extern void RunSceneAwakeAndStart();	
        extern void RunPersistentAwakeAndStart();
        extern void RetrieveLuaScriptPaths();
        extern bool CheckLuaScriptFile(std::string filePath);
        extern void LoadLuaGameObject(FL::GameObject* object, std::string scriptName);
        
        // Lua/Sol Events	
        template <class T>
        extern void CallVoidLuaFunction(std::string functionName, T param);
        template <class T>
        extern void CallVoidLuaFunction(std::string functionName);
        extern void CallLuaCollisionFunction(LuaEventFunction eventFunc, FL::Body* caller, FL::Body* collidedWith, b2Manifold manifold = {});
        extern void CallLuaSensorFunction(LuaEventFunction eventFunc, FL::Body* caller, FL::Body* touched);
        extern void CallLuaCollisionFunction2D(LuaEventFunction eventFunc, FL::Body2D* caller, FL::Body2D* collidedWith, b2Manifold manifold = {});
        extern void CallLuaSensorFunction2D(LuaEventFunction eventFunc, FL::Body2D* caller, FL::Body2D* touched);
        extern void CallLuaButtonEventFunction(FL::GameObject* caller, LuaEventFunction eventFunc);        
        extern void CallLuaAnimationEventFunction(FL::GameObject* caller, std::string functionName, LuaParameterContainer paramsContainer = LuaParameterContainer());
        extern void CallLuaButtonOnClickFunction(FL::GameObject* caller, std::string eventFunc);
        extern void CallLuaButtonOnClickFunction(FL::GameObject* caller, std::string eventFunc, LuaParameter params);
    }
}