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
#include "components/Light.h"
#include "components/Mesh.h"
#include "components/Script.h"
#include "components/Sprite.h"
#include "components/Text.h"
#include "components/TileMap.h"
#include "components/Transform.h"
#include "GameObject.h"
#include "Types.h"

#include <map>
#include <string>
#include <vector>


namespace FlatEngine
{
	class Scene
	{
	public:
		Scene();
		// static bool SortHierarchyObjects(GameObject* gameObjectA, GameObject* gameObjectB);

		void Unload();
		GameObject* CreateGameObject(long parentID = -1, long myID = -1);			
		GameObject* AddSceneObject(json objectJson = json::object(), long parentID = -1, long ID = -1);
		void ReduceHierarchyPositions();
		void InsertSortedHierarchyObject(GameObject* object);
		void RemoveSortedHierarchyObject(GameObject* object);
		std::vector<GameObject>& GetSceneObjects();
		void SetAnimatorPreviewObjects(std::vector<GameObject*> previewObjects);
		std::vector<GameObject*> GetAnimatorPreviewObjects();
		GameObject* GetObjectByID(long ID); // *NOT PERSISTENT* Be careful! If you add objects to the scene after getting this pointer, it WILL eventually become garbage because Scene::m_sceneObjects is a std::vector under the hood
		GameObject* GetObjectByName(std::string name);
		GameObject* GetObjectByTag(std::string tag);			
		void DeleteGameObject(long sceneObjectID);
		void DeleteGameObject(GameObject* objectToDelete);

		void KeepNextGameObjectIDUpToDate(long id);
		void SetNextGameObjectID(long nextID);
		long GetNextGameObjectID();		
		void CollectUnusedGameObjectIDs();
		long GetNextJoint2DID();
		void AddFreedJoint2DID(long freedID);

		void SortSceneObjects();
		std::vector<long> GetSortedHierarchyObjects();		
		long GetNextHierarchyPosition();

		std::vector<Script*> GetScripts(long ownerID);
		// Raw member variables
		template<class T> UMapVector<T>& GetAll() {	return GetContainer<T>(); }				
		std::map<std::string, std::vector<Mesh>>& GetMeshesByMaterial();

		void RemoveComponent(Component* component);
		void RemoveScript(long ownerID, long scriptID);

		Camera* GetPrimaryCamera();
		void SetPrimaryCamera(long cameraID);
		void RemovePrimaryCamera();

		Component* AddComponent(ComponentType type, long ownerID);
		
		template<class T> T* Add(long ownerID)
		{
			return GetContainer<T>().Add(ownerID, T(ownerID));			
		}
		template<class T> T* Get(long ownerID)	
		{			
			return GetContainer<T>().Get(ownerID);		
		}
		template<class T> void Remove(long ownerID)
		{
			GetContainer<T>().Remove(ownerID);
		}

		std::string name;
		std::string path;

	private:		
		template<class T> UMapVector<T>& GetContainer();
		void DeleteChildrenAndSelf(GameObject *objectToDelete);
		void RemoveSceneObject(long ID);

		UMapVector<GameObject> m_sceneObjects;
		std::vector<long> m_sortedHierarchyObjects;		
		std::vector<GameObject*> m_animatorPreviewObjects;
		long m_nextGameObjectID;
		std::vector<long> m_freedGameObjectIDs;
		long m_nextJoint2DID;
		std::vector<long> m_freedJoint2DIDs;
		long m_primaryCameraID;

		UMapVector<Transform> m_Transforms;
		UMapVector<Animation> m_Animations;
		UMapVector<Audio> m_Audios;
		UMapVector<Body> m_Bodies;
		UMapVector<Body2D> m_Bodies2D;
		UMapVector<Button> m_Buttons;
		UMapVector<Camera> m_Cameras;
		UMapVector<Canvas> m_Canvases;
		UMapVector<CharacterController> m_CharacterControllers;		
		UMapVector<Light> m_Lights;
		UMapVector<Mesh> m_Meshes;
		std::map<std::string, std::vector<Mesh>> m_MeshesByMaterial;
		UMapVector<Script> m_Scripts;		
		UMapVector<Sprite> m_Sprites;
		UMapVector<Text> m_Texts;			
		UMapVector<TileMap> m_TileMaps;		
	};
}