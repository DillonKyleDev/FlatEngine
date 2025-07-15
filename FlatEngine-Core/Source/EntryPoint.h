#pragma once
#include "Application.h"
#include "FlatEngine.h"
#include <cstdlib>


namespace FlatEngine
{
	extern std::shared_ptr<Application> CreateApplication(int argc, char** argv);
	bool b_ApplicationRunning = true;
	bool b_HasStarted = false;

	int Main(int argc, char** argv)
	{
		// This loop allows us to restart our application instead of just closing it
		while (b_ApplicationRunning && !b_HasStarted)
		{
			b_HasStarted = true;
			FlatEngine::F_Application = CreateApplication(argc, argv);

			// Initialize FlatEngine. Start up SDL and create window
			if (!FlatEngine::Init(F_Application->WindowWidth(), F_Application->WindowHeight()))
			{
				printf("Failed to initialize.\n");
			}

			//Load media
			if (!LoadFonts())
			{
				printf("Failed to load fonts.\n");
			}
			else
			{
				F_Application->Init();
				F_Application->Run();
			}

			SaveDebugLogToFile();
			CloseProgram();

			if (F_b_shouldReloadProjectFiles)
			{
				std::system("..\\Premake\\Setup-Windows.bat");
			}
		}

		return 1;
	}
}