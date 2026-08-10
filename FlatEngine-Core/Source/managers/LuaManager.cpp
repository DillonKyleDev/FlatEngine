
// #include "components/Body.h"
#include "components/Body2D.h"
#include "components/Button.h"
#include "components/CharacterController.h"
#include "components/Script.h"
#include "components/Sprite.h"
#include "components/Text.h"
#include "components/Transform.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "managers/Assets.h"
#include "managers/Controls.h"
#include "managers/LuaManager.h"
#include "managers/PrefabManager.h"
#include "managers/ProjectManager.h"
#include "managers/SceneManager.h"
#include "render/GameView.h"
#include "render/SceneView.h"
#include "scripting/CPPScript.h"
#include "tools/FileHelper.h"
#include "tools/Logger.h"
#include "tools/Numbers.h"
#include "tools/Quaternion.h"
#include "tools/Vector2.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"

#include <fstream>
#include <lua.h>
#include <optional>


namespace FlatEngine
{
	namespace LuaManager
	{
		sol::state lua;	
		std::map<std::string, sol::protected_function> loadedSceneScriptFiles = std::map<std::string, sol::protected_function>();
		std::map<std::string, sol::protected_function> loadedPersistentScriptFiles = std::map<std::string, sol::protected_function>();
		std::vector<std::string> luaScriptPaths = std::vector<std::string>();
		std::vector<std::string> luaScriptNames = std::vector<std::string>();
		std::map<std::string, std::string> luaScriptsMap = std::map<std::string, std::string>();		

		void InitLua()
		{
			lua.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
			RegisterLuaFunctions();
			RegisterLuaTypes();
		}

		std::string LuaObjectToString(const sol::object& obj)
		{
			switch (obj.get_type())
			{
				case sol::type::string:  return obj.as<std::string>();
				case sol::type::number:  return std::to_string(obj.as<double>());
				case sol::type::boolean: return obj.as<bool>() ? "true" : "false";
				case sol::type::nil:     return "nil";
				default:                 return "<unsupported type>";
			}
		}

		sol::object LuaParamToLuaObject(const LuaParameter& param)
		{
			switch(param.type)
			{				
				case ParameterType_String: return sol::make_object(lua, param.p_string);
				case ParameterType_Int32:  return sol::make_object(lua, param.p_int32);
				case ParameterType_Int64:  return sol::make_object(lua, param.p_int64);
				case ParameterType_Float:  return sol::make_object(lua, param.p_float);
				case ParameterType_Double: return sol::make_object(lua, param.p_double);
				case ParameterType_Bool:   return sol::make_object(lua, param.p_bool);
				case ParameterType_Vec2:   return sol::make_object(lua, param.p_vec2);
				case ParameterType_Vec3:   return sol::make_object(lua, param.p_vec3);
				case ParameterType_Vec4:   return sol::make_object(lua, param.p_vec4);
				default:                   return sol::make_object(lua, sol::nil);
			}
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
			lua["CreateGameObject"] = [](long parentID)
			{
				return SceneManager::loadedScene.CreateGameObject(parentID);
			};
			lua["CreateGameObject"] = []()
			{
				return SceneManager::loadedScene.CreateGameObject(-1);
			};
			lua["CreateStringVector"] = []()
			{
				return CreateStringVector();
			};
			lua["CreateIntVector"] = []()
			{
				return CreateIntVector();
			};
			lua["CreateLongVector"] = []()
			{
				return CreateLongVector();
			};
			lua["CreateFloatVector"] = []()
			{
				return CreateFloatVector();
			};
			lua["CreateDoubleVector"] = []()
			{
				return CreateDoubleVector();
			};
			lua["CreateBoolVector"] = []()
			{
				return CreateBoolVector();
			};
			lua["IntToString"] = [](int value)
			{
				return std::to_string(value);
			};
			lua["LongToString"] = [](long value)
			{
				return std::to_string(value);
			};
			lua["FloatToString"] = [](float value)
			{
				return std::to_string(value);
			};
			lua["DoubleToString"] = [](double value)
			{
				return std::to_string(value);
			};
			lua["ContainsData"] = [](std::string scriptName, long ID)
			{
				// Checks the Lua state to see if there is a table value for the given script in the called functions Script file
				std::optional<sol::table> instanceData = lua[scriptName][ID];
				return instanceData.has_value();
			};
			lua["GetInstanceData"] = [](std::string scriptName, long ID)
			{
				// Checks the Lua state to see if there is a table value for the given script in the called functions Script file
				std::optional<sol::table> instanceData = lua[scriptName][ID];
				if (!instanceData.has_value())
				{
					std::string scriptName = lua["calling_script_name"].get_or<std::string>("Script (Lua)");
					std::string truncatedName = scriptName.substr(0, scriptName.size() - 6);
					Logger::log.Err("No instance data for {} found using id: {}.\n -- Lua Function called by GameObject : {}.\n -- Specific calling Script component : {}.\n -- Lua Script where called function lives : {}.\n -- The calling Script file may be missing the function called and is using another Script files version instead. Make sure the called function exists in the {} Script file.", scriptName, std::to_string(ID), SceneManager::loadedScene.GetObjectByID(ID)->GetName(), truncatedName, scriptName, SceneManager::loadedScene.GetObjectByID(ID)->GetName());
				}
				return instanceData;
			};
			lua["GetScriptParam"] = [](std::string paramName, std::optional<long> ID, std::optional<std::string> scriptName)
			{
				LuaParameter parameter = LuaParameter();
				std::string defaultScriptName = lua["calling_script_name"].get_or<std::string>("Script (Lua)");
				// std::string truncatedName = defaultScriptName.substr(0, defaultScriptName.size() - 6);
				GameObject* thisObject = SceneManager::loadedScene.GetObjectByID(ID.value_or(lua["my_id"]));
				ScriptData* scriptData = nullptr;
				std::string objectName = "<OBJECT NOT VALID>";

				if (ID == FL::ProjectManager::PERSISTENT_SCRIPT_ID)
				{
					scriptData = FL::ProjectManager::loadedProject.persistentScript.FindScript(scriptName.value_or(defaultScriptName));
					objectName = "Persistent Script";
				}
				else if (thisObject != nullptr)
				{
					scriptData = thisObject->Get<Script>()->FindScript(scriptName.value_or(defaultScriptName));
					objectName = thisObject->GetName();
				}								

				if (thisObject != nullptr || ID == FL::ProjectManager::PERSISTENT_SCRIPT_ID)
				{					
					if (scriptData != nullptr)
					{
						parameter = scriptData->scriptParamContainer.Get(paramName);

						if (parameter.type == ParameterType_None)
						{
							std::string errorMessage = "No parameter with the name \"" + paramName + "\" found in " + scriptName.value_or(defaultScriptName) + " Script on the " + objectName + " GameObject";
							Logger::log.Err("{}", errorMessage);
						}
					}
					else
					{
						std::string errorMessage = objectName + " does not contain the Script named " + scriptName.value_or(defaultScriptName);
						Logger::log.Err("{}", errorMessage);
					}
				}
				else
				{
					Logger::log.Err("GameObject with that id not found.");
				}

				return LuaParamToLuaObject(parameter);
			};
			lua["LoadScene"] = [](std::string sceneName)
			{
				std::string scenePath = FileHelper::GetFilePathUsingFileName(Assets::assetManager.GetDir("projectDir"), sceneName + ".scn");
				if (scenePath != "")
				{
					SceneManager::QueueLoadScene(scenePath);
				}
				else
				{
					Logger::log.Err("Failed to load scene. Scene {} not found.", sceneName);
				}
			};
			lua["LoadGameObject"] = [](long ID)
			{
				LoadLuaGameObject(SceneManager::loadedScene.GetObjectByID(ID), lua["calling_script_name"]);
			};
			lua["GetObjectByID"] = [](long ID)
			{
				return SceneManager::loadedScene.GetObjectByID(ID);
			};
			lua["GetObjectByName"] = [](std::string objectName)
			{
				return SceneManager::loadedScene.GetObjectByName(objectName);
			};
			lua["ReloadScene"] = []()
			{				
				SceneManager::QueueLoadScene(SceneManager::loadedScene.path);
			};
			lua["GetLoadedScene"] = []()
			{
				return &SceneManager::loadedScene;
			};		
			lua["Log"] = [](sol::variadic_args va)
			{
				std::string message;
				for (auto v : va)
				{
					message += LuaObjectToString(v);
					message += " ";
				}
				
				if (lua["my_id"] == FL::ProjectManager::PERSISTENT_SCRIPT_ID)
					Logger::log.PersistentDebug(message, lua["calling_script_name"], "PS");
				else
				 	Logger::log.LuaDebug(message, lua["calling_script_name"], std::to_string((long)lua["my_id"]));				
			};
			lua["GetMappingContext"] = [](std::string contextName)
			{
				return Controls::GetMappingContext(contextName);
			};
			lua["Instantiate"] = [](std::string prefabName, Vector3 position)
			{
				// track what GameObject called Instantiate() so we can set the Lua state back to that GameObject after Instantiate() initializes any scripts it creates by calling Awake()/Start() on them
				GameObject* currentLuaObject = lua["this_object"];
				std::string callingScript = lua["calling_script_name"];
				GameObject *newObject = PrefabManager::Instantiate(prefabName, position);
				LoadLuaGameObject(currentLuaObject, callingScript);

				// Maybe try Instantiations on a new thread so we don't keep going deeper into the RunLuaFuncOnSingleScript() nesting

				return newObject;
			};
			lua["CloseProgram"] = []()
			{
				F_b_closeProgramQueued = true;
			};
			lua["DebugDrawLine"] = [](Vector3 startPos, Vector3 endPos, std::optional<std::string> color, std::optional<Vector3> rotation)
			{
				SceneView::DebugDrawLine(startPos, endPos, color.value_or("debugDraw"));
			};
			lua["DebugDrawQuad"] = [](Vector3 position, Vector2 size, std::optional<std::string> color, std::optional<Vector3> rotation)
			{
				SceneView::DebugDrawQuad(position, size, color.value_or("debugDraw"), rotation.value_or(Vector3()));
			};
			lua["DebugDrawCircle"] = [](Vector3 position, float radius, std::optional<std::string> color, std::optional<Vector3> rotation)
			{
				SceneView::DebugDrawCircle(position, radius, color.value_or("debugDraw"), rotation.value_or(Vector3()));
			};
			lua["DrawLineInGame"] = [](Vector2 startPoint, Vector2 endPoint, std::string color, float thickness)
			{
				GameView::DrawLineInGame(startPoint, endPoint, color, thickness);
			};
			lua["GetTime"] = []()
			{
				return GetElapsedGameTimeInMs();
			};
			lua["Destroy"] = [](long ID)
			{
				F_Application->GetGameLoop()->AddObjectToDeleteQueue(ID);
			};
			lua["GetColor"] = [](std::string color)
			{
				return Assets::assetManager.GetColor(color);
			};
			lua["RandomInt"] = [](int min, int max)
			{
				return Numbers::GetRandInt(min, max);
			};
			lua["RandomFloat"] = [](float min, float max)
			{
				return Numbers::GetRandFloat(min, max);
			};
			lua["Remap"] = [](std::string contextName, std::string inputAction, int timeoutTime)
			{
				Controls::RemapInputAction(contextName, inputAction, Uint32(timeoutTime));
			};
			lua["Scene_GetMousePosWorld"] = []()
			{
				return SceneView::Scene_GetMousePosWorld();
			};
			lua["GetMousePosWorld"] = []()
			{
				return GameView::GetMousePosWorld();
			};
			lua["GetMousePosScreen"] = []()
			{
				return GameView::GetMousePosScreen();
			};
			lua["Rotate"] = [](Vector2 vector, float angleDegrees)
				{
					return Vector2::Rotate(vector, angleDegrees);
				};
			lua["Normalize"] = [](Vector2 vector)
				{
					return Vector2::Normalize(vector);
				};
			lua["GetMagnitude"] = [](Vector2 vector)
				{
					return vector.GetMagnitude();
				};
			lua["GetAngleBetween"] = [](Vector2 vector1, Vector2 vector2)
				{
					return Vector2::GetAngleBetween(vector1, vector2);
				};
			lua["SubtractVectors"] = [](Vector2 vector1, Vector2 vector2)
				{
					return vector1 - vector2;
				};
			lua["AddVectors"] = [](Vector2 vector1, Vector2 vector2)
				{
					return vector1 + vector2;
				};
			lua["ToInt"] = [](float value)
			{
				return (int)value;
			};
			lua["ToFloat"] = [](int value)
			{
				return (float)value;
			};
		}

		// Map C++ types to Lua "Types" -- https://sol2.readthedocs.io/en/latest/api/usertype.html
		void RegisterLuaTypes()
		{
			lua.new_usertype<Scene>("Scene",
				"name", sol::readonly(&Scene::name),
				"path", sol::readonly(&Scene::path)
			);

			lua.new_usertype<Vector2>("Vector2",
				sol::constructors<Vector2(), Vector2(float x,float y)>(),				
				"x", &Vector2::x,				
				"y", &Vector2::y,				
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

			lua.new_usertype<Vector3>("Vector3",
				sol::constructors<Vector3(), Vector3(float x, float y, float z)>(),				
				"x", &Vector3::x,				
				"y", &Vector3::y,				
				"z", &Vector3::z
			);

			lua.new_usertype<Vector4>("Vector4",
				sol::constructors<Vector4(), Vector4(float x, float y, float z, float w)>(),		
				"x", &Vector4::x,
				"y", &Vector4::y,
				"z", &Vector4::z,
				"w", &Vector4::w
			);

			lua.new_usertype<Quaternion>("Quaternion",
				sol::constructors<Quaternion(), Quaternion(float s, Vector3 v)>(),				
				"s", &Quaternion::s,
				"x", &Quaternion::x,
				"y", &Quaternion::y,
				"z", &Quaternion::z,
				"EulerToQuaternion", &Quaternion::EulerToQuaternion,
				"QuaternionToEuler", &Quaternion::QuaternionToEuler,
				"Inverse", &Quaternion::Inverse,
				"Times", &Quaternion::operator*
			);

			lua.new_usertype<GameObject>("GameObject",
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
				"GetTransform", &GameObject::Get<Transform>,
				"GetSprite", &GameObject::Get<Sprite>,
				"GetScript", &GameObject::Get<Script>,
				"GetCamera", &GameObject::Get<Camera>,
				"GetAnimation", &GameObject::Get<Animation>,
				"GetAudio", &GameObject::Get<Audio>,
				"GetButton", &GameObject::Get<Button>,
				"GetCanvas", &GameObject::Get<Canvas>,
				"GetText", &GameObject::Get<Text>,
				"GetBody", &GameObject::Get<Body>,
				"GetMesh", &GameObject::Get<Mesh>,
				"GetCharacterController", &GameObject::Get<CharacterController>,		
				"GetTileMap", &GameObject::Get<TileMap>,
				"AddSprite", &GameObject::Add<Sprite>,
				"AddScript", &GameObject::Add<Script>,
				"AddCamera", &GameObject::Add<Camera>,
				"AddAnimation", &GameObject::Add<Animation>,
				"AddAudio", &GameObject::Add<Audio>,
				"AddButton", &GameObject::Add<Button>,
				"AddCanvas", &GameObject::Add<Canvas>,
				"AddText", &GameObject::Add<Text>,
				"AddBody", &GameObject::Add<Body>,
				"AddCharacterController", &GameObject::Add<CharacterController>,
				"AddTileMap", &GameObject::Add<TileMap>,
				"AddMesh", & GameObject::Add<Mesh>,

				"AddChild", &GameObject::AddChild,
				"RemoveChild", &GameObject::RemoveChild,
				"GetFirstChild", &GameObject::GetFirstChild,
				"HasChildren", &GameObject::HasChildren,
				"GetChildren", &GameObject::GetChildren,
				"FindChildByName", &GameObject::FindChildByName
			);

			lua.new_usertype<Transform>("Transform",
				"GetOwningObject", &Transform::GetOwningObject,
				"GetOwnerID", &Transform::GetOwnerID,
				"SetPosition", &Transform::SetPosition,
				"GetPosition", &Transform::GetPosition,
				"GetAbsolutePosition", &Transform::GetAbsolutePosition,
				"GetRotation", &Transform::GetRotation,
				"SetScale", &Transform::SetScale,
				"GetScale", &Transform::GetScale,
				"GetAbsoluteScale", &Transform::GetAbsoluteScale,
				"LookAt", &Transform::LookAt
			);

			lua.new_usertype<Sprite>("Sprite",
				"SetActive", &Sprite::SetActive,
				"IsActive", &Sprite::IsActive,
				"GetOwningObject", &Sprite::GetOwningObject,
				"GetOwnerID", &Sprite::GetOwnerID,
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

			lua.new_usertype<Text>("Text",
				"GetOwningObject", &Text::GetOwningObject,
				"GetOwnerID", &Text::GetOwnerID,
				"SetActive", &Text::SetActive,
				"IsActive", &Text::IsActive,
				"SetText", &Text::SetText,
				"SetPivotPoint", &Text::SetPivotPointLua
			);

			lua.new_usertype<Audio>("Audio",
				"GetOwningObject", &Audio::GetOwningObject,
				"GetOwnerID", &Audio::GetOwnerID,
				"SetActive", &Audio::SetActive,
				"IsActive", &Audio::IsActive,
				"IsSoundPlaying", &Audio::IsMusicPlaying,
				"Play", &Audio::PlaySound,
				"Pause", &Audio::PauseSound,
				"Stop", &Audio::PauseSound,
				"StopAll", &Audio::StopAll,
				"SetEffectVolume", &Audio::SetEffectVolume,
				"SetMusicVolume", &Audio::SetMusicVolume
			);

			lua.new_usertype<Button>("Button",
				"GetOwningObject", &Button::GetOwningObject,
				"GetOwnerID", &Button::GetOwnerID,
				"SetActive", &Button::SetActive,
				"IsActive", &Button::IsActive,
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

			lua.new_usertype<Script>("Script",
				"GetOwningObject", &Script::GetOwningObject,
				"GetOwnerID", &Script::GetOwnerID,
				"SetActive", &Script::SetActive,
				"IsActive", &Script::IsActive
				// "SetAttachedScript", &Script::SetAttachedScript,
				// "GetAttachedScript", &Script::GetAttachedScript,
				// "RunAwakeAndStart", &Script::RunAwakeAndStart
			);

			lua.new_usertype<Animation>("Animation",
				"GetOwningObject", &Animation::GetOwningObject,
				"GetOwnerID", &Animation::GetOwnerID,
				"SetActive", &Animation::SetActive,
				"IsActive", &Animation::IsActive,
				"Play", &Animation::PlayFromLua,
				"Stop", &Animation::Stop,
				"StopAll", &Animation::StopAll,
				"IsPlaying", &Animation::IsPlaying,
				"HasAnimation", &Animation::HasAnimation
			);

			// lua.new_usertype<Animation::S_EventFunctionParam>("AnimationEventParameter",
			// 	"string", &Animation::S_EventFunctionParam::GetString,
			// 	"int", &Animation::S_EventFunctionParam::GetInt,
			// 	"long", &Animation::S_EventFunctionParam::GetLong,
			// 	"float", &Animation::S_EventFunctionParam::GetFloat,
			// 	"double", &Animation::S_EventFunctionParam::GetDouble,
			// 	"bool", &Animation::S_EventFunctionParam::GetBool,
			// 	"Vector2", &Animation::S_EventFunctionParam::GetVector2,
			// 	"SetString", &Animation::S_EventFunctionParam::SetString,
			// 	"SetInt", &Animation::S_EventFunctionParam::SetInt,
			// 	"SetLong", &Animation::S_EventFunctionParam::SetLong,
			// 	"SetFloat", &Animation::S_EventFunctionParam::SetFloat,
			// 	"SetDouble", &Animation::S_EventFunctionParam::SetDouble,
			// 	"SetBool", &Animation::S_EventFunctionParam::SetBool,
			// 	"SetVector2", &Animation::S_EventFunctionParam::SetVector2
			// );

			// lua.new_usertype<Script::S_ScriptParam>("ScriptParameter",
			// 	"string", &Script::S_ScriptParam::GetString,
			// 	"int", &Script::S_ScriptParam::GetInt,
			// 	"long", &Script::S_ScriptParam::GetLong,
			// 	"float", &Script::S_ScriptParam::GetFloat,
			// 	"double", &Script::S_ScriptParam::GetDouble,
			// 	"bool", &Script::S_ScriptParam::GetBool,
			// 	"Vector2", &Script::S_ScriptParam::GetVector2,			
			// 	"SetString", & Script::S_ScriptParam::SetString,
			// 	"SetInt", & Script::S_ScriptParam::SetInt,
			// 	"SetLong", & Script::S_ScriptParam::SetLong,
			// 	"SetFloat", & Script::S_ScriptParam::SetFloat,
			// 	"SetDouble", & Script::S_ScriptParam::SetDouble,
			// 	"SetBool", & Script::S_ScriptParam::SetBool,
			// 	"SetVector2", & Script::S_ScriptParam::SetVector2
			// );

			//lua.new_usertype<Animation::S_Event>("ParameterList",
			//	"SetParameters", &Animation::S_Event::SetParameters,
			//	"AddParameter", &Animation::S_Event::AddParameter
			//);

			//lua.new_usertype<Physics::BodyProps>("BodyProps",

			//);	
				
			lua.new_usertype<b2Vec2>("b2Vec2",
				"x", sol::readonly(&b2Vec2::x),
				"y", sol::readonly(&b2Vec2::y)			
			);
			lua.new_usertype<b2Manifold>("Manifold",
				"pointCount", sol::readonly(&b2Manifold::pointCount),
				"GetPoints", &b2Manifold::GetPoints,
				"normal", sol::readonly(&b2Manifold::normal)
			);
			lua.new_usertype<b2ManifoldPoint>("ManifoldPoint",
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

			// lua.new_usertype<Body>("Body",
			// 	"SetActive", &Body::SetActive,
			// 	"IsActive", &Body::IsActive,
			// 	"GetOwningObject", &Body::GetOwningObject,
			// 	"GetOwnerID", &Body::GetOwnerID,
			// 	"GetID", &Body::GetID,
			// 	"GetBodyProps", &Body::GetBodyProps,
			// 	"SetGravity", &Body::SetGravityScale,			
			// 	"SetLinearDamping", &Body::SetLinearDamping,
			// 	"SetAngularDamping", &Body::SetAngularDamping,
			// 	"ApplyForce", &Body::ApplyForce,
			// 	"ApplyLinearInpulse", &Body::ApplyLinearInpulse,
			// 	"ApplyForceToCenter", &Body::ApplyForceToCenter,
			// 	"ApplyLinearImpulseToCenter", &Body::ApplyLinearImpulseToCenter,
			// 	"ApplyTorque", &Body::ApplyTorque,
			// 	"ApplyAngularImpulse", &Body::ApplyAngularImpulse,
			// 	"GetLinearVelocity", &Body::GetLinearVelocity,
			// 	"GetAngularVelocity", &Body::GetAngularVelocity
			// );

			lua.new_usertype<Body2D>("Body2D",
				"SetActive", &Body2D::SetActive,
				"IsActive", &Body2D::IsActive,
				"GetOwningObject", &Body2D::GetOwningObject,
				"GetOwnerID", &Body2D::GetOwnerID,
				"SetGravity", &Body2D::SetGravityScale,			
				"SetLinearDamping", &Body2D::SetLinearDamping,
				"SetAngularDamping", &Body2D::SetAngularDamping,
				"ApplyForce", &Body2D::ApplyForce,
				"ApplyLinearInpulse", &Body2D::ApplyLinearInpulse,
				"ApplyForceToCenter", &Body2D::ApplyForceToCenter,
				"ApplyLinearImpulseToCenter", &Body2D::ApplyLinearImpulseToCenter,
				"ApplyTorque", &Body2D::ApplyTorque,
				"ApplyAngularImpulse", &Body2D::ApplyAngularImpulse,
				"GetLinearVelocity", &Body2D::GetLinearVelocity,
				"GetAngularVelocity", &Body2D::GetAngularVelocity
			);

			lua.new_usertype<Mesh>("Mesh",
				"SetUBOVec4", &Mesh::SetUBOVec4,
				"AddTexture", &Mesh::AddTextureLua,
				"SetModel", &Mesh::SetModel,
				"SetMaterial", &Mesh::SetMaterial,
				"CreateResources", &Mesh::CreateResources			
			);

			lua.new_usertype<CharacterController>("CharacterController",
				"MoveToward", &CharacterController::MoveToward,
				"GetOwningObject", &CharacterController::GetOwningObject,
				"GetOwnerID", &CharacterController::GetOwnerID,
				"SetActive", &CharacterController::SetActive,
				"IsActive", &CharacterController::IsActive
			);

			lua.new_usertype<Controls::InputMapping>("InputMapping",
				"KeyCode", &Controls::InputMapping::GetKeyCode
				//"InputActionName", &InputMapping::GetActionName
			);

			lua.new_usertype<Controls::MappingContext>("MappingContext",			
				"ActionPressed", &Controls::MappingContext::ActionPressed,
				"GetName", &Controls::MappingContext::GetName,
				"GetInputMappings", &Controls::MappingContext::GetInputMappingsLua,
				"GetInputMapping", &Controls::MappingContext::GetInputMapping
			);
		}

		void RunLuaFuncOnAllScripts(std::string functionName)
		{
			for (Script& script : SceneManager::loadedScene.GetAll<Script>().GetAll())
			{
				if (script.IsActive())
				{
					for (ScriptData scriptData : script.GetScripts())
					{
						if (scriptData.name != "")
						{
							if (scriptData.GetCPPScript() != nullptr)
							{
								scriptData.GetCPPScript()->Update();
							}
							else
							{
								RunLuaFuncOnSingleScript(scriptData, script.GetOwningObject(), functionName);
							}
						}
					}
				}
			}
			Script& persistentScript = ProjectManager::loadedProject.persistentScript;

			if (persistentScript.IsActive())
			{
				for (ScriptData scriptData : persistentScript.GetScripts())
				{
					if (scriptData.name != "")
					{
						RunLuaFuncOnSingleScript(scriptData, nullptr, functionName);
					}
				}
			}
		}

		void RunLuaFuncOnSingleScript(ScriptData scriptData, GameObject* caller, std::string functionName)
		{
			std::string attachedScript = scriptData.name;

			if (attachedScript != "")
			{
				if (loadedSceneScriptFiles.count(attachedScript) || loadedPersistentScriptFiles.count(attachedScript))
				{
					std::string message = "";
					if (ReadyScriptFile(attachedScript, message))
					{
						LoadLuaGameObject(caller, attachedScript);
						sol::protected_function func = lua[functionName];
						auto calledFunction = func();

						if (!calledFunction.valid())
						{
							sol::error err = calledFunction;						
							Logger::log.Err("{}", err.what());
						}
					}
					else
					{
						Logger::log.Err("Could not invoke script file {} on {}\n{}", attachedScript, caller != nullptr ? caller->GetName() : "Caller was nullptr", message);
					}
				}
				else
				{
					Logger::log.Err("Script {} not already loaded.", attachedScript);
				}
			}
		}

		void RunSceneAwakeAndStart()
		{
			loadedSceneScriptFiles.clear();

			for (Script& script : SceneManager::loadedScene.GetAll<Script>().GetAll())
			{
				if (script.IsActive())
				{
					for (ScriptData scriptData : script.GetScripts())
					{
						if (scriptData.name != "")
						{
							std::string attachedScript = scriptData.name;
							if (attachedScript.find("C++") != std::string::npos)
							{
								scriptData.GetCPPScript()->Awake();
							}
							else
							{
								InitLuaScript(scriptData, SceneManager::loadedScene.GetObjectByID(script.GetOwnerID()), loadedSceneScriptFiles);
								RunLuaFuncOnSingleScript(scriptData, SceneManager::loadedScene.GetObjectByID(script.GetOwnerID()), "Awake");
							}
						}
					}
				}
			}
			for (Script& script : SceneManager::loadedScene.GetAll<Script>().GetAll())
			{
				if (script.IsActive())
				{
					for (ScriptData scriptData : script.GetScripts())
					{
						if (scriptData.name != "")
						{
							std::string scriptName = scriptData.name;
							if (scriptName.find("C++") != std::string::npos)
							{
								scriptData.GetCPPScript()->Start();
							}
							else
							{
								InitLuaScript(scriptData, SceneManager::loadedScene.GetObjectByID(script.GetOwnerID()), loadedSceneScriptFiles);
								RunLuaFuncOnSingleScript(scriptData, SceneManager::loadedScene.GetObjectByID(script.GetOwnerID()), "Start");
							}
						}
					}
				}		
			}
		}

		void RunPersistentAwakeAndStart()
		{
			loadedPersistentScriptFiles.clear();
			Script& persistentScript = ProjectManager::loadedProject.persistentScript;

			if (persistentScript.IsActive())
			{
				for (ScriptData scriptData : persistentScript.GetScripts())
				{
					if (scriptData.name != "")
					{
						InitLuaScript(scriptData, nullptr, loadedPersistentScriptFiles);
						RunLuaFuncOnSingleScript(scriptData, nullptr, "Awake");
					}
				}
				for (ScriptData scriptData : persistentScript.GetScripts())
				{
					if (scriptData.name != "")
					{
						InitLuaScript(scriptData, nullptr, loadedPersistentScriptFiles);
						RunLuaFuncOnSingleScript(scriptData, nullptr, "Start");
					}
				}
			}
		}

		void RetrieveLuaScriptPaths()
		{
			luaScriptPaths.clear();
			luaScriptNames.clear();
			luaScriptsMap.clear();

			luaScriptNames.push_back(""); // Empty string for when Scripts don't have any selected script attached in RenderScriptComponent()

			std::vector<std::string> scriptPaths = FileHelper::FindAllFilesWithExtension(Assets::assetManager.GetDir("projectDir"), ".scp.lua");
			for (std::string path : scriptPaths)
			{			
				luaScriptPaths.push_back(path);
				luaScriptNames.push_back(FileHelper::GetFilenameFromPath(path) + " (Lua)");
				luaScriptsMap.emplace(FileHelper::GetFilenameFromPath(path) + " (Lua)", path);

				lua.script({
					FileHelper::GetFilenameFromPath(path) + " = {} "
				});
			}
		}

		void CreateNewLuaScript(std::string fileName, std::string path)
		{
			for (std::string scriptPath : luaScriptPaths)
			{
				if (fileName == FileHelper::GetFilenameFromPath(scriptPath))
				{
					Logger::log.Err("Script name already taken.  Please enter a different name for the Lua script.");
					return;
				}
			}

			std::ofstream outfile;
			std::string fileNameWExtention;

			if (path == "")
			{
				fileNameWExtention = "../projects/" + FileHelper::GetFilenameFromPath(ProjectManager::loadedProject.path) + "/scripts/lua/" + fileName + ".scp.lua";
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
				lua.safe_script_file(filePath);
				return true;
			}
			catch (const sol::error& err)
			{
				Logger::log.Err("Lua script failed to load");
				Logger::log.Err("{}", err.what());
				return false;
			}
		}

		void LoadLuaGameObject(GameObject* object, std::string scriptName)
		{
			// Store the name of the script being called in the Lua state (for hands-off named logging from Lua)
			lua["calling_script_name"] = scriptName;
			// Store this object object the Lua state to be accessed by the next Lua function calls
			lua["this_object"] = object;// != nullptr ? sol::make_object(lua., object) : sol::make_object(lua, sol::lua_nil);
			// Store object id
			lua["my_id"] = object != nullptr ? object->GetID() : FL::ProjectManager::PERSISTENT_SCRIPT_ID;
		}

		// Checks that the script filePath is good and sends the Lua state contextual data
		bool InitLuaScript(ScriptData scriptData, GameObject* caller, std::map<std::string, sol::protected_function>& scriptTracker)
		{
			std::string attachedScript = scriptData.name;
			std::string filePath = "";

			if (luaScriptsMap.count(attachedScript))
			{
				filePath = luaScriptsMap.at(attachedScript);
			}
			else
			{
				Logger::log.Err("Could not initialize {} on {} \n Not found in luaScriptsMap.", attachedScript, caller != nullptr ? caller->GetName() : "<Caller was nullptr>");
				return false;
			}

			if (!FileHelper::DoesFileExist(filePath))
			{
				if (caller != nullptr)
				{
					Logger::log.Err("Could not initialize {} script on {}.. File does not exist", attachedScript, caller != nullptr ? caller->GetName() : "<Caller was nullptr>");
				}
				else
				{
					Logger::log.Err("Could not initialize {} script on object.. File does not exist and object is nullptr.", attachedScript);
				}
				return false;
			}

			auto scriptFile = lua.load_file(filePath);
			if (scriptFile.valid())
			{
				sol::protected_function loadedScriptFile = scriptFile.get<sol::protected_function>();
				scriptTracker.emplace(attachedScript, loadedScriptFile);
				std::string message = "";

				if (!ReadyScriptFile(attachedScript, message))
				{
					Logger::log.Err("Could not invoke script file {} on {}\n{}", attachedScript, caller != nullptr ? caller->GetName() : "Caller was nullptr.", message);
					return false;
				}
			}
			else
			{
				sol::error error = scriptFile;
				Logger::log.Err("Could not load script file {} on {}\n{}", attachedScript, caller != nullptr ? caller->GetName() : "Caller was nullptr.", error.what());
				return false;
			}

			return true;
		}

		bool ReadyScriptFile(std::string scriptToLoad, std::string &message)
		{
			if (loadedSceneScriptFiles.count(scriptToLoad))
			{
				sol::protected_function loadedScriptFile = loadedSceneScriptFiles.at(scriptToLoad);
				sol::protected_function_result scriptResult;
				scriptResult = loadedScriptFile(); // invoke the script and get the result
				lua["loaded_script_file"] = scriptToLoad;
				if (!scriptResult.valid())
				{
					sol::error error = scriptResult;
					message = error.what();
					return false;
				}
				return true;
			}
			else if (loadedPersistentScriptFiles.count(scriptToLoad))
			{
				sol::protected_function loadedScriptFile = loadedPersistentScriptFiles.at(scriptToLoad);
				sol::protected_function_result scriptResult;
				scriptResult = loadedScriptFile(); // invoke the script and get the result
				lua["loaded_script_file"] = scriptToLoad;
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
			sol::protected_function protectedFunc = lua[functionName];
			if (protectedFunc)
			{
				auto result = protectedFunc(param);
				if (!result.valid())
				{
					sol::error err = result;
					Logger::log.Err("Something went wrong in Lua function: {}()", functionName);
					Logger::log.Err("{}", err.what());
				}
			}
		}
		template <class T>
		void CallVoidLuaFunction(std::string functionName)
		{
			sol::protected_function protectedFunc = lua[functionName];
			if (protectedFunc)
			{
				auto result = protectedFunc();
				if (!result.valid())
				{
					sol::error err = result;
					Logger::log.Err("Something went wrong in Lua function: {}()", functionName);
					Logger::log.Err("{}", err.what());
				}
			}
		}

		// Collision Events Passed to Lua
		void CallLuaCollisionFunction(LuaEventFunction eventFunc, Body* caller, Body* collidedWith, b2Manifold manifold)
		{
			GameObject* callingObject = caller->GetOwningObject();
			Script* script = callingObject->Get<Script>();

			if (script->IsActive() && script->GetScripts().size())
			{
				for (ScriptData scriptData : callingObject->Get<Script>()->GetScripts())
				{
					std::string attachedScript = scriptData.name;

					if (luaScriptsMap.count(attachedScript) > 0)
					{						
						std::string functionName = luaEventNames[eventFunc];
						std::string message = "";
						if (!ReadyScriptFile(attachedScript, message))
						{
							Logger::log.Err("Could not invoke script file {} on {}\n{}", attachedScript, script->GetOwningObject()->GetName(), message);
						}
						LoadLuaGameObject(callingObject, attachedScript);

						sol::protected_function protectedFunc = lua[functionName];
						if (protectedFunc)
						{
							auto result = protectedFunc(collidedWith, manifold);
							if (!result.valid())
							{
								sol::error err = result;
								Logger::log.Err("Something went wrong in Lua function: {}()", functionName);
								Logger::log.Err("{}", err.what());
							}
						}
					}
				}
			}
		}

		// Sensor Events Passed to Lua
		void CallLuaSensorFunction(LuaEventFunction eventFunc, Body* caller, Body* touched)
		{
			GameObject* callingObject = caller->GetOwningObject();
			Script* script = callingObject->Get<Script>();

			if (script->IsActive() && script->GetScripts().size())
			{
				for (ScriptData scriptData : callingObject->Get<Script>()->GetScripts())
				{
					std::string attachedScript = scriptData.name;

					if (luaScriptsMap.count(attachedScript) > 0)
					{
						std::string functionName = luaEventNames[eventFunc];
						std::string message = "";
						if (!ReadyScriptFile(attachedScript, message))
						{
							Logger::log.Err("Could not invoke script file {} on {}\n{}", attachedScript, script->GetOwningObject()->GetName(), message);
						}
						LoadLuaGameObject(callingObject, attachedScript);

						sol::protected_function protectedFunc = lua[functionName];
						if (protectedFunc)
						{
							auto result = protectedFunc(touched);
							if (!result.valid())
							{
								sol::error err = result;
								Logger::log.Err("Something went wrong in Lua function: {}()", functionName);
								Logger::log.Err("{}", err.what());
							}
						}
					}
				}
			}
		}

		void CallLuaCollisionFunction2D(LuaEventFunction eventFunc, Body2D* caller, Body2D* collidedWith, b2Manifold manifold)
		{
			GameObject* callingObject = caller->GetOwningObject();
			Script* script = callingObject->Get<Script>();

			if (script && script->IsActive() && script->GetScripts().size())
			{
				for (ScriptData scriptData : callingObject->Get<Script>()->GetScripts())
				{
					std::string attachedScript = scriptData.name;

					if (luaScriptsMap.count(attachedScript) > 0)
					{						
						std::string functionName = luaEventNames[eventFunc];
						std::string message = "";
						if (!ReadyScriptFile(attachedScript, message))
						{
							Logger::log.Err("Could not invoke script file {} on {}\n{}", attachedScript, script->GetOwningObject()->GetName(), message);
						}
						LoadLuaGameObject(callingObject, attachedScript);

						sol::protected_function protectedFunc = lua[functionName];
						if (protectedFunc)
						{
							auto result = protectedFunc(collidedWith, manifold);
							if (!result.valid())
							{
								sol::error err = result;
								Logger::log.Err("Something went wrong in Lua function: {}()", functionName);
								Logger::log.Err("{}", err.what());
							}
						}
					}
				}
			}
			else 
			{
				Logger::log.Trace("No Script component found on object {}. Lua function {} not called.", callingObject->GetName(), luaEventNames[eventFunc]);
			}
		}

		// Sensor Events Passed to Lua
		void CallLuaSensorFunction2D(LuaEventFunction eventFunc, Body2D* caller, Body2D* touched)
		{
			GameObject* callingObject = caller->GetOwningObject();
			Script* script = callingObject->Get<Script>();

			if (script && script->IsActive() && script->GetScripts().size())
			{
				for (ScriptData scriptData : callingObject->Get<Script>()->GetScripts())
				{
					std::string attachedScript = scriptData.name;

					if (luaScriptsMap.count(attachedScript) > 0)
					{
						std::string functionName = luaEventNames[eventFunc];
						std::string message = "";
						if (!ReadyScriptFile(attachedScript, message))
						{
							Logger::log.Err("Could not invoke script file {} on {}\n{}", attachedScript, script->GetOwningObject()->GetName(), message);
						}
						LoadLuaGameObject(callingObject, attachedScript);

						sol::protected_function protectedFunc = lua[functionName];
						if (protectedFunc)
						{
							auto result = protectedFunc(touched);
							if (!result.valid())
							{
								sol::error err = result;
								Logger::log.Err("Something went wrong in Lua function: {}()", functionName);
								Logger::log.Err("{}", err.what());
							}
						}
					}
				}
			}
			else 
			{
				Logger::log.Trace("No Script component found on object {}. Lua function {} not called.", callingObject->GetName(), luaEventNames[eventFunc]);
			}
		}

		// Button Events passed to Lua through attached Script files
		void CallLuaButtonEventFunction(GameObject* caller, LuaEventFunction eventFunc)
		{
			Script* script = caller->Get<Script>();

			if (script->IsActive() && script->GetScripts().size())
			{
				for (ScriptData scriptData : caller->Get<Script>()->GetScripts())
				{
					std::string attachedScript = scriptData.name;

					if (luaScriptsMap.count(attachedScript) > 0)
					{
						std::string filePath = luaScriptsMap.at(attachedScript);
						std::string message = "";
						if (!ReadyScriptFile(attachedScript, message))
						{
							Logger::log.Err("Could not invoke script file {} on {}\n{}", attachedScript, script->GetOwningObject()->GetName(), message);
						}
						LoadLuaGameObject(caller, attachedScript);
						CallVoidLuaFunction<GameObject*>(luaEventNames[eventFunc]);						
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
		void CallLuaButtonOnClickFunction(GameObject* caller, std::string eventFunc, LuaParameter params)
		{
			LoadLuaGameObject(caller, eventFunc + " (Lua)");
			sol::protected_function protectedFunc = lua[eventFunc];
			if (protectedFunc)
			{
				auto result = sol::function_result();
				result = protectedFunc(params);

				if (!result.valid())
				{
					sol::error err = result;
					Logger::log.Err("Something went wrong in Lua function: {}()", eventFunc);
					Logger::log.Err("{}", err.what());
				}
			}
		}

		void CallLuaAnimationEventFunction(GameObject* caller, std::string functionName, LuaParameterContainer paramsContainer)
		{
			Script* script = caller->Get<Script>();
			bool b_functionFound = false;

			if (script != nullptr && script->IsActive() && script->GetScripts().size())
			{
				for (ScriptData scriptData : caller->Get<Script>()->GetScripts())
				{
					std::string attachedScript = scriptData.name;

					if (luaScriptsMap.count(attachedScript))
					{							
						std::string filePath = luaScriptsMap.at(attachedScript);
						std::string message = "";
						if (!ReadyScriptFile(attachedScript, message))
						{
							Logger::log.Err("Could not invoke script file {} on {}\n{}", attachedScript, script->GetOwningObject()->GetName(), message);
						}

						LoadLuaGameObject(caller, attachedScript);

						sol::protected_function protectedFunc = lua[functionName];												
						if (protectedFunc)
						{
							b_functionFound = true;
							lua_State* L = lua.lua_state();

							protectedFunc.push(L);
							
							for (auto paramIter : paramsContainer.parameters)
							{
								sol::object arg = LuaParamToLuaObject(paramIter.second);
								arg.push(L);								
							}
							
							int status = lua_pcall(L, paramsContainer.parameters.size(), LUA_MULTRET, 0);

							if (status != LUA_OK)
							{
								std::string error = lua_tostring(L, -1);
								lua_pop(L, 1);
								Logger::log.Err("Lua call to {} failed: {}", functionName, error);								
							}							
						}							
					}
				}

				if (!b_functionFound)
				{
					Logger::log.Err("Lua call to {} failed, no function with that name found.", functionName);						
				}		
			}
			else if (script == nullptr) 
			{
				Logger::log.Err("CallLuaAnimationEventFunction() : Script component not found on GameObject: {}", caller->GetName());
			}
		}
	}
}

// Lua / Sol cheat sheet
// 
//// We can also inject variables directly into the Lua state as well
//lua["newVariable"] = 56;
//
//// We can also inject entire scripts into the Lua state that can use global functions and variables
//// This creates a new lua function called newFunction(a)
//lua.script(
//	"function newFunction(a)"
//	" return newVariable + a "
//	"end");
//
//// This then calls that function with the value of 2 passed in
//int result = lua["newFunction"](2);
//LogInt(result, "Returned Value from new function: ");

// Getting values from Lua globals
//LogInt(lua["b"].get_or(0));
//LogString(lua["a"].get_or<std::string>("0"));
//// Existence checking
//sol::optional<int> anOptionalInt = lua["b"];
//if (anOptionalInt)
//LogInt(anOptionalInt.value(), "an Optional int: ");
//// Workaround for above
//auto intVar = lua["b"].get<std::optional<int>>();
//if (intVar)
//LogInt(intVar.value(), "Optional int 2: ");

//std::optional<sol::table> playerTable = lua["player"];
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
/*sol::table playerTable2 = lua["player"];
player.name = playerTable2["Name"].get<std::string>();
player.level = playerTable2["Level"].get<int>();*/

// Calling Lua functions from C++
// 
// Unsafe, something might go wrong during execution
//float returnedValue = lua["AddStuff"](3, 4);
//LogFloat(returnedValue, "Returned value unsafe: ");	

// Safely call Lua functions
//sol::protected_function awake = lua["Awake"];
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
//lua["allObjects"] = &objects;
//
//LogInt((int)objects.size(), "Vector size before: ");
//lua["CreateAllObjects"](15); // Call a Lua function to manipulate the new allCharacters variable
//LogInt((int)objects.size(), "Vector size after: ");
//
//for (int i = 0; i < objects.size(); i++)
//{
//	LogString("Name: " + objects[i].GetName());
//}
