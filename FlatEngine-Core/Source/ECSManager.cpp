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
#include "Collider.h"
#include "CompositeCollider.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
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
		m_CompositeColliders = std::map<long, CompositeCollider>();
		m_BoxColliders = std::map<long, std::map<long, BoxCollider>>();
		m_CircleColliders = std::map<long, std::map<long, CircleCollider>>();
		m_RigidBodies = std::map<long, RigidBody>();
		m_CharacterControllers = std::map<long, CharacterController>();

		m_ColliderPairs = std::vector<std::pair<Collider*, Collider*>>();
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
		m_CompositeColliders.clear();
		m_BoxColliders.clear();
		m_CircleColliders.clear();
		m_RigidBodies.clear();
		m_CharacterControllers.clear();
		m_TileMaps.clear();
		m_ColliderPairs.clear();
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

	CompositeCollider* ECSManager::AddCompositeCollider(CompositeCollider collider, long ownerID)
	{
		m_CompositeColliders.emplace(ownerID, collider);
		return &m_CompositeColliders.at(ownerID);
	}

	BoxCollider* ECSManager::AddBoxCollider(BoxCollider collider, long ownerID)
	{
		std::pair<long, BoxCollider> newPair = { collider.GetID(), collider };

		if (m_BoxColliders.count(ownerID))
		{
			m_BoxColliders.at(ownerID).emplace(newPair);
		}
		else
		{
			std::map<long, BoxCollider> newMap;
			newMap.emplace(newPair);
			m_BoxColliders.emplace(ownerID, newMap);
		}

		// Update m_ColliderPairs
		if (GetObjectByID(ownerID) != nullptr) // If BoxCollider added to object, but object not yet added to Scene, (will be caught in Scene::AddSceneObject())
		{
			UpdateColliderPairs();
			FL::UpdateColliderPairs();
		}

		return &m_BoxColliders.at(ownerID).at(collider.GetID());
	}

	CircleCollider* ECSManager::AddCircleCollider(CircleCollider collider, long ownerID)
	{
		std::pair<long, CircleCollider> newPair = { collider.GetID(), collider };

		if (m_CircleColliders.count(ownerID))
		{
			m_CircleColliders.at(ownerID).emplace(newPair);
		}
		else
		{
			std::map<long, CircleCollider> newMap;
			newMap.emplace(newPair);
			m_CircleColliders.emplace(ownerID, newMap);
		}

		// Update m_ColliderPairs
		if (GetObjectByID(ownerID) != nullptr) // If CircleCollider added to object, but object not yet added to Scene, (will be caught in Scene::AddSceneObject())
		{
			UpdateColliderPairs();
			FL::UpdateColliderPairs();
		}

		return &m_CircleColliders.at(ownerID).at(collider.GetID());
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

	RigidBody* ECSManager::AddRigidBody(RigidBody rigidBody, long ownerID)
	{
		m_RigidBodies.emplace(ownerID, rigidBody);
		return &m_RigidBodies.at(ownerID);
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
	}

	// Get Components
	Transform* ECSManager::GetTransformByOwner(long ownerID)
	{
		if (m_Transforms.count(ownerID))
			return &m_Transforms.at(ownerID);
		else return nullptr;
	}

	Sprite* ECSManager::GetSpriteByOwner(long ownerID)
	{
		if (m_Sprites.count(ownerID))
			return &m_Sprites.at(ownerID);
		else return nullptr;
	}

	Camera* ECSManager::GetCameraByOwner(long ownerID)
	{
		if (m_Cameras.count(ownerID))
			return &m_Cameras.at(ownerID);
		else return nullptr;
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
			return &m_Canvases.at(ownerID);
		else return nullptr;
	}

	Audio* ECSManager::GetAudioByOwner(long ownerID)
	{
		if (m_Audios.count(ownerID))
			return &m_Audios.at(ownerID);
		else return nullptr;
	}

	Text* ECSManager::GetTextByOwner(long ownerID)
	{
		if (m_Texts.count(ownerID))
			return &m_Texts.at(ownerID);
		else return nullptr;
	}

	CompositeCollider* ECSManager::GetCompositeColliderByOwner(long ownerID)
	{
		if (m_CompositeColliders.count(ownerID))
			return &m_CompositeColliders.at(ownerID);
		else return nullptr;
	}

	std::vector<BoxCollider*> ECSManager::GetBoxCollidersByOwner(long ownerID)
	{
		std::vector<BoxCollider*> colliders = std::vector<BoxCollider*>();
		if (m_BoxColliders.count(ownerID))
		{
			for (std::map<long, BoxCollider>::iterator iter = m_BoxColliders.at(ownerID).begin(); iter != m_BoxColliders.at(ownerID).end();)
			{
				colliders.push_back(&(*iter).second);
				iter++;
			}
		}
		return colliders;
	}

	std::vector<CircleCollider*> ECSManager::GetCircleCollidersByOwner(long ownerID)
	{
		std::vector<CircleCollider*> colliders = std::vector<CircleCollider*>();
		if (m_CircleColliders.count(ownerID))
		{
			for (std::map<long, CircleCollider>::iterator iter = m_CircleColliders.at(ownerID).begin(); iter != m_CircleColliders.at(ownerID).end();)
			{
				colliders.push_back(&(*iter).second);
				iter++;
			}
		}
		return colliders;
	}

	Animation* ECSManager::GetAnimationByOwner(long ownerID)
	{
		if (m_Animations.count(ownerID))
			return &m_Animations.at(ownerID);
		else return nullptr;
	}

	Button* ECSManager::GetButtonByOwner(long ownerID)
	{
		if (m_Buttons.count(ownerID))
			return &m_Buttons.at(ownerID);
		else return nullptr;
	}

	RigidBody* ECSManager::GetRigidBodyByOwner(long ownerID)
	{
		if (m_RigidBodies.count(ownerID))
			return &m_RigidBodies.at(ownerID);
		else return nullptr;
	}

	CharacterController* ECSManager::GetCharacterControllerByOwner(long ownerID)
	{
		if (m_CharacterControllers.count(ownerID))
			return &m_CharacterControllers.at(ownerID);
		else return nullptr;
	}

	TileMap* ECSManager::GetTileMapByOwner(long ownerID)
	{
		if (m_TileMaps.count(ownerID))
			return &m_TileMaps.at(ownerID);
		else return nullptr;
	}

	// Remove Components
	bool ECSManager::RemoveComponent(Component *component)
	{
		long ownerID = component->GetParentID();
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
			F_Lua[static_cast<Script*>(component)->GetAttachedScript()][ownerID] = "nil";
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
		else if (component->GetTypeString() == "CompositeCollider")
		{
			return RemoveCompositeCollider(ownerID);
		}
		else if (component->GetTypeString() == "BoxCollider")
		{
			return RemoveBoxCollider(component->GetID(), ownerID);
		}
		else if (component->GetTypeString() == "CircleCollider")
		{
			return RemoveCircleCollider(component->GetID(), ownerID);
		}
		else if (component->GetTypeString() == "RigidBody")
		{
			return RemoveRigidBody(ownerID);
		}
		else if (component->GetTypeString() == "CharacterController")
		{
			return RemoveCharacterController(ownerID);
		}
		else if (component->GetTypeString() == "TileMap")
		{
			TileMap* tileMap = static_cast<TileMap*>(component);
			if (tileMap->GetCollisionAreas().size() > 0)
			{
				for (std::pair<std::string, std::vector<CollisionAreaData>> collisionArea : tileMap->GetCollisionAreas())
				{
					for (CollisionAreaData collData : collisionArea.second)
					{
						RemoveBoxCollider(collData.collider->GetID(), ownerID);
					}
				}
			}
			return RemoveTileMap(ownerID);
		}
		else
			return false;
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

	bool ECSManager::RemoveCompositeCollider(long ownerID)
	{
		bool b_success = false;
		if (m_CompositeColliders.count(ownerID))
		{
			m_CompositeColliders.erase(ownerID);
			b_success = true;
			UpdateColliderPairs();
			FL::UpdateColliderPairs();
		}
		return b_success;
	}

	bool ECSManager::RemoveBoxCollider(long componentID, long ownerID)
	{
		bool b_success = false;
		if (m_BoxColliders.count(ownerID))
		{
			if (m_BoxColliders.at(ownerID).count(componentID))
			{
				m_BoxColliders.at(ownerID).erase(componentID);
				b_success = true;
				UpdateColliderPairs();
				FL::UpdateColliderPairs();
			}
		}
		return b_success;
	}

	bool ECSManager::RemoveCircleCollider(long componentID, long ownerID)
	{
		bool b_success = false;
		if (m_CircleColliders.count(ownerID))
		{
			if (m_CircleColliders.at(ownerID).count(componentID))
			{
				m_CircleColliders.at(ownerID).erase(componentID);
				b_success = true;
				UpdateColliderPairs();
				FL::UpdateColliderPairs();
			}
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

	bool ECSManager::RemoveRigidBody(long ownerID)
	{
		bool b_success = false;
		if (m_RigidBodies.count(ownerID))
		{
			m_RigidBodies.erase(ownerID);
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

	void ECSManager::UpdateColliderPairs()
	{
		// Remake colliderPairs
		m_ColliderPairs.clear();
		std::vector<BoxCollider*> colliders;

		// Collect BoxColliders into a simple to navigate vector
		for (std::map<long, std::map<long, BoxCollider>>::iterator colliderMap = m_BoxColliders.begin(); colliderMap != m_BoxColliders.end();)
		{
			for (std::map<long, BoxCollider>::iterator innerMap = colliderMap->second.begin(); innerMap != colliderMap->second.end();)
			{
				colliders.push_back(&innerMap->second);
				innerMap++;
			}
			colliderMap++;
		}
	
		int colliderCounter = 1;
		for (std::vector<BoxCollider*>::iterator collider1 = colliders.begin(); collider1 != colliders.end(); collider1++)
		{
			for (std::vector<BoxCollider*>::iterator collider2 = collider1 + colliderCounter; collider2 != colliders.end(); collider2++)
			{
				if ((*collider1)->GetParentID() != (*collider2)->GetParentID())
				{
					TagList coll1TagList = (*collider1)->GetParent()->GetTagList();
					TagList coll2TagList = (*collider2)->GetParent()->GetTagList();

					std::vector<std::string> coll1Ignored = coll1TagList.GetIgnoredTags();
					std::vector<std::string> coll2Ignored = coll2TagList.GetIgnoredTags();

					bool _ignorePair = false;

					for (std::string ignoredTag : coll1Ignored)
					{
						if (coll2TagList.HasTag(ignoredTag))
						{
							_ignorePair = true;
							break;
						}
					}
					if (!_ignorePair)
					{
						for (std::string ignoredTag : coll2Ignored)
						{
							if (coll1TagList.HasTag(ignoredTag))
							{
								_ignorePair = true;
								break;
							}
						}
					}
					if (!_ignorePair)
					{
						std::pair<Collider*, Collider*> newPair = { (*collider1), (*collider2) };
						m_ColliderPairs.push_back(newPair);
					}
				}
			}
		}
	}

	std::vector<std::pair<Collider*, Collider*>> ECSManager::GetColliderPairs()
	{
		return m_ColliderPairs;
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
	std::vector<Collider*> ECSManager::GetColliders()
	{
		// Collect all colliders when needed so we don't have to keep an member variable updated
		std::vector<Collider*> colliders;
		for (std::pair<long, CompositeCollider> collider : m_CompositeColliders)
		{
			colliders.push_back(&collider.second);
		}
		for (std::pair<long, std::map<long, BoxCollider>> colliderMap : m_BoxColliders)
		{
			for (std::pair<long, BoxCollider> collider : colliderMap.second)
			{
				colliders.push_back(&collider.second);
			}
		}
		for (std::pair<long, std::map<long, CircleCollider>> colliderMap : m_CircleColliders)
		{
			for (std::pair<long, CircleCollider> collider : colliderMap.second)
			{
				colliders.push_back(&collider.second);
			}
		}
		return colliders;
	}
	std::map<long, CompositeCollider> &ECSManager::GetCompositeColliders()
	{
		return m_CompositeColliders;
	}
	std::map<long, std::map<long, BoxCollider>> &ECSManager::GetBoxColliders()
	{
		return m_BoxColliders;
	}
	std::map<long, std::map<long, CircleCollider>> &ECSManager::GetCircleColliders()
	{
		return m_CircleColliders;
	}
	std::map<long, RigidBody> &ECSManager::GetRigidBodies()
	{
		return m_RigidBodies;
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