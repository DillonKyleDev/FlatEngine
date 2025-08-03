#pragma once
#include "Vector2.h"
#include "TagList.h"
#include "Animation.h"
#include "Audio.h"    // SoundData
#include "TileMap.h"  // Tile
#include "Scene.h"

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;

namespace FlatEngine 
{
	class GameObject;

	struct GameObjectPrefabData {
		long parentID = -1;
		long ID = -1;
		bool b_isActive = true;
		std::string name = "";
		TagList tagList = TagList();
		std::vector<long> componentIDs = std::vector<long>();
		std::vector<long> childrenIDs = std::vector<long>();
	};
	struct ComponentPrefabData {
		std::string type = "";
		long ownerId = -1;
		long id = -1;
		bool b_isActive = true;
		bool b_isCollapsed = false;
	};

	struct TransformPrefabData : public ComponentPrefabData {
		Vector2 origin = Vector2();
		Vector2 position = Vector2();
		Vector2 scale = Vector2();
		float rotation = 0.0f;
	};
	struct SpritePrefabData : public ComponentPrefabData {		
		int renderOrder = 0;
		float textureWidth = 1.0f;
		float textureHeight = 1.0f;
		Vector2 scale = Vector2();
		std::string pivotPoint = "";
		Vector2 pivotOffset = Vector2();
		Vector2 offset = Vector2();
		std::string path = "";
		Vector4 tintColor = Vector4();
	};
	struct CameraPrefabData : public ComponentPrefabData {		
		float width = 10.0f;
		float height = 10.0f;
		float zoom = 30.0f;
		ImVec4 frustrumColor = { 0.0f, 0.0f, 0.0f, 0.0f };
		bool b_isPrimaryCamera = false;
		bool b_shouldFollow = false;
		long toFollowID = -1;
		float followSmoothing = 0.5f;
	};
	struct ScriptPrefabData : public ComponentPrefabData {		
		std::string attachedScript = "";
		std::map<std::string, Animation::S_EventFunctionParam> scriptParams = std::map<std::string, Animation::S_EventFunctionParam>();
	};
	struct ButtonPrefabData : public ComponentPrefabData {
		Vector2 activeDimensions = Vector2(2.0f, 2.0f);
		Vector2 activeOffset = Vector2();
		int activeLayer = 0;
	};
	struct CanvasPrefabData : public ComponentPrefabData {		
		int layerNumber = 0;
		bool b_blocksLayers = false;
		float width = 10.0f;
		float height = 10.0f;
	};
	struct AnimationPrefabData : public ComponentPrefabData {
		std::vector<Animation::AnimationData> animations = std::vector<Animation::AnimationData>();
	};
	struct AudioPrefabData : public ComponentPrefabData {
		std::vector<SoundData> sounds = std::vector<SoundData>();
	};
	struct TextPrefabData : public ComponentPrefabData {
		std::string fontPath = "";
		int fontSize = 14;
		std::string text = "";
		Vector4 color = Vector4();
		Vector2 offset = Vector2();
		int renderOrder = 0;
	};
	struct BoxBodyPrefabData : public ComponentPrefabData {	
		b2BodyType bodyType = b2_dynamicBody;
		//bool b_isEnabled = true;
		bool b_lockedRotation;
		bool b_lockedXAxis;
		bool b_lockedYAxis;
		float gravityScale;
		float linearDamping;
		float angularDamping;
		float restitution;
		float density;
		float friction;		
		Vector2 dimensions;
	};
	struct CircleBodyPrefabData : public ComponentPrefabData {
		b2BodyType bodyType = b2_dynamicBody;
		//bool b_isEnabled = true;
		bool b_lockedRotation = false;
		bool b_lockedXAxis = false;
		bool b_lockedYAxis = false;
		float gravityScale = 1.0f;
		float linearDamping = 0.0f;
		float angularDamping = 0.0f;
		float restitution = 0.3f;
		float density = 1.0f;
		float friction = 0.3f;
		float radius = 1.0f;
	};
	struct CapsuleBodyPrefabData : public ComponentPrefabData {
		b2BodyType bodyType = b2_dynamicBody;
		//bool b_isEnabled = true;
		bool b_lockedRotation = false;
		bool b_lockedXAxis = false;
		bool b_lockedYAxis = false;
		float gravityScale = 1.0f;
		float linearDamping = 0.0f;
		float angularDamping = 0.0f;
		float restitution = 0.3f;
		float density = 1.0f;
		float friction = 0.3f;
		float radius = 1.0f;
		float capsuleLength;
		bool b_horizontal;
	};
	struct PolygonBodyPrefabData : public ComponentPrefabData {
		b2BodyType bodyType = b2_dynamicBody;
		//bool b_isEnabled = true;
		bool b_lockedRotation = false;
		bool b_lockedXAxis = false;
		bool b_lockedYAxis = false;
		float gravityScale = 1.0f;
		float linearDamping = 0.0f;
		float angularDamping = 0.0f;
		float restitution = 0.3f;
		float density = 1.0f;
		float friction = 0.3f;
	};
	struct CharacterControllerPrefabData : public ComponentPrefabData {
		float maxAcceleration = 10.0f;
		float maxSpeed = 10.0f;
		float airControl = 1.0f;
	};
	struct TileMapPrefabData : public ComponentPrefabData {
		int width;
		int height;
		int tileWidth;
		int tileHeight;
		int renderOrder;
		std::map<int, std::map<int, Tile>> tiles;
		std::vector<std::string> tileSetNames;
		std::map<std::string, std::vector<std::pair<Vector2, Vector2>>> collisionAreas;
	};

	struct Prefab {
		std::string name;
		Vector2 spawnLocation;
		GameObjectPrefabData rootObject;
		std::map<long, GameObjectPrefabData> objects;
		std::map<long, std::shared_ptr<ComponentPrefabData>> components;
	};

	class PrefabManager
	{
	public:
		PrefabManager();
		~PrefabManager();
		void CreatePrefabFromJson(json objectJson, Prefab &prefab);
		void CreatePrefab(std::string path, GameObject gameObject);		
		void AddPrefab(std::string path);
		void InitializePrefabs();
		GameObject* InstantiateSelfAndChildren(long parentID, long myID, Prefab prefab, Scene* scene, Vector2 spawnLocation = Vector2(0, 0));
		GameObject* Instantiate(std::string prefabName, Vector2 spawnLocation, Scene* scene, long parentID = -1, long ID = -1);
		std::map<std::string, Prefab> GetPrefabs();

	private:
		std::map<std::string, Prefab> m_prefabs;
	};
}