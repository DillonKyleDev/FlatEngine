#include "FlatEngine.h"
#include "Vector2.h"
#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Canvas.h"
#include "Button.h"
#include "Audio.h"
#include "Animation.h"
#include "Text.h"
#include "CharacterController.h"
#include "Script.h"
#include "MappingContext.h"
#include "Project.h"
#include "Body.h"

#include "box2d.h"
#include <fstream>
#include <random>
#include <vector>
#include <map>
#include <string>


// https://github.com/ThePhD/sol2/issues/354

// Lua helpers + Functions that Lua can call
namespace FlatEngine
{
	sol::state F_Lua;	
	std::map<std::string, sol::protected_function> F_LoadedSceneScriptFiles = std::map<std::string, sol::protected_function>();
	std::map<std::string, sol::protected_function> F_LoadedPersistantScriptFiles = std::map<std::string, sol::protected_function>();
	std::vector<std::string> F_luaScriptPaths = std::vector<std::string>();
	std::vector<std::string> F_luaScriptNames = std::vector<std::string>();
	std::map<std::string, std::string> F_LuaScriptsMap = std::map<std::string, std::string>();

	void InitLua()
	{
		F_Lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
		RegisterLuaFunctions();
		RegisterLuaTypes();
	}

	std::vector<std::string> CreateStringVector()
	{
		std::vector<std::string> newVec = std::vector<std::string>();
		return newVec;
	}
	std::vector<int> CreateIntVector()
	{
		std::vector<int> newVec = std::vector<int>();
		return newVec;
	}
	std::vector<long> CreateLongVector()
	{
		std::vector<long> newVec = std::vector<long>();
		return newVec;
	}
	std::vector<float> CreateFloatVector()
	{
		std::vector<float> newVec = std::vector<float>();
		return newVec;
	}
	std::vector<double> CreateDoubleVector()
	{
		std::vector<double> newVec = std::vector<double>();
		return newVec;
	}
	std::vector<bool> CreateBoolVector()
	{
		std::vector<bool> newVec = std::vector<bool>();
		return newVec;
	}
	std::map<std::string, std::string> CreateStringStringMap()
	{
		std::map<std::string, std::string> newMap = std::map<std::string, std::string>();
		return newMap;
	}

	// Inject functions that can be called from within Lua directly into the Lua state
	void RegisterLuaFunctions()
	{
		F_Lua["CreateGameObject"] = [](long parentID)
		{
			return CreateGameObject(parentID);
		};
		F_Lua["CreateGameObject"] = []()
		{
			return CreateGameObject(-1);
		};
		F_Lua["CreateStringVector"] = []()
		{
			return CreateStringVector();
		};
		F_Lua["CreateIntVector"] = []()
		{
			return CreateIntVector();
		};
		F_Lua["CreateLongVector"] = []()
		{
			return CreateLongVector();
		};
		F_Lua["CreateFloatVector"] = []()
		{
			return CreateFloatVector();
		};
		F_Lua["CreateDoubleVector"] = []()
		{
			return CreateDoubleVector();
		};
		F_Lua["CreateBoolVector"] = []()
		{
			return CreateBoolVector();
		};
		//F_Lua["CreateStringStringMap"] = []()
		//{
		//	return CreateStringStringMap();
		//};
		/*
		F_Lua["CreateStringIntMap"] = []()
		{
			std::map<std::string, int> newMap = std::map<std::string, int>();
			return newMap;
		};
		F_Lua["CreateStringFloatMap"] = []()
		{
			std::map<std::string, float> newMap = std::map<std::string, float>();
			return newMap;
		};
		F_Lua["CreateStringBoolMap"] = []()
		{
			std::map<std::string, bool> newMap = std::map<std::string, bool>();
			return newMap;
		};
		F_Lua["CreateIntStringMap"] = []()
		{
			std::map<int, std::string> newMap = std::map<int, std::string>();
			return newMap;
		};
		F_Lua["CreateIntIntMap"] = []()
		{
			std::map<int, int> newMap = std::map<int, int>();
			return newMap;
		};
		F_Lua["CreateIntFloatMap"] = []()
		{
			std::map<int, float> newMap = std::map<int, float>();
			return newMap;
		};
		F_Lua["CreateIntBoolMap"] = []()
		{
			std::map<int, bool> newMap = std::map<int, bool>();
			return newMap;
		};*/
		F_Lua["IntToString"] = [](int value)
		{
			return std::to_string(value);
		};
		F_Lua["LongToString"] = [](long value)
		{
			return std::to_string(value);
		};
		F_Lua["FloatToString"] = [](float value)
		{
			return std::to_string(value);
		};
		F_Lua["DoubleToString"] = [](double value)
		{
			return std::to_string(value);
		};
		F_Lua["GetInstanceData"] = [](std::string scriptName, long ID)
		{
			// Checks the Lua state to see if there is a table value for the given script in the called functions Script file
			std::optional<sol::table> instanceData = F_Lua[scriptName][ID];
			if (!instanceData.has_value())
			{
				std::string scriptName = F_Lua["calling_script_name"].get_or<std::string>("Script (Lua)");
				std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
				LogError("No instance data for " + scriptName + " found using id: " + std::to_string(ID) + ".\n -- Lua Function called by GameObject : " + GetObjectByID(ID)->GetName() + ".\n -- Specific calling Script component : " + truncatedName + ".\n -- Lua Script where called function lives : " + scriptName + ".\n -- The calling Script file may be missing the function called and is using another Script files version instead. Make sure the called function exists in the " + GetObjectByID(ID)->GetName() + " Script file.");
			}
			return instanceData;
		};
		F_Lua["ContainsData"] = [](std::string scriptName, long ID)
		{
			// Checks the Lua state to see if there is a table value for the given script in the called functions Script file
			std::optional<sol::table> instanceData = F_Lua[scriptName][ID];
			return instanceData.has_value();
		};
		F_Lua["GetScriptParam"] = [](std::string paramName, long ID, std::string scriptName)
		{
			GameObject* thisObject = GetObjectByID(ID);
			Script::S_ScriptParam parameter = Script::S_ScriptParam();

			if (thisObject != nullptr)
			{
				Script* script = thisObject->GetScript(scriptName);
				if (script != nullptr)
				{
					parameter = script->GetParam(paramName);

					if (parameter.type == "empty")
					{
						LogError("No parameter with the name \"" + paramName + "\" found in " + scriptName + " Script on the " + thisObject->GetName() + " GameObject");
					}
				}
				else
				{
					LogError(thisObject->GetName() + " does not contain the Script named " + scriptName);
				}
			}
			else
			{
				LogError("GameObject with that id not found.");
			}

			return parameter;
		};
		F_Lua["GetScriptParam"] = [](std::string paramName, long ID)
		{
			GameObject* thisObject = GetObjectByID(ID);
			std::string scriptName = F_Lua["calling_script_name"].get_or<std::string>("Script (Lua)");
			std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
			Script::S_ScriptParam parameter = Script::S_ScriptParam();

			if (thisObject != nullptr)
			{
				Script* script = thisObject->GetScript(scriptName);
				if (script != nullptr)
				{
					parameter = script->GetParam(paramName);

					if (parameter.type == "empty")
					{
						LogError("No parameter with the name \"" + paramName + "\" found in " + scriptName + " Script on the " + thisObject->GetName() + " GameObject");
					}
				}
				else
				{
					LogError(thisObject->GetName() + " does not contain the Script named " + scriptName);
				}
			}
			else
			{
				LogError("GameObject with that id not found.");
			}

			return parameter;
		};
		F_Lua["LoadGameObject"] = [](long ID)
		{
			LoadLuaGameObject(GetObjectByID(ID), F_Lua["calling_script_name"]);
		};
		F_Lua["GetObjectByID"] = [](long ID)
		{
			return GetObjectByID(ID);
		};
		F_Lua["GetObjectByName"] = [](std::string objectName)
		{
			return GetObjectByName(objectName);
		};
		F_Lua["LoadScene"] = [](std::string sceneName)
		{
			std::string scenePath = GetFilePathUsingFileName(GetDir("projectDir"), sceneName + ".scn");
			if (scenePath != "")
			{
				QueueLoadScene(scenePath);
			}
			else
			{
				LogError("Failed to load scene. Scene \"" + sceneName + "\" not found.");
			}
		};
		F_Lua["ReloadScene"] = []()
		{				
			QueueLoadScene(GetLoadedScenePath());
		};
		F_Lua["GetLoadedScene"] = []()
		{
			return GetLoadedScene();
		};
		F_Lua["LogString"] = [](std::string line)
		{
			std::string scriptName = F_Lua["calling_script_name"].get_or<std::string>("Script (Lua)");
			std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
			std::string prefix = "[LUA] " + truncatedName + " :";
			LogString(line, prefix);
		};
		F_Lua["LogInt"] = [](int value, std::string line)
		{
			std::string scriptName = F_Lua["calling_script_name"].get_or<std::string>("Script (Lua)");
			std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
			std::string prefix = "[LUA] " + truncatedName + " :";
			LogInt(value, line, prefix);
		};
		F_Lua["LogInt"] = [](int value)
		{
			std::string scriptName = F_Lua["calling_script_name"].get_or<std::string>("Script (Lua)");
			std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
			std::string prefix = "[LUA] " + truncatedName + " :";
			LogInt(value, "", prefix);
		};
		F_Lua["LogFloat"] = [](float value, std::string line)
		{
			std::string scriptName = F_Lua["calling_script_name"].get_or<std::string>("Script (Lua)");
			std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
			std::string prefix = "[LUA] " + truncatedName + " :";
			LogFloat(value, line, prefix);
		};
		F_Lua["LogFloat"] = [](float value)
		{
			std::string scriptName = F_Lua["calling_script_name"].get_or<std::string>("Script (Lua)");
			std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
			std::string prefix = "[LUA] " + truncatedName + " :";
			LogFloat(value, "", prefix);
		};
		F_Lua["LogDouble"] = [](double value, std::string line)
		{
			std::string scriptName = F_Lua["calling_script_name"].get_or<std::string>("Script (Lua)");
			std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
			std::string prefix = "[LUA] " + truncatedName + " :";
			LogDouble(value, line, prefix);
		};
		F_Lua["LogDouble"] = [](double value)
		{
			std::string scriptName = F_Lua["calling_script_name"].get_or<std::string>("Script (Lua)");
			std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
			std::string prefix = "[LUA] " + truncatedName + " :";
			LogDouble(value, "", prefix);
		};
		F_Lua["LogLong"] = [](long value, std::string line)
		{
			std::string scriptName = F_Lua["calling_script_name"].get_or<std::string>("Script (Lua)");
			std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
			std::string prefix = "[LUA] " + truncatedName + " :";
			LogLong(value, line, prefix);
		};
		F_Lua["LogLong"] = [](long value)
		{
			std::string scriptName = F_Lua["calling_script_name"].get_or<std::string>("Script (Lua)");
			std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
			std::string prefix = "[LUA] " + truncatedName + " :";
			LogLong(value, "", prefix);
		};
		F_Lua["LogVector2"] = [](Vector2 value)
			{
				std::string scriptName = F_Lua["calling_script_name"].get_or<std::string>("Script (Lua)");
				std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
				std::string prefix = "[LUA] " + truncatedName + " :";
				LogVector2(value, "", prefix);
			};
		F_Lua["GetMappingContext"] = [](std::string contextName)
		{
			return GetMappingContext(contextName);
		};
		F_Lua["Instantiate"] = [](std::string prefabName, Vector2 position)
		{
			// track what GameObject called Instantiate() so we can set the Lua state back to that GameObject after Instantiate() initializes any scripts it creates by calling Awake()/Start() on them
			GameObject* currentLuaObject = F_Lua["this_object"];
			std::string callingScript = F_Lua["calling_script_name"];
			GameObject *newObject = Instantiate(prefabName, position, GetLoadedScene());
			LoadLuaGameObject(currentLuaObject, callingScript);

			// Maybe try Instantiations on a new thread so we don't keep going deeper into the RunLuaFuncOnSingleScript() nesting

			return newObject;
		};
		F_Lua["CloseProgram"] = []()
			{
				F_b_closeProgramQueued = true;
			};
		F_Lua["DrawLineInScene"] = [](Vector2 startPoint, Vector2 endPoint, Vector4 color, float thickness)
		{
			DrawLineInScene(startPoint, endPoint, color, thickness);
		};
		F_Lua["DrawLineInGame"] = [](Vector2 startPoint, Vector2 endPoint, Vector4 color, float thickness)
		{
			DrawLineInGame(startPoint, endPoint, color, thickness);
		};
		F_Lua["GetTime"] = []()
		{
			return GetEllapsedGameTimeInMs();
		};
		F_Lua["Destroy"] = [](long ID)
		{
 			F_Application->GetGameLoop()->AddObjectToDeleteQueue(ID);
		};
		F_Lua["GetColor"] = [](std::string color)
		{
			return GetColor(color);
		};
		F_Lua["RandomInt"] = [](int min, int max)
		{
			return GetRandInt(min, max);
		};
		F_Lua["RandomFloat"] = [](float min, float max)
		{
			return GetRandFloat(min, max);
		};
		F_Lua["Remap"] = [](std::string contextName, std::string inputAction, int timeoutTime)
		{
			RemapInputAction(contextName, inputAction, Uint32(timeoutTime));
		};
		F_Lua["Scene_GetMousePosWorld"] = []()
			{
				return Scene_GetMousePosWorld();
			};
		F_Lua["GetMousePosWorld"] = []()
		{
			return GetMousePosWorld();
		};
		F_Lua["GetMousePosScreen"] = []()
		{
			return GetMousePosScreen();
		};
		F_Lua["Rotate"] = [](Vector2 vector, float angleDegrees)
			{
				return Vector2::Rotate(vector, angleDegrees);
			};
		F_Lua["Normalize"] = [](Vector2 vector)
			{
				return Vector2::Normalize(vector);
			};
		F_Lua["GetMagnitude"] = [](Vector2 vector)
			{
				return vector.GetMagnitude();
			};
		F_Lua["GetAngleBetween"] = [](Vector2 vector1, Vector2 vector2)
			{
				return Vector2::GetAngleBetween(vector1, vector2);
			};
		F_Lua["SubtractVectors"] = [](Vector2 vector1, Vector2 vector2)
			{
				return vector1 - vector2;
			};
		F_Lua["AddVectors"] = [](Vector2 vector1, Vector2 vector2)
			{
				return vector1 + vector2;
			};
		F_Lua["ToInt"] = [](float value)
			{
				return (int)value;
			};
		F_Lua["ToFloat"] = [](int value)
			{
				return (float)value;
			};
	}

	// Map C++ types to Lua "Types" -- https://sol2.readthedocs.io/en/latest/api/usertype.html
	void RegisterLuaTypes()
	{
		F_Lua.new_usertype<Scene>("Scene",
			"SetName", &Scene::SetName,
			"GetName", &Scene::GetName,
			"GetPath", &Scene::GetPath
		);

		F_Lua.new_usertype<Vector2>("Vector2",
			sol::constructors<Vector2(), Vector2(float x,float y)>(),
			"SetX", &Vector2::SetX,
			"x", sol::readonly(&Vector2::x),
			"SetY", &Vector2::SetY,
			"y", sol::readonly(&Vector2::y),
			"SetXY", &Vector2::_xy,
			"Normalize", &Vector2::NormalizeSelf,
			"Rotate", &Vector2::RotateSelf,
			"GetMagnitude", &Vector2::GetMagnitude,
			"ProjectOnto", &Vector2::ProjectedOnto,
			"CrossKResult", &Vector2::CrossKResult,
			"AmountProjectedOnto", &Vector2::AmountProjectedOnto,
			"Dot", &Vector2::Dot,
			"+", &Vector2::operator+,
			"-", &Vector2::operator-
		);

		F_Lua.new_usertype<Vector4>("Vector4",
			sol::constructors<Vector4(), Vector4(float x, float y, float z, float w)>(),
			"SetX", &Vector4::SetX,
			"x", &Vector4::GetX,
			"SetY", &Vector4::SetY,
			"y", &Vector4::GetY,
			"SetZ", &Vector4::SetZ,
			"z", &Vector4::GetZ,
			"SetW", &Vector4::SetW,
			"w", &Vector4::GetW,
			"SetXYZW", &Vector4::_xyzw
		);

		F_Lua.new_usertype<GameObject>("GameObject",
			"GetID", &GameObject::GetID,
			"GetName", &GameObject::GetName,
			"SetName", &GameObject::SetName,
			"IsActive", &GameObject::IsActive,
			"SetActive", &GameObject::SetActive,
			"GetParent", &GameObject::GetParent,
			"GetParentID", &GameObject::GetParentID,
			"HasTag", &GameObject::HasTag,
			"SetTag", &GameObject::SetTag,
			"SetCollides", &GameObject::SetCollides,
			"GetTransform", &GameObject::GetTransform,
			"GetSprite", &GameObject::GetSprite,
			"GetScript", &GameObject::GetScript,
			"GetCamera", &GameObject::GetCamera,
			"GetAnimation", &GameObject::GetAnimation,
			"GetAudio", &GameObject::GetAudio,
			"GetButton", &GameObject::GetButton,
			"GetCanvas", &GameObject::GetCanvas,
			"GetText", &GameObject::GetText,
			"GetBody", &GameObject::GetBody,
			"GetCharacterController", &GameObject::GetCharacterController,		
			"GetTileMap", &GameObject::GetTileMap,
			"HasComponent", &GameObject::HasComponentLua,
			
			"AddSprite", &GameObject::AddSpriteLua,
			"AddScript", &GameObject::AddScriptLua,
			"AddCamera", &GameObject::AddCameraLua,
			"AddAnimation", &GameObject::AddAnimationLua,
			"AddAudio", &GameObject::AddAudioLua,
			"AddButton", &GameObject::AddButtonLua,
			"AddCanvas", &GameObject::AddCanvasLua,
			"AddText", &GameObject::AddTextLua,
			"AddBody", &GameObject::AddBody,
			"AddCharacterController", &GameObject::AddCharacterControllerLua,
			"AddTileMap", &GameObject::AddTileMapLua,

			"AddChild", &GameObject::AddChild,
			"RemoveChild", &GameObject::RemoveChild,
			"GetFirstChild", &GameObject::GetFirstChild,
			"HasChildren", &GameObject::HasChildren,
			"GetChildren", &GameObject::GetChildren,
			"HasScript", &GameObject::HasScript,
			"FindChildByName", &GameObject::FindChildByName
		);

		F_Lua.new_usertype<Transform>("Transform",
			"GetParent", &Transform::GetParent,
			"GetParentID", &Transform::GetParentID,
			"GetID", &Transform::GetID,
			"SetPosition", &Transform::SetPosition,
			"GetPosition", &Transform::GetPosition,
			"GetAbsolutePosition", &Transform::GetAbsolutePosition,
			"SetRotation", &Transform::SetRotation,
			"GetRotation", &Transform::GetRotation,
			"GetAbsoluteRotation", &Transform::GetAbsoluteRotation,
			"SetScale", &Transform::SetScale,
			"GetScale", &Transform::GetScale,
			"GetAbsoluteScale", &Transform::GetAbsoluteScale,
			"LookAt", &Transform::LookAt
		);

		F_Lua.new_usertype<Sprite>("Sprite",
			"SetActive", &Sprite::SetActive,
			"IsActive", &Sprite::IsActive,
			"GetParent", &Sprite::GetParent,
			"GetParentID", &Sprite::GetParentID,
			"GetID", &Sprite::GetID,
			"SetTexture", &Sprite::SetTexture,
			"GetPath", &Sprite::GetPath,
			"SetScale", &Sprite::SetScale,
			"GetScale", &Sprite::GetScale,
			"GetTextureWidth", &Sprite::GetTextureWidth,
			"GetTextureHeight", &Sprite::GetTextureHeight,			
			"SetTintColor", &Sprite::SetTintColor,
			"GetTintColor", &Sprite::GetTintColor,
			"SetAlpha", &Sprite::SetAlpha,
			"GetAlpha", &Sprite::GetAlpha
		);

		F_Lua.new_usertype<Text>("Text",
			"GetParent", &Text::GetParent,
			"GetParentID", &Text::GetParentID,
			"SetActive", &Text::SetActive,
			"IsActive", &Text::IsActive,
			"GetID", &Text::GetID,
			"SetText", &Text::SetText,
			"SetPivotPoint", &Text::SetPivotPointLua
		);

		F_Lua.new_usertype<Audio>("Audio",
			"GetParent", &Audio::GetParent,
			"GetParentID", &Audio::GetParentID,
			"SetActive", &Audio::SetActive,
			"IsActive", &Audio::IsActive,
			"GetID", &Audio::GetID,
			"IsSoundPlaying", &Audio::IsMusicPlaying,
			"Play", &Audio::PlaySound,
			"Pause", &Audio::PauseSound,
			"Stop", &Audio::PauseSound,
			"StopAll", &Audio::StopAll,
			"SetEffectVolume", &Audio::SetEffectVolume,
			"SetMusicVolume", &Audio::SetMusicVolume
		);

		F_Lua.new_usertype<Button>("Button",
			"GetParent", &Button::GetParent,
			"GetParentID", &Button::GetParentID,
			"SetActive", &Button::SetActive,
			"IsActive", &Button::IsActive,
			"GetID", &Button::GetID,
			"SetActiveDimensions", &Button::SetActiveDimensions,
			"SetActiveOffset", & Button::SetActiveOffset,
			"GetActiveOffset", &Button::GetActiveOffset,
			"SetActiveLayer", & Button::SetActiveLayer,
			"GetActiveLayer", & Button::GetActiveLayer,
			"GetActiveWidth", & Button::GetActiveWidth,
			"GetActiveHeight", &Button::GetActiveHeight,
			"MouseIsOver", &Button::MouseIsOver,
			"SetLeftClick", &Button::SetLeftClick,
			"GetLeftClick", & Button::GetLeftClick,
			"SetRightClick", & Button::SetRightClick,
			"GetRightClick", & Button::GetRightClick,
			"SetFunctionName", &Button::SetFunctionName,
			"GetFunctionName", &Button::GetFunctionName,
			"SetFunctionParams", &Button::SetFunctionParamsLua
		);

		F_Lua.new_usertype<Script>("Script",
			"GetParent", &Script::GetParent,
			"GetParentID", &Script::GetParentID,
			"SetActive", &Script::SetActive,
			"IsActive", &Script::IsActive,
			"GetID", &Script::GetID,
			"SetAttachedScript", &Script::SetAttachedScript,
			"GetAttachedScript", &Script::GetAttachedScript,
			"RunAwakeAndStart", &Script::RunAwakeAndStart
		);

		F_Lua.new_usertype<Animation>("Animation",
			"GetParent", &Animation::GetParent,
			"GetParentID", &Animation::GetParentID,
			"SetActive", &Animation::SetActive,
			"IsActive", &Animation::IsActive,
			"GetID", &Animation::GetID,
			"Play", &Animation::PlayFromLua,
			"Stop", &Animation::Stop,
			"StopAll", &Animation::StopAll,
			"IsPlaying", &Animation::IsPlaying,
			"HasAnimation", &Animation::HasAnimation
		);

		F_Lua.new_usertype<Animation::S_EventFunctionParam>("AnimationEventParameter",
			"string", &Animation::S_EventFunctionParam::GetString,
			"int", &Animation::S_EventFunctionParam::GetInt,
			"long", &Animation::S_EventFunctionParam::GetLong,
			"float", &Animation::S_EventFunctionParam::GetFloat,
			"double", &Animation::S_EventFunctionParam::GetDouble,
			"bool", &Animation::S_EventFunctionParam::GetBool,
			"Vector2", &Animation::S_EventFunctionParam::GetVector2,
			"SetString", &Animation::S_EventFunctionParam::SetString,
			"SetInt", &Animation::S_EventFunctionParam::SetInt,
			"SetLong", &Animation::S_EventFunctionParam::SetLong,
			"SetFloat", &Animation::S_EventFunctionParam::SetFloat,
			"SetDouble", &Animation::S_EventFunctionParam::SetDouble,
			"SetBool", &Animation::S_EventFunctionParam::SetBool,
			"SetVector2", &Animation::S_EventFunctionParam::SetVector2
		);

		F_Lua.new_usertype<Script::S_ScriptParam>("ScriptParameter",
			"string", &Script::S_ScriptParam::GetString,
			"int", &Script::S_ScriptParam::GetInt,
			"long", &Script::S_ScriptParam::GetLong,
			"float", &Script::S_ScriptParam::GetFloat,
			"double", &Script::S_ScriptParam::GetDouble,
			"bool", &Script::S_ScriptParam::GetBool,
			"Vector2", &Script::S_ScriptParam::GetVector2,			
			"SetString", & Script::S_ScriptParam::SetString,
			"SetInt", & Script::S_ScriptParam::SetInt,
			"SetLong", & Script::S_ScriptParam::SetLong,
			"SetFloat", & Script::S_ScriptParam::SetFloat,
			"SetDouble", & Script::S_ScriptParam::SetDouble,
			"SetBool", & Script::S_ScriptParam::SetBool,
			"SetVector2", & Script::S_ScriptParam::SetVector2
		);

		//F_Lua.new_usertype<Animation::S_Event>("ParameterList",
		//	"SetParameters", &Animation::S_Event::SetParameters,
		//	"AddParameter", &Animation::S_Event::AddParameter
		//);

		//F_Lua.new_usertype<Physics::BodyProps>("BodyProps",

		//);	
			
		F_Lua.new_usertype<b2Vec2>("b2Vec2",
			"x", sol::readonly(&b2Vec2::x),
			"y", sol::readonly(&b2Vec2::y)			
		);
		F_Lua.new_usertype<b2Manifold>("Manifold",
			"pointCount", sol::readonly(&b2Manifold::pointCount),
			"GetPoints", &b2Manifold::GetPoints,
			"normal", sol::readonly(&b2Manifold::normal)
		);
		F_Lua.new_usertype<b2ManifoldPoint>("ManifoldPoint",
			"point", sol::readonly(&b2ManifoldPoint::point),
			"anchorA", sol::readonly(&b2ManifoldPoint::anchorA),
			"anchorB", sol::readonly(&b2ManifoldPoint::anchorB),
			"separation", sol::readonly(&b2ManifoldPoint::separation),
			"normalImpulse", sol::readonly(&b2ManifoldPoint::normalImpulse),
			"tangentImpulse", sol::readonly(&b2ManifoldPoint::tangentImpulse),
			"totalNormalImpulse", sol::readonly(&b2ManifoldPoint::totalNormalImpulse),
			"normalVelocity", sol::readonly(&b2ManifoldPoint::normalVelocity),
			"id", sol::readonly(&b2ManifoldPoint::id),
			"persisted", sol::readonly(&b2ManifoldPoint::persisted)
		);

		F_Lua.new_usertype<Body>("Body",
			"SetActive", &Body::SetActive,
			"IsActive", &Body::IsActive,
			"GetParent", &Body::GetParent,
			"GetParentID", &Body::GetParentID,
			"GetID", &Body::GetID,
			"GetBodyProps", &Body::GetBodyProps,
			"SetGravity", &Body::SetGravityScale,			
			"SetLinearDamping", &Body::SetLinearDamping,
			"SetAngularDamping", &Body::SetAngularDamping,
			"ApplyForce", &Body::ApplyForce,
			"ApplyLinearInpulse", &Body::ApplyLinearInpulse,
			"ApplyForceToCenter", &Body::ApplyForceToCenter,
			"ApplyLinearImpulseToCenter", &Body::ApplyLinearImpulseToCenter,
			"ApplyTorque", &Body::ApplyTorque,
			"ApplyAngularImpulse", &Body::ApplyAngularImpulse,
			"GetLinearVelocity", &Body::GetLinearVelocity,
			"GetAngularVelocity", &Body::GetAngularVelocity
		);

		F_Lua.new_usertype<CharacterController>("CharacterController",
			"MoveToward", &CharacterController::MoveToward,
			"GetParent", &CharacterController::GetParent,
			"GetParentID", &CharacterController::GetParentID,
			"SetActive", &CharacterController::SetActive,
			"IsActive", &CharacterController::IsActive,
			"GetID", &CharacterController::GetID
		);

		F_Lua.new_usertype<InputMapping>("InputMapping",
			"KeyCode", &InputMapping::GetKeyCode
			//"InputActionName", &InputMapping::GetActionName
		);

		F_Lua.new_usertype<MappingContext>("MappingContext",			
			"ActionPressed", &MappingContext::ActionPressed,
			"GetName", &MappingContext::GetName,
			"GetInputMappings", &MappingContext::GetInputMappingsLua,
			"GetInputMapping", &MappingContext::GetInputMapping
		);
	}

	void RunLuaFuncOnAllScripts(std::string functionName)
	{
		for (std::pair<long, std::map<long, Script>> object : GetLoadedScene()->GetScripts())
		{
			for (auto& script : object.second)
			{
				if (script.second.IsActive() && script.second.GetAttachedScript() != "")
				{
					std::string scriptName = script.second.GetAttachedScript();
					if (script.second.GetCPPScript() != nullptr)
					{
						script.second.GetCPPScript()->Update();
					}
					else
					{
						RunLuaFuncOnSingleScript(&script.second, functionName);
					}

					//float processTime = (float)GetEngineTime();
					
					//processTime = (float)GetEngineTime() - processTime;
					//if (script.second.GetAttachedScript() == "PropSpawner")
					//{
						//LogString(std::to_string(processTime) + " : " + script.second.GetAttachedScript());
					//}
				}
			}
		}
		for (std::pair<long, std::map<long, Script>> object : GetLoadedProject().GetPersistantGameObjectScene()->GetScripts())
		{
			for (auto& script : object.second)
			{
				if (script.second.IsActive() && script.second.GetAttachedScript() != "")
				{
					RunLuaFuncOnSingleScript(&script.second, functionName);
				}
			}
		}
	}

	void RunLuaFuncOnSingleScript(Script* script, std::string functionName)
	{
		std::string attachedScript = script->GetAttachedScript();

		if (script->IsActive() && attachedScript != "")
		{
			if (F_LoadedSceneScriptFiles.count(attachedScript) || F_LoadedPersistantScriptFiles.count(attachedScript))
			{
				std::string message = "";
				if (ReadyScriptFile(attachedScript, message))
				{
					LoadLuaGameObject(script->GetParent(), attachedScript);
					sol::protected_function func = F_Lua[functionName];
					auto calledFunction = func();

					if (!calledFunction.valid())
					{
						sol::error err = calledFunction;						
						LogError(err.what());
					}
				}
				else
				{
					LogError("Could not invoke script file " + attachedScript + " on " + script->GetParent()->GetName() + "\n" + message);
				}
			}
			else
			{
				LogError("Script " + attachedScript + " not already loaded.");
			}
		}
	}

	void RunSceneAwakeAndStart()
	{
		F_LoadedSceneScriptFiles.clear();

		for (std::pair<long, std::map<long, Script>> object : GetLoadedScene()->GetScripts())
		{
			for (auto& script : object.second)
			{
				if (script.second.IsActive() && script.second.GetAttachedScript() != "")
				{
					std::string scriptName = script.second.GetAttachedScript();
					if (scriptName.find("C++") != std::string::npos)
					{
						script.second.GetCPPScript()->Awake();
					}
					else
					{
						InitLuaScript(&script.second, F_LoadedSceneScriptFiles);
						RunLuaFuncOnSingleScript(&script.second, "Awake");
					}
				}
			}
		}
		for (std::pair<long, std::map<long, Script>> object : GetLoadedScene()->GetScripts())
		{
			for (auto& script : object.second)
			{
				if (script.second.IsActive() && script.second.GetAttachedScript() != "")
				{
					std::string scriptName = script.second.GetAttachedScript();
					if (scriptName.find("C++") != std::string::npos)
					{
						script.second.GetCPPScript()->Start();
					}
					else
					{
						InitLuaScript(&script.second, F_LoadedSceneScriptFiles);
						RunLuaFuncOnSingleScript(&script.second, "Start");
					}
				}
			}
		}
	}

	void RunPersistantAwakeAndStart()
	{
		F_LoadedPersistantScriptFiles.clear();

		for (std::pair<long, std::map<long, Script>> object : GetLoadedProject().GetPersistantGameObjectScene()->GetScripts())
		{
			for (auto& script : object.second)
			{
				if (script.second.IsActive() && script.second.GetAttachedScript() != "")
				{
					InitLuaScript(&script.second, F_LoadedPersistantScriptFiles);
					RunLuaFuncOnSingleScript(&script.second, "Awake");
				}
			}
		}
		for (std::pair<long, std::map<long, Script>> object : GetLoadedProject().GetPersistantGameObjectScene()->GetScripts())
		{
			for (auto& script : object.second)
			{
				if (script.second.IsActive() && script.second.GetAttachedScript() != "")
				{
					InitLuaScript(&script.second, F_LoadedPersistantScriptFiles);
					RunLuaFuncOnSingleScript(&script.second, "Start");
				}
			}
		}
	}

	void RetrieveLuaScriptPaths()
	{
		F_luaScriptPaths.clear();
		F_luaScriptNames.clear();
		F_LuaScriptsMap.clear();

		F_luaScriptNames.push_back(""); // Empty string for when Scripts don't have any selected script attached in RenderScriptComponent()

		std::vector<std::string> scriptPaths = FindAllFilesWithExtension(GetDir("projectDir"), ".scp.lua");
		for (std::string path : scriptPaths)
		{			
			F_luaScriptPaths.push_back(path);
			F_luaScriptNames.push_back(GetFilenameFromPath(path) + " (Lua)");
			F_LuaScriptsMap.emplace(GetFilenameFromPath(path) + " (Lua)", path);

			F_Lua.script({
				GetFilenameFromPath(path) + " = {} "
			});
		}
	}

	void CreateNewLuaScript(std::string fileName, std::string path)
	{
		for (std::string scriptPath : F_luaScriptPaths)
		{
			if (fileName == GetFilenameFromPath(scriptPath))
			{
				LogError("Script name already taken.  Please enter a different name for the Lua script.");
				return;
			}
		}

		std::ofstream outfile;
		std::string fileNameWExtention;

		if (path == "")
		{
			fileNameWExtention = "..\\projects\\" + GetFilenameFromPath(F_LoadedProject.GetPath()) + "\\scripts\\lua\\" + fileName + ".scp.lua";
		}
		else
		{
			fileNameWExtention = path + "/" + fileName + ".scp.lua";
		}

		outfile.open(fileNameWExtention, std::ios_base::app);
		outfile <<
			"-- " + fileName + ".scp.lua\n" +
			"-- use \"this_object\" to reference the object that owns this script and \"my_id\" to access it's id\n\n\n" +

			"function Awake() \n" +
			"     " + fileName + "[my_id] =\n" +
			"     {\n" +
			"		-- Key value pairs here\n" +
			"     }\n" +
			"     local data = GetInstanceData(\"" + fileName + "\", my_id)\n" +
			"end\n\n" +

			"function Start()\n" +
			"     -- required to access instance data\n" +
			"     local data = GetInstanceData(\"" + fileName + "\", my_id)\n" +
			"end\n\n" +

			"function Update()\n" +
			"     local data = GetInstanceData(\"" + fileName + "\", my_id)\n" +
			"end\n\n"+

			"-- each of these functions must be present in each Lua script file otherwise other script's implementations will be used with this object instead\n" +
			"function OnBeginCollision(collidedWith, manifold)\n" +
			"     local data = GetInstanceData(\"" + fileName + "\", my_id)\n" +
			"end\n\n" +

			"function OnEndCollision(collidedWith, manifold)\n" +
			"     local data = GetInstanceData(\"" + fileName + "\", my_id)\n" +
			"end\n\n" +

			"function OnBeginSensorTouch(touched)\n" +
			"     local data = GetInstanceData(\"" + fileName + "\", my_id)\n" +
			"end\n\n" +

			"function OnEndSensorTouch(touched)\n" +
			"     local data = GetInstanceData(\"" + fileName + "\", my_id)\n" +
			"end\n\n" +

			"function OnButtonMouseOver()\n" +
			"end\n\n" +

			"function OnButtonMouseEnter()\n" +
			"end\n\n" +

			"function OnButtonMouseLeave()\n" +
			"end\n\n" +

			"function OnButtonLeftClick()\n" +
			"end\n\n" +

			"function OnButtonRightClick()\n" +
			"end\n\n\n" +
			
			"--Lua cheatsheet\n\n" +
			"--Lua if statements:\n--if (test) then\n-- <do something>\n--elseif\n-- <do something>\n--end\n\n" +
			"--Lua for loops:\n--for init, min/max value, increment\n--do\n-- <do something>\n--end\n\n\n" +
			"--example:\n--for i = 0, 10, 1\n--do\n--LogInt(i)\n--end";
		outfile.close();

		RetrieveLuaScriptPaths();
	}

	bool CheckLuaScriptFile(std::string filePath)
	{
		try
		{
			F_Lua.safe_script_file(filePath);
			return true;
		}
		catch (const sol::error& err)
		{
			LogError("Lua script failed to load");
			LogError(err.what());
			return false;
		}
	}

	void LoadLuaGameObject(GameObject* object, std::string scriptName)
	{
		if (object != nullptr)
		{			
			// Store the name of the script being called in the Lua state (for hands-off named logging from Lua)
			F_Lua["calling_script_name"] = scriptName;
			// Store this object object the Lua state to be accessed by the next Lua function calls
			F_Lua["this_object"] = object;
			// Store object id
			F_Lua["my_id"] = object->GetID();
		}
	}

	// Checks that the script filePath is good and sends the Lua state contextual data
	bool InitLuaScript(Script* script, std::map<std::string, sol::protected_function>& scriptTracker)
	{
		std::string attachedScript = script->GetAttachedScript();
		std::string filePath = "";
		GameObject* caller = script->GetParent();

		if (F_LuaScriptsMap.count(attachedScript))
		{
			filePath = F_LuaScriptsMap.at(attachedScript);
		}
		else
		{
			LogError("Could not initialize " + attachedScript + " on " + caller->GetName() + "\n Not found in F_LuaScriptsMap.");
			return false;
		}

		if (!DoesFileExist(filePath))
		{
			if (caller != nullptr)
			{
				LogError("Could not initialize " + attachedScript + " script on " + caller->GetName() + ".. File does not exist");
			}
			else
			{
				LogError("Could not initialize " + attachedScript + " script on object.. File does not exist and object is nullptr.");
			}
			return false;
		}

		auto scriptFile = F_Lua.load_file(filePath);
		if (scriptFile.valid())
		{
			sol::protected_function loadedScriptFile = scriptFile.get<sol::protected_function>();
			scriptTracker.emplace(attachedScript, loadedScriptFile);
			std::string message = "";

			if (!ReadyScriptFile(attachedScript, message))
			{
				LogError("Could not invoke script file " + attachedScript + " on " + caller->GetName() + "\n" + message);
				return false;
			}
		}
		else
		{
			sol::error error = scriptFile;
			LogError("Could not load script file " + attachedScript + " on " + caller->GetName() + "\n" + error.what());
			return false;
		}

		return true;
	}

	bool ReadyScriptFile(std::string scriptToLoad, std::string &message)
	{
		if (F_LoadedSceneScriptFiles.count(scriptToLoad))
		{
			sol::protected_function loadedScriptFile = F_LoadedSceneScriptFiles.at(scriptToLoad);
			sol::protected_function_result scriptResult;
			scriptResult = loadedScriptFile(); // invoke the script and get the result
			F_Lua["loaded_script_file"] = scriptToLoad;
			if (!scriptResult.valid())
			{
				sol::error error = scriptResult;
				message = error.what();
				return false;
			}
			return true;
		}
		else if (F_LoadedPersistantScriptFiles.count(scriptToLoad))
		{
			sol::protected_function loadedScriptFile = F_LoadedPersistantScriptFiles.at(scriptToLoad);
			sol::protected_function_result scriptResult;
			scriptResult = loadedScriptFile(); // invoke the script and get the result
			F_Lua["loaded_script_file"] = scriptToLoad;
			if (!scriptResult.valid())
			{
				sol::error error = scriptResult;
				message = error.what();
				return false;
			}
			return true;
		}
		else
		{
			message = "Script does not exist in F_LoadedScriptFiles";
			return false;
		}
	}

	template <class T>
	void CallVoidLuaFunction(std::string functionName, T param)
	{
		sol::protected_function protectedFunc = F_Lua[functionName];
		if (protectedFunc)
		{
			auto result = protectedFunc(param);
			if (!result.valid())
			{
				sol::error err = result;
				LogError("Something went wrong in Lua function: " + functionName + "()");
				LogError(err.what());
			}
		}
	}
	template <class T>
	void CallVoidLuaFunction(std::string functionName)
	{
		sol::protected_function protectedFunc = F_Lua[functionName];
		if (protectedFunc)
		{
			auto result = protectedFunc();
			if (!result.valid())
			{
				sol::error err = result;
				LogError("Something went wrong in Lua function: " + functionName + "()");
				LogError(err.what());
			}
		}
	}

	// Collision Events Passed to Lua
	void CallLuaCollisionFunction(LuaEventFunction eventFunc, Body* caller, Body* collidedWith, b2Manifold manifold)
	{
		GameObject* callingObject = caller->GetParent();

		if (callingObject->HasComponent("Script"))
		{
			for (Script* script : callingObject->GetScripts())
			{
				if (script->IsActive())
				{
					std::string attachedScript = script->GetAttachedScript();

					if (F_LuaScriptsMap.count(attachedScript) > 0)
					{						
						std::string functionName = F_LuaEventNames[eventFunc];
						std::string message = "";
						if (!ReadyScriptFile(attachedScript, message))
						{
							LogError("Could not invoke script file " + attachedScript + " on " + script->GetParent()->GetName() + "\n" + message);
						}
						LoadLuaGameObject(callingObject, attachedScript);

						sol::protected_function protectedFunc = F_Lua[functionName];
						if (protectedFunc)
						{
							auto result = protectedFunc(collidedWith, manifold);
							if (!result.valid())
							{
								sol::error err = result;
								LogError("Something went wrong in Lua function: " + functionName + "()");
								LogError(err.what());
							}
						}
					}
				}
			}
		}
	}

	// Sensor Events Passed to Lua
	void CallLuaSensorFunction(LuaEventFunction eventFunc, Body* caller, Body* touched)
	{
		GameObject* callingObject = caller->GetParent();

		if (callingObject->HasComponent("Script"))
		{
			for (Script* script : callingObject->GetScripts())
			{
				if (script->IsActive())
				{
					std::string attachedScript = script->GetAttachedScript();

					if (F_LuaScriptsMap.count(attachedScript) > 0)
					{
						std::string functionName = F_LuaEventNames[eventFunc];
						std::string message = "";
						if (!ReadyScriptFile(attachedScript, message))
						{
							LogError("Could not invoke script file " + attachedScript + " on " + script->GetParent()->GetName() + "\n" + message);
						}
						LoadLuaGameObject(callingObject, attachedScript);

						sol::protected_function protectedFunc = F_Lua[functionName];
						if (protectedFunc)
						{
							auto result = protectedFunc(touched);
							if (!result.valid())
							{
								sol::error err = result;
								LogError("Something went wrong in Lua function: " + functionName + "()");
								LogError(err.what());
							}
						}
					}
				}
			}
		}
	}

	// Button Events passed to Lua through attached Script files
	void CallLuaButtonEventFunction(GameObject* caller, LuaEventFunction eventFunc)
	{
		if (caller->HasComponent("Script"))
		{
			for (Script* script : caller->GetScripts())
			{
				if (script->IsActive())
				{
					if (F_LuaScriptsMap.count(script->GetAttachedScript()) > 0)
					{
						std::string attachedScript = script->GetAttachedScript();
						std::string filePath = F_LuaScriptsMap.at(attachedScript);
						std::string message = "";
						if (!ReadyScriptFile(attachedScript, message))
						{
							LogError("Could not invoke script file " + attachedScript + " on " + script->GetParent()->GetName() + "\n" + message);
						}
						LoadLuaGameObject(caller, attachedScript);
						CallVoidLuaFunction<GameObject*>(F_LuaEventNames[eventFunc]);						
					}
				}
			}
		}
	}

	// Button On Click function events directly added through the Button Component in the Inspector window
	void CallLuaButtonOnClickFunction(GameObject* caller, std::string eventFunc)
	{
		LoadLuaGameObject(caller, eventFunc + " (Lua)");
		CallVoidLuaFunction<GameObject*>(eventFunc);
	}
	// Button On Click function events directly added through the Button Component in the Inspector window
	void CallLuaButtonOnClickFunction(GameObject* caller, std::string eventFunc, Animation::S_EventFunctionParam params)
	{
		LoadLuaGameObject(caller, eventFunc + " (Lua)");
		sol::protected_function protectedFunc = F_Lua[eventFunc];
		if (protectedFunc)
		{
			auto result = sol::function_result();
			result = protectedFunc(params);

			if (!result.valid())
			{
				sol::error err = result;
				LogError("Something went wrong in Lua function: " + eventFunc + "()");
				LogError(err.what());
			}
		}
	}


	void CallLuaAnimationEventFunction(GameObject* caller, std::string eventFunc)
	{
		if (caller->HasComponent("Script"))
		{
			for (Script* script : caller->GetScripts())
			{
				if (script->IsActive())
				{
					if (F_LuaScriptsMap.count(script->GetAttachedScript()) > 0)
					{
						std::string attachedScript = script->GetAttachedScript();
						std::string filePath = F_LuaScriptsMap.at(attachedScript);
						std::string message = "";
						if (!ReadyScriptFile(attachedScript, message))
						{
							LogError("Could not invoke script file " + attachedScript + " on " + script->GetParent()->GetName() + "\n" + message);
						}
						LoadLuaGameObject(caller, attachedScript);
						CallVoidLuaFunction<GameObject*>(eventFunc);				
					}
				}
			}
		}
	}
	void CallLuaAnimationEventFunction(GameObject* caller, std::string eventFunc, Animation::S_EventFunctionParam params)
	{
		if (caller->HasComponent("Script"))
		{
			for (Script* script : caller->GetScripts())
			{
				if (script->IsActive())
				{
					if (F_LuaScriptsMap.count(script->GetAttachedScript()) > 0)
					{
						std::string attachedScript = script->GetAttachedScript();
						std::string filePath = F_LuaScriptsMap.at(attachedScript);
						std::string message = "";
						if (!ReadyScriptFile(attachedScript, message))
						{
							LogError("Could not invoke script file " + attachedScript + " on " + script->GetParent()->GetName() + "\n" + message);
						}
						LoadLuaGameObject(caller, attachedScript);
						sol::protected_function protectedFunc = F_Lua[eventFunc];
						if (protectedFunc)
						{
							auto result = sol::function_result();
							result = protectedFunc(params);

							if (!result.valid())
							{
								sol::error err = result;
								LogError("Something went wrong in Lua function: " + eventFunc + "()");
								LogError(err.what());
							}
						}				
					}
				}
			}
		}
	}
}

// Lua / Sol cheat sheet
// 
//// We can also inject variables directly into the Lua state as well
//F_Lua["newVariable"] = 56;
//
//// We can also inject entire scripts into the Lua state that can use global functions and variables
//// This creates a new lua function called newFunction(a)
//F_Lua.script(
//	"function newFunction(a)"
//	" return newVariable + a "
//	"end");
//
//// This then calls that function with the value of 2 passed in
//int result = F_Lua["newFunction"](2);
//LogInt(result, "Returned Value from new function: ");

// Getting values from Lua globals
//LogInt(F_Lua["b"].get_or(0));
//LogString(F_Lua["a"].get_or<std::string>("0"));
//// Existence checking
//sol::optional<int> anOptionalInt = F_Lua["b"];
//if (anOptionalInt)
//LogInt(anOptionalInt.value(), "an Optional int: ");
//// Workaround for above
//auto intVar = F_Lua["b"].get<std::optional<int>>();
//if (intVar)
//LogInt(intVar.value(), "Optional int 2: ");

//std::optional<sol::table> playerTable = F_Lua["player"];
//if (playerTable)
//{
//	for (const auto& entry : playerTable.value())
//	{
//		sol::object key = entry.first;
//		sol::object value = entry.second;

//		std::string sKey = key.as<std::string>(); // cast key as a string

//		if (sKey == "Name")
//			player.name = value.as<std::string>();
//		if (sKey == "Level")
//			player.level = value.as<int>();
//		// etc...
//	}
//}

// Slightly faster way but unsafe
/*sol::table playerTable2 = F_Lua["player"];
player.name = playerTable2["Name"].get<std::string>();
player.level = playerTable2["Level"].get<int>();*/

// Calling Lua functions from C++
// 
// Unsafe, something might go wrong during execution
//float returnedValue = F_Lua["AddStuff"](3, 4);
//LogFloat(returnedValue, "Returned value unsafe: ");	

// Safely call Lua functions
//sol::protected_function awake = F_Lua["Awake"];
//if (awake)
//{
//	auto result = awake();
//	if (result.valid())
//	{
//		//LogFloat(result.get<float>(), "Returned value safe: ");
//	}
//	else
//	{
//		//sol::error err = result;
//		//LogString(err.what());						
//	}
//}

// Using std containers within Lua
//std::vector<GameObject> objects;
//// Create a Lua variable called allCharacters that holds a reference to characterVec in C++ side
//F_Lua["allObjects"] = &objects;
//
//LogInt((int)objects.size(), "Vector size before: ");
//F_Lua["CreateAllObjects"](15); // Call a Lua function to manipulate the new allCharacters variable
//LogInt((int)objects.size(), "Vector size after: ");
//
//for (int i = 0; i < objects.size(); i++)
//{
//	LogString("Name: " + objects[i].GetName());
//}
