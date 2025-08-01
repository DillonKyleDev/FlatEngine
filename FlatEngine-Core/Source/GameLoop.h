#pragma once
#include "Vector2.h"
#include "Canvas.h"
#include "Button.h"

#include <string>
#include <vector>
#include "SDL.h" // Uint32

namespace FlatEngine
{
	class GameLoop
	{
	public:
		GameLoop();
		~GameLoop();

		void Start();
		void Update(float gridstep, Vector2 viewportCenter);
		void Stop();
		void Pause();
		void Unpause();
		bool IsGamePaused();
		void PauseGame();
		void UnpauseGame();
		float TimeEllapsedInSec();
		long TimeEllapsedInMs();
		bool IsStarted();
		bool IsPaused();
		void AddFrame();
		float GetDeltaTime();
		void SetFrameSkipped(bool b_skipped);
		void SkipFrames(int framesToSkip);
		bool IsFrameSkipped();
		int GetFramesToSkip();
		long GetFramesCounted();
		void ResetCurrentTime();

		// Component Handling
		void HandleCamera();
		void ResetCharacterControllers();
		void HandleButtons();
		bool CheckForMouseOver();
		void ResetHoveredButtons();
		int GetFirstUnblockedLayer();
		Canvas GetFirstUnblockedCanvas();
		void RunUpdateOnScripts();
		void AddObjectToDeleteQueue(long objectID);
		void DeleteObjectsInDeleteQueue();
		
		float m_time; // Total time in deltaTime increments
		float m_pausedTime;
		float m_activeTime;
		float m_deltaTime;
		float m_accumulator;		
		Uint32 m_currentTime;

	private:
		bool m_b_started;
		bool m_b_paused;
		bool m_b_frameSkipped;
		bool m_b_gamePaused;
		long m_framesCounted;		
		std::vector<Button> m_hoveredButtons;
		std::vector<long> m_objectsQueuedForDelete;
		int m_framesToSkip;
	};
}