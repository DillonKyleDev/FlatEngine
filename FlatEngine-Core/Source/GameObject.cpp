#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Text.h"
#include "Script.h"
#include "Animation.h"
#include "Audio.h"
#include "Button.h"
#include "Canvas.h"
#include "CharacterController.h"
#include "TileMap.h"
#include "Project.h"
#include "BoxBody.h"
#include "CircleBody.h"
#include "CapsuleBody.h"
#include "PolygonBody.h"


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
		m_tagList = TagList(m_ID);
		m_parentID = newParentID;
		m_name = "GameObject(" + std::to_string(m_ID) + ")";
		m_components = std::vector<Component*>();
		m_b_isActive = true;
		m_childrenIDs = std::vector<long>();
		m_b_persistant = false;
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
			if (script != nullptr && script->GetAttachedScript() == scriptName)
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
			if (script != nullptr && script->GetAttachedScript() == scriptName)
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
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				GetLoadedProject().GetPersistantGameObjectScene()->RemoveComponent(component);
			}
			else if (GetLoadedScene() != nullptr)
			{
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
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
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
				transform.SetOrigin(parentTruePosition);
			}
		}

		Transform* transformPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			transformPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddTransform(transform, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			transformPtr = GetLoadedScene()->AddTransform(transform, m_ID);
		}

		if (transformPtr != nullptr)
		{
			m_components.push_back(transformPtr);
		}
		return transformPtr;
	}

	Sprite* GameObject::AddSprite(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		Sprite sprite = Sprite(nextID, m_ID);
		sprite.SetActive(b_active);
		sprite.SetCollapsed(b_collapsed);
		
		Sprite* spritePtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			spritePtr = GetLoadedProject().GetPersistantGameObjectScene()->AddSprite(sprite, m_ID);
		}
		else
		{
			spritePtr = GetLoadedScene()->AddSprite(sprite, m_ID);
		}

		if (spritePtr != nullptr)
		{
			m_components.push_back(spritePtr);
		}
		return spritePtr;
	}

	Camera* GameObject::AddCamera(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		Camera camera = Camera(nextID, m_ID);
		camera.SetActive(b_active);
		camera.SetCollapsed(b_collapsed);
		
		Camera* cameraPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			cameraPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddCamera(camera, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			cameraPtr = GetLoadedScene()->AddCamera(camera, m_ID);
		}

		if (cameraPtr != nullptr)
		{
			m_components.push_back(cameraPtr);
		}
		return cameraPtr;
	}

	Script* GameObject::AddScript(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		Script script = Script(nextID, m_ID);
		script.SetActive(b_active);
		script.SetCollapsed(b_collapsed);

		Script* scriptPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			scriptPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddScript(script, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			scriptPtr = GetLoadedScene()->AddScript(script, m_ID);
		}

		if (scriptPtr != nullptr)
		{
			m_components.push_back(scriptPtr);
		}
		return scriptPtr;
	}

	Button* GameObject::AddButton(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		Button button = Button(nextID, m_ID);
		button.SetActive(b_active);
		button.SetCollapsed(b_collapsed);
		
		Button* buttonPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			buttonPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddButton(button, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			buttonPtr = GetLoadedScene()->AddButton(button, m_ID);
		}

		if (buttonPtr != nullptr)
		{
			m_components.push_back(buttonPtr);
		}
		return buttonPtr;
	}

	Canvas* GameObject::AddCanvas(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		Canvas canvas = Canvas(nextID, m_ID);
		canvas.SetActive(b_active);
		canvas.SetCollapsed(b_collapsed);

		Canvas* canvasPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			canvasPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddCanvas(canvas, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			canvasPtr = GetLoadedScene()->AddCanvas(canvas, m_ID);
		}

		if (canvasPtr != nullptr)
		{
			m_components.push_back(canvasPtr);
		}
		return canvasPtr;
	}

	Animation* GameObject::AddAnimation(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		Animation animation = Animation(nextID, m_ID);
		animation.SetActive(b_active);
		animation.SetCollapsed(b_collapsed);
	
		Animation* animationPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			animationPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddAnimation(animation, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			animationPtr = GetLoadedScene()->AddAnimation(animation, m_ID);
		}

		if (animationPtr != nullptr)
		{
			m_components.push_back(animationPtr);
		}
		return animationPtr;
	}

	Audio* GameObject::AddAudio(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		Audio audio = Audio(nextID, m_ID);
		audio.SetActive(b_active);
		audio.SetCollapsed(b_collapsed);

		Audio* audioPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			audioPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddAudio(audio, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			audioPtr = GetLoadedScene()->AddAudio(audio, m_ID);
		}

		if (audioPtr != nullptr)
		{
			m_components.push_back(audioPtr);
		}
		return audioPtr;
	}

	Text* GameObject::AddText(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		Text text = Text(nextID, m_ID);
		text.SetActive(b_active);
		text.SetCollapsed(b_collapsed);
		
		Text* textPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			textPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddText(text, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			textPtr = GetLoadedScene()->AddText(text, m_ID);
		}

		if (textPtr != nullptr)
		{
			m_components.push_back(textPtr);
		}
		return textPtr;
	}

	BoxBody* GameObject::AddBoxBody(long ID, bool b_active, bool b_collapsed)
	{
		Vector2 position = GetTransform()->GetPosition();
		float rotation = GetTransform()->GetRotation();

		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		BoxBody boxBody = BoxBody(nextID, m_ID);
		boxBody.SetActive(b_active);
		boxBody.SetCollapsed(b_collapsed);
		boxBody.SetPosition(position);
		boxBody.SetRotation(rotation);

		BoxBody* boxBodyPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			boxBodyPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddBoxBody(boxBody, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			boxBodyPtr = GetLoadedScene()->AddBoxBody(boxBody, m_ID);
		}
		
		if (HasComponent("Sprite"))
		{			 
			Sprite* sprite = GetSprite();
			// Get sprite dimensions in terms of grid squares because that is the unit used by the engine
			Vector2 dimensions = Vector2(sprite->GetTextureWidth() / F_pixelsPerGridSpace, sprite->GetTextureHeight() / F_pixelsPerGridSpace);
			boxBodyPtr->SetDimensions(dimensions);
		}
		
		if (boxBodyPtr != nullptr)
		{
			m_components.push_back(boxBodyPtr);
		}

		return boxBodyPtr;
	}

	CircleBody* GameObject::AddCircleBody(long ID, bool b_active, bool b_collapsed)
	{
		Vector2 position = GetTransform()->GetPosition();		

		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		CircleBody circleBody = CircleBody(nextID, m_ID);
		circleBody.SetActive(b_active);
		circleBody.SetCollapsed(b_collapsed);
		circleBody.SetPosition(position);		

		CircleBody* circleBodyPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			circleBodyPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddCircleBody(circleBody, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			circleBodyPtr = GetLoadedScene()->AddCircleBody(circleBody, m_ID);
		}

		if (circleBodyPtr != nullptr)
		{
			m_components.push_back(circleBodyPtr);
		}

		return circleBodyPtr;
	}

	CapsuleBody* GameObject::AddCapsuleBody(long ID, bool b_active, bool b_collapsed)
	{
		Vector2 position = GetTransform()->GetPosition();
		float rotation = GetTransform()->GetRotation();

		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		CapsuleBody capsuleBody = CapsuleBody(nextID, m_ID);
		capsuleBody.SetActive(b_active);
		capsuleBody.SetCollapsed(b_collapsed);
		capsuleBody.SetPosition(position);
		capsuleBody.SetRotation(rotation);

		CapsuleBody* capsuleBodyPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			capsuleBodyPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddCapsuleBody(capsuleBody, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			capsuleBodyPtr = GetLoadedScene()->AddCapsuleBody(capsuleBody, m_ID);
		}

		if (capsuleBodyPtr != nullptr)
		{
			m_components.push_back(capsuleBodyPtr);
		}

		return capsuleBodyPtr;
	}

	PolygonBody* GameObject::AddPolygonBody(long ID, bool b_active, bool b_collapsed)
	{
		Vector2 position = GetTransform()->GetPosition();
		float rotation = GetTransform()->GetRotation();

		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		PolygonBody polygonBody = PolygonBody(nextID, m_ID);
		polygonBody.SetActive(b_active);
		polygonBody.SetCollapsed(b_collapsed);
		polygonBody.SetPosition(position);

		PolygonBody* polygonBodyPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			polygonBodyPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddPolygonBody(polygonBody, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			polygonBodyPtr = GetLoadedScene()->AddPolygonBody(polygonBody, m_ID);
		}

		if (polygonBodyPtr != nullptr)
		{
			m_components.push_back(polygonBodyPtr);
		}

		return polygonBodyPtr;
	}

	CharacterController* GameObject::AddCharacterController(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		CharacterController characterController = CharacterController(nextID, m_ID);
		characterController.SetActive(b_active);
		characterController.SetCollapsed(b_collapsed);

		CharacterController* characterControllerPtr = nullptr;
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			characterControllerPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddCharacterController(characterController, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			characterControllerPtr = GetLoadedScene()->AddCharacterController(characterController, m_ID);
		}
		
		if (characterControllerPtr != nullptr)
		{
			m_components.push_back(characterControllerPtr);
		}
		return characterControllerPtr;
	}

	TileMap* GameObject::AddTileMap(long ID, bool b_active, bool b_collapsed)
	{
		long nextID = ID;
		if (nextID == -1)
		{
			if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				nextID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextComponentID();
			}
			else if (GetLoadedScene() != nullptr)
			{
				nextID = GetLoadedScene()->GetNextComponentID();
			}
		}

		TileMap tileMap = TileMap(nextID, m_ID);
		tileMap.SetActive(b_active);
		tileMap.SetCollapsed(b_collapsed);
		TileMap* tileMapPtr = nullptr;

		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			tileMapPtr = GetLoadedProject().GetPersistantGameObjectScene()->AddTileMap(tileMap, m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			tileMapPtr = GetLoadedScene()->AddTileMap(tileMap, m_ID);
		}

		if (tileMapPtr != nullptr)
		{
			m_components.push_back(tileMapPtr);
		}
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
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetTransformByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetTransformByOwner(m_ID);
		}
		return nullptr;
	}
	Sprite* GameObject::GetSprite()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetSpriteByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetSpriteByOwner(m_ID);
		}
		return nullptr;
	}
	Camera* GameObject::GetCamera()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetCameraByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetCameraByOwner(m_ID);
		}
		return nullptr;
	}
	Animation* GameObject::GetAnimation()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetAnimationByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetAnimationByOwner(m_ID);
		}
		return nullptr;
	}
	Audio* GameObject::GetAudio()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetAudioByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetAudioByOwner(m_ID);
		}
		return nullptr;
	}
	Button* GameObject::GetButton()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetButtonByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetButtonByOwner(m_ID);
		}
		return nullptr;
	}
	Canvas* GameObject::GetCanvas()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetCanvasByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetCanvasByOwner(m_ID);
		}
		return nullptr;
	}
	std::vector<Script*> GameObject::GetScripts()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetScriptsByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetScriptsByOwner(m_ID);
		}
		return std::vector<Script*>();
	}
	Text* GameObject::GetText()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetTextByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetTextByOwner(m_ID);
		}
		return nullptr;
	}
	CharacterController* GameObject::GetCharacterController()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetCharacterControllerByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetCharacterControllerByOwner(m_ID);
		}
		return nullptr;
	}
	Body* GameObject::GetBody()
	{
		BoxBody* boxBody = GetBoxBody();
		CircleBody* circleBody = GetCircleBody();
		CapsuleBody* capsuleBody = GetCapsuleBody();
		PolygonBody* polygonBody = GetPolygonBody();

		if (boxBody != nullptr)
		{
			return boxBody;
		}
		else if (circleBody != nullptr)
		{
			return circleBody;
		}
		else if (capsuleBody != nullptr)
		{
			return capsuleBody;
		}
		else if (polygonBody != nullptr)
		{
			return polygonBody;
		}
		else
		{
			return nullptr;
		}
	}
	BoxBody* GameObject::GetBoxBody()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetBoxBodyByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetBoxBodyByOwner(m_ID);
		}
		return nullptr;
	}
	CircleBody* GameObject::GetCircleBody()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetCircleBodyByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetCircleBodyByOwner(m_ID);
		}
		return nullptr;
	}

	CapsuleBody* GameObject::GetCapsuleBody()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetCapsuleBodyByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetCapsuleBodyByOwner(m_ID);
		}
		return nullptr;
	}

	PolygonBody* GameObject::GetPolygonBody()
	{
		if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			return GetLoadedProject().GetPersistantGameObjectScene()->GetPolygonBodyByOwner(m_ID);
		}
		else if (GetLoadedScene() != nullptr)
		{
			return GetLoadedScene()->GetPolygonBodyByOwner(m_ID);
		}
		return nullptr;
	}

	//TileMap* GameObject::GetTileMap()
	//{
	//	if (m_b_persistant && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
	//	{
	//		return GetLoadedProject().GetPersistantGameObjectScene()->GetTileMapByOwner(m_ID);
	//	}
	//	else if (GetLoadedScene() != nullptr)
	//	{
	//		return GetLoadedScene()->GetTileMapByOwner(m_ID);
	//	}
	//	return nullptr;
	//}

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
			if (GetObjectByID(m_childrenIDs[i]) != nullptr && GetObjectByID(m_childrenIDs[i])->GetName() == name)
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
			if (GetObjectByID(child) != nullptr)
			{
				GetObjectByID(child)->SetActive(b_active);
			}
		}
	}

	bool GameObject::IsActive()
	{
		return m_b_isActive;
	}

	void GameObject::SetPersistant(bool b_persistant)
	{
		m_b_persistant = b_persistant;
	}

	bool GameObject::IsPersistant()
	{
		return m_b_persistant;
	}
	
	GameObject *GameObject::GetParent()
	{
		return GetObjectByID(m_parentID);
	}
}