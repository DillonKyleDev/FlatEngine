// Hookup file for all CPP scripts
#include "CPPScript.h"
#include "CPPScriptHeaders.h"
#include "FlatEngine.h"
#include "Project.h"

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <memory>


std::map<std::string, std::shared_ptr<FlatEngine::CPPScript>> FlatEngine::F_CPPScriptsMap = std::map<std::string, std::shared_ptr<FlatEngine::CPPScript>>();
bool FlatEngine::F_b_shouldReloadProjectFiles;

namespace FlatEngine
{

	void CreateNewCPPScript(std::string fileName, std::string path)
	{
		for (std::string scriptPath : F_luaScriptPaths)
		{
			if (fileName == GetFilenameFromPath(scriptPath))
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
		outfile << "#include \"" + fileName + ".h\"\n";
		outfile.close();

		//outfile.open(hookupFilePath, std::ios_base::app);
		//outfile <<
		//	"std::shared_ptr<" + fileName + "> " + fileName + "Script = std::make_shared<" + fileName + ">(); \n" +
		//	"F_CPPScriptsMap.emplace(\"" + fileName + "\", " + fileName + "Script); \n";
		//outfile.close();

		std::ifstream fileIn(hookupFilePath);
		std::stringstream buffer;
		buffer << fileIn.rdbuf();
		fileIn.close();
		std::string contents = buffer.str();

		while (contents.back() != 125)
		{
			std::string back = &contents.back();
			contents.pop_back();
		}
		contents.pop_back();

		contents += "\n\n	std::shared_ptr<" + fileName + "> " + fileName + "Script = std::make_shared<" + fileName + ">(); \n" +
			"	F_CPPScriptsMap.emplace(\"" + fileName + "\", " + fileName + "Script); \n}";
		std::ofstream hookupFile(hookupFilePath, std::ios::trunc);
		hookupFile << contents;
		hookupFile.close();

		F_b_shouldReloadProjectFiles = true;

		LogString("To use the new C++ script, please close the engine and recompile the project code.");
	}
}

void FlatEngine::InitCPPScripts()
{
	F_CPPScriptsMap.clear();
}