#include "FlatEngine.h"
#include "Project.h"
#include "CPPScript.h"

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>


std::vector<std::string> FlatEngine::F_CPPScriptNames = std::vector<std::string>();
bool FlatEngine::F_b_shouldReloadProjectFiles;

namespace FlatEngine
{
	void CreateNewCPPScript(std::string fileName, std::string path)
	{
		for (std::string scriptName : F_CPPScriptNames)
		{
			if (scriptName == fileName + " (C++)")
			{
				LogError("Script name already taken.  Please enter a different name for your new C++ script.");
				return;
			}
		}

		std::ofstream outfile;
		std::string cppFileNameWExtention;
		std::string hFileNameWExtention;
		std::string hookupFilePath = "..\\FlatEngine-Core\\Source\\CPPScriptsHookup.cpp";
		std::string headersFilePath = "..\\FlatEngine-Core\\Source\\CPPScriptHeaders.h";

		if (path == "")
		{
			cppFileNameWExtention = "..\\projects\\" + GetFilenameFromPath(F_LoadedProject.GetPath()) + "\\scripts\\cpp\\" + fileName + ".cpp";
			hFileNameWExtention = "..\\projects\\" + GetFilenameFromPath(F_LoadedProject.GetPath()) + "\\scripts\\cpp\\" + fileName + ".h";
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

		F_b_shouldReloadProjectFiles = true;

		LogString("To use the new C++ script, please close the engine and recompile the project code.");
		LogString("If closing the editor does not reload the project files, please do so manually inside your editor.");
		LogString("You may also double-click the Setup-Windows.bat file found in the Premake directory to prompt a reload in VS.");
	}

	void RetrieveCPPScriptNames()
	{
		F_CPPScriptNames.clear();

		std::vector<std::string> scriptPaths = FindAllFilesWithExtension(GetDir("projectDir"), ".cpp");
		for (std::string path : scriptPaths)
		{
			F_CPPScriptNames.push_back(GetFilenameFromPath(path) + " (C++)");
		}
	}
}