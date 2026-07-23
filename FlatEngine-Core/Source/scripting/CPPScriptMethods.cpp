#include "managers/Assets.h"
#include "managers/ProjectManager.h"
#include "scripting/CPPScriptMethods.h"
#include "tools/FileHelper.h"
#include "tools/Logger.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>


namespace FlatEngine
{
	std::vector<std::string> cppScriptNames = std::vector<std::string>();
	bool b_shouldReloadProjectFiles;

	void CreateNewCPPScript(std::string fileName, std::string path)
	{
		for (std::string scriptName : cppScriptNames)
		{
			if (scriptName == fileName + " (C++)")
			{
				Logger::log.Err("Script name already taken.  Please enter a different name for your new C++ script.");
				return;
			}
		}

		std::ofstream outfile;
		std::string cppFileNameWExtention;
		std::string hFileNameWExtention;
		std::string hookupFilePath = "../FlatEngine-Core/Source/CPPScriptsHookup.cpp";
		std::string headersFilePath = "../FlatEngine-Core/Source/CPPScriptHeaders.h";

		if (path == "")
		{
			cppFileNameWExtention = "../projects/" + FileHelper::GetFilenameFromPath(ProjectManager::loadedProject.path) + "/scripts/cpp/" + fileName + ".cpp";
			hFileNameWExtention = "../projects/" + FileHelper::GetFilenameFromPath(ProjectManager::loadedProject.path) + "/scripts/cpp/" + fileName + ".h";
		}
		else
		{
			cppFileNameWExtention = path + "/" + fileName + ".cpp";
			hFileNameWExtention = path + "/" + fileName + ".h";
		}

		outfile.open(cppFileNameWExtention, std::ios_base::app);
		outfile <<
			"#include \"" + fileName + ".h\"\n\n" +

			"namespace FlatEngine\n{\n" +

			"	" + fileName + "::" + fileName + "()\n	{\n\n	}\n\n" +

			"	" + fileName + "::~" + fileName + "()\n	{\n\n	}\n\n" +

			"	void " + fileName + "::Awake()\n	{\n\n	}\n\n" +

			"	void " + fileName + "::Start()\n	{\n\n	}\n\n" +

			"	void " + fileName + "::Update()\n    {\n\n	}\n" +

			"}";
		outfile.close();

		outfile.open(hFileNameWExtention, std::ios_base::app);
		outfile <<
			"#pragma once\n#include \"FlatEngine.h\"\n#include \"CPPScript.h\"\n\nnamespace FlatEngine\n{\n	class " + fileName + " : public CPPScript\n	{\n	public:\n" +
			"		" + fileName + "(); \n" +
			"		~" + fileName + "(); \n\n" +
			"		void Awake();\n" +

			"		void Start();\n" +

			"		void Update();\n" +

			"	};\n}";
		outfile.close();

		outfile.open(headersFilePath, std::ios_base::app);
		outfile << "\n#include \"" + fileName + ".h\"";
		outfile.close();

		std::ifstream fileIn(hookupFilePath);
		std::stringstream buffer;
		buffer << fileIn.rdbuf();
		fileIn.close();
		std::string contents = buffer.str();

		int rCount = 0;
		int rCharCode = 114;
		while (rCount < 3)
		{
			if (contents.back() == rCharCode)
			{
				rCount++;
			}
			contents.pop_back();
		}

		contents += "if (scriptName == \"" + fileName + " (C++)\")\n	{\n		newScript = std::make_shared<FlatEngine::" + fileName + ">();\n		newScript->SetName(\"" + fileName + " (C++)\");\n	}\n	return newScript; \n }";

		std::ofstream hookupFile(hookupFilePath, std::ios::trunc);
		hookupFile << contents;
		hookupFile.close();

		b_shouldReloadProjectFiles = true;

		Logger::log.Info("To use the new C++ script, please close the engine and recompile the project code.");
		Logger::log.Info("If closing the editor does not reload the project files, please do so manually inside your editor.");
		Logger::log.Info("You may also double-click the Setup-Windows.bat file found in the Premake directory to prompt a reload in VS.");
	}

	void RetrieveCPPScriptNames()
	{
		cppScriptNames.clear();

		std::vector<std::string> scriptPaths = FileHelper::FindAllFilesWithExtension(Assets::assetManager.GetDir("projectDir"), ".cpp");
		for (std::string path : scriptPaths)
		{
			cppScriptNames.push_back(FileHelper::GetFilenameFromPath(path) + " (C++)");
		}
	}
}