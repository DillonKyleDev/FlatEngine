#include "components/Button.h"
#include "components/Transform.h"
#include "GameObject.h"
#include "managers/Assets.h"
#include "managers/PrefabManager.h"
#include "managers/SceneManager.h"
#include "render/VulkanManager.h"
#include "tools/FileHelper.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"

#include <fstream>


namespace FlatEngine
{
	namespace PrefabManager
	{
		std::map<std::string, Prefab> prefabs = std::map<std::string, Prefab>();

		// void CreatePrefabFromJson(json objectJson, Prefab& prefab)
		// {
		// 	GameObjectPrefabData object;
		// 	object.name = JsonHelper::CheckJsonString(objectJson, "name", "Name");
		// 	std::string objectName = object.name;
		// 	object.b_isActive = JsonHelper::CheckJsonBool(objectJson, "b_isActive", objectName);
		// 	object.ID = JsonHelper::CheckJsonLong(objectJson, "id", objectName);
		// 	object.parentID = JsonHelper::CheckJsonLong(objectJson, "parent", objectName);
		// 	TagList tags = TagList(object.ID);

		// 	if (JsonHelper::JsonContains(objectJson, "children", objectName))
		// 	{
		// 		for (int c = 0; c < objectJson["children"].size(); c++)
		// 		{
		// 			object.childrenIDs.push_back(objectJson["children"][c]);
		// 		}
		// 	}

		// 	// TagList			
		// 	if (JsonHelper::JsonContains(objectJson, "tags", objectName))
		// 	{
		// 		json tagsJson = objectJson["tags"];
		// 		for (json jsonTag : tagsJson)
		// 		{
		// 			std::string tag = jsonTag.items().begin().key();
		// 			bool b_hasTag = jsonTag.items().begin().value();
		// 			tags.SetTag(tag, b_hasTag);
		// 		}
		// 	}
		// 	if (JsonHelper::JsonContains(objectJson, "collidesTags", objectName))
		// 	{
		// 		json collidesTagsJson = objectJson["collidesTags"];
		// 		for (json jsonCollidesTag : collidesTagsJson)
		// 		{
		// 			std::string collidesTag = jsonCollidesTag.items().begin().key();
		// 			bool b_collidesTag = jsonCollidesTag.items().begin().value();
		// 			tags.SetCollides(collidesTag, b_collidesTag);
		// 		}
		// 	}

		// 	object.tagList = tags;


		// 	Vector3 objectRotation = Vector3();
		// 	// Loop through the components in this PrefabsJson
		// 	for (int j = 0; j < objectJson["components"].size(); j++)
		// 	{
		// 		json componentJson = objectJson["components"][j];
		// 		std::string type = JsonHelper::CheckJsonString(componentJson, "type", objectName);
		// 		long componentID = JsonHelper::CheckJsonLong(componentJson, "id", objectName);
		// 		bool b_isCollapsed = JsonHelper::CheckJsonBool(componentJson, "b_isCollapsed", objectName);
		// 		bool b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);

		// 		// connect this component to the object being saved
		// 		object.componentIDs.push_back(componentID);

		// 		//Add each loaded component to the newly created GameObject
		// 		if (type == "Transform")
		// 		{
		// 			std::shared_ptr<TransformPrefabData> transform = std::make_shared<TransformPrefabData>();
		// 			transform->b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);
		// 			transform->type = "Transform";
		// 			transform->id = componentID;
		// 			transform->b_isActive = b_isActive;
		// 			transform->b_isCollapsed = b_isCollapsed;
		// 			transform->ownerId = object.ID;				
		// 			transform->position = Vector3(JsonHelper::CheckJsonFloat(componentJson, "xPosition", objectName), JsonHelper::CheckJsonFloat(componentJson, "yPosition", objectName), JsonHelper::CheckJsonFloat(componentJson, "zPosition", objectName));
		// 			transform->scale = Vector3(JsonHelper::CheckJsonFloat(componentJson, "xScale", objectName), JsonHelper::CheckJsonFloat(componentJson, "yScale", objectName), JsonHelper::CheckJsonFloat(componentJson, "zScale", objectName));				
		// 			transform->rotation = Vector3(JsonHelper::CheckJsonFloat(componentJson, "xRotation", objectName), JsonHelper::CheckJsonFloat(componentJson, "yRotation", objectName), JsonHelper::CheckJsonFloat(componentJson, "zRotation", objectName));
		// 			objectRotation = transform->rotation;

		// 			prefab.components.emplace(componentID, transform);
		// 		}
		// 		else if (type == "Sprite")
		// 		{
		// 			std::shared_ptr<SpritePrefabData> sprite = std::make_shared<SpritePrefabData>();
		// 			sprite->b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);
		// 			sprite->type = "Sprite";
		// 			sprite->id = componentID;
		// 			sprite->b_isActive = b_isActive;
		// 			sprite->b_isCollapsed = b_isCollapsed;
		// 			sprite->ownerId = object.ID;
		// 			sprite->path = JsonHelper::CheckJsonString(componentJson, "path", objectName);
		// 			std::string pivotPoint = "Center";
		// 			if (JsonHelper::CheckJsonString(componentJson, "pivotPoint", objectName) != "")
		// 			{
		// 				pivotPoint = JsonHelper::CheckJsonString(componentJson, "pivotPoint", objectName);
		// 			}
		// 			sprite->pivotPoint = pivotPoint;
		// 			sprite->scale = Vector2(JsonHelper::CheckJsonFloat(componentJson, "xScale", objectName), JsonHelper::CheckJsonFloat(componentJson, "yScale", objectName));
		// 			sprite->offset = Vector2(JsonHelper::CheckJsonFloat(componentJson, "xOffset", objectName), JsonHelper::CheckJsonFloat(componentJson, "yOffset", objectName));
		// 			sprite->renderOrder = JsonHelper::CheckJsonInt(componentJson, "renderOrder", objectName);
		// 			sprite->tintColor = Vector4(
		// 				JsonHelper::CheckJsonFloat(componentJson, "tintColorX", objectName),
		// 				JsonHelper::CheckJsonFloat(componentJson, "tintColorY", objectName),
		// 				JsonHelper::CheckJsonFloat(componentJson, "tintColorZ", objectName),
		// 				JsonHelper::CheckJsonFloat(componentJson, "tintColorW", objectName)
		// 			);

		// 			prefab.components.emplace(componentID, sprite);
		// 		}
		// 		else if (type == "Camera")
		// 		{
		// 			std::shared_ptr<CameraPrefabData> camera = std::make_shared<CameraPrefabData>();
		// 			camera->b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);
		// 			camera->type = "Camera";
		// 			camera->id = componentID;
		// 			camera->b_isActive = b_isActive;
		// 			camera->b_isCollapsed = b_isCollapsed;
		// 			camera->ownerId = object.ID;
		// 			camera->width = JsonHelper::CheckJsonFloat(componentJson, "width", objectName);
		// 			camera->height = JsonHelper::CheckJsonFloat(componentJson, "height", objectName);
		// 			camera->b_isPrimaryCamera = JsonHelper::CheckJsonBool(componentJson, "b_isPrimaryCamera", objectName);
		// 			camera->zoom = JsonHelper::CheckJsonFloat(componentJson, "zoom", objectName);
		// 			camera->frustrumColor = Vector4(
		// 				JsonHelper::CheckJsonFloat(componentJson, "frustrumRed", objectName),
		// 				JsonHelper::CheckJsonFloat(componentJson, "frustrumGreen", objectName),
		// 				JsonHelper::CheckJsonFloat(componentJson, "frustrumBlue", objectName),
		// 				JsonHelper::CheckJsonFloat(componentJson, "frustrumAlpha", objectName)
		// 			);
		// 			camera->b_shouldFollow = JsonHelper::CheckJsonBool(componentJson, "b_follow", objectName);
		// 			camera->followSmoothing = JsonHelper::CheckJsonFloat(componentJson, "followSmoothing", objectName);
		// 			camera->toFollowID = JsonHelper::CheckJsonLong(componentJson, "following", objectName);

		// 			prefab.components.emplace(componentID, camera);
		// 		}
		// 		else if (type == "Script")
		// 		{
		// 			std::shared_ptr<ScriptPrefabData> script = std::make_shared<ScriptPrefabData>();
		// 			script->b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);
		// 			script->type = "Script";
		// 			script->id = componentID;
		// 			script->b_isActive = b_isActive;
		// 			script->b_isCollapsed = b_isCollapsed;
		// 			script->ownerId = object.ID;
		// 			script->attachedScript = JsonHelper::CheckJsonString(componentJson, "attachedScript", objectName);

		// 			json scriptParamsJson = componentJson["scriptParameters"];

		// 			for (int i = 0; i < scriptParamsJson.size(); i++)
		// 			{
		// 				json param = scriptParamsJson[i];
		// 				LuaManager::LuaParameter parameter;
		// 				parameter.PutData(param, objectName);
		// 				script->scriptParams.emplace(parameter.name, parameter);
		// 			}

		// 			prefab.components.emplace(componentID, script);
		// 		}
		// 		else if (type == "Button")
		// 		{
		// 			std::shared_ptr<ButtonPrefabData> button = std::make_shared<ButtonPrefabData>();
		// 			button->b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);
		// 			button->type = "Button";
		// 			button->ownerId = object.ID;
		// 			button->id = componentID;
		// 			button->b_isActive = b_isActive;
		// 			button->b_isCollapsed = b_isCollapsed;
		// 			button->activeDimensions = Vector2(JsonHelper::CheckJsonFloat(componentJson, "activeWidth", objectName), JsonHelper::CheckJsonFloat(componentJson, "activeHeight", objectName));
		// 			button->activeOffset = Vector2(JsonHelper::CheckJsonFloat(componentJson, "activeOffsetX", objectName), JsonHelper::CheckJsonFloat(componentJson, "activeOffsetY", objectName));
		// 			button->activeLayer = JsonHelper::CheckJsonInt(componentJson, "activeLayer", objectName);
		// 			button->functionName = JsonHelper::CheckJsonString(componentJson, "functionName", objectName);
		// 			button->b_cppEvent = JsonHelper::CheckJsonBool(componentJson, "b_cppEvent", objectName);
		// 			button->b_luaEvent = JsonHelper::CheckJsonBool(componentJson, "b_luaEvent", objectName);

		// 			prefab.components.emplace(componentID, button);
		// 		}
		// 		else if (type == "Canvas")
		// 		{
		// 			std::shared_ptr<CanvasPrefabData> canvas = std::make_shared<CanvasPrefabData>();
		// 			canvas->b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);
		// 			canvas->type = "Canvas";
		// 			canvas->ownerId = object.ID;
		// 			canvas->id = componentID;
		// 			canvas->b_isActive = b_isActive;
		// 			canvas->b_isCollapsed = b_isCollapsed;
		// 			canvas->width = JsonHelper::CheckJsonFloat(componentJson, "width", objectName);
		// 			canvas->height = JsonHelper::CheckJsonFloat(componentJson, "height", objectName);
		// 			canvas->layerNumber = JsonHelper::CheckJsonInt(componentJson, "layerNumber", objectName);
		// 			canvas->b_blocksLayers = JsonHelper::CheckJsonBool(componentJson, "b_blocksLayers", objectName);

		// 			prefab.components.emplace(componentID, canvas);
		// 		}
		// 		else if (type == "Animation")
		// 		{
		// 			std::shared_ptr<AnimationPrefabData> animation = std::make_shared<AnimationPrefabData>();
		// 			animation->b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);
		// 			animation->type = "Animation";
		// 			animation->ownerId = object.ID;
		// 			animation->id = componentID;
		// 			animation->b_isActive = b_isActive;
		// 			animation->b_isCollapsed = b_isCollapsed;

		// 			if (JsonHelper::JsonContains(componentJson, "animationData", objectName))
		// 			{
		// 				for (int anim = 0; anim < componentJson["animationData"].size(); anim++)
		// 				{
		// 					json animationJson = componentJson["animationData"][anim];
		// 					std::string path = JsonHelper::CheckJsonString(animationJson, "path", objectName);
		// 					std::string name = JsonHelper::CheckJsonString(animationJson, "name", objectName);

		// 					AnimationData animData;
		// 					animData.name = name;
		// 					animData.path = path;

		// 					animation->animations.push_back(animData);
		// 				}
		// 			}

		// 			prefab.components.emplace(componentID, animation);
		// 		}
		// 		else if (type == "Audio")
		// 		{
		// 			std::shared_ptr<AudioPrefabData> audio = std::make_shared<AudioPrefabData>();
		// 			audio->b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);
		// 			audio->type = "Audio";
		// 			audio->ownerId = object.ID;
		// 			audio->id = componentID;
		// 			audio->b_isActive = b_isActive;
		// 			audio->b_isCollapsed = b_isCollapsed;

		// 			if (JsonHelper::JsonContains(componentJson, "soundData", objectName))
		// 			{
		// 				for (int sound = 0; sound < componentJson["soundData"].size(); sound++)
		// 				{
		// 					json audioJson = componentJson["soundData"][sound];
		// 					std::string path = JsonHelper::CheckJsonString(audioJson, "path", objectName);
		// 					std::string name = JsonHelper::CheckJsonString(audioJson, "name", objectName);
		// 					bool b_isMusic = JsonHelper::CheckJsonBool(audioJson, "b_isMusic", objectName);

		// 					SoundData soundData;
		// 					soundData.name = name;
		// 					soundData.b_isMusic = b_isMusic;
		// 					soundData.path = path;
		// 					soundData.sound = std::make_shared<AudioManager::Sound>();

		// 					if (soundData.path != "" && soundData.sound != nullptr)
		// 					{
		// 						if (soundData.b_isMusic)
		// 						{
		// 							soundData.sound->LoadMusic(soundData.path);
		// 						}
		// 						else
		// 						{
		// 							soundData.sound->LoadEffect(soundData.path);
		// 						}
		// 					}

		// 					audio->sounds.push_back(soundData);
		// 				}
		// 			}

		// 			prefab.components.emplace(componentID, audio);
		// 		}
		// 		else if (type == "Text")
		// 		{
		// 			std::shared_ptr<TextPrefabData> text = std::make_shared<TextPrefabData>();
		// 			text->b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);
		// 			text->type = "Text";
		// 			text->ownerId = object.ID;
		// 			text->id = componentID;
		// 			text->b_isActive = b_isActive;
		// 			text->b_isCollapsed = b_isCollapsed;
		// 			text->fontPath = JsonHelper::CheckJsonString(componentJson, "fontPath", objectName);
		// 			text->fontSize = JsonHelper::CheckJsonInt(componentJson, "fontSize", objectName);
		// 			text->color = Vector4(
		// 				JsonHelper::CheckJsonFloat(componentJson, "tintColorX", objectName),
		// 				JsonHelper::CheckJsonFloat(componentJson, "tintColorY", objectName),
		// 				JsonHelper::CheckJsonFloat(componentJson, "tintColorZ", objectName),
		// 				JsonHelper::CheckJsonFloat(componentJson, "tintColorW", objectName)
		// 			);
		// 			text->text = JsonHelper::CheckJsonString(componentJson, "text", objectName);
		// 			text->offset = Vector2(JsonHelper::CheckJsonFloat(componentJson, "xOffset", objectName), JsonHelper::CheckJsonFloat(componentJson, "yOffset", objectName));
		// 			text->renderOrder = JsonHelper::CheckJsonInt(componentJson, "renderOrder", objectName);

		// 			prefab.components.emplace(componentID, text);
		// 		}
		// 		else if (type == "CharacterController")
		// 		{
		// 			std::shared_ptr<CharacterControllerPrefabData> characterController = std::make_shared<CharacterControllerPrefabData>();
		// 			characterController->b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);
		// 			characterController->type = "CharacterController";
		// 			characterController->ownerId = object.ID;
		// 			characterController->id = componentID;
		// 			characterController->b_isActive = b_isActive;
		// 			characterController->b_isCollapsed = b_isCollapsed;
		// 			characterController->maxSpeed = JsonHelper::CheckJsonFloat(componentJson, "maxSpeed", objectName);
		// 			characterController->maxAcceleration = JsonHelper::CheckJsonFloat(componentJson, "maxAcceleration", objectName);
		// 			characterController->airControl = JsonHelper::CheckJsonFloat(componentJson, "airControl", objectName);

		// 			prefab.components.emplace(componentID, characterController);
		// 		}
		// 		else if (type == "Body")
		// 		{

		// 			std::shared_ptr<BodyPrefabData> body = std::make_shared<BodyPrefabData>();
		// 			body->b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);				
		// 			body->type = "Body";
		// 			body->ownerId = object.ID;
		// 			body->id = componentID;
		// 			body->b_isActive = b_isActive;
		// 			body->b_isCollapsed = b_isCollapsed;
		// 			body->bodyType = (b2BodyType)JsonHelper::CheckJsonInt(componentJson, "bodyType", objectName);
		// 			body->b_lockedRotation = JsonHelper::CheckJsonBool(componentJson, "b_lockedRotation", objectName);
		// 			body->b_lockedXAxis = JsonHelper::CheckJsonBool(componentJson, "b_lockedXAxis", objectName);
		// 			body->b_lockedYAxis = JsonHelper::CheckJsonBool(componentJson, "b_lockedYAxis", objectName);				
		// 			body->linearDamping = JsonHelper::CheckJsonFloat(componentJson, "linearDamping", objectName);
		// 			body->angularDamping = JsonHelper::CheckJsonFloat(componentJson, "angularDamping", objectName);
		// 			body->gravityScale = JsonHelper::CheckJsonFloat(componentJson, "gravityScale", objectName);

		// 			if (JsonHelper::JsonContains(componentJson, "shapes", objectName))
		// 			{
		// 				for (int i = 0; i < componentJson.at("shapes").size(); i++)
		// 				{
		// 					try
		// 					{
		// 						json shapeJson = componentJson.at("shapes").at(i);
		// 						std::shared_ptr<ShapePrefabData> shape = std::make_shared<ShapePrefabData>();

		// 						shape->b_enableContactEvents = JsonHelper::CheckJsonBool(shapeJson, "b_enableContactEvents", objectName);;
		// 						shape->b_enableSensorEvents = JsonHelper::CheckJsonBool(shapeJson, "b_enableSensorEvents", objectName);;
		// 						shape->b_isSensor = JsonHelper::CheckJsonBool(shapeJson, "b_isSensor", objectName);;
		// 						shape->shape = (Shape::ShapeType)JsonHelper::CheckJsonInt(shapeJson, "shape", objectName);
		// 						shape->positionOffset = Vector2(JsonHelper::CheckJsonFloat(shapeJson, "xOffset", objectName) , JsonHelper::CheckJsonFloat(shapeJson, "yOffset", objectName));
		// 						shape->rotationOffset.c = JsonHelper::CheckJsonFloat(shapeJson, "rotationOffsetCos", objectName);
		// 						shape->rotationOffset.s = JsonHelper::CheckJsonFloat(shapeJson, "rotationOffsetSin", objectName);
		// 						shape->restitution = JsonHelper::CheckJsonFloat(shapeJson, "restitution", objectName);
		// 						shape->density = JsonHelper::CheckJsonFloat(shapeJson, "density", objectName);
		// 						shape->friction = JsonHelper::CheckJsonFloat(shapeJson, "friction", objectName);
		// 						shape->dimensions = Vector2(JsonHelper::CheckJsonFloat(shapeJson, "width", objectName) , JsonHelper::CheckJsonFloat(shapeJson, "height", objectName));
		// 						shape->radius = JsonHelper::CheckJsonFloat(shapeJson, "radius", objectName);
		// 						shape->capsuleLength = JsonHelper::CheckJsonFloat(shapeJson, "capsuleLength", objectName);
		// 						shape->b_horizontal = JsonHelper::CheckJsonBool(shapeJson, "b_horizontal", objectName);
		// 						shape->cornerRadius = JsonHelper::CheckJsonFloat(shapeJson, "cornerRadius", objectName);
								
		// 						if (JsonHelper::JsonContains(shapeJson, "points", objectName))
		// 						{
		// 							for (int i = 0; i < shapeJson.at("points").size(); i++)
		// 							{
		// 								try
		// 								{
		// 									json pointsJson = shapeJson.at("points").at(i);
		// 									shape->points.push_back(Vector2(JsonHelper::CheckJsonFloat(pointsJson, "xPos", objectName), JsonHelper::CheckJsonFloat(pointsJson, "yPos", objectName)));
		// 								}
		// 								catch (const json::out_of_range& e)
		// 								{
		// 									Logger::log.Err("{}", e.what());
		// 								}
		// 							}
		// 						}						

		// 						shape->b_isLoop = JsonHelper::CheckJsonBool(componentJson, "b_lockedRotation", objectName);
		// 						shape->tangentSpeed = JsonHelper::CheckJsonFloat(componentJson, "linearDamping", objectName);
		// 						shape->rollingResistance = JsonHelper::CheckJsonFloat(componentJson, "linearDamping", objectName);
								
		// 						body->shapes.push_back(shape);
		// 					}
		// 					catch (const json::out_of_range& e)
		// 					{
		// 						Logger::log.Err("{}", e.what());
		// 					}
		// 				}
		// 			}
		// 			prefab.components.emplace(componentID, body);
		// 		}
		// 		else if (type == "TileMap")
		// 		{
		// 			std::shared_ptr<TileMapPrefabData> tileMap = std::make_shared<TileMapPrefabData>();
		// 			tileMap->type = "TileMap";
		// 			tileMap->ownerId = object.ID;
		// 			tileMap->id = componentID;
		// 			tileMap->b_isActive = b_isActive;
		// 			tileMap->b_isCollapsed = b_isCollapsed;
		// 			tileMap->width = JsonHelper::CheckJsonInt(componentJson, "width", objectName);
		// 			tileMap->height = JsonHelper::CheckJsonInt(componentJson, "height", objectName);
		// 			tileMap->tileWidth = JsonHelper::CheckJsonInt(componentJson, "tileWidth", objectName);
		// 			tileMap->tileHeight = JsonHelper::CheckJsonInt(componentJson, "tileHeight", objectName);

		// 			// Get used TileSet names
		// 			if (JsonHelper::JsonContains(componentJson, "tileSets", objectName))
		// 			{
		// 				for (int tileSet = 0; tileSet < componentJson["tileSets"].size(); tileSet++)
		// 				{
		// 					json tileSetJson = componentJson["tileSets"][tileSet];
		// 					tileMap->tileSetNames.push_back(JsonHelper::CheckJsonString(tileSetJson, "name", objectName));
		// 				}
		// 			}
		// 			// Get Tile data
		// 			// if (JsonHelper::JsonContains(componentJson, "tiles", objectName))
		// 			// {
		// 			// 	std::map<int, std::map<int, Tile>> tiles;

		// 			// 	for (int tile = 0; tile < componentJson["tiles"].size(); tile++)
		// 			// 	{
		// 			// 		json tileJson = componentJson["tiles"][tile];
		// 			// 		float x = JsonHelper::CheckJsonFloat(tileJson, "tileCoordX", objectName);
		// 			// 		float y = JsonHelper::CheckJsonFloat(tileJson, "tileCoordY", objectName);
		// 			// 		std::string tileSetName = JsonHelper::CheckJsonString(tileJson, "tileSetName", objectName);
		// 			// 		int tileSetIndex = JsonHelper::CheckJsonInt(tileJson, "tileSetIndex", objectName);
		// 			// 		Vector2 coord = Vector2(x, y);

		// 			// 		if (tileSetName != "" && tileSetIndex != -1)
		// 			// 		{
		// 			// 			TileSet* tileSet = GetTileSet(tileSetName);
		// 			// 			if (tileSet != nullptr)
		// 			// 			{
		// 			// 				VkDescriptorSet texture = tileSet->GetTexture()->GetTexture();
		// 			// 				Vector2 uvStart = tileSet->GetIndexUVs(tileSetIndex).first;
		// 			// 				Vector2 uvEnd = tileSet->GetIndexUVs(tileSetIndex).second;

		// 			// 				Tile newTile = Tile();
		// 			// 				newTile.tileCoord = coord;
		// 			// 				int x = (int)coord.x;
		// 			// 				int y = (int)coord.y;
		// 			// 				newTile.tileSetName = tileSet->GetName();
		// 			// 				newTile.tileSetIndex = tileSetIndex;
		// 			// 				newTile.tileSetTexture = texture;
		// 			// 				newTile.uvStart = uvStart;
		// 			// 				newTile.uvEnd = uvEnd;

		// 			// 				if (tiles.count(x) > 0 && tiles.at(x).count(y) > 0)
		// 			// 				{
		// 			// 					tiles.at(x).at(y) = newTile;
		// 			// 				}
		// 			// 				else if (tiles.count(x) > 0 && tiles.at(x).count(y) == 0)
		// 			// 				{
		// 			// 					std::pair<int, Tile> newPair = { y, newTile };
		// 			// 					tiles.at(x).emplace(newPair);
		// 			// 				}
		// 			// 				else if (tiles.count(x) == 0)
		// 			// 				{
		// 			// 					std::pair<int, Tile> newPair = { y, newTile };
		// 			// 					std::map<int, Tile> yCoords;
		// 			// 					yCoords.emplace(newPair);
		// 			// 					tiles.emplace(x, yCoords);
		// 			// 				}
		// 			// 			}
		// 			// 			else
		// 			// 			{
		// 			// 				Logger::log.Err("TileSet: {} could not be found when initializing prefab: {}", tileSetName, prefab.name);
		// 			// 			}
		// 			// 		}
		// 			// 	}

		// 			// 	tileMap->tiles = tiles;
		// 			// }

		// 			prefab.components.emplace(componentID, tileMap);
		// 		}
		// 		else if (type == "Mesh")
		// 		{
		// 			std::shared_ptr<MeshPrefabData> mesh = std::make_shared<MeshPrefabData>();
		// 			mesh->b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);
		// 			mesh->type = "Mesh";
		// 			mesh->ownerId = object.ID;
		// 			mesh->id = componentID;
		// 			mesh->b_isActive = b_isActive;
		// 			mesh->b_isCollapsed = b_isCollapsed;
		// 			mesh->materialName = JsonHelper::CheckJsonString(componentJson, "materialName", objectName);
		// 			mesh->modelPath = JsonHelper::CheckJsonString(componentJson, "modelPath", objectName);
		// 			mesh->materialName = JsonHelper::CheckJsonString(componentJson, "materialName", objectName);

		// 			if (JsonHelper::JsonContains(componentJson, "textures", objectName))
		// 			{
		// 				json texturesShaderData = componentJson["textures"];

		// 				if (texturesShaderData.size())
		// 				{
		// 					for (auto item = texturesShaderData.begin(); item != texturesShaderData.end(); ++item)
		// 					{
		// 						try
		// 						{
		// 							mesh->texturesByIndex.emplace((uint32_t)std::stoi(item.key()), item.value());
		// 						}
		// 						catch (const json::out_of_range& e)
		// 						{
		// 							Logger::log.Err("{}", e.what());
		// 						}
		// 					}
		// 				}
		// 			}

		// 			std::shared_ptr<Material> material = VulkanManager::vulkan.GetMaterial(mesh->materialName);
		// 			if (material != nullptr)
		// 			{
		// 				if (JsonHelper::JsonContains(componentJson, "uboVec4s", objectName))
		// 				{
		// 					std::map<uint32_t, std::string>& uboVec4Names = material->GetUBOVec4Names();
		// 					for (std::map<uint32_t, std::string>::iterator uboVec4Name = uboVec4Names.begin(); uboVec4Name != uboVec4Names.end(); uboVec4Name++)
		// 					{
		// 						try
		// 						{
		// 							json uboVec4Data = componentJson["uboVec4s"][uboVec4Name->second];
		// 							Vector4 uboVec4 = Vector4(JsonHelper::CheckJsonFloat(uboVec4Data, "x", objectName), JsonHelper::CheckJsonFloat(uboVec4Data, "y", objectName), JsonHelper::CheckJsonFloat(uboVec4Data, "z", objectName), JsonHelper::CheckJsonFloat(uboVec4Data, "w", objectName));
		// 							mesh->uboVec4s.emplace(uboVec4Name->second, uboVec4);
		// 						}
		// 						catch (const json::out_of_range& e)
		// 						{
		// 							Logger::log.Err("{}", e.what());
		// 						}
		// 					}
		// 				}
		// 			}

		// 			prefab.components.emplace(componentID, mesh);
		// 		}
		// 	}

		// 	// Save copy of the root object
		// 	prefab.objects.emplace(object.ID, object);
		// 	if (object.parentID == -1)
		// 	{
		// 		prefab.rootObject = object;
		// 	}
		// }

		void CreatePrefab(std::string path, GameObject gameObject)
		{
			std::string prefabName = FileHelper::GetFilenameFromPath(path);

			std::ofstream file_obj;
			std::ifstream ifstream(path);

			file_obj.open(path, std::ofstream::out | std::ofstream::trunc);
			file_obj.close();

			file_obj.open(path, std::ios::app);

			json prefabObjectJsonArray;

			gameObject.SetName(prefabName);
			gameObject.SetIsPrefab(false);
			gameObject.SetPrefabName("");
			gameObject.SetParentID(-1);
			gameObject.SetPrefabSpawnLocation(Vector3(0, 0, 0));

			// prefabObjectJsonArray.push_back(JsonHelper::CreateJsonFromObject(&gameObject));

			std::vector<long> childIDs = gameObject.GetChildren();
			for (int i = 0; i < childIDs.size(); i++)
			{
				// prefabObjectJsonArray.push_back(JsonHelper::CreateJsonFromObject(SceneManager::loadedScene.GetObjectByID(childIDs[i])));
			}

			json prefabObject = json::object({ { "Prefab", prefabObjectJsonArray }, { "Name", prefabName } });

			file_obj << prefabObject.dump(4).c_str() << std::endl;
			file_obj.close();

			AddPrefab(path);
		}

		// void AddPrefab(std::string path)
		// {
		// 	std::filesystem::path prefabPath(path);

		// 	Prefab prefab;
		// 	prefab.name = FileHelper::GetFilenameFromPath(path);

		// 	json prefabJson = JsonHelper::LoadFileData(prefabPath.string());
		// 	if (prefabJson != NULL)
		// 	{
		// 		auto prefabObjects = prefabJson["Prefab"];

		// 		if (prefabObjects != "NULL")
		// 		{
		// 			for (int i = 0; i < prefabObjects.size(); i++)
		// 			{
		// 				CreatePrefabFromJson(prefabObjects[i], prefab);
		// 			}
		// 		}

		// 		if (prefabs.count(prefabJson["Name"]))
		// 		{
		// 			prefabs.at(prefabJson["Name"]) = prefab;
		// 		}
		// 		else
		// 		{
		// 			prefabs.emplace(prefabJson["Name"], prefab);
		// 		}
		// 	}
		// }

		void InitializePrefabs()
		{
			printf("Initializing prefabs...\n");
			prefabs.clear();
			std::vector<std::string> prefabPaths = FileHelper::FindAllFilesWithExtension(Assets::assetManager.GetDir("projectDir"), ".prf");

			for (std::string path : prefabPaths)
			{
				AddPrefab(path);
			}
			printf("Prefabs initialized.\n");
		}

		// GameObject *Instantiate(std::string prefabName, Vector3 spawnLocation, Scene* scene, long parentID, long ID)
		// {
		// 	GameObject* rootObject = nullptr;

		// 	if (prefabs.count(prefabName) > 0)
		// 	{
		// 		Prefab prefab = prefabs.at(prefabName);
		// 		GameObjectPrefabData root = prefab.rootObject;

		// 		rootObject = InstantiateSelfAndChildren(-1, root.ID, prefab, scene, spawnLocation);
		// 		rootObject->SetParentID(parentID);
		// 	}

		// 	return rootObject;
		// }

		std::map<std::string, Prefab> GetPrefabs()
		{
			return prefabs;
		}
	}
}