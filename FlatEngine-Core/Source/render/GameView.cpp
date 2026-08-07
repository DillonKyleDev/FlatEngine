#include "components/Animation.h"
#include "components/Button.h"
#include "components/Camera.h"
#include "components/Canvas.h"
#include "components/Sprite.h"
#include "components/Text.h"
#include "components/TileMap.h"
#include "components/Transform.h"
#include "GameView.h"
#include "GuiCore.h"
#include "managers/SceneManager.h"
#include "render/VulkanManager.h"
#include "tools/Logger.h"
#include "tools/Time.h"
#include "tools/Vector2.h"
#include "tools/Vector4.h"

#include "imgui.h"

namespace FL = FlatEngine;


namespace FlatEngine
{
	namespace GameView
	{
		// Game view default values
		float GAME_VIEWPORT_WIDTH = 600.0f;
		float GAME_VIEWPORT_HEIGHT = 400.0f;
		float xGameCenter = 600.0f / 2;
		float yGameCenter = 400.0f / 2;
		FL::Vector2 gameViewCenter = FL::Vector2();
		float gameViewGridStep = 50.0f;


		void RenderGameView(bool& b_show, bool b_inRuntime)
		{
			if (!b_show)
				return;

			ImGuiWindowFlags flags = ImGuiWindowFlags_None;

			// If Release - Make GameView full screen and disable tab decoration and resizing
			if (b_inRuntime)
			{			
				ImGuiIO& inputOutput = ImGui::GetIO();
				float xSize = inputOutput.DisplaySize.x;
				float ySize = inputOutput.DisplaySize.y;
				ImGui::SetNextWindowSize(FL::Vector2(xSize, ySize));
				ImGui::SetNextWindowPos(FL::Vector2());
				flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize;
			}

			if (FL::GuiCore::BeginWindow("Game View", b_show, flags, "black"))
			{			
				static bool opt_enable_context_menu = true;

				FL::Vector2 canvasP0 = ImGui::GetCursorScreenPos();
				FL::Vector2 canvasSize = ImGui::GetContentRegionAvail();
				if (canvasSize.x < 50.0f) canvasSize.x = 50.0f;
				if (canvasSize.y < 50.0f) canvasSize.y = 50.0f;
				FL::Vector2 canvas_p1 = FL::Vector2(canvasP0.x + canvasSize.x, canvasP0.y + canvasSize.y);

				// Set viewport dimensions for rendering objects in game view. We want this to always be centered in game view so we can get it every frame.
				GAME_VIEWPORT_WIDTH = canvas_p1.x - canvasP0.x + 1;
				GAME_VIEWPORT_HEIGHT = canvas_p1.y - canvasP0.y + 1;

				FL::Vector2 currentPos = ImGui::GetCursorScreenPos();
				bool b_weightedScroll = false;
				FL::Vector2 size;
				FL::Vector2 startingPos = ImGui::GetCursorScreenPos();
				
				std::vector<VkDescriptorSet> descriptors = FL::VulkanManager::vulkan.GetGameViewDescriptorSets();

				if (descriptors.size() > 0 && descriptors[FL::VulkanManager::currentFrame])
				{
					FL::Vector2 regionAvailable = ImGui::GetContentRegionAvail();
					float targetAspect = 16.0f / 9.0f;
					float regionAspect = regionAvailable.x / regionAvailable.y;

					if (regionAspect > targetAspect)
					{
						// Too wide — pillarbox
						float w = regionAvailable.y * targetAspect;
						float offset = (regionAvailable.x - w) / 2.0f;
						startingPos.x += offset;
						size = Vector2(w, regionAvailable.y);
					}
					else
					{
						// Too tall — letterbox
						float h = regionAvailable.x / targetAspect;
						float offset = (regionAvailable.y - h) / 2.0f;
						startingPos.y += offset;
						size = Vector2(regionAvailable.x, h);
					}

					ImGui::SetCursorScreenPos(startingPos);
					ImGui::Image(descriptors[FL::VulkanManager::currentFrame], size);
				}

				// Render GameObjects in game view
				//RenderGameViewObjects(canvasP0, canvasSize);				
			}	

			FL::GuiCore::EndWindow(); // Game View
		}

		void RenderGameViewObject(FL::GameObject self, FL::Vector2 canvasP0, FL::Vector2 canvasSize, ImDrawList* drawList, ImDrawListSplitter* drawSplitter, FL::Vector2 cameraPosition, float cameraWidth, float cameraHeight)
		{
			FL::Transform* transform = self.Get<FL::Transform>();
			FL::Sprite* sprite = self.Get<FL::Sprite>();
			FL::TileMap* tileMap = self.Get<FL::TileMap>();
			FL::Animation* animation = self.Get<FL::Animation>();
			FL::Text* text = self.Get<FL::Text>();
			FL::Button* button = self.Get<FL::Button>();
			FL::Canvas* canvas = self.Get<FL::Canvas>();
			FL::Body* body = self.Get<FL::Body>();

			/*
			
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
					VkDescriptorSet spriteTexture = sprite->GetTexture();
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
						if (renderOrder <= VulkanManager::maxSpriteLayers && renderOrder >= 0)
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
							AddImageToDrawList(spriteTexture, position, gameViewCenter, textureWidth, textureHeight, offset, spriteScale, b_scalesWithZoom, gameViewGridStep, drawList, rotation, ImGui::GetColorU32(tintColor));
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
							if (renderOrder <= VulkanManager::maxSpriteLayers && renderOrder >= 0)
							{
								drawSplitter->SetCurrentChannel(drawList, renderOrder);
							}
							else
							{
								drawSplitter->SetCurrentChannel(drawList, 0);
							}

							AddImageToDrawList(textTexture->GetTexture(), position, gameViewCenter, textWidth, textHeight, offset, newScale, b_spriteScalesWithZoom, gameViewGridStep, drawList, rotation, ImGui::GetColorU32(tintColor));
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

									VkDescriptorSet texture = tile.tileSetTexture;
									int textureWidth = usedTileSet->GetTexture()->GetWidth();
									int textureHeight = usedTileSet->GetTexture()->GetHeight();
									Vector2 uvStart = Vector2(tile.uvStart.x / textureWidth, tile.uvStart.y / textureHeight);
									Vector2 uvEnd = Vector2(tile.uvEnd.x / textureWidth, tile.uvEnd.y / textureHeight);
									float gridXPosition = (position.x - (gridWidth / 2)) + 2 * (float)w;
									float gridYPosition = (position.y + (gridHeight / 2)) - 2 * (float)h;
									Vector2 tilePosition = Vector2(gridXPosition, gridYPosition);

									if (renderOrder <= VulkanManager::maxSpriteLayers && renderOrder >= 0)
									{
										drawSplitter->SetCurrentChannel(drawList, renderOrder);
									}
									else
									{
										drawSplitter->SetCurrentChannel(drawList, 0);
									}

									FL::AddImageToDrawList(texture, tilePosition, gameViewCenter, tileWidth, tileHeight, Vector2(0, 0), scale, true, gameViewGridStep, drawList, 0, FL::GetColor32("white"), uvStart, uvEnd);
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

					//drawSplitter->SetCurrentChannel(drawList, VulkanManager::maxSpriteLayers + 2);

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

							drawSplitter->SetCurrentChannel(drawList, FL::VulkanManager::maxSpriteLayers + 2);

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
							float radius = shapeProps.radius * gameViewGridStep;
							Vector2 offset = shapeProps.positionOffset;
							Vector2 center = ConvertWorldToScreen(position + Vector2::Rotate(offset, rotation));

							drawSplitter->SetCurrentChannel(drawList, FL::VulkanManager::maxSpriteLayers + 2);

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
							float radiusScreen = radius * gameViewGridStep;
							Vector2 offset = shapeProps.positionOffset;
							float rotation = FL::RadiansToDegrees(b2Rot_GetAngle(shapeProps.rotationOffset));

							Vector2 center1 = ConvertWorldToScreen(Vector2(b2Body_GetWorldPoint(body->GetBodyID(), capsuleShape.center1)));
							Vector2 center2 = ConvertWorldToScreen(Vector2(b2Body_GetWorldPoint(body->GetBodyID(), capsuleShape.center2)));
							Vector2 difference = center2 - center1;
							Vector2 diffN = Vector2::Normalize(difference);
							Vector2 diffNR = diffN * radiusScreen;
							Vector2 diffPerp = Vector2::Rotate(diffNR, 90);
							Vector2 flippedDiffPerp = Vector2::Rotate(diffNR, -90);

							drawSplitter->SetCurrentChannel(drawList, FL::VulkanManager::maxSpriteLayers + 2);

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

									FL::DrawCircle(circleStart, cornerRadius * gameViewGridStep, colorLight, drawList, thickness + 2.0f);
									FL::DrawCircle(circleStart, cornerRadius * gameViewGridStep, color, drawList, thickness);

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

			*/
			
		}

		void RenderGameViewObjects(Vector2 canvasP0, Vector2 canvasSize)
		{		
			std::vector<FL::GameObject> sceneObjects = SceneManager::loadedScene.GetSceneObjects();	
			FL::Camera* primaryCamera = SceneManager::loadedScene.GetPrimaryCamera();
			FL::Transform* cameraTransform = nullptr;	
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();
			// 3 channels for now in this scene view. 0 = scene objects, 1 = other UI (camera icon, etc), 2 = transform arrow
			drawSplitter->Split(drawList, VulkanManager::maxSpriteLayers + 5);

			FL::Vector2 cameraPosition = FL::Vector2();
			float cameraWidth = 50;
			float cameraHeight = 30;		
			FL::Vector4 frustrumColor = FL::Vector4(1);	
			float cameraStartTime = (float)FL::Time::Time();
			
			if (primaryCamera != nullptr)
			{
				FL::GameObject* owner = primaryCamera->GetOwningObject();
				if (owner != nullptr)
				{
					cameraTransform = owner->Get<FL::Transform>();
				}
				cameraWidth = primaryCamera->GetWidth();
				cameraHeight = primaryCamera->GetHeight();
				gameViewGridStep = primaryCamera->GetZoom();				
				frustrumColor = primaryCamera->GetFrustrumColor();
				
				if (cameraTransform != nullptr)
				{
					cameraPosition = cameraTransform->GetAbsolutePosition();
				}
				else
				{
					cameraPosition = FL::Vector2();
				}
			}
			
			gameViewCenter = FL::Vector2((GAME_VIEWPORT_WIDTH / 2) - (cameraPosition.x * gameViewGridStep) + canvasP0.x, (GAME_VIEWPORT_HEIGHT / 2) + (cameraPosition.y * gameViewGridStep) + canvasP0.y);
			FL::Vector2 viewportCenterPoint = FL::Vector2((GAME_VIEWPORT_WIDTH / 2) + canvasP0.x, (GAME_VIEWPORT_HEIGHT / 2) + canvasP0.y);
			
			float renderStartTime = 0;
			renderStartTime = (float)FL::Time::Time();

			for (FL::GameObject& sceneObject : sceneObjects)
			{
				if (sceneObject.IsActive())
				{
					RenderGameViewObject(sceneObject, canvasP0, canvasSize, drawList, drawSplitter, cameraPosition, cameraWidth, cameraHeight);
				}
			}

			drawSplitter->Merge(drawList);
		}

		// Converts from world grid space in Game View to screen space
		Vector2 ConvertWorldToScreen(Vector2 positionInWorld)
		{
			float x = gameViewCenter.x + (positionInWorld.x * gameViewGridStep);
			float y = gameViewCenter.y - (positionInWorld.y * gameViewGridStep);

			return Vector2(x, y);
		}

		// Converts from screen space to world grid space in Game View
		Vector2 ConvertScreenToWorld(Vector2 positionOnScreen)
		{
			float x = (positionOnScreen.x - gameViewCenter.x) / gameViewGridStep;
			float y = (gameViewCenter.y - positionOnScreen.y) / gameViewGridStep;

			return Vector2(x, y);
		}
			
		Vector2 GetMousePosWorld()
		{
			return ConvertScreenToWorld(ImGui::GetIO().MousePos);
		}

		Vector2 GetMousePosScreen()
		{
			return ImGui::GetIO().MousePos;
		}

		// Start and End are in world coordinates
		void DrawLineInGame(Vector2 startingPoint, Vector2 endingPoint, std::string color, float thickness)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));		
			ImGui::Begin("Game View", 0, 16 | 8);		
			// {

			Vector2 start = ConvertWorldToScreen(startingPoint);
			Vector2 end = ConvertWorldToScreen(endingPoint);
			Logger::log.DrawLine(start, end, color, thickness, ImGui::GetWindowDrawList());

			// }
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::End(); // Game View
		}
	}
}