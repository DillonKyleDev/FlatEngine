#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Text.h"
#include "Script.h"
#include "Text.h"
#include "Animation.h"
#include "Audio.h"
#include "Button.h"
#include "Canvas.h"
#include "CharacterController.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "CompositeCollider.h"
#include "TileMap.h"


namespace FlatEngine
{
	GameObject::GameObject(long newParentID, long myID)
	{
		if (myID == -1)
		{
			m_ID = GetNextGameObjectID();
		}
		else
		{
			m_ID = myID;
		}
		m_b_isPrefab = false;
		m_prefabName = "";
		m_prefabSpawnLocation = Vector2();
		m_tagList = TagList();
		m_parentID = newParentID;
		m_name = "GameObject(" + std::to_string(m_ID) + ")";
		m_components = std::vector<Component*>();
		m_b_isActive = true;
		m_childrenIDs = std::vector<long>();
	}

	GameObject::~GameObject()
	{
	}

	void GameObject::SetIsPrefab(bool b_isPrefab)
	{
		m_b_isPrefab = b_isPrefab;
	}

	bool GameObject::IsPrefab()
	{
		return m_b_isPrefab;
	}

	void GameObject::SetPrefabName(std::string prefabName)
	{
		m_prefabName = prefabName;
	}

	std::string GameObject::GetPrefabName()
	{
		return m_prefabName;
	}

	void GameObject::SetPrefabSpawnLocation(Vector2 spawnLocation)
	{
		m_prefabSpawnLocation = spawnLocation;
	}

	Vector2 GameObject::GetPrefabSpawnLocation()
	{
		return m_prefabSpawnLocation;
	}

	void GameObject::SetID(long newID)
	{
		m_ID = newID;
	}

	long GameObject::GetID()
	{
		return m_ID;
	}

	void GameObject::SetName(std::string name)
	{
		m_name = name;
	}

	std::string GameObject::GetName()
	{
		return m_name;
	}

	TagList& GameObject::GetTagList()
	{
		return m_tagList;
	}

	void GameObject::SetTagList(TagList tagList)
	{
		m_tagList = tagList;
	}

	bool GameObject::HasTag(std::string tagName)
	{
		return m_tagList.HasTag(tagName);
	}

	void GameObject::SetTag(std::string tagName, bool b_hasTag)
	{
		m_tagList.SetTag(tagName, b_hasTag);
	}

	void GameObject::SetIgnore(std::string tagName, bool b_hasTag)
	{
		m_tagList.SetIgnore(tagName, b_hasTag);
	}

	bool GameObject::HasScript(std::string scriptName)
	{
		bool b_hasScript = false;

		for (Script* script : GetScripts())
		{
			if (script->GetAttachedScript() == scriptName)
			{
				b_hasScript = true;
			}
		}

		return b_hasScript;
	}

	Script* GameObject::GetScript(std::string scriptName)
	{
		for (Script* script : GetScripts())
		{
			if (script->GetAttachedScript() == scriptName)
			{
				return script;
			}
		}

		return nullptr;
	}

	void GameObject::RemoveComponent(Component* component)
	{
		if (component != nullptr)
		{
			// Remove from ECSManager
			GetLoadedScene()->RemoveComponent(component);
		}

		for (std::vector<Component*>::iterator iter = m_components.begin(); iter != m_components.end();)
		{
			if ((*iter)->GetID() == component->GetID())
			{
				// Remove GameObject ptr to it
				m_components.erase(iter);
				break;
			}
			iter++;
		}
	}

	void GameObject::DeleteComponents()
	{
		m_components.clear();
	}

	Transform* GameObject::AddTransform(long id, bool b_active, bool b_collapsed)
	{
		long nextID = id;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		Transform transform = Transform(nextID, m_ID);
		transform.SetActive(b_active);
		transform.SetCollapsed(b_collapsed);

		// Set transforms origin to parents true position
		if (m_parentID != -1)
		{
			GameObject *parent = GetObjectByID(m_parentID);
			if (parent != nullptr && parent->HasComponent("Transform"))
			{
				Transform* parentTransform = parent->GetTransform();
				Vector2 parentTruePosition = parentTransform->GetTruePosition();
				//transform.SetBaseScale(parentTransform->GetTotalScale());
				transform.SetOrigin(parentTruePosition);
			}
		}

		Transform* transformPtr = nullptr;
		transformPtr = GetLoadedScene()->AddTransform(transform, m_ID);
		m_components.push_back(transformPtr);
		return transformPtr;
	}

	Sprite* GameObject::AddSprite(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		Sprite sprite = Sprite(nextID, m_ID);
		sprite.SetActive(b_active);
		sprite.SetCollapsed(b_collapsed);
		
		Sprite* spritePtr;		
		spritePtr = GetLoadedScene()->AddSprite(sprite, m_ID);
		m_components.push_back(spritePtr);
		return spritePtr;
	}

	Camera* GameObject::AddCamera(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		Camera camera = Camera(nextID, m_ID);
		camera.SetActive(b_active);
		camera.SetCollapsed(b_collapsed);
		
		Camera* cameraPtr = GetLoadedScene()->AddCamera(camera, m_ID);
		m_components.push_back(cameraPtr);
		return cameraPtr;
	}

	Script* GameObject::AddScript(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		Script script = Script(nextID, m_ID);
		script.SetActive(b_active);
		script.SetCollapsed(b_collapsed);

		Script* scriptPtr = GetLoadedScene()->AddScript(script, m_ID);
		m_components.push_back(scriptPtr);

		return scriptPtr;
	}

	Button* GameObject::AddButton(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		Button button = Button(nextID, m_ID);
		button.SetActive(b_active);
		button.SetCollapsed(b_collapsed);
		
		Button* buttonPtr = GetLoadedScene()->AddButton(button, m_ID);
		m_components.push_back(buttonPtr);
		return buttonPtr;
	}

	Canvas* GameObject::AddCanvas(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		Canvas canvas = Canvas(nextID, m_ID);
		canvas.SetActive(b_active);
		canvas.SetCollapsed(b_collapsed);

		Canvas* canvasPtr = GetLoadedScene()->AddCanvas(canvas, m_ID);
		m_components.push_back(canvasPtr);
		return canvasPtr;
	}

	Animation* GameObject::AddAnimation(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		Animation animation = Animation(nextID, m_ID);
		animation.SetActive(b_active);
		animation.SetCollapsed(b_collapsed);
	
		Animation* animationPtr = GetLoadedScene()->AddAnimation(animation, m_ID);
		m_components.push_back(animationPtr);
		return animationPtr;
	}

	Audio* GameObject::AddAudio(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		Audio audio = Audio(nextID, m_ID);
		audio.SetActive(b_active);
		audio.SetCollapsed(b_collapsed);

		Audio* audioPtr = GetLoadedScene()->AddAudio(audio, m_ID);
		m_components.push_back(audioPtr);
		return audioPtr;
	}

	Text* GameObject::AddText(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		Text text = Text(nextID, m_ID);
		text.SetActive(b_active);
		text.SetCollapsed(b_collapsed);
		
		Text* textPtr = GetLoadedScene()->AddText(text, m_ID);
		m_components.push_back(textPtr);
		return textPtr;
	}

	BoxCollider* GameObject::AddBoxCollider(long ID, bool b_active, bool b_collapsed)
	{
		Vector2 dimensions = Vector2(0,0);
		if (HasComponent("Sprite"))
		{
			Sprite* sprite = GetSprite();
			// Get sprite dimensions in terms of grid squares because that is the unit used by the engine
			dimensions = Vector2(sprite->GetTextureWidth() / F_pixelsPerGridSpace, sprite->GetTextureHeight() / F_pixelsPerGridSpace);
		}

		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		BoxCollider boxCollider = BoxCollider(nextID, m_ID);
		boxCollider.SetActive(b_active);
		boxCollider.SetCollapsed(b_collapsed);

		BoxCollider* colliderPtr;
		colliderPtr = GetLoadedScene()->AddBoxCollider(boxCollider, m_ID);
		if (dimensions.x != 0 && dimensions.y != 0)
		{
			colliderPtr->SetActiveDimensions(dimensions.x, dimensions.y);
		}
		m_components.push_back(colliderPtr);
		return colliderPtr;
	}

	CircleCollider* GameObject::AddCircleCollider(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		CircleCollider circleCollider = CircleCollider(nextID, m_ID);
		circleCollider.SetActive(b_active);
		circleCollider.SetCollapsed(b_collapsed);

		CircleCollider* colliderPtr = GetLoadedScene()->AddCircleCollider(circleCollider, m_ID);
		m_components.push_back(colliderPtr);
		return colliderPtr;
	}

	CompositeCollider* GameObject::AddCompositeCollider(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		CompositeCollider compositeCollider = CompositeCollider(nextID, m_ID);
		compositeCollider.SetActive(b_active);
		compositeCollider.SetCollapsed(b_collapsed);

		CompositeCollider* colliderPtr = GetLoadedScene()->AddCompositeCollider(compositeCollider, m_ID);
		m_components.push_back(colliderPtr);
		return colliderPtr;
	}

	RigidBody* GameObject::AddRigidBody(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		RigidBody rigidBody = RigidBody(nextID, m_ID);
		rigidBody.SetActive(b_active);
		rigidBody.SetCollapsed(b_collapsed);

		RigidBody* rigidBodyPtr = GetLoadedScene()->AddRigidBody(rigidBody, m_ID);
		m_components.push_back(rigidBodyPtr);
		return rigidBodyPtr;
	}

	CharacterController* GameObject::AddCharacterController(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		CharacterController characterController = CharacterController(nextID, m_ID);
		characterController.SetActive(b_active);
		characterController.SetCollapsed(b_collapsed);

		CharacterController* characterControllerPtr = GetLoadedScene()->AddCharacterController(characterController, m_ID);
		m_components.push_back(characterControllerPtr);
		return characterControllerPtr;
	}

	TileMap* GameObject::AddTileMap(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			nextID = GetLoadedScene()->GetNextComponentID();
		}

		TileMap tileMap = TileMap(nextID, m_ID);
		tileMap.SetActive(b_active);
		tileMap.SetCollapsed(b_collapsed);

		TileMap* tileMapPtr = GetLoadedScene()->AddTileMap(tileMap, m_ID);
		m_components.push_back(tileMapPtr);
		return tileMapPtr;
	}


	Component* GameObject::GetComponent(ComponentTypes type)
	{
		for (int i = 0; i < m_components.size(); i++)
		{
			if (m_components[i]->GetType() == type)
			{
				return m_components[i];
			}
		}

		return nullptr;
	}

	bool GameObject::HasComponent(ComponentTypes type)
	{
		return GetComponent(type) != nullptr;
	}

	bool GameObject::HasComponent(std::string type)
	{
		for (Component* component : m_components)
		{
			if (component->GetTypeString() == type)
			{
				return true;
			}
		}
		return false;
	}

	Transform* GameObject::GetTransform()
	{
		return GetLoadedScene()->GetTransformByOwner(m_ID);
	}
	Sprite* GameObject::GetSprite()
	{
		return GetLoadedScene()->GetSpriteByOwner(m_ID);
	}
	Camera* GameObject::GetCamera()
	{
		return GetLoadedScene()->GetCameraByOwner(m_ID);
	}
	Animation* GameObject::GetAnimation()
	{
		return GetLoadedScene()->GetAnimationByOwner(m_ID);
	}
	Audio* GameObject::GetAudio()
	{
		return GetLoadedScene()->GetAudioByOwner(m_ID);
	}
	Button* GameObject::GetButton()
	{
		return GetLoadedScene()->GetButtonByOwner(m_ID);
	}
	Canvas* GameObject::GetCanvas()
	{
		return GetLoadedScene()->GetCanvasByOwner(m_ID);
	}
	std::vector<Script*> GameObject::GetScripts()
	{
		return GetLoadedScene()->GetScriptsByOwner(m_ID);
	}
	Text* GameObject::GetText()
	{
		return GetLoadedScene()->GetTextByOwner(m_ID);
	}
	CharacterController* GameObject::GetCharacterController()
	{
		return GetLoadedScene()->GetCharacterControllerByOwner(m_ID);
	}
	RigidBody* GameObject::GetRigidBody()
	{
		return GetLoadedScene()->GetRigidBodyByOwner(m_ID);
	}
	std::vector<BoxCollider*> GameObject::GetBoxColliders()
	{
		return GetLoadedScene()->GetBoxCollidersByOwner(m_ID);
	}
	BoxCollider* GameObject::GetBoxCollider()
	{
		std::vector<BoxCollider*> colliders = GetBoxColliders();
		
		if (colliders.size() > 0)
		{
			return colliders[0];
		}
		else
		{
			return nullptr;
		}
	}
	std::vector<CircleCollider*> GameObject::GetCircleColliders()
	{
		return GetLoadedScene()->GetCircleCollidersByOwner(m_ID);
	}
	CompositeCollider* GameObject::GetCompositeCollider()
	{
		return GetLoadedScene()->GetCompositeColliderByOwner(m_ID);
	}
	TileMap* GameObject::GetTileMap()
	{
		return GetLoadedScene()->GetTileMapByOwner(m_ID);
	}

	std::vector<Component*> GameObject::GetComponents()
	{
		return m_components;
	}

	void GameObject::SetParentID(long newParentID)
	{
		m_parentID = newParentID;
	}

	long GameObject::GetParentID()
	{
		return m_parentID;
	}

	void GameObject::AddChild(long childID)
	{
		if (childID != -1)
		{
			bool b_contains = false;

			for (long ID : m_childrenIDs)
			{
				if (ID == childID)
				{
					b_contains = true;
				}
			}
			if (!b_contains)
			{
				m_childrenIDs.push_back(childID);
			}
		}
	}

	void GameObject::RemoveChild(long childID)
	{
		for (int i = 0; i < m_childrenIDs.size(); i++)
		{
			if (m_childrenIDs[i] == childID)
			{
				m_childrenIDs.erase(m_childrenIDs.begin() + i);
			}
		}
	}

	GameObject *GameObject::GetFirstChild()
	{
		return GetObjectByID(m_childrenIDs[0]);
	}

	GameObject *GameObject::FindChildByName(std::string name)
	{
		for (int i = 0; i < m_childrenIDs.size(); i++)
		{
			if (GetObjectByID(m_childrenIDs[i])->GetName() == name)
			{
				return GetObjectByID(m_childrenIDs[i]);
			}
		}

		return nullptr;
	}

	std::vector<long> GameObject::GetChildren()
	{
		return m_childrenIDs;
	}

	bool GameObject::HasChildren()
	{
		return m_childrenIDs.size() > 0;
	}

	void GameObject::SetActive(bool b_active)
	{
		m_b_isActive = b_active;

		for (long child : GetChildren())
		{
			GetObjectByID(child)->SetActive(b_active);
		}
	}

	bool GameObject::IsActive()
	{
		return m_b_isActive;
	}
	
	GameObject *GameObject::GetParent()
	{
		return GetObjectByID(m_parentID);
	}
}