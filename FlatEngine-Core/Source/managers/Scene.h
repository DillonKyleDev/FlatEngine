#pragma once
#include "components/Animation.h"
#include "components/Audio.h"
#include "components/Body.h"
#include "components/Body2D.h"
#include "components/Button.h"
#include "components/Camera.h"
#include "components/Canvas.h"
#include "components/CharacterController.h"
#include "components/Component.h"
#include "components/JointMaker.h"
#include "components/Light.h"
#include "components/Mesh.h"
#include "components/Script.h"
#include "components/Sprite.h"
#include "components/Text.h"
#include "components/TileMap.h"
#include "components/Transform.h"
#include "GameObject.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>


namespace FlatEngine
{
	class Scene
	{
	public:
		Scene();
		static bool SortHierarchyObjects(GameObject* gameObjectA, GameObject* gameObjectB);

		void Unload();
		GameObject* AddSceneObject(GameObject sceneObject);
		std::vector<GameObject>& GetSceneObjects();
		void SetAnimatorPreviewObjects(std::vector<GameObject*> previewObjects);
		std::vector<GameObject*> GetAnimatorPreviewObjects();
		GameObject* GetObjectByID(long ID);
		GameObject* GetObjectByName(std::string name);
		GameObject* GetObjectByTag(std::string tag);
		GameObject* CreateGameObject(long parentID = -1, long myID = -1);
		GameObject* CreateEmptyGameObject(long parentID = -1, long myID = -1);
		void DeleteGameObject(long sceneObjectID);
		void DeleteGameObject(GameObject* objectToDelete);

		void KeepNextGameObjectIDUpToDate(long id);
		void KeepNextComponentIDUpToDate(long ID);
		void SetNextGameObjectID(long nextID);
		long GetNextGameObjectID();
		void SetNextComponentID(long nextID);
		long GetNextComponentID();

		void OnPrefabInstantiated();
		void SortSceneObjects();
		std::vector<GameObject*> GetSortedHierarchyObjects();
		void CreateJoints();

		std::vector<Script*> GetScripts(long ownerID);
		// Raw member variables
		template<class T> std::map<long, T>& GetAll() {	return GetContainer<T>(); }		
		std::map<long, std::vector<std::string>>& GetLuaScriptsByOwner();
		std::map<std::string, std::vector<Mesh>>& GetMeshesByMaterial();

		void RemoveComponent(Component* component);
		void RemoveScript(long ownerID, long scriptID);

		Camera* GetPrimaryCamera();
		void SetPrimaryCamera(Camera* camera);
		void RemovePrimaryCamera();

		Component* AddComponent(ComponentType type, long ownerID, long componentID = -1);
		
		template<class T> T* Add(long ownerID, long componentID)
		{
			auto [iter, inserted] = GetContainer<T>().emplace(ownerID, T(componentID == -1 ? GetNextComponentID() : componentID, ownerID));			
			return &iter->second;
		}
		template<class T> T* Get(long ownerID)	
		{
			if (GetContainer<T>().count(ownerID))
				return &GetContainer<T>().at(ownerID);
			else
				return nullptr;
		}
		template<class T> void Remove(long ownerID)
		{
			if (GetContainer<T>().count(ownerID))
			{
				GetContainer<T>().erase(ownerID);
			}
		}

		std::string name;
		std::string path;

	private:
		template<class T> std::map<long, T>& GetContainer();
		void DeleteChildrenAndSelf(GameObject *objectToDelete);
		void RemoveSceneObject(long ID);

		std::vector<GameObject> m_sceneObjects;
		std::unordered_map<long, size_t> m_IDToIndex;
		std::vector<GameObject*> m_sortedHierarchyObjects;
		std::vector<GameObject*> m_animatorPreviewObjects;
		long m_nextGameObjectID;
		long m_nextComponentID;
		std::vector<long> m_freedComponentIDs;
		std::vector<long> m_freedGameObjectIDs;
		Camera* m_primaryCamera;

		std::map<long, Transform> m_Transforms;
		std::map<long, Animation> m_Animations;
		std::map<long, Audio> m_Audios;
		std::map<long, Body> m_Bodies;
		std::map<long, Body2D> m_Bodies2D;
		std::map<long, Button> m_Buttons;
		std::map<long, Camera> m_Cameras;
		std::map<long, Canvas> m_Canvases;
		std::map<long, CharacterController> m_CharacterControllers;
		std::map<long, JointMaker> m_JointMakers;
		std::map<long, Light> m_Lights;
		std::map<long, std::vector<std::string>> m_LuaScriptsByOwner;
		std::map<long, Mesh> m_Meshes;
		std::map<std::string, std::vector<Mesh>> m_MeshesByMaterial;
		std::map<long, Script> m_Scripts;		
		std::map<long, Sprite> m_Sprites;
		std::map<long, Text> m_Texts;			
		std::map<long, TileMap> m_TileMaps;		
	};
}