#pragma once
#include "ECSManager.h"
#include "GameObject.h"
#include "TileMap.h"
#include "Button.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Canvas.h"
#include "Script.h"
#include "Audio.h"
#include "Text.h"
#include "CharacterController.h"
#include "Body.h"
#include "Box.h"
#include "Circle.h"
#include "Capsule.h"
#include "Polygon.h"
#include "Chain.h"
#include "JointMaker.h"

#include <vector>
#include <map>
#include <string>


namespace FlatEngine
{
	class Scene
	{
		friend class SceneManager;
	public:
		Scene();
		~Scene();

		static bool SortHierarchyObjects(GameObject* gameObjectA, GameObject* gameObjectB);

		void SetName(std::string name);
		std::string GetName();
		void SetPath(std::string path);
		std::string GetPath();
		void UnloadECSManager();
		void UnloadSceneObjects();
		GameObject* AddSceneObject(GameObject sceneObject);
		void KeepNextGameObjectIDUpToDate(long id);
		std::map<long, GameObject> &GetSceneObjects();
		void SetAnimatorPreviewObjects(std::vector<GameObject*> previewObjects);
		std::vector<GameObject*> GetAnimatorPreviewObjects();
		GameObject* GetObjectByID(long ID);
		GameObject* GetObjectByName(std::string name);
		GameObject* GetObjectByTag(std::string tag);
		GameObject* CreateGameObject(long parentID = -1, long myID = -1);
		void DeleteGameObject(long sceneObjectID);
		void DeleteGameObject(GameObject* objectToDelete);
		void DeleteChildrenAndSelf(GameObject *objectToDelete);
		void IncrementGameObjectID();
		void SetNextGameObjectID(long nextID);
		long GetNextGameObjectID();
		void IncrementComponentID();
		void SetNextComponentID(long nextID);
		long GetNextComponentID();
		void OnPrefabInstantiated();
		void SortSceneObjects();
		std::vector<GameObject*> GetSortedHierarchyObjects();
		void CreateJoints();

		// ECS Wrappers
		void KeepNextComponentIDUpToDate(long ID);
		Transform* AddTransform(Transform transform, long ownerID);
		Sprite* AddSprite(Sprite sprite, long ownerID);
		Camera* AddCamera(Camera camera, long ownerID);
		Script* AddScript(Script script, long ownerID);
		Canvas* AddCanvas(Canvas canvas, long ownerID);
		Audio* AddAudio(Audio audio, long ownerID);
		Text* AddText(Text text, long ownerID);		
		Body* AddBody(Body boxBody, long ownerID);
		Animation* AddAnimation(Animation animation, long ownerID);
		Button* AddButton(Button button, long ownerID);
		JointMaker* AddJointMaker(JointMaker jointMaker, long ownerID);
		CharacterController* AddCharacterController(CharacterController characterController, long ownerID);
		TileMap* AddTileMap(TileMap tileMap, long ownerID);

		void RemoveComponent(Component* component, long ownerID = -1);

		Transform* GetTransformByOwner(long ownerID);
		Sprite* GetSpriteByOwner(long ownerID);
		Camera* GetCameraByOwner(long ownerID);
		std::vector<Script*> GetScriptsByOwner(long ownerID);
		Canvas* GetCanvasByOwner(long ownerID);
		Audio* GetAudioByOwner(long ownerID);
		Text* GetTextByOwner(long ownerID);
		Animation* GetAnimationByOwner(long ownerID);
		Button* GetButtonByOwner(long ownerID);
		Body* GetBodyByOwner(long ownerID);
		JointMaker* GetJointMakerByOwner(long ownerID);
		CharacterController* GetCharacterControllerByOwner(long ownerID);
		TileMap* GetTileMapByOwner(long ownerID);

		std::map<long, Transform> &GetTransforms();
		std::map<long, Sprite>& GetSprites();
		std::map<long, Camera>& GetCameras();
		std::map<long, std::map<long, Script>>& GetScripts();
		std::map<long, std::vector<std::string>> &GetLuaScriptsByOwner();
		std::map<long, Button>& GetButtons();
		std::map<long, Canvas>& GetCanvases();
		std::map<long, Animation>& GetAnimations();
		std::map<long, Audio>& GetAudios();
		std::map<long, Text>& GetTexts();
		std::map<long, Body>& GetBodies();
		std::map<long, JointMaker>& GetJointMakers();
		std::map<long, CharacterController>& GetCharacterControllers();
		std::map<long, TileMap>& GetTileMaps();		
		void SetPersistantScene(bool b_persistant);
		bool IsPersistantScene();

	private:
		std::string m_name;
		std::string m_path;
		std::map<long, GameObject> m_sceneObjects;
		std::vector<GameObject*> m_sortedHierarchyObjects;
		std::vector<GameObject*> m_animatorPreviewObjects;
		ECSManager m_ECSManager;
		long m_nextGameObjectID;
		long m_nextComponentID;
		std::vector<long> m_freedComponentIDs;
		std::vector<long> m_freedGameObjectIDs;
		bool m_b_persistantScene;
	};
}