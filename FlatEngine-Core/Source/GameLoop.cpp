#include "FlatEngine.h"
#include "components/Animation.h"
#include "components/CharacterController.h"
#include "components/Transform.h"
#include "GameLoop.h"
#include "GameObject.h"
#include "managers/Controls.h"
#include "managers/SceneManager.h"
#include "managers/Settings.h"
#include "physics/PhysicsManager.h"
#include "tools/Time.h"

#ifdef _WINDOWS
#include <crtdefs.h>
#endif
#include <vector>


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

	void GameLoop::Start()
	{
		m_time = 0.0f;
		m_activeTime = m_time - m_pausedTime;
		m_b_paused = false;
		m_accumulator = 0.0f;

		// Save the name of the scene we started with so we can load it back up when we stop
		m_b_started = true;
		LuaManager::RunSceneAwakeAndStart();
		LuaManager::RunPersistentAwakeAndStart();
		m_currentTime = Time::Time();
	}

	void HandleMappingContextEvents()
	{
		// Unfire all keybinds that were fired in the last frame then clear the saved keys
		static std::vector<std::string> firedKeys = std::vector<std::string>();
		static std::vector<std::string> firedLastFrameKeys = std::vector<std::string>();
		firedLastFrameKeys = firedKeys;

		for (std::string keybind : firedKeys)
		{
			for (Controls::MappingContext& context : Controls::mappingContexts)
			{
				context.UnFireEvent(keybind);
			}
		}
		firedKeys.clear();

		for (SDL_Event event : events)
		{
			for (Controls::MappingContext& context : Controls::mappingContexts)
			{
				HandleContextEvents(context, event, firedKeys);
			}
		}

		events.clear();
	}

	void GameLoop::Update()
	{
		static Uint32 frameStart = FL::Time::Time();
		static int framesSkipped = 0;	

		if ((m_b_started && !m_b_paused) || (m_b_paused && m_b_frameSkipped))
		{			
			int iterations = 0;								

			if (m_b_paused && m_b_frameSkipped)
			{
				if (framesSkipped < m_framesToSkip)
				{
					framesSkipped++;
				}
				else
				{
					framesSkipped = 0;
					m_b_frameSkipped = false;
				}
			}			

			float frameTime = (float)(FL::Time::Time() - frameStart) / 1000.0f; // actual deltaTime (in seconds)

			// Only add accumulated time if the GameLoop is not paused or if a frame was skipped while paused, then add a small fixed amount of time
			if (!m_b_paused)
			{
				m_accumulator += frameTime;
				if (m_accumulator > 0.25f)
				{
					m_accumulator = 0.25f; // prevent death spiral
				}
			}
			else if (m_b_frameSkipped)
			{
				m_accumulator += m_deltaTime;
			}

			if (!m_b_paused || m_b_frameSkipped)
			{				
				while (m_accumulator >= m_deltaTime)
				{
					AddFrame();
					m_activeTime = m_time - m_pausedTime;

					SceneView::ClearDebugDrawObjects();						
					HandleMappingContextEvents();
					HandleCamera();
					ResetCharacterControllers();
					HandleButtons();	
					RunUpdateOnScripts();
					HandleAnimations();	
					PhysicsManager::physics2D.Update(GetDeltaTime());

					m_time += m_deltaTime;
					if (m_accumulator >= m_deltaTime)
					{
						m_accumulator -= m_deltaTime;
					}

					iterations++;
				}
			}
			
			// Get time it took to get back to GameLoopUpdate()
			frameStart = FL::Time::Time();

			// Artificially slow GameLoop if frameTime is less than 
			if (!Settings::settings.b_vsyncEnabled && frameTime < m_deltaTime)
			{
				SDL_Delay((Uint32)(m_deltaTime - frameTime) * 1000);
			}		
		}

		// If gameloop isn't running, make sure our framestart keeps up with current engine time otherwise it will cause a freeze on initially starting gameloop
		if (!m_b_started || m_b_paused)
		{
			frameStart = FL::Time::Time();
		}	
	}

	void GameLoop::Stop()
	{
		SceneView::ClearDebugDrawObjects();	
		m_b_started = false;
		m_b_paused = false;
		m_framesCounted = 0;
	}

	void GameLoop::PauseGameLoop()
	{		
		if (m_b_started && !m_b_paused)
		{
			m_b_paused = true;
			m_activeTime = m_time - m_pausedTime;
		}
	}

	void GameLoop::UnpauseGameLoop()
	{		
		if (m_b_started && m_b_paused)
		{
			m_b_paused = false;
			ResetCurrentTime();
			m_pausedTime = m_time - m_activeTime;			
		}
	}

	void GameLoop::TogglePauseGameLoop()
	{
		if (m_b_paused)
			UnpauseGameLoop();
		else
		 	PauseGameLoop();
	}

	void GameLoop::ResetCurrentTime()
	{
		m_currentTime = Time::Time();
	}

	void GameLoop::HandleCamera()
	{
		if (SceneManager::loadedScene.GetPrimaryCamera() != nullptr)
		{
			SceneManager::loadedScene.GetPrimaryCamera()->Update();
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
					GameObject* owner = hovered.GetOwningObject();
					LuaManager::LuaParameter functionParams = hovered.GetFunctionParams();					
					std::string functionName = hovered.GetFunctionName();

					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !b_hasLeftClicked)
					{
						// For OnButtonLeftClick() event function in Lua and user defined function in C++ Scripts
						b_hasLeftClicked = true;

						if (hovered.LeftClickSet())
						{
							hovered.OnLeftClick();
						}

						CallLuaButtonEventFunction(owner, LuaManager::LuaEventFunction::OnButtonLeftClick);
						

						// For Button On Click events in Button Inspector Component
						if (hovered.GetLeftClick() && functionName != "")
						{													
							// if (functionParams->b_cppEvent)
							// {
							// 	if (F_CPPAnimationEventFunctions.count(functionName))
							// 	{
							// 		F_CPPAnimationEventFunctions.at(functionName)(hovered.GetOwningObject(), functionParams->parameters);
							// 	}
							// }
							// else if (functionParams->b_luaEvent)
							// {
								CallLuaButtonOnClickFunction(owner, functionName, functionParams);
							// }
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

						CallLuaButtonEventFunction(owner, LuaManager::LuaEventFunction::OnButtonRightClick);

						// Inspector
						if (hovered.GetRightClick() && functionName != "")
						{
							// if (functionParams->b_cppEvent)
							// {
							// 	if (F_CPPAnimationEventFunctions.count(functionName))
							// 	{
							// 		F_CPPAnimationEventFunctions.at(functionName)(hovered.GetOwningObject(), functionParams->parameters);
							// 	}
							// }
							// else if (functionParams->b_luaEvent)
							// {						
								CallLuaButtonOnClickFunction(owner, functionName, functionParams);
							// }							
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

		for (Button& button : SceneManager::loadedScene.GetAll<Button>().GetAll())
		{
			if (button.IsActive() && button.GetOwningObject()->IsActive())
			{
				Transform* transform = button.GetOwningObject()->Get<Transform>();
				Vector4 activeEdges = button.GetActiveEdges();
				Vector2 mousePos = ImGui::GetIO().MousePos;

				if (PhysicsManager::physics2D.AreCollidingViewport(activeEdges, Vector4(mousePos.y, mousePos.x, mousePos.y, mousePos.x)))
				{
					if (button.GetActiveLayer() >= GetFirstUnblockedLayer())
					{
						m_hoveredButtons.push_back(button);
						button.SetMouseIsOver(true);
						GameObject* owner = button.GetOwningObject();

						if (button.MouseOverSet())
						{
							button.OnMouseOver();
						}

						CallLuaButtonEventFunction(owner, LuaManager::LuaEventFunction::OnButtonMouseOver);
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
				if (hoveredButton.GetOwnerID() == lastHovered.GetOwnerID())
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

				CallLuaButtonEventFunction(hoveredButton.GetOwningObject(), LuaManager::LuaEventFunction::OnButtonMouseEnter);			
			}
		}

		// Mouse Leave
		for (Button& lastHovered : lastHovered)
		{
			bool b_stillHovered = false;
			for (Button& hoveredButton : m_hoveredButtons)
			{
				if (hoveredButton.GetOwnerID() == lastHovered.GetOwnerID())
				{
					b_stillHovered = true;
				}
			}
			if (!b_stillHovered && lastHovered.GetOwningObject() != nullptr)
			{
				if (lastHovered.MouseLeaveSet())
				{
					lastHovered.OnMouseLeave();
				}

				CallLuaButtonEventFunction(lastHovered.GetOwningObject(), LuaManager::LuaEventFunction::OnButtonMouseLeave);			
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
		if (canvas.GetOwnerID() != -1)
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

		for (Canvas& canvas : SceneManager::loadedScene.GetAll<Canvas>().GetAll())
		{
			Vector4 activeEdges = canvas.GetActiveEdges();
			bool b_blocksLayers = canvas.GetBlocksLayers();
			int layerNumber = canvas.GetLayerNumber();

			if (PhysicsManager::physics2D.AreCollidingViewport(activeEdges, Vector4(mousePos.y, mousePos.x, mousePos.y, mousePos.x)) && b_blocksLayers && layerNumber >= lowestUnblockedLayer)
			{
				lowestUnblockedCanvas = canvas;
				lowestUnblockedLayer = canvas.GetLayerNumber();
			}
		}

		return lowestUnblockedCanvas;
	}

	void GameLoop::ResetCharacterControllers()
	{
		for (CharacterController& controller : SceneManager::loadedScene.GetAll<CharacterController>().GetAll())
		{
			controller.SetMoving(false);
		}
	}

	void GameLoop::HandleAnimations()
	{
		for (Animation& animation : SceneManager::loadedScene.GetAll<Animation>().GetAll())
		{
			animation.PlayAnimations(TimeElapsedInMs());
		}
	}

	void GameLoop::RunUpdateOnScripts()
	{
		//float processTime = (float)Time::Time();
		LuaManager::RunLuaFuncOnAllScripts("Update");
		//processTime = (float)Time::Time() - processTime;
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
			SceneManager::loadedScene.DeleteGameObject(objectID);
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

	void GameLoop::TogglePauseGame()
	{
		if (m_b_gamePaused)
		{
			m_b_gamePaused = false;
		}
		else
		{
			m_b_gamePaused = true;
		}
	}

	float GameLoop::TimeElapsedInSec()
	{
		if (m_b_started)
		{
			return m_activeTime;
		}
		return 0;
	}

	long GameLoop::TimeElapsedInMs()
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