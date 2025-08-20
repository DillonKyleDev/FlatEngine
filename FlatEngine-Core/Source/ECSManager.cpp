#include "ECSManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "TagList.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Script.h"
#include "Button.h"
#include "Canvas.h"
#include "Animation.h"
#include "Audio.h"
#include "Text.h"
#include "Body.h"
#include "Box.h"
#include "Circle.h"
#include "Capsule.h"
#include "Polygon.h"
#include "Chain.h"
#include "JointMaker.h"
#include "CharacterController.h"
#include "TileMap.h"
#include "FlatEngine.h"

namespace FL = FlatEngine;


namespace FlatEngine
{
	ECSManager::ECSManager()
	{
		m_Transforms = std::map<long, Transform>();
		m_Sprites = std::map<long, Sprite>();
		m_Cameras = std::map<long, Camera>();
		m_Scripts = std::map<long, std::map<long, Script>>();
		m_LuaScriptsByOwner = std::map<long, std::vector<std::string>>();
		m_Buttons = std::map<long, Button>();
		m_Canvases = std::map<long, Canvas>();
		m_Animations = std::map<long, Animation>();
		m_Audios = std::map<long, Audio>();
		m_Texts = std::map<long, Text>();		
		m_Bodies = std::map<long, Body>();
		m_CharacterControllers = std::map<long, CharacterController>();		
		m_TileMaps = std::map<long, TileMap>();
	}

	ECSManager::~ECSManager()
	{		
	}

	void ECSManager::Cleanup()
	{
		m_Transforms.clear();
		m_Sprites.clear();
		m_Cameras.clear();
		m_Scripts.clear();
		m_LuaScriptsByOwner.clear();
		m_Buttons.clear();
		m_Canvases.clear();
		m_Animations.clear();
		m_Audios.clear();
		m_Texts.clear();
		m_CharacterControllers.clear();		
		m_TileMaps.clear();

		for (std::map<long, Body>::iterator iterator = m_Bodies.begin(); iterator != m_Bodies.end(); iterator++)
		{
			iterator->second.Cleanup();
		}
		m_Bodies.clear();

		for (std::map<long, JointMaker>::iterator iterator = m_JointMakers.begin(); iterator != m_JointMakers.end(); iterator++)
		{
			iterator->second.Cleanup();
		}
		m_JointMakers.clear();
	}

	Transform* ECSManager::AddTransform(Transform transform, long ownerID)
	{
		m_Transforms.emplace(ownerID, transform);
		return &m_Transforms.at(ownerID);
	}

	Sprite* ECSManager::AddSprite(Sprite sprite, long ownerID)
	{
		m_Sprites.emplace(ownerID, sprite);
		return &m_Sprites.at(ownerID);
	}

	Camera* ECSManager::AddCamera(Camera camera, long ownerID)
	{		
		m_Cameras.emplace(ownerID, camera);
		return &m_Cameras.at(ownerID);
	}

	Script* ECSManager::AddScript(Script script, long ownerID)
	{
		std::map<long, Script> newMap;
		std::pair<long, Script> newPair = { script.GetID(), script };

		if (m_Scripts.count(ownerID))
		{
			m_Scripts.at(ownerID).emplace(newPair);
		}
		else
		{
			newMap.emplace(newPair);
			m_Scripts.emplace(ownerID, newMap);
		}

		return &m_Scripts.at(ownerID).at(script.GetID());
	}

	Canvas* ECSManager::AddCanvas(Canvas canvas, long ownerID)
	{
		m_Canvases.emplace(ownerID, canvas);
		return &m_Canvases.at(ownerID);
	}

	Audio* ECSManager::AddAudio(Audio audio, long ownerID)
	{
		m_Audios.emplace(ownerID, audio);
		return &m_Audios.at(ownerID);
	}

	Text* ECSManager::AddText(Text text, long ownerID)
	{
		m_Texts.emplace(ownerID, text);
		return &m_Texts.at(ownerID);
	}

	Body* ECSManager::AddBody(Body body, long ownerID)
	{
		m_Bodies.emplace(ownerID, body);
		return &m_Bodies.at(ownerID);
	}

	Animation* ECSManager::AddAnimation(Animation animation, long ownerID)
	{
		m_Animations.emplace(ownerID, animation);
		return &m_Animations.at(ownerID);
	}

	Button* ECSManager::AddButton(Button button, long ownerID)
	{
		m_Buttons.emplace(ownerID, button);
		return &m_Buttons.at(ownerID);
	}

	JointMaker* ECSManager::AddJointMaker(JointMaker jointMaker, long ownerID)
	{
		m_JointMakers.emplace(ownerID, jointMaker);
		return &m_JointMakers.at(ownerID);
	}

	CharacterController* ECSManager::AddCharacterController(CharacterController characterController, long ownerID)
	{
		m_CharacterControllers.emplace(ownerID, characterController);
		return &m_CharacterControllers.at(ownerID);
	}

	TileMap* ECSManager::AddTileMap(TileMap tileMap, long ownerID)
	{
		m_TileMaps.emplace(ownerID, tileMap);
		return &m_TileMaps.at(ownerID);
		return nullptr;
	}

	// Get Components
	Transform* ECSManager::GetTransformByOwner(long ownerID)
	{
		if (m_Transforms.count(ownerID))
		{
			return &m_Transforms.at(ownerID);
		}
		else 
		{
			return nullptr;
		}
	}

	Sprite* ECSManager::GetSpriteByOwner(long ownerID)
	{
		if (m_Sprites.count(ownerID))
		{
			return &m_Sprites.at(ownerID);
		}
		else 
		{
			return nullptr;
		}
	}

	Camera* ECSManager::GetCameraByOwner(long ownerID)
	{
		if (m_Cameras.count(ownerID))
		{
			return &m_Cameras.at(ownerID);
		}
		else
		{
			return nullptr;
		}
	}

	std::vector<Script*> ECSManager::GetScriptsByOwner(long ownerID)
	{
		std::vector<Script*> scripts = std::vector<Script*>();
		if (m_Scripts.count(ownerID))
		{			 
			for (std::map<long, Script>::iterator iter = m_Scripts.at(ownerID).begin(); iter != m_Scripts.at(ownerID).end();)
			{
				scripts.push_back(&(*iter).second);
				iter++;
			}
		}
		return scripts;
	}

	Canvas* ECSManager::GetCanvasByOwner(long ownerID)
	{
		if (m_Canvases.count(ownerID))
		{
			return &m_Canvases.at(ownerID);
		}
		else 
		{
			return nullptr;
		}
	}

	Audio* ECSManager::GetAudioByOwner(long ownerID)
	{
		if (m_Audios.count(ownerID))
		{
			return &m_Audios.at(ownerID);
		}
		else
		{
			return nullptr;
		}
	}

	Text* ECSManager::GetTextByOwner(long ownerID)
	{
		if (m_Texts.count(ownerID))
		{
			return &m_Texts.at(ownerID);
		}
		else
		{
			return nullptr;
		}
	}

	Animation* ECSManager::GetAnimationByOwner(long ownerID)
	{
		if (m_Animations.count(ownerID))
		{
			return &m_Animations.at(ownerID);
		}
		else
		{
			return nullptr;
		}
	}

	Button* ECSManager::GetButtonByOwner(long ownerID)
	{
		if (m_Buttons.count(ownerID))
		{
			return &m_Buttons.at(ownerID);
		}
		else
		{
			return nullptr;
		}
	}

	Body* ECSManager::GetBodyByOwner(long ownerID)
	{
		if (m_Bodies.count(ownerID))
		{
			return &m_Bodies.at(ownerID);
		}
		else
		{
			return nullptr;
		}
	}

	JointMaker* ECSManager::GetJointMakerByOwner(long ownerID)
	{
		if (m_JointMakers.count(ownerID))
		{
			return &m_JointMakers.at(ownerID);
		}
		else
		{
			return nullptr;
		}
	}

	CharacterController* ECSManager::GetCharacterControllerByOwner(long ownerID)
	{
		if (m_CharacterControllers.count(ownerID))
		{
			return &m_CharacterControllers.at(ownerID);
		}
		else
		{
			return nullptr;
		}
	}

	TileMap* ECSManager::GetTileMapByOwner(long ownerID)
	{
		if (m_TileMaps.count(ownerID))
		{
			return &m_TileMaps.at(ownerID);
		}
		else
		{
			return nullptr;
		}
	}
	
	bool ECSManager::RemoveComponent(Component *component, long ownerID)
	{
		if (ownerID == -1)
		{
			ownerID = component->GetParentID();
		}

		if (component->GetTypeString() == "Transform")
		{
			return RemoveTransform(ownerID);
		}
		else if (component->GetTypeString() == "Sprite")
		{
			return RemoveSprite(ownerID);
		}
		else if (component->GetTypeString() == "Camera")
		{
			return RemoveCamera(ownerID);
		}
		else if (component->GetTypeString() == "Script")
		{
			std::string attachedScript = static_cast<Script*>(component)->GetAttachedScript();
			
			if (attachedScript.find("Lua") != std::string::npos && attachedScript != "")
			{
				attachedScript = attachedScript.substr(0, attachedScript.size() - 6);
				F_Lua[attachedScript][ownerID] = "nil";
			}

			return RemoveScript(ownerID, component->GetID());
		}
		else if (component->GetTypeString() == "Button")
		{
			return RemoveButton(ownerID);
		}
		else if (component->GetTypeString() == "Canvas")
		{
			return RemoveCanvas(ownerID);
		}
		else if (component->GetTypeString() == "Audio")
		{
			return RemoveAudio(ownerID);
		}
		else if (component->GetTypeString() == "Text")
		{
			return RemoveText(ownerID);
		}
		else if (component->GetTypeString() == "Animation")
		{
			return RemoveAnimation(ownerID);
		}
		else if (component->GetTypeString() == "Body")
		{
			return RemoveBody(ownerID);
		}
		else if (component->GetTypeString() == "CharacterController")
		{
			return RemoveCharacterController(ownerID);
		}
		else if (component->GetTypeString() == "TileMap")
		{
			return RemoveTileMap(ownerID);
		}
		else
		{
			return false;
		}
	}

	bool ECSManager::RemoveTransform(long ownerID)
	{
		bool b_success = false;
		if (m_Transforms.count(ownerID))
		{
			m_Transforms.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveSprite(long ownerID)
	{
		bool b_success = false;
		if (m_Sprites.count(ownerID))
		{			
			m_Sprites.erase(ownerID);			
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveCamera(long ownerID)
	{
		bool b_success = false;
		if (m_Cameras.count(ownerID))
		{
			if (m_Cameras.at(ownerID).IsPrimary())
			{
				RemovePrimaryCamera();
			}
			m_Cameras.erase(ownerID);			
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveScript(long ownerID, long scriptID)
	{
		bool b_success = false;
		if (m_Scripts.count(ownerID))
		{
			if (m_Scripts.at(ownerID).count(scriptID))
			{
				// TODO: Remove Lua script from F_Lua state also			
				m_Scripts.at(ownerID).erase(scriptID);
			}
		}

		return b_success;
	}

	bool ECSManager::RemoveCanvas(long ownerID)
	{
		bool b_success = false;
		if (m_Canvases.count(ownerID))
		{
			m_Canvases.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveAudio(long ownerID)
	{
		bool b_success = false;
		if (m_Audios.count(ownerID))
		{
			m_Audios.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveText(long ownerID)
	{
		bool b_success = false;
		if (m_Texts.count(ownerID))
		{
			m_Texts.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveBody(long ownerID)
	{
		bool b_success = false;
		if (m_Bodies.count(ownerID))
		{
			m_Bodies.at(ownerID).Cleanup();
			m_Bodies.erase(ownerID);
			b_success = true;							
		}
		return b_success;
	}

	bool ECSManager::RemoveAnimation(long ownerID)
	{
		bool b_success = false;
		if (m_Animations.count(ownerID))
		{
			m_Animations.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveButton(long ownerID)
	{
		bool b_success = false;
		if (m_Buttons.count(ownerID))
		{
			m_Buttons.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveJointMaker(long ownerID)
	{
		bool b_success = false;
		if (m_JointMakers.count(ownerID))
		{
			m_JointMakers.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveCharacterController(long ownerID)
	{
		bool b_success = false;
		if (m_CharacterControllers.count(ownerID))
		{
			m_CharacterControllers.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	bool ECSManager::RemoveTileMap(long ownerID)
	{
		bool b_success = false;
		if (m_TileMaps.count(ownerID))
		{
			m_TileMaps.erase(ownerID);
			b_success = true;
		}
		return b_success;
	}

	std::map<long, Transform> &ECSManager::GetTransforms()
	{
		return m_Transforms;
	}
	std::map<long, Sprite> &ECSManager::GetSprites()
	{
		return m_Sprites;
	}
	std::map<long, Camera> &ECSManager::GetCameras()
	{
		return m_Cameras;
	}
	std::map<long, std::map<long, Script>> &ECSManager::GetScripts()
	{
		return m_Scripts;
	}
	std::map<long, std::vector<std::string>> &ECSManager::GetLuaScriptsByOwner()
	{
		return m_LuaScriptsByOwner;
	}
	std::map<long, Button> &ECSManager::GetButtons()
	{
		return m_Buttons;
	}
	std::map<long, Canvas> &ECSManager::GetCanvases()
	{
		return m_Canvases;
	}
	std::map<long, Animation> &ECSManager::GetAnimations()
	{
		return m_Animations;
	}
	std::map<long, Audio> &ECSManager::GetAudios()
	{
		return m_Audios;
	}
	std::map<long, Text> &ECSManager::GetTexts()
	{
		return m_Texts;
	}
	std::map<long, Body>& ECSManager::GetBodies()
	{
		return m_Bodies;
	}
	std::map<long, JointMaker>& ECSManager::GetJointMakers()
	{
		return m_JointMakers;
	}
	std::map<long, CharacterController> &ECSManager::GetCharacterControllers()
	{
		return m_CharacterControllers;
	}
	std::map<long, TileMap>& ECSManager::GetTileMaps()
	{
		return m_TileMaps;
	}
}