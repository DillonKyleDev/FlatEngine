#include "GameLoop.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"
#include "Script.h"
#include "Button.h"
#include "Canvas.h"
#include "TagList.h"
#include "Camera.h"
#include "Project.h"
#include "CharacterController.h"
#include "MappingContext.h"
#include "Physics.h"

#include <vector>
#include <process.h>
#include <crtdefs.h>


namespace FL = FlatEngine;


namespace FlatEngine
{
	GameLoop::GameLoop()
	{
		m_b_started = false;
		m_b_gamePaused = false;
		m_b_paused = false;
		m_b_frameSkipped = false;
		m_framesToSkip = 0;
		m_time = 0.0f;
		m_activeTime = 0.0f;
		m_currentTime = 0;
		m_pausedTime = 0;
		m_framesCounted = 0;
		m_deltaTime = 0.01f; // Minimum "time" to pass for each frame 0.12f = 120fps
		m_accumulator = m_deltaTime;		
		m_hoveredButtons = std::vector<Button>();
		m_objectsQueuedForDelete = std::vector<long>();
	}

	GameLoop::~GameLoop()
	{
	}

	void GameLoop::Start()
	{
		m_time = 0.0f;
		m_activeTime = m_time - m_pausedTime;
		m_b_paused = false;
		m_accumulator = 0.0f;

		// Save the name of the scene we started with so we can load it back up when we stop
		m_b_started = true;
		RunSceneAwakeAndStart();
		RunPersistantAwakeAndStart();
		m_currentTime = GetEngineTime();
	}

	void GameLoop::Update(float gridstep, Vector2 viewportCenter)
	{
		AddFrame();
		m_activeTime = m_time - m_pausedTime;

		HandleCamera();
		ResetCharacterControllers();
		HandleButtons();
		RunUpdateOnScripts();
		F_Physics->Update(GetDeltaTime());

		static bool b_made = false;
		static b2BodyId id;

		//if (!b_made)
		//{
		//	Physics::BodyProps newprops;
		//	F_Physics->CreateBox(newprops, id);
		//	b_made = true;
		//}
		//Vector2 pos = Vector2(b2Body_GetPosition(id).x, b2Body_GetPosition(id).y);
		//LogVector2(pos, "Pos: ");

		std::map<long, Body> bodies = GetLoadedScene()->GetBodies();
		for (std::map<long, Body>::iterator iterator = bodies.begin(); iterator != bodies.end(); iterator++)
		{
			//LogVector2(iterator->second.GetPosition(), "Pos: ");
			for (Box& box : iterator->second.GetBoxes())
			{

			}
			//for (Circle& circle : iterator->second.GetCircles())
			//{
			//	LogString("Circle");
			//	Circle* circlePtr = static_cast<Circle*>(b2Shape_GetUserData(circle.GetShapeID()));
			//}
			//LogInt((int)iterator->second.GetBodyProps().type);
			//LogVector2(iterator->second.GetBodyProps().position, iterator->second.GetParent()->GetName() + " Props Pos: ");	
			
			//LogVector2(iterator->second.GetBodyProps().dimensions);
		}
		//std::map<long, CircleBody> circleBodies = GetLoadedScene()->GetCircleBodies();
		//for (std::map<long, CircleBody>::iterator iterator = circleBodies.begin(); iterator != circleBodies.end(); iterator++)
		//{
		//	//LogVector2(iterator->second.GetPosition(), "Pos: ");
		//}
		//std::map<long, CapsuleBody> capsuleBodies = GetLoadedScene()->GetCapsuleBodies();
		//for (std::map<long, CapsuleBody>::iterator iterator = capsuleBodies.begin(); iterator != capsuleBodies.end(); iterator++)
		//{
		//	b2ShapeId id = iterator->second.GetShapeIDs().back();
		//	b2Capsule capsule = b2Shape_GetCapsule(id);
		//	//LogVector2(capsule.center1, "Center1: ");
		//	//LogVector2(capsule.center2, "Center2: ");
		//}
	}

	void GameLoop::Stop()
	{
		m_b_started = false;
		m_b_paused = false;
		m_framesCounted = 0;
	}

	void GameLoop::Pause()
	{		
		if (m_b_started && !m_b_paused)
		{
			m_b_paused = true;
			m_activeTime = m_time - m_pausedTime;
		}
	}

	void GameLoop::Unpause()
	{		
		if (m_b_started && m_b_paused)
		{
			m_b_paused = false;
			ResetCurrentTime();
			m_pausedTime = m_time - m_activeTime;			
		}
	}

	void GameLoop::ResetCurrentTime()
	{
		m_currentTime = GetEngineTime();
	}

	void GameLoop::HandleCamera()
	{
		if (GetPrimaryCamera() != nullptr)
		{
			GetPrimaryCamera()->Follow();
		}
	}

	void GameLoop::HandleButtons()
	{
		static bool b_hasLeftClicked = false;
		static bool b_hasRightClicked = false;

		if (!ImGui::IsKeyDown(ImGuiKey_MouseLeft))
		{
			b_hasLeftClicked = false;
		}
		if (!ImGui::IsKeyDown(ImGuiKey_MouseRight))
		{
			b_hasRightClicked = false;
		}

		if (CheckForMouseOver())
		{
			for (Button& hovered : m_hoveredButtons)
			{
				if (hovered.GetActiveLayer() >= GetFirstUnblockedLayer())
				{
					GameObject* owner = hovered.GetParent();
					std::shared_ptr<Animation::S_Event> functionParams = hovered.GetFunctionParams();					
					std::string functionName = hovered.GetFunctionName();

					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !b_hasLeftClicked)
					{
						// For OnButtonLeftClick() event function in Lua and user defined function in C++ Scripts
						b_hasLeftClicked = true;

						if (hovered.LeftClickSet())
						{
							hovered.OnLeftClick();
						}

						CallLuaButtonEventFunction(owner, LuaEventFunction::OnButtonLeftClick);
						

						// For Button On Click events in Button Inspector Component
						if (hovered.GetLeftClick() && functionName != "")
						{													
							if (functionParams->b_cppEvent)
							{
								if (F_CPPAnimationEventFunctions.count(functionName))
								{
									F_CPPAnimationEventFunctions.at(functionName)(hovered.GetParent(), functionParams->parameters);
								}
							}
							else if (functionParams->b_luaEvent)
							{
								CallLuaButtonOnClickFunction(owner, functionName, functionParams->parameters);
							}
						}
					}					
					else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
					{
						b_hasLeftClicked = false;
					}	

					if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !b_hasRightClicked)
					{
						// Scripts
						b_hasRightClicked = true;
					
						if (hovered.RightClickSet())
						{
							hovered.OnRightClick();
						}

						CallLuaButtonEventFunction(owner, LuaEventFunction::OnButtonRightClick);

						// Inspector
						if (hovered.GetRightClick() && functionName != "")
						{
							if (functionParams->b_cppEvent)
							{
								if (F_CPPAnimationEventFunctions.count(functionName))
								{
									F_CPPAnimationEventFunctions.at(functionName)(hovered.GetParent(), functionParams->parameters);
								}
							}
							else if (functionParams->b_luaEvent)
							{						
								CallLuaButtonOnClickFunction(owner, functionName, functionParams->parameters);
							}							
						}
					}					
					else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
					{
						b_hasRightClicked = false;
					}
				}
			}
		}
	}

	bool GameLoop::CheckForMouseOver()
	{
		std::vector<Button> lastHovered = m_hoveredButtons;
		ResetHoveredButtons();

		std::map<long, Button>& sceneButtons = GetLoadedScene()->GetButtons();
		std::map<long, Button>& persistantButtons = GetLoadedProject().GetPersistantGameObjectScene()->GetButtons();

		for (std::pair<long, Button> buttonPair : sceneButtons)
		{
			if (buttonPair.second.IsActive() && buttonPair.second.GetParent()->IsActive())
			{
				Transform* transform = buttonPair.second.GetParent()->GetTransform();
				Vector4 activeEdges = buttonPair.second.GetActiveEdges();
				Vector2 mousePos = ImGui::GetIO().MousePos;

				if (AreCollidingViewport(activeEdges, Vector4(mousePos.y, mousePos.x, mousePos.y, mousePos.x)))
				{
					if (buttonPair.second.GetActiveLayer() >= GetFirstUnblockedLayer())
					{
						m_hoveredButtons.push_back(buttonPair.second);
						buttonPair.second.SetMouseIsOver(true);
						GameObject* owner = buttonPair.second.GetParent();

						if (buttonPair.second.MouseOverSet())
						{
							buttonPair.second.OnMouseOver();
						}

						CallLuaButtonEventFunction(owner, LuaEventFunction::OnButtonMouseOver);
					}
				}
			}
		}
		for (std::pair<long, Button> buttonPair : persistantButtons)
		{
			if (buttonPair.second.IsActive() && buttonPair.second.GetParent()->IsActive())
			{
				Transform* transform = buttonPair.second.GetParent()->GetTransform();
				Vector4 activeEdges = buttonPair.second.GetActiveEdges();
				Vector2 mousePos = ImGui::GetIO().MousePos;

				if (AreCollidingViewport(activeEdges, Vector4(mousePos.y, mousePos.x, mousePos.y, mousePos.x)))
				{
					if (buttonPair.second.GetActiveLayer() >= GetFirstUnblockedLayer())
					{
						m_hoveredButtons.push_back(buttonPair.second);
						buttonPair.second.SetMouseIsOver(true);
						GameObject* owner = buttonPair.second.GetParent();

						if (buttonPair.second.MouseOverSet())
						{
							buttonPair.second.OnMouseOver();
						}

						CallLuaButtonEventFunction(owner, LuaEventFunction::OnButtonMouseOver);					
					}
				}
			}
		}

		// Mouse Enter
		for (Button& hoveredButton : m_hoveredButtons)
		{
			bool b_mouseJustEntered = true;
			for (Button& lastHovered : lastHovered)
			{
				if (hoveredButton.GetID() == lastHovered.GetID())
				{
					b_mouseJustEntered = false;
				}
			}
			if (b_mouseJustEntered)
			{
				if (hoveredButton.MouseEnterSet())
				{
					hoveredButton.OnMouseEnter();
				}

				CallLuaButtonEventFunction(hoveredButton.GetParent(), LuaEventFunction::OnButtonMouseEnter);			
			}
		}

		// Mouse Leave
		for (Button& lastHovered : lastHovered)
		{
			bool b_stillHovered = false;
			for (Button& hoveredButton : m_hoveredButtons)
			{
				if (hoveredButton.GetID() == lastHovered.GetID())
				{
					b_stillHovered = true;
				}
			}
			if (!b_stillHovered && lastHovered.GetParent() != nullptr)
			{
				if (lastHovered.MouseLeaveSet())
				{
					lastHovered.OnMouseLeave();
				}

				CallLuaButtonEventFunction(lastHovered.GetParent(), LuaEventFunction::OnButtonMouseLeave);			
			}
		}

		return m_hoveredButtons.size() > 0;
	}

	void GameLoop::ResetHoveredButtons()
	{
		for (Button button : m_hoveredButtons)
		{
			button.SetMouseIsOver(false);
		}

		m_hoveredButtons.clear();
	}

	int GameLoop::GetFirstUnblockedLayer()
	{
		Canvas canvas = GetFirstUnblockedCanvas();
		if (canvas.GetID() != -1)
		{
			return canvas.GetLayerNumber();
		}
		else
		{
			return -1;
		}
	}

	Canvas GameLoop::GetFirstUnblockedCanvas()
	{
		Canvas lowestUnblockedCanvas = Canvas(-1);
		int lowestUnblockedLayer = 0;
		Vector2 mousePos = ImGui::GetIO().MousePos;
		std::map<long, Canvas> &canvases = GetLoadedScene()->GetCanvases();

		for (std::pair<long, Canvas> canvasPair : canvases)
		{
			Canvas canvas = canvasPair.second;
			Vector4 activeEdges = canvas.GetActiveEdges();
			bool b_blocksLayers = canvas.GetBlocksLayers();
			int layerNumber = canvas.GetLayerNumber();

			if (AreCollidingViewport(activeEdges, Vector4(mousePos.y, mousePos.x, mousePos.y, mousePos.x)) && b_blocksLayers && layerNumber >= lowestUnblockedLayer)
			{
				lowestUnblockedCanvas = canvas;
				lowestUnblockedLayer = canvas.GetLayerNumber();
			}
		}

		return lowestUnblockedCanvas;
	}

	void GameLoop::ResetCharacterControllers()
	{
		for (std::pair<const long, CharacterController>& owner : FL::GetLoadedScene()->GetCharacterControllers())
		{
			owner.second.SetMoving(false);
		}
		for (std::pair<const long, CharacterController>& owner : FL::GetLoadedProject().GetPersistantGameObjectScene()->GetCharacterControllers())
		{
			owner.second.SetMoving(false);
		}
	}

	void GameLoop::RunUpdateOnScripts()
	{
		//float processTime = (float)GetEngineTime();
		RunLuaFuncOnAllScripts("Update");
		//processTime = (float)GetEngineTime() - processTime;
		//LogFloat(processTime, "Update Scripts: ");

		//F_CPPScriptsMap.at("Bonker")->Awake();
	}

	void GameLoop::AddObjectToDeleteQueue(long objectID)
	{
		m_objectsQueuedForDelete.push_back(objectID);
	}

	void GameLoop::DeleteObjectsInDeleteQueue()
	{		
		for (long objectID : m_objectsQueuedForDelete)
		{
			DeleteGameObject(objectID);
		}
		m_objectsQueuedForDelete.clear();
	}

	bool GameLoop::IsGamePaused()
	{
		return m_b_gamePaused;
	}

	void GameLoop::PauseGame()
	{
		m_b_gamePaused = true;
	}

	void GameLoop::UnpauseGame()
	{
		m_b_gamePaused = false;
	}

	float GameLoop::TimeEllapsedInSec()
	{
		if (m_b_started)
		{
			return m_activeTime;
		}
		return 0;
	}

	long GameLoop::TimeEllapsedInMs()
	{
		if (m_b_started)
		{
			return (long)(m_time * 1000.0f);
		}
		return 0;
	}

	bool GameLoop::IsStarted()
	{
		return m_b_started;
	}

	bool GameLoop::IsPaused()
	{
		return m_b_paused && m_b_started;
	}

	long GameLoop::GetFramesCounted()
	{
		return m_framesCounted;
	}

	void GameLoop::AddFrame()
	{
		m_framesCounted++;
	}

	float GameLoop::GetDeltaTime()
	{
		return m_deltaTime;
	}

	void GameLoop::SetFrameSkipped(bool b_skipped)
	{
		m_b_frameSkipped = b_skipped;
		if (!m_b_frameSkipped)
		{
			m_framesToSkip = 0;
		}
	}

	void GameLoop::SkipFrames(int framesToSkip)
	{
		ResetCurrentTime();
		m_framesToSkip = framesToSkip;
		m_b_frameSkipped = true;
	}

	bool GameLoop::IsFrameSkipped()
	{
		return m_b_frameSkipped;
	}

	int GameLoop::GetFramesToSkip()
	{
		return m_framesToSkip;
	}
}