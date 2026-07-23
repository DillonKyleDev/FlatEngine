#pragma once
#include "components/Animation.h"
#include "components/Audio.h"
#include "components/TileMap.h"
#include "managers/LuaManager.h"
#include "render/Texture.h"
#include "shapes/Shape.h"
#include "TagList.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"

#include "box2d.h"
#include <string>
#include <map>
#include <memory>
#include <vector>


namespace FlatEngine 
{
	class GameObject;
	class Scene;

	namespace PrefabManager
	{
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
			Vector3 origin = Vector3();
			Vector3 position = Vector3();
			Vector3 scale = Vector3();
			Vector3 rotation = Vector3();
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
			std::map<std::string, LuaManager::LuaParameter> scriptParams = std::map<std::string, LuaManager::LuaParameter>();
		};
		struct ButtonPrefabData : public ComponentPrefabData {
			Vector2 activeDimensions = Vector2(2.0f, 2.0f);
			Vector2 activeOffset = Vector2();
			int activeLayer = 0;
			std::string functionName;
			bool b_cppEvent = false;
			bool b_luaEvent = false;
		};
		struct CanvasPrefabData : public ComponentPrefabData {		
			int layerNumber = 0;
			bool b_blocksLayers = false;
			float width = 10.0f;
			float height = 10.0f;
		};
		struct AnimationPrefabData : public ComponentPrefabData {
			std::vector<AnimationData> animations = std::vector<AnimationData>();
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
		struct ShapePrefabData : public ComponentPrefabData {
			bool b_enableContactEvents = true;
			bool b_enableSensorEvents = true;
			bool b_isSensor = false;
			Shape::ShapeType shape = Shape::ShapeType::BS_None;
			Vector2 positionOffset = Vector2(0, 0);
			b2Rot rotationOffset = b2MakeRot(0);
			float restitution = 0.3f;
			float density = 1.0f;
			float friction = 0.3f;
			Vector2 dimensions = Vector2(1.0f, 1.0f);
			float radius = 1.0f;
			float capsuleLength = 4.0f;
			bool b_horizontal = false;
			float cornerRadius = 0.0f;
			std::vector<Vector2> points = std::vector<Vector2>();
			bool b_isLoop = false;
			float tangentSpeed = 0.0f;
			float rollingResistance = 0.0f;
		};
		struct BodyPrefabData : public ComponentPrefabData {	
			b2BodyType bodyType = b2_dynamicBody;		
			bool b_lockedRotation = false;
			bool b_lockedXAxis = false;
			bool b_lockedYAxis = false;
			float gravityScale = 1.0f;
			float linearDamping = 0.0f;
			float angularDamping = 0.0f;	
			std::vector<std::shared_ptr<ShapePrefabData>> shapes = std::vector<std::shared_ptr<ShapePrefabData>>();
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
		struct MeshPrefabData : public ComponentPrefabData {
			std::string materialName = "";
			std::string modelPath = "";
			std::map<uint32_t, Texture> texturesByIndex = std::map<uint32_t, Texture>();
			std::map<std::string, Vector4> uboVec4s = std::map<std::string, Vector4>();
		};

		struct Prefab {
			std::string name;
			Vector2 spawnLocation;
			GameObjectPrefabData rootObject;
			std::map<long, GameObjectPrefabData> objects;
			std::map<long, std::shared_ptr<ComponentPrefabData>> components;
		};


		extern std::map<std::string, Prefab> prefabs;

		extern void CreatePrefabFromJson(json objectJson, Prefab &prefab);
		extern void CreatePrefab(std::string path, GameObject gameObject);		
		extern void AddPrefab(std::string path);
		extern void InitializePrefabs();
		extern GameObject* InstantiateSelfAndChildren(long parentID, long myID, Prefab prefab, Scene* scene, Vector3 spawnLocation = Vector3(0, 0, 0));
		extern GameObject* Instantiate(std::string prefabName, Vector3 spawnLocation, Scene* scene, long parentID = -1, long ID = -1);
		extern std::map<std::string, Prefab> GetPrefabs();
	}
}