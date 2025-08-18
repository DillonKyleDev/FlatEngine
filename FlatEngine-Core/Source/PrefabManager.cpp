#include "PrefabManager.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"
#include "Sprite.h"
#include "CharacterController.h"
#include "Canvas.h"
#include "Button.h"
#include "Text.h"
#include "Camera.h"
#include "Script.h"
#include "TileSet.h"

#include <fstream>


namespace FlatEngine
{
	PrefabManager::PrefabManager()
	{
		m_prefabs = std::map<std::string, Prefab>();
	}

	PrefabManager::~PrefabManager()
	{
	}

	void PrefabManager::CreatePrefabFromJson(json objectJson, Prefab& prefab)
	{
		GameObjectPrefabData object;
		object.name = CheckJsonString(objectJson, "name", "Name");
		std::string objectName = object.name;
		object.b_isActive = CheckJsonBool(objectJson, "_isActive", objectName);
		object.ID = CheckJsonLong(objectJson, "id", objectName);
		object.parentID = CheckJsonLong(objectJson, "parent", objectName);
		TagList tags = TagList(object.ID);

		if (JsonContains(objectJson, "children", objectName))
		{
			for (int c = 0; c < objectJson["children"].size(); c++)
			{
				object.childrenIDs.push_back(objectJson["children"][c]);
			}
		}

		// TagList			
		if (JsonContains(objectJson, "tags", objectName))
		{
			json tagsJson = objectJson["tags"];
			for (json jsonTag : tagsJson)
			{
				std::string tag = jsonTag.items().begin().key();
				bool b_hasTag = jsonTag.items().begin().value();
				tags.SetTag(tag, b_hasTag);
			}
		}
		if (JsonContains(objectJson, "collidesTags", objectName))
		{
			json collidesTagsJson = objectJson["collidesTags"];
			for (json jsonCollidesTag : collidesTagsJson)
			{
				std::string collidesTag = jsonCollidesTag.items().begin().key();
				bool b_collidesTag = jsonCollidesTag.items().begin().value();
				tags.SetCollides(collidesTag, b_collidesTag);
			}
		}

		object.tagList = tags;


		float objectRotation = 0;
		// Loop through the components in this PrefabsJson
		for (int j = 0; j < objectJson["components"].size(); j++)
		{
			json componentJson = objectJson["components"][j];
			std::string type = CheckJsonString(componentJson, "type", objectName);
			long componentID = CheckJsonLong(componentJson, "id", objectName);
			bool b_isCollapsed = CheckJsonBool(componentJson, "_isCollapsed", objectName);
			bool b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);

			// connect this component to the object being saved
			object.componentIDs.push_back(componentID);

			//Add each loaded component to the newly created GameObject
			if (type == "Transform")
			{
				std::shared_ptr<TransformPrefabData> transform = std::make_shared<TransformPrefabData>();
				transform->b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);
				transform->type = "Transform";
				transform->id = componentID;
				transform->b_isActive = b_isActive;
				transform->b_isCollapsed = b_isCollapsed;
				transform->ownerId = object.ID;
				transform->rotation = CheckJsonFloat(componentJson, "rotation", objectName);
				transform->position = Vector2(CheckJsonFloat(componentJson, "xPos", objectName), CheckJsonFloat(componentJson, "yPos", objectName));
				transform->scale = Vector2(CheckJsonFloat(componentJson, "xScale", objectName), CheckJsonFloat(componentJson, "yScale", objectName));
				transform->rotation = CheckJsonFloat(componentJson, "rotation", objectName);
				objectRotation = transform->rotation;

				prefab.components.emplace(componentID, transform);
			}
			else if (type == "Sprite")
			{
				std::shared_ptr<SpritePrefabData> sprite = std::make_shared<SpritePrefabData>();
				sprite->b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);
				sprite->type = "Sprite";
				sprite->id = componentID;
				sprite->b_isActive = b_isActive;
				sprite->b_isCollapsed = b_isCollapsed;
				sprite->ownerId = object.ID;
				sprite->path = CheckJsonString(componentJson, "path", objectName);
				std::string pivotPoint = "Center";
				if (CheckJsonString(componentJson, "pivotPoint", objectName) != "")
				{
					pivotPoint = CheckJsonString(componentJson, "pivotPoint", objectName);
				}
				sprite->pivotPoint = pivotPoint;
				sprite->scale = Vector2(CheckJsonFloat(componentJson, "xScale", objectName), CheckJsonFloat(componentJson, "yScale", objectName));
				sprite->offset = Vector2(CheckJsonFloat(componentJson, "xOffset", objectName), CheckJsonFloat(componentJson, "yOffset", objectName));
				sprite->renderOrder = CheckJsonInt(componentJson, "renderOrder", objectName);
				sprite->tintColor = Vector4(
					CheckJsonFloat(componentJson, "tintColorX", objectName),
					CheckJsonFloat(componentJson, "tintColorY", objectName),
					CheckJsonFloat(componentJson, "tintColorZ", objectName),
					CheckJsonFloat(componentJson, "tintColorW", objectName)
				);

				prefab.components.emplace(componentID, sprite);
			}
			else if (type == "Camera")
			{
				std::shared_ptr<CameraPrefabData> camera = std::make_shared<CameraPrefabData>();
				camera->b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);
				camera->type = "Camera";
				camera->id = componentID;
				camera->b_isActive = b_isActive;
				camera->b_isCollapsed = b_isCollapsed;
				camera->ownerId = object.ID;
				camera->width = CheckJsonFloat(componentJson, "width", objectName);
				camera->height = CheckJsonFloat(componentJson, "height", objectName);
				camera->b_isPrimaryCamera = CheckJsonBool(componentJson, "_isPrimaryCamera", objectName);
				camera->zoom = CheckJsonFloat(componentJson, "zoom", objectName);
				camera->frustrumColor = Vector4(
					CheckJsonFloat(componentJson, "frustrumRed", objectName),
					CheckJsonFloat(componentJson, "frustrumGreen", objectName),
					CheckJsonFloat(componentJson, "frustrumBlue", objectName),
					CheckJsonFloat(componentJson, "frustrumAlpha", objectName)
				);
				camera->b_shouldFollow = CheckJsonBool(componentJson, "_follow", objectName);
				camera->followSmoothing = CheckJsonFloat(componentJson, "followSmoothing", objectName);
				camera->toFollowID = CheckJsonLong(componentJson, "following", objectName);

				prefab.components.emplace(componentID, camera);
			}
			else if (type == "Script")
			{
				std::shared_ptr<ScriptPrefabData> script = std::make_shared<ScriptPrefabData>();
				script->b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);
				script->type = "Script";
				script->id = componentID;
				script->b_isActive = b_isActive;
				script->b_isCollapsed = b_isCollapsed;
				script->ownerId = object.ID;
				script->attachedScript = CheckJsonString(componentJson, "attachedScript", objectName);

				json scriptParamsJson = componentJson["scriptParameters"];

				for (int i = 0; i < scriptParamsJson.size(); i++)
				{
					json param = scriptParamsJson[i];
					Script::S_ScriptParam parameter;
					std::string paramName = CheckJsonString(param, "paramName", objectName);
					parameter.type = CheckJsonString(param, "type", objectName);
					parameter.e_string = CheckJsonString(param, "string", objectName);
					parameter.e_int = CheckJsonInt(param, "int", objectName);
					parameter.e_float = CheckJsonFloat(param, "float", objectName);
					parameter.e_long = CheckJsonLong(param, "long", objectName);
					parameter.e_double = CheckJsonDouble(param, "double", objectName);
					parameter.e_boolean = CheckJsonBool(param, "bool", objectName);
					parameter.e_Vector2 = Vector2(CheckJsonFloat(param, "vector2X", objectName), CheckJsonFloat(param, "vector2Y", objectName));

					script->scriptParams.emplace(paramName, parameter);
				}

				prefab.components.emplace(componentID, script);
			}
			else if (type == "Button")
			{
				std::shared_ptr<ButtonPrefabData> button = std::make_shared<ButtonPrefabData>();
				button->b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);
				button->type = "Button";
				button->ownerId = object.ID;
				button->id = componentID;
				button->b_isActive = b_isActive;
				button->b_isCollapsed = b_isCollapsed;
				button->activeDimensions = Vector2(CheckJsonFloat(componentJson, "activeWidth", objectName), CheckJsonFloat(componentJson, "activeHeight", objectName));
				button->activeOffset = Vector2(CheckJsonFloat(componentJson, "activeOffsetX", objectName), CheckJsonFloat(componentJson, "activeOffsetY", objectName));
				button->activeLayer = CheckJsonInt(componentJson, "activeLayer", objectName);
				button->functionName = CheckJsonString(componentJson, "functionName", objectName);
				button->b_cppEvent = CheckJsonBool(componentJson, "_cppEvent", objectName);
				button->b_luaEvent = CheckJsonBool(componentJson, "_luaEvent", objectName);

				prefab.components.emplace(componentID, button);
			}
			else if (type == "Canvas")
			{
				std::shared_ptr<CanvasPrefabData> canvas = std::make_shared<CanvasPrefabData>();
				canvas->b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);
				canvas->type = "Canvas";
				canvas->ownerId = object.ID;
				canvas->id = componentID;
				canvas->b_isActive = b_isActive;
				canvas->b_isCollapsed = b_isCollapsed;
				canvas->width = CheckJsonFloat(componentJson, "width", objectName);
				canvas->height = CheckJsonFloat(componentJson, "height", objectName);
				canvas->layerNumber = CheckJsonInt(componentJson, "layerNumber", objectName);
				canvas->b_blocksLayers = CheckJsonBool(componentJson, "_blocksLayers", objectName);

				prefab.components.emplace(componentID, canvas);
			}
			else if (type == "Animation")
			{
				std::shared_ptr<AnimationPrefabData> animation = std::make_shared<AnimationPrefabData>();
				animation->b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);
				animation->type = "Animation";
				animation->ownerId = object.ID;
				animation->id = componentID;
				animation->b_isActive = b_isActive;
				animation->b_isCollapsed = b_isCollapsed;

				if (JsonContains(componentJson, "animationData", objectName))
				{
					for (int anim = 0; anim < componentJson["animationData"].size(); anim++)
					{
						json animationJson = componentJson["animationData"][anim];
						std::string path = CheckJsonString(animationJson, "path", objectName);
						std::string name = CheckJsonString(animationJson, "name", objectName);

						Animation::AnimationData animData;
						animData.name = name;
						animData.path = path;

						animation->animations.push_back(animData);
					}
				}

				prefab.components.emplace(componentID, animation);
			}
			else if (type == "Audio")
			{
				std::shared_ptr<AudioPrefabData> audio = std::make_shared<AudioPrefabData>();
				audio->b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);
				audio->type = "Audio";
				audio->ownerId = object.ID;
				audio->id = componentID;
				audio->b_isActive = b_isActive;
				audio->b_isCollapsed = b_isCollapsed;

				if (JsonContains(componentJson, "soundData", objectName))
				{
					for (int sound = 0; sound < componentJson["soundData"].size(); sound++)
					{
						json audioJson = componentJson["soundData"][sound];
						std::string path = CheckJsonString(audioJson, "path", objectName);
						std::string name = CheckJsonString(audioJson, "name", objectName);
						bool b_isMusic = CheckJsonBool(audioJson, "b_isMusic", objectName);

						SoundData soundData;
						soundData.name = name;
						soundData.b_isMusic = b_isMusic;
						soundData.path = path;
						soundData.sound = std::make_shared<Sound>();

						if (soundData.path != "" && soundData.sound != nullptr)
						{
							if (soundData.b_isMusic)
							{
								soundData.sound->LoadMusic(soundData.path);
							}
							else
							{
								soundData.sound->LoadEffect(soundData.path);
							}
						}

						audio->sounds.push_back(soundData);
					}
				}

				prefab.components.emplace(componentID, audio);
			}
			else if (type == "Text")
			{
				std::shared_ptr<TextPrefabData> text = std::make_shared<TextPrefabData>();
				text->b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);
				text->type = "Text";
				text->ownerId = object.ID;
				text->id = componentID;
				text->b_isActive = b_isActive;
				text->b_isCollapsed = b_isCollapsed;
				text->fontPath = CheckJsonString(componentJson, "fontPath", objectName);
				text->fontSize = CheckJsonInt(componentJson, "fontSize", objectName);
				text->color = Vector4(
					CheckJsonFloat(componentJson, "tintColorX", objectName),
					CheckJsonFloat(componentJson, "tintColorY", objectName),
					CheckJsonFloat(componentJson, "tintColorZ", objectName),
					CheckJsonFloat(componentJson, "tintColorW", objectName)
				);
				text->text = CheckJsonString(componentJson, "text", objectName);
				text->offset = Vector2(CheckJsonFloat(componentJson, "xOffset", objectName), CheckJsonFloat(componentJson, "yOffset", objectName));
				text->renderOrder = CheckJsonInt(componentJson, "renderOrder", objectName);

				prefab.components.emplace(componentID, text);
			}
			else if (type == "CharacterController")
			{
				std::shared_ptr<CharacterControllerPrefabData> characterController = std::make_shared<CharacterControllerPrefabData>();
				characterController->b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);
				characterController->type = "CharacterController";
				characterController->ownerId = object.ID;
				characterController->id = componentID;
				characterController->b_isActive = b_isActive;
				characterController->b_isCollapsed = b_isCollapsed;
				characterController->maxSpeed = CheckJsonFloat(componentJson, "maxSpeed", objectName);
				characterController->maxAcceleration = CheckJsonFloat(componentJson, "maxAcceleration", objectName);
				characterController->airControl = CheckJsonFloat(componentJson, "airControl", objectName);

				prefab.components.emplace(componentID, characterController);
			}
			else if (type == "Body")
			{

				std::shared_ptr<BodyPrefabData> body = std::make_shared<BodyPrefabData>();
				body->b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);				
				body->type = "Body";
				body->ownerId = object.ID;
				body->id = componentID;
				body->b_isActive = b_isActive;
				body->b_isCollapsed = b_isCollapsed;
				body->bodyType = (b2BodyType)CheckJsonInt(componentJson, "bodyType", objectName);
				body->b_lockedRotation = CheckJsonBool(componentJson, "_lockedRotation", objectName);
				body->b_lockedXAxis = CheckJsonBool(componentJson, "_lockedXAxis", objectName);
				body->b_lockedYAxis = CheckJsonBool(componentJson, "_lockedYAxis", objectName);				
				body->linearDamping = CheckJsonFloat(componentJson, "linearDamping", objectName);
				body->angularDamping = CheckJsonFloat(componentJson, "angularDamping", objectName);
				body->gravityScale = CheckJsonFloat(componentJson, "gravityScale", objectName);

				if (JsonContains(componentJson, "shapes", objectName))
				{
					for (int i = 0; i < componentJson.at("shapes").size(); i++)
					{
						try
						{
							json shapeJson = componentJson.at("shapes").at(i);
							std::shared_ptr<ShapePrefabData> shape = std::make_shared<ShapePrefabData>();

							shape->b_enableContactEvents = CheckJsonBool(shapeJson, "_enableContactEvents", objectName);;
							shape->b_enableSensorEvents = CheckJsonBool(shapeJson, "_enableSensorEvents", objectName);;
							shape->b_isSensor = CheckJsonBool(shapeJson, "_isSensor", objectName);;
							shape->shape = (Shape::ShapeType)CheckJsonInt(shapeJson, "shape", objectName);
							shape->positionOffset = Vector2(CheckJsonFloat(shapeJson, "xOffset", objectName) , CheckJsonFloat(shapeJson, "yOffset", objectName));
							shape->rotationOffset.c = CheckJsonFloat(shapeJson, "rotationOffsetCos", objectName);
							shape->rotationOffset.s = CheckJsonFloat(shapeJson, "rotationOffsetSin", objectName);
							shape->restitution = CheckJsonFloat(shapeJson, "restitution", objectName);
							shape->density = CheckJsonFloat(shapeJson, "density", objectName);
							shape->friction = CheckJsonFloat(shapeJson, "friction", objectName);
							shape->dimensions = Vector2(CheckJsonFloat(shapeJson, "width", objectName) , CheckJsonFloat(shapeJson, "height", objectName));
							shape->radius = CheckJsonFloat(shapeJson, "radius", objectName);
							shape->capsuleLength = CheckJsonFloat(shapeJson, "capsuleLength", objectName);
							shape->b_horizontal = CheckJsonBool(shapeJson, "_horizontal", objectName);
							shape->cornerRadius = CheckJsonFloat(shapeJson, "cornerRadius", objectName);
							
							if (JsonContains(shapeJson, "points", objectName))
							{
								for (int i = 0; i < shapeJson.at("points").size(); i++)
								{
									try
									{
										json pointsJson = shapeJson.at("points").at(i);
										shape->points.push_back(Vector2(CheckJsonFloat(pointsJson, "xPos", objectName), CheckJsonFloat(pointsJson, "yPos", objectName)));
									}
									catch (const json::out_of_range& e)
									{
										LogError(e.what());
									}
								}
							}						

							shape->b_isLoop = CheckJsonBool(componentJson, "_lockedRotation", objectName);
							shape->tangentSpeed = CheckJsonFloat(componentJson, "linearDamping", objectName);
							shape->rollingResistance = CheckJsonFloat(componentJson, "linearDamping", objectName);
							
							body->shapes.push_back(shape);
						}
						catch (const json::out_of_range& e)
						{
							LogError(e.what());
						}
					}
				}
				prefab.components.emplace(componentID, body);
			}
			else if (type == "TileMap")
			{
				std::shared_ptr<TileMapPrefabData> tileMap = std::make_shared<TileMapPrefabData>();
				tileMap->type = "TileMap";
				tileMap->ownerId = object.ID;
				tileMap->id = componentID;
				tileMap->b_isActive = b_isActive;
				tileMap->b_isCollapsed = b_isCollapsed;
				tileMap->width = CheckJsonInt(componentJson, "width", objectName);
				tileMap->height = CheckJsonInt(componentJson, "height", objectName);
				tileMap->tileWidth = CheckJsonInt(componentJson, "tileWidth", objectName);
				tileMap->tileHeight = CheckJsonInt(componentJson, "tileHeight", objectName);

				// Get used TileSet names
				if (JsonContains(componentJson, "tileSets", objectName))
				{
					for (int tileSet = 0; tileSet < componentJson["tileSets"].size(); tileSet++)
					{
						json tileSetJson = componentJson["tileSets"][tileSet];
						tileMap->tileSetNames.push_back(CheckJsonString(tileSetJson, "name", objectName));
					}
				}
				// Get Tile data
				if (JsonContains(componentJson, "tiles", objectName))
				{
					std::map<int, std::map<int, Tile>> tiles;

					for (int tile = 0; tile < componentJson["tiles"].size(); tile++)
					{
						json tileJson = componentJson["tiles"][tile];
						float x = CheckJsonFloat(tileJson, "tileCoordX", objectName);
						float y = CheckJsonFloat(tileJson, "tileCoordY", objectName);
						std::string tileSetName = CheckJsonString(tileJson, "tileSetName", objectName);
						int tileSetIndex = CheckJsonInt(tileJson, "tileSetIndex", objectName);
						Vector2 coord = Vector2(x, y);

						if (tileSetName != "" && tileSetIndex != -1)
						{
							TileSet* tileSet = GetTileSet(tileSetName);
							if (tileSet != nullptr)
							{
								SDL_Texture* texture = tileSet->GetTexture()->GetTexture();
								Vector2 uvStart = tileSet->GetIndexUVs(tileSetIndex).first;
								Vector2 uvEnd = tileSet->GetIndexUVs(tileSetIndex).second;

								Tile newTile = Tile();
								newTile.tileCoord = coord;
								int x = (int)coord.x;
								int y = (int)coord.y;
								newTile.tileSetName = tileSet->GetName();
								newTile.tileSetIndex = tileSetIndex;
								newTile.tileSetTexture = texture;
								newTile.uvStart = uvStart;
								newTile.uvEnd = uvEnd;

								if (tiles.count(x) > 0 && tiles.at(x).count(y) > 0)
								{
									tiles.at(x).at(y) = newTile;
								}
								else if (tiles.count(x) > 0 && tiles.at(x).count(y) == 0)
								{
									std::pair<int, Tile> newPair = { y, newTile };
									tiles.at(x).emplace(newPair);
								}
								else if (tiles.count(x) == 0)
								{
									std::pair<int, Tile> newPair = { y, newTile };
									std::map<int, Tile> yCoords;
									yCoords.emplace(newPair);
									tiles.emplace(x, yCoords);
								}
							}
							else
							{
								LogError("TileSet: " + tileSetName + " could not be found when initializing prefab: " + prefab.name);
							}
						}
					}

					tileMap->tiles = tiles;
				}

				prefab.components.emplace(componentID, tileMap);
			}
		}

		// Save copy of the root object
		prefab.objects.emplace(object.ID, object);
		if (object.parentID == -1)
		{
			prefab.rootObject = object;
		}
	}

	void PrefabManager::CreatePrefab(std::string path, GameObject gameObject)
	{
		std::string prefabName = GetFilenameFromPath(path);

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
		gameObject.SetPrefabSpawnLocation(Vector2(0, 0));

		prefabObjectJsonArray.push_back(CreateJsonFromObject(gameObject));

		std::vector<long> childIDs = gameObject.GetChildren();
		for (int i = 0; i < childIDs.size(); i++)
		{
			prefabObjectJsonArray.push_back(CreateJsonFromObject(*GetObjectByID(childIDs[i])));
		}

		json prefabObject = json::object({ { "Prefab", prefabObjectJsonArray }, { "Name", prefabName } });

		file_obj << prefabObject.dump(4).c_str() << std::endl;
		file_obj.close();

		AddPrefab(path);
	}

	void PrefabManager::AddPrefab(std::string path)
	{
		std::filesystem::path prefabPath(path);

		Prefab prefab;
		prefab.name = GetFilenameFromPath(path);

		json prefabJson = LoadFileData(prefabPath.string());
		if (prefabJson != NULL)
		{
			auto prefabObjects = prefabJson["Prefab"];

			if (prefabObjects != "NULL")
			{
				for (int i = 0; i < prefabObjects.size(); i++)
				{
					CreatePrefabFromJson(prefabObjects[i], prefab);
				}
			}

			if (m_prefabs.count(prefabJson["Name"]))
			{
				m_prefabs.at(prefabJson["Name"]) = prefab;
			}
			else
			{
				m_prefabs.emplace(prefabJson["Name"], prefab);
			}
		}
	}

	void PrefabManager::InitializePrefabs()
	{
		printf("Initializing prefabs...\n");
		m_prefabs.clear();
		std::vector<std::string> prefabPaths = FindAllFilesWithExtension(GetDir("projectDir"), ".prf");

		for (std::string path : prefabPaths)
		{
			AddPrefab(path);
		}
		printf("Prefabs initialized.\n");
	}

	GameObject* PrefabManager::InstantiateSelfAndChildren(long parentID, long childIDToFind, Prefab prefab, Scene* scene, Vector2 spawnLocation)
	{
		GameObject* self = CreateGameObject(parentID, -1, scene);

		if (prefab.objects.count(childIDToFind) > 0)
		{
			GameObjectPrefabData myData = prefab.objects.at(childIDToFind);

			// Root object settings
			if (parentID == -1)
			{
				self->SetIsPrefab(true);
				self->SetPrefabName(prefab.name);
				self->SetPrefabSpawnLocation(spawnLocation);
			}

			self->SetName(myData.name);
			self->SetActive(myData.b_isActive);
			self->SetTagList(myData.tagList);
			self->GetTagList().SetOwnerID(self->GetID());

			for (long componentID : myData.componentIDs)
			{
				if (prefab.components.count(componentID) > 0)
				{
					if (prefab.components.at(componentID)->type == "Transform")
					{
						std::shared_ptr<TransformPrefabData> transformData = std::static_pointer_cast<TransformPrefabData>(prefab.components.at(componentID));
						Transform* transform = self->GetTransform();
						transform->SetActive(transformData->b_isActive);
						transform->SetCollapsed(transformData->b_isCollapsed);
						transform->SetScale(transformData->scale);
						transform->SetRotation(transformData->rotation);						

						if (parentID != -1)
						{
							Vector2 parentPosition = GetObjectByID(parentID)->GetTransform()->GetAbsolutePosition();							
							transform->SetPosition(transformData->position);
						}
						else
						{							
							transform->SetPosition(spawnLocation);
						}
					}
					else if (prefab.components.at(componentID)->type == "Sprite")
					{
						std::shared_ptr<SpritePrefabData> spriteData = std::static_pointer_cast<SpritePrefabData>(prefab.components.at(componentID));
						Sprite* sprite = self->AddSprite(-1, spriteData->b_isActive, spriteData->b_isCollapsed);
						sprite->SetTexture(spriteData->path);
						if (spriteData->path != "" && !DoesFileExist(spriteData->path))
						{
							LogError("Sprite file not found for Prefab: \"" + prefab.name + "\". This may lead to unexpected behavior.  \npath: " + spriteData->path);
						}
						sprite->SetScale(spriteData->scale);
						sprite->SetPivotPoint(spriteData->pivotPoint);
						sprite->SetOffset(spriteData->offset);
						sprite->SetRenderOrder(spriteData->renderOrder);
						sprite->SetTintColor(spriteData->tintColor);
					}
					else if (prefab.components.at(componentID)->type == "Camera")
					{
						std::shared_ptr<CameraPrefabData> cameraData = std::static_pointer_cast<CameraPrefabData>(prefab.components.at(componentID));
						Camera* camera = self->AddCamera(-1, cameraData->b_isActive, cameraData->b_isCollapsed);
						camera->SetDimensions(cameraData->width, cameraData->height);
						camera->SetZoom(cameraData->zoom);
						camera->SetFrustrumColor(cameraData->frustrumColor);
						camera->SetPrimaryCamera(cameraData->b_isPrimaryCamera);
						camera->SetToFollowID(cameraData->toFollowID);
						camera->SetShouldFollow(cameraData->b_shouldFollow);
						camera->SetFollowSmoothing(cameraData->followSmoothing);
					}
					else if (prefab.components.at(componentID)->type == "Script")
					{
						std::shared_ptr<ScriptPrefabData> scriptData = std::static_pointer_cast<ScriptPrefabData>(prefab.components.at(componentID));
						Script* script = self->AddScript(-1, scriptData->b_isActive, scriptData->b_isCollapsed);
						script->SetAttachedScript(scriptData->attachedScript);
						script->SetScriptParams(scriptData->scriptParams);
					}
					else if (prefab.components.at(componentID)->type == "Button")
					{
						std::shared_ptr<ButtonPrefabData> buttonData = std::static_pointer_cast<ButtonPrefabData>(prefab.components.at(componentID));
						Button* button = self->AddButton(-1, buttonData->b_isActive, buttonData->b_isCollapsed);
						button->SetActiveDimensions(buttonData->activeDimensions.x, buttonData->activeDimensions.y);
						button->SetActiveOffset(buttonData->activeOffset);
						button->SetActiveLayer(buttonData->activeLayer);
						button->SetFunctionName(buttonData->functionName);
						button->SetIsCPP(buttonData->b_cppEvent);
						button->SetIsLua(buttonData->b_luaEvent);
					}
					else if (prefab.components.at(componentID)->type == "Canvas")
					{
						std::shared_ptr<CanvasPrefabData> canvasData = std::static_pointer_cast<CanvasPrefabData>(prefab.components.at(componentID));
						Canvas* canvas = self->AddCanvas(-1, canvasData->b_isActive, canvasData->b_isCollapsed);
						canvas->SetLayerNumber(canvasData->layerNumber);
						canvas->SetBlocksLayers(canvasData->b_blocksLayers);
						canvas->SetDimensions(canvasData->width, canvasData->height);
					}
					else if (prefab.components.at(componentID)->type == "Animation")
					{
						std::shared_ptr<AnimationPrefabData> animationData = std::static_pointer_cast<AnimationPrefabData>(prefab.components.at(componentID));
						Animation* animation = self->AddAnimation(-1, animationData->b_isActive, animationData->b_isCollapsed);

						for (Animation::AnimationData animData : animationData->animations)
						{
							animation->AddAnimation(animData.name, animData.path);
							if (!DoesFileExist(animData.path))
							{
								LogError("Animation file not found for Prefab: \"" + prefab.name + "\" - on Animation: \"" + animData.name + "\". This may lead to unexpected behavior.  \npath: " + animData.path);
							}
						}
					}
					else if (prefab.components.at(componentID)->type == "Audio")
					{
						std::shared_ptr<AudioPrefabData> audioData = std::static_pointer_cast<AudioPrefabData>(prefab.components.at(componentID));
						Audio* audio = self->AddAudio(-1, audioData->b_isActive, audioData->b_isCollapsed);
						audio->SetSounds(audioData->sounds);
						for (SoundData sound : audioData->sounds)
						{
							if (!DoesFileExist(sound.path))
							{
								LogError("Audio file not found for Prefab: \"" + prefab.name + "\" - on Audio: \"" + sound.name + "\". This may lead to unexpected behavior.  \npath: " + sound.path);
							}
						}
					}
					else if (prefab.components.at(componentID)->type == "Text")
					{
						std::shared_ptr<TextPrefabData> textData = std::static_pointer_cast<TextPrefabData>(prefab.components.at(componentID));
						Text* text = self->AddText(-1, textData->b_isActive, textData->b_isCollapsed);
						if (!DoesFileExist(textData->fontPath))
						{
							LogError("Font file not found for Prefab: \"" + prefab.name + "\". This may lead to unexpected behavior.  \npath: " + textData->fontPath);
						}
						text->SetFontPath(textData->fontPath);
						text->SetFontSize(textData->fontSize);
						text->SetColor(textData->color);
						text->SetText(textData->text);
						text->SetOffset(textData->offset);
						text->SetRenderOrder(textData->renderOrder);
						text->LoadText();
					}
					else if (prefab.components.at(componentID)->type == "Body")
					{
						std::shared_ptr<BodyPrefabData> bodyData = std::static_pointer_cast<BodyPrefabData>(prefab.components.at(componentID));					
						Physics::BodyProps bodyProps;
						bodyProps.type = bodyData->bodyType;						;
						bodyProps.b_lockedRotation = bodyData->b_lockedRotation;
						bodyProps.b_lockedXAxis = bodyData->b_lockedXAxis;
						bodyProps.b_lockedYAxis = bodyData->b_lockedYAxis;
						bodyProps.linearDamping = bodyData->linearDamping;
						bodyProps.angularDamping = bodyData->angularDamping;
						bodyProps.gravityScale = bodyData->gravityScale;

						Body* body = self->AddBody(bodyProps, -1, bodyData->b_isActive, bodyData->b_isCollapsed);

						for (std::shared_ptr<ShapePrefabData> shapeData : bodyData->shapes)
						{
							Shape::ShapeProps shapeProps;
							shapeProps.shape = shapeData->shape;
							shapeProps.b_enableContactEvents = shapeData->b_enableContactEvents;
							shapeProps.b_enableSensorEvents = shapeData->b_enableSensorEvents;
							shapeProps.b_isSensor = shapeData->b_isSensor;
							shapeProps.positionOffset = shapeData->positionOffset;
							shapeProps.rotationOffset = shapeData->rotationOffset;
							shapeProps.restitution = shapeData->restitution;
							shapeProps.density = shapeData->density;
							shapeProps.friction = shapeData->friction;
							shapeProps.dimensions = shapeData->dimensions;
							shapeProps.radius = shapeData->radius;
							shapeProps.capsuleLength = shapeData->capsuleLength;
							shapeProps.b_horizontal = shapeData->b_horizontal;
							shapeProps.cornerRadius = shapeData->cornerRadius;
							shapeProps.points = shapeData->points;
							shapeProps.b_isLoop = shapeData->b_isLoop;
							shapeProps.tangentSpeed = shapeData->tangentSpeed;
							shapeProps.rollingResistance = shapeData->rollingResistance;

							switch (shapeProps.shape)
							{
							case Shape::BS_Box:
								body->AddBox(shapeProps);
								break;
							case Shape::BS_Circle:
								body->AddCircle(shapeProps);
								break;
							case Shape::BS_Capsule:
								body->AddCapsule(shapeProps);
								break;
							case Shape::BS_Polygon:
								body->AddPolygon(shapeProps);
								break;
							case Shape::BS_Chain:
								body->AddChain(shapeProps);
								break;
							}
						}
					}
					else if (prefab.components.at(componentID)->type == "CharacterController")
					{
						std::shared_ptr<CharacterControllerPrefabData> characterControllerData = std::static_pointer_cast<CharacterControllerPrefabData>(prefab.components.at(componentID));
						CharacterController* characterController = self->AddCharacterController(-1, characterControllerData->b_isActive, characterControllerData->b_isCollapsed);
						characterController->SetMaxAcceleration(characterControllerData->maxAcceleration);
						characterController->SetMaxSpeed(characterControllerData->maxSpeed);
						characterController->SetAirControl(characterControllerData->airControl);
					}
					else if (prefab.components.at(componentID)->type == "TileMap")
					{
						std::shared_ptr<TileMapPrefabData> tileMapData = std::static_pointer_cast<TileMapPrefabData>(prefab.components.at(componentID));
						TileMap* tileMap = self->AddTileMap(-1, tileMapData->b_isActive, tileMapData->b_isCollapsed);
						tileMap->SetWidth(tileMapData->width);
						tileMap->SetHeight(tileMapData->height);
						tileMap->SetTileWidth(tileMapData->tileWidth);
						tileMap->SetTileHeight(tileMapData->tileHeight);
						tileMap->SetTileSets(tileMapData->tileSetNames);
						tileMap->SetTiles(tileMapData->tiles);

						for (std::pair<std::string, std::vector<std::pair<Vector2, Vector2>>> collisionAreaPair : tileMapData->collisionAreas)
						{
							std::string areaName = collisionAreaPair.first;
							tileMap->SetCollisionAreaValues(areaName, collisionAreaPair.second);
						}
					}
				}
			}

			for (long prefabChildID : myData.childrenIDs)
			{
				GameObject* child = InstantiateSelfAndChildren(self->GetID(), prefabChildID, prefab, scene);
				child->SetIsPrefabChild(true);
			}

			// After all components are initialized, activate the script components if there are any
			if (GameLoopStarted())
			{
				if (self->IsPersistant())
				{
					for (Script* script : self->GetScripts())
					{
						if (InitLuaScript(script, F_LoadedPersistantScriptFiles))
						{
							RunLuaFuncOnSingleScript(script, "Awake");
						}
					}
					for (Script* script : self->GetScripts())
					{
						if (InitLuaScript(script, F_LoadedPersistantScriptFiles))
						{
							RunLuaFuncOnSingleScript(script, "Start");
						}
					}
				}
				else
				{
					for (Script* script : self->GetScripts())
					{
						if (InitLuaScript(script, F_LoadedSceneScriptFiles))
						{
							RunLuaFuncOnSingleScript(script, "Awake");
						}
					}
					for (Script* script : self->GetScripts())
					{
						if (InitLuaScript(script, F_LoadedSceneScriptFiles))
						{
							RunLuaFuncOnSingleScript(script, "Start");
						}
					}
				}
			}
		}

		return self;
	}

	GameObject *PrefabManager::Instantiate(std::string prefabName, Vector2 spawnLocation, Scene* scene, long parentID, long ID)
	{
		GameObject* rootObject = nullptr;

		if (m_prefabs.count(prefabName) > 0)
		{
			Prefab prefab = m_prefabs.at(prefabName);
			GameObjectPrefabData root = prefab.rootObject;

			rootObject = InstantiateSelfAndChildren(-1, root.ID, prefab, scene, spawnLocation);
			rootObject->SetParentID(parentID);
		}

		return rootObject;
	}

	std::map<std::string, Prefab> PrefabManager::GetPrefabs()
	{
		return m_prefabs;
	}
}