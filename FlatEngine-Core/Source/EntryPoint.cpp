#include "EntryPoint.h"
#include "FlatEngine.h"
#include "scripting/CPPScriptMethods.h"


namespace FlatEngine
{
	bool b_applicationRunning = true;

	int Main(int argc, char** argv)
	{
		// This loop allows us to restart our application instead of just closing it
		// while (b_applicationRunning)
		// {
			FlatEngine::application = CreateApplication(argc, argv);

			// Initialize FlatEngine. Start up SDL and create window
			if (!Init(application->GetWindowWidth(), application->GetWindowHeight()))
			{
				printf("FlatEngine initialization failed...\n");
			}
			else
			{
				application->Init();
				application->Run();
			}

			application->Cleanup();

			Cleanup();

			if (b_shouldReloadProjectFiles)
			{
				std::system("../Premake/Setup-Windows.bat");
			}
		// }

		return 1;
	}
}