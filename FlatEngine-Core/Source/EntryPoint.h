#pragma once
#include "Application.h"

#include <memory>


namespace FlatEngine
{
	class Application;

	extern bool b_applicationRunning;

	extern int Main(int argc, char** argv);
	extern std::shared_ptr<Application> CreateApplication(int argc, char** argv);
}