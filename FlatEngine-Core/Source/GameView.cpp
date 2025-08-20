#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"
#include "Scene.h"
#include "Sprite.h"
#include "Text.h"
#include "Animation.h"
#include "Button.h"
#include "Canvas.h"
#include "Texture.h"
#include "TileMap.h"
#include "TileSet.h"
#include "Project.h"
#include "Vector4.h"

#include "imgui.h"


namespace FL = FlatEngine;

using Button = FL::Button;
using Transform = FL::Transform;
using Sprite = FL::Sprite;
using Text = FL::Text;
using Texture = FL::Texture;


namespace FlatEngine
{
	// Game view default values
	float F_GAME_VIEWPORT_WIDTH = 600;
	float F_GAME_VIEWPORT_HEIGHT = 400;
	float F_xGameCenter = 600 / 2;
	float F_yGameCenter = 400 / 2;
	Vector2 F_gameViewCenter = Vector2(0, 0);
	Vector2 F_gameViewGridStep = Vector2(50, 50);


	void Game_RenderView(bool b_inRuntime)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_None;

		// If Release - Make GameView full screen and disable tab decoration and resizing
		if (b_inRuntime)
		{			
			ImGuiIO& inputOutput = ImGui::GetIO();
			float xSize = inputOutput.DisplaySize.x;
			float ySize = inputOutput.DisplaySize.y;
			ImGui::SetNextWindowSize(Vector2(xSize, ySize));
			ImGui::SetNextWindowPos(Vector2(0, 0));
			flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		FL::PushWindowStyles();
		ImGui::Begin("Game View", 0, flags);
		// {
		
			static bool opt_enable_context_menu = true;

			Vector2 canvasP0 = ImGui::GetCursorScreenPos();
			Vector2 canvasSize = ImGui::GetContentRegionAvail();
			if (canvasSize.x < 50.0f) canvasSize.x = 50.0f;
			if (canvasSize.y < 50.0f) canvasSize.y = 50.0f;
			Vector2 canvas_p1 = Vector2(canvasP0.x + canvasSize.x, canvasP0.y + canvasSize.y);

			// Set viewport dimensions for rendering objects in game view. We want this to always be centered in game view so we can get it every frame.
			F_GAME_VIEWPORT_WIDTH = canvas_p1.x - canvasP0.x + 1;
			F_GAME_VIEWPORT_HEIGHT = canvas_p1.y - canvasP0.y + 1;

			// Render GameObjects in game view
			Game_RenderObjects(canvasP0, canvasSize);

		// }
		ImGui::End();
		FL::PopWindowStyles();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}

	void Game_RenderObjects(Vector2 canvasP0, Vector2 canvasSize)
	{		
		Scene* loadedScene = FL::GetLoadedScene();
		std::map<long, GameObject> sceneObjects;
		Scene* persistantObjectScene = GetLoadedProject().GetPersistantGameObjectScene();
		std::map<long, GameObject> persistantObjects = std::map<long, GameObject>();
		Camera* primaryCamera = GetPrimaryCamera();
		Transform* cameraTransform = nullptr;		

		if (loadedScene != nullptr)
		{
			sceneObjects = loadedScene->GetSceneObjects();			
		}
		else
		{
			sceneObjects = std::map<long, GameObject>();
		}

		if (persistantObjectScene != nullptr)
		{
			persistantObjects = persistantObjectScene->GetSceneObjects();
		}
		
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();
		// 3 channels for now in this scene view. 0 = scene objects, 1 = other UI (camera icon, etc), 2 = transform arrow
		drawSplitter->Split(drawList, F_maxSpriteLayers + 5);

		Vector2 cameraPosition(0, 0);
		float cameraWidth = 50;
		float cameraHeight = 30;		
		ImVec4 frustrumColor = ImVec4(1, 1, 1, 1);	
		float cameraStartTime = (float)FL::GetEngineTime();
		
		if (primaryCamera != nullptr)
		{
			GameObject* owner = primaryCamera->GetParent();
			if (owner != nullptr)
			{
				cameraTransform = owner->GetTransform();
			}
			cameraWidth = primaryCamera->GetWidth();
			cameraHeight = primaryCamera->GetHeight();
			F_gameViewGridStep.x = primaryCamera->GetZoom();
			F_gameViewGridStep.y = primaryCamera->GetZoom();
			frustrumColor = primaryCamera->GetFrustrumColor();
			
			if (cameraTransform != nullptr)
			{
				cameraPosition = cameraTransform->GetAbsolutePosition();
			}
			else
			{
				cameraPosition = Vector2(0, 0);
			}
		}
		
		F_gameViewCenter = Vector2((F_GAME_VIEWPORT_WIDTH / 2) - (cameraPosition.x * F_gameViewGridStep.x) + canvasP0.x, (F_GAME_VIEWPORT_HEIGHT / 2) + (cameraPosition.y * F_gameViewGridStep.x) + canvasP0.y);
		Vector2 viewportCenterPoint = Vector2((F_GAME_VIEWPORT_WIDTH / 2) + canvasP0.x, (F_GAME_VIEWPORT_HEIGHT / 2) + canvasP0.y);
		
		float renderStartTime = 0;
		renderStartTime = (float)FL::GetEngineTime();

		for (std::map<long, GameObject>::iterator iter = sceneObjects.begin(); iter != sceneObjects.end();)
		{
			if (iter->second.IsActive())
			{
				Game_RenderObject(iter->second, canvasP0, canvasSize, drawList, drawSplitter, cameraPosition, cameraWidth, cameraHeight);
			}

			iter++;
		}
		for (std::map<long, GameObject>::iterator iter = persistantObjects.begin(); iter != persistantObjects.end();)
		{
			if (iter->second.IsActive())
			{
				Game_RenderObject(iter->second, canvasP0, canvasSize, drawList, drawSplitter, cameraPosition, cameraWidth, cameraHeight);
			}

			iter++;
		}

		drawSplitter->Merge(drawList);
	}

	void Game_RenderObject(GameObject self, Vector2 canvasP0, Vector2 canvasSize, ImDrawList* drawList, ImDrawListSplitter* drawSplitter, Vector2 cameraPosition, float cameraWidth, float cameraHeight)
	{
		FL::Transform* transform = self.GetTransform();
		Sprite* sprite = self.GetSprite();
		TileMap* tileMap = self.GetTileMap();
		Animation* animation = self.GetAnimation();
		Text* text = self.GetText();
		Button* button = self.GetButton();
		Canvas* canvas = self.GetCanvas();
		Body* body = self.GetBody();

		if (transform != nullptr && transform->IsActive())
		{
			Vector2 position = transform->GetAbsolutePosition();
			Vector2 scale = transform->GetAbsoluteScale();
			float rotation = transform->GetAbsoluteRotation();

			float cameraLeftEdge = cameraPosition.x - cameraWidth / 2;
			float cameraRightEdge = cameraPosition.x + cameraWidth / 2;
			float cameraTopEdge = cameraPosition.y + cameraHeight / 2;
			float cameraBottomEdge = cameraPosition.y - cameraHeight / 2;
			
			// I may want to find somewhere else to do this that is not dependent on a view.
			if (animation != nullptr && animation->IsActive())
			{
				for (Animation::AnimationData animData : animation->GetAnimations())
				{
					if (animData.b_playing)
					{
						animation->PlayAnimation(animData.name, F_Application->GetGameLoop()->TimeEllapsedInMs());
					}
				}
			}

			if (sprite != nullptr && sprite->GetTexture() != nullptr && sprite->IsActive())
			{
				SDL_Texture* spriteTexture = sprite->GetTexture();
				float textureWidth = (float)sprite->GetTextureWidth();
				float textureHeight = (float)sprite->GetTextureHeight();
				Vector2 spriteScale = sprite->GetScale();
				Vector2 offset = sprite->GetOffset();
				bool b_scalesWithZoom = true;
				int renderOrder = sprite->GetRenderOrder();
				Vector4 tintColor = sprite->GetTintColor();
				Vector2 newScale = Vector2(scale.x * spriteScale.x * F_spriteScaleMultiplier, scale.y * spriteScale.y * F_spriteScaleMultiplier);

				float spriteLeftEdge = position.x - offset.x * newScale.x;
				float spriteRightEdge = position.x + offset.x * newScale.x;
				float spriteTopEdge = position.y + offset.y * newScale.y;
				float spriteBottomEdge = position.y - offset.y * newScale.y;

				bool b_isIntersecting = false;

				if (spriteLeftEdge < cameraRightEdge && spriteRightEdge > cameraLeftEdge && spriteTopEdge > cameraBottomEdge && spriteBottomEdge < cameraTopEdge)
				{
					b_isIntersecting = true;
				}

				if (b_isIntersecting)
				{
					if (renderOrder <= F_maxSpriteLayers && renderOrder >= 0)
					{
						drawSplitter->SetCurrentChannel(drawList, renderOrder);
					}
					else
					{
						drawSplitter->SetCurrentChannel(drawList, 0);
					}
				
					spriteScale.x *= scale.x;
					spriteScale.y *= scale.y;

					if (spriteScale.x > 0 && spriteScale.y > 0 && spriteTexture != nullptr)
					{
						AddImageToDrawList(spriteTexture, position, F_gameViewCenter, textureWidth, textureHeight, offset, spriteScale, b_scalesWithZoom, F_gameViewGridStep.x, drawList, rotation, ImGui::GetColorU32(tintColor));
					}
				}
			}

			if (text != nullptr && text->IsActive())
			{
				std::shared_ptr<Texture> textTexture = text->GetTexture();
				float textWidth = (float)textTexture->GetWidth();
				float textHeight = (float)textTexture->GetHeight();				
				int renderOrder = text->GetRenderOrder();
				Vector2 offset = text->GetOffset();
				bool b_spriteScalesWithZoom = true;
				Vector4 tintColor = text->GetColor();
				Vector2 newScale = Vector2(scale.x * F_spriteScaleMultiplier, scale.y * F_spriteScaleMultiplier);

				float spriteLeftEdge = position.x - offset.x * newScale.x;
				float spriteRightEdge = position.x + offset.x * newScale.x;
				float spriteTopEdge = position.y + offset.y * newScale.y;
				float spriteBottomEdge = position.y - offset.y * newScale.y;

				bool b_isIntersecting = false;

				if (spriteLeftEdge < cameraRightEdge && spriteRightEdge > cameraLeftEdge && spriteTopEdge > cameraBottomEdge && spriteBottomEdge < cameraTopEdge)
				{
					b_isIntersecting = true;
				}

				if (b_isIntersecting)
				{
					if (textTexture->GetTexture() != nullptr)
					{
						if (renderOrder <= F_maxSpriteLayers && renderOrder >= 0)
						{
							drawSplitter->SetCurrentChannel(drawList, renderOrder);
						}
						else
						{
							drawSplitter->SetCurrentChannel(drawList, 0);
						}

						AddImageToDrawList(textTexture->GetTexture(), position, F_gameViewCenter, textWidth, textHeight, offset, newScale, b_spriteScalesWithZoom, F_gameViewGridStep.x, drawList, rotation, ImGui::GetColorU32(tintColor));
					}
				}
			}

			if (tileMap != nullptr && tileMap->IsActive())
			{
				long id = tileMap->GetID();
				bool b_isActive = tileMap->IsActive();
				float width = (float)tileMap->GetWidth();							// in tiles
				float height = (float)tileMap->GetHeight();							// in tiles
				float tileWidth = (float)tileMap->GetTileWidth();
				float tileHeight = (float)tileMap->GetTileHeight();
				float gridWidth = width * tileWidth / FL::F_pixelsPerGridSpace;		// in grid tiles
				float gridHeight = height * tileHeight / FL::F_pixelsPerGridSpace;	// in grid tiles
				int renderOrder = tileMap->GetRenderOrder();

				std::map<int, std::map<int, Tile>> tiles = tileMap->GetTiles();
				for (int w = 0; w < width; w++)
				{
					if (tiles.count((int)w) > 0)
					{
						for (int h = 0; h < height; h++)
						{
							if (tiles.at((int)w).count((int)h) > 0)
							{
								Tile tile = tiles.at(w).at(h);

								// Get TileSet for this tiles texture data
								TileSet* usedTileSet = nullptr;
								std::string tileSetName = tile.tileSetName;

								if (tileSetName != "")
								{
									usedTileSet = FL::GetTileSet(tileSetName);
								}

								SDL_Texture* texture = tile.tileSetTexture;
								int textureWidth = usedTileSet->GetTexture()->GetWidth();
								int textureHeight = usedTileSet->GetTexture()->GetHeight();
								Vector2 uvStart = Vector2(tile.uvStart.x / textureWidth, tile.uvStart.y / textureHeight);
								Vector2 uvEnd = Vector2(tile.uvEnd.x / textureWidth, tile.uvEnd.y / textureHeight);
								float gridXPosition = (position.x - (gridWidth / 2)) + 2 * (float)w;
								float gridYPosition = (position.y + (gridHeight / 2)) - 2 * (float)h;
								Vector2 tilePosition = Vector2(gridXPosition, gridYPosition);

								if (renderOrder <= F_maxSpriteLayers && renderOrder >= 0)
								{
									drawSplitter->SetCurrentChannel(drawList, renderOrder);
								}
								else
								{
									drawSplitter->SetCurrentChannel(drawList, 0);
								}

								FL::AddImageToDrawList(texture, tilePosition, F_gameViewCenter, tileWidth, tileHeight, Vector2(0, 0), scale, true, F_gameViewGridStep.x, drawList, 0, FL::GetColor32("white"), uvStart, uvEnd);
							}
						}
					}
				}
			}

			if (button != nullptr && button->IsActive())
			{
				float activeWidth = button->GetActiveWidth();
				float activeHeight = button->GetActiveHeight();
				Vector2 activeOffset = button->GetActiveOffset();				
				Vector4 activeEdges = button->GetActiveEdges();

				float activeTop = activeEdges.x;
				float activeRight = activeEdges.y;
				float activeBottom = activeEdges.z;
				float activeLeft = activeEdges.w;

				// Active Edges depends on gameViewCenter, which can change with every call to Game_RenderView(), so we recalculate
				button->CalculateActiveEdges();

				// For drawing border in game view //
				//Vector2 topLeft = { activeLeft, activeTop };
				//Vector2 bottomRight = { activeRight, activeBottom };

				//drawSplitter->SetCurrentChannel(drawList, F_maxSpriteLayers + 2);

				//if (_isActive)
				//	FL::DrawRectangle(topLeft, bottomRight, canvasP0, canvasSize, FL::GetColor("buttonComponentActive"), 3.0f, drawList);
				//else
				//	FL::DrawRectangle(topLeft, bottomRight, canvasP0, canvasSize, FL::GetColor("buttonComponentInactive"), 3.0f, drawList);
			}

			if (canvas != nullptr && canvas->IsActive())
			{
				// Active Edges depends on gameViewCenter, which can change with every call to Game_RenderView(), so we recalculate
				canvas->CalculateActiveEdges();
			}

			if (body != nullptr)
			{
				std::list<Box>& boxes = body->GetBoxes();
				std::list<Circle> circles = body->GetCircles();
				std::list<Capsule>& capsules = body->GetCapsules();
				std::list<FL::Polygon>& polygons = body->GetPolygons();
				std::list<Chain>& chains = body->GetChains();

				for (Box& box : boxes)
				{
					bool b_isActive = body->IsActive();
					bool b_drawBoxInGame = box.DrawInGame();

					if (b_drawBoxInGame && b_isActive)
					{
						FL::Physics::BodyProps bodyProps = body->GetBodyProps();
						Shape::ShapeProps shapeProps = box.GetShapeProps();
						Vector4 drawColor = box.GetInGameDrawColor();
						float thickness = box.GetInGameDraThickness();
						
						box.UpdateCorners();

						std::vector<Vector2> cornersVec = box.GetCornersScreen();
						Vector2 corners[4] = {
							cornersVec[0],
							cornersVec[1],
							cornersVec[2],
							cornersVec[3]
						};

						drawSplitter->SetCurrentChannel(drawList, FL::F_maxSpriteLayers + 2);

						FL::DrawRectangleFromLines(corners, drawColor, thickness, drawList);
						FL::DrawRectangleFromLines(corners, Vector4(drawColor.x, drawColor.y, drawColor.z, 0.35f), thickness + 2, drawList);
					}
				}

				for (Circle& circle : circles)
				{
					bool b_isActive = body->IsActive();
					bool b_drawBoxInGame = circle.DrawInGame();

					if (b_drawBoxInGame && b_isActive)
					{
						Vector4 drawColor = circle.GetInGameDrawColor();
						float thickness = circle.GetInGameDraThickness();
						FL::Physics::BodyProps bodyProps = body->GetBodyProps();
						Shape::ShapeProps shapeProps = circle.GetShapeProps();
						bool b_isSensor = shapeProps.b_isSensor;
						float radius = shapeProps.radius * F_gameViewGridStep.x;
						Vector2 offset = shapeProps.positionOffset;
						Vector2 center = ConvertWorldToScreen(position + Vector2::Rotate(offset, rotation));

						drawSplitter->SetCurrentChannel(drawList, FL::F_maxSpriteLayers + 2);

						FL::DrawCircle(center, radius, Vector4(drawColor.x, drawColor.y, drawColor.z, 0.35f), drawList, thickness + 2);
						FL::DrawCircle(center, radius, drawColor, drawList);										
					}
				}

				for (Capsule& capsule : capsules)
				{
					bool b_isActive = body->IsActive();
					bool b_drawBoxInGame = capsule.DrawInGame();

					if (b_drawBoxInGame && b_isActive)
					{
						Vector4 color = capsule.GetInGameDrawColor();
						Vector4 colorLight = Vector4(color.x, color.y, color.z, 0.35f);
						float thickness = capsule.GetInGameDraThickness();
						FL::Physics::BodyProps bodyProps = body->GetBodyProps();
						Shape::ShapeProps shapeProps = capsule.GetShapeProps();
						bool b_isSensor = shapeProps.b_isSensor;
						b2Capsule capsuleShape = b2Shape_GetCapsule(capsule.GetShapeID());
						float length = shapeProps.capsuleLength;
						float radius = shapeProps.radius;
						float radiusScreen = radius * F_gameViewGridStep.x;
						Vector2 offset = shapeProps.positionOffset;
						float rotation = FL::RadiansToDegrees(b2Rot_GetAngle(shapeProps.rotationOffset));

						Vector2 center1 = ConvertWorldToScreen(Vector2(b2Body_GetWorldPoint(body->GetBodyID(), capsuleShape.center1)));
						Vector2 center2 = ConvertWorldToScreen(Vector2(b2Body_GetWorldPoint(body->GetBodyID(), capsuleShape.center2)));
						Vector2 difference = center2 - center1;
						Vector2 diffN = Vector2::Normalize(difference);
						Vector2 diffNR = diffN * radiusScreen;
						Vector2 diffPerp = Vector2::Rotate(diffNR, 90);
						Vector2 flippedDiffPerp = Vector2::Rotate(diffNR, -90);

						drawSplitter->SetCurrentChannel(drawList, FL::F_maxSpriteLayers + 2);

						FL::DrawCircle(center1, radiusScreen, colorLight, drawList, thickness + 2.0f);
						FL::DrawCircle(center1, radiusScreen, color, drawList);

						FL::DrawCircle(center2, radiusScreen, colorLight, drawList, thickness + 2.0f);
						FL::DrawCircle(center2, radiusScreen, color, drawList);

						FL::DrawLine(center1 - diffNR, center1 + diffNR, colorLight, thickness + 2.0f, drawList);
						FL::DrawLine(center2 - diffNR, center2 + diffNR, colorLight, thickness + 2.0f, drawList);
						FL::DrawLine(center1 - diffPerp, center1 + diffPerp, colorLight, thickness + 2.0f, drawList);
						FL::DrawLine(center2 - diffPerp, center2 + diffPerp, colorLight, thickness + 2.0f, drawList);

						// Sides
						FL::DrawLine(center1 + diffPerp, center1 + diffPerp + difference, colorLight, thickness + 2.0f, drawList);
						FL::DrawLine(center1 + diffPerp, center1 + diffPerp + difference, color, thickness, drawList);

						FL::DrawLine(center1 + flippedDiffPerp, center1 + flippedDiffPerp + difference, colorLight, thickness + 2.0f, drawList);
						FL::DrawLine(center1 + flippedDiffPerp, center1 + flippedDiffPerp + difference, color, thickness, drawList);
					}
				}

				for (FL::Polygon& polygon : polygons)
				{					
					bool b_isActive = body->IsActive();
					bool b_drawBoxInGame = polygon.DrawInGame();

					if (b_drawBoxInGame && b_isActive)
					{
						Vector4 color = polygon.GetInGameDrawColor();
						Vector4 colorLight = Vector4(color.x, color.y, color.z, 0.35f);
						float thickness = polygon.GetInGameDraThickness();
						FL::Physics::BodyProps& bodyProps = body->GetBodyProps();
						Shape::ShapeProps& shapeProps = polygon.GetShapeProps();
						bool b_isSensor = shapeProps.b_isSensor;
						bool b_isLoop = shapeProps.b_isLoop;
						std::vector<Vector2>& points = shapeProps.points;
						int pointCount = (int)points.size();
						float cornerRadius = shapeProps.cornerRadius;
						int minPolygonBodyVertices = 3;
						int maxPolygonBodyVertices = 8;
						bool b_editingPoints = polygon.IsEditingPoints();

						for (int i = 0; i < pointCount; i++)
						{
							Vector2 rPerpStart = points[i] + Vector2::Rotate(Vector2::Normalize(points[FL::Fmod(i + 1, pointCount)] - points[i]) * cornerRadius, -90);
							Vector2 rPerpEnd = rPerpStart + (points[FL::Fmod(i + 1, pointCount)] - points[i]);
							Vector2 rotatedStart = Vector2::Rotate(rPerpStart, rotation);
							Vector2 rotatedEnd = Vector2::Rotate(rPerpEnd, rotation);
							Vector2 lineStart = FL::ConvertWorldToScreen(position + rotatedStart);
							Vector2 lineEnd = FL::ConvertWorldToScreen(position + rotatedEnd);

							FL::DrawLine(lineStart, lineEnd, colorLight, thickness + 2.0f, drawList);
							FL::DrawLine(lineStart, lineEnd, color, thickness, drawList);

							if (cornerRadius > 0)
							{
								Vector2 rotatedCircleStart = Vector2::Rotate(points[i], rotation);
								Vector2 rotatedCircleEnd = Vector2::Rotate(points[FL::Fmod(i + 1, pointCount)], rotation);
								Vector2 circleStart = FL::ConvertWorldToScreen(position + rotatedStart);
								Vector2 circleEnd = FL::ConvertWorldToScreen(position + rotatedEnd);

								FL::DrawCircle(circleStart, cornerRadius * F_gameViewGridStep.x, colorLight, drawList, thickness + 2.0f);
								FL::DrawCircle(circleStart, cornerRadius * F_gameViewGridStep.x, color, drawList, thickness);

								FL::DrawLine(circleStart, circleEnd, colorLight, thickness + 2.0f, drawList);
								FL::DrawLine(circleStart, circleEnd, color, thickness, drawList);
							}
						}
					}
				}

				for (Chain& chain : chains)
				{
					bool b_isActive = body->IsActive();
					bool b_drawBoxInGame = chain.DrawInGame();

					if (b_drawBoxInGame && b_isActive)
					{
						FL::Physics::BodyProps& bodyProps = body->GetBodyProps();
						Shape::ShapeProps& shapeProps = chain.GetShapeProps();
						bool b_isLoop = shapeProps.b_isLoop;
						std::vector<Vector2>& points = shapeProps.points;
						int pointCount = (int)points.size();
						int minChainBodyVertices = 4;
						bool b_editingPoints = chain.IsEditingPoints();

						Vector4 color = chain.GetInGameDrawColor();
						Vector4 colorLight = Vector4(color.x, color.y, color.z, 0.35f);
						float thickness = chain.GetInGameDraThickness();

						for (int i = 0; i < pointCount; i++)
						{
							if (i < pointCount - 1 || b_isLoop)
							{
								Vector2 start = FL::ConvertWorldToScreen(position + Vector2::Rotate(points[i], rotation));
								Vector2 end = FL::ConvertWorldToScreen(position + Vector2::Rotate(points[FL::Fmod(i + 1, pointCount)], rotation));

								if (b_isLoop || (i > 0 && i < pointCount - 2))
								{
									FL::DrawLine(start, end, colorLight, thickness + 2.0f, drawList);
									FL::DrawLine(start, end, color, thickness, drawList);
								}
							}
						}
					}
				}
			}
		}
	}
}