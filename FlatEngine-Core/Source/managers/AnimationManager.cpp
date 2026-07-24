#include "GameObject.h"
#include "managers/AnimationManager.h"
#include "managers/Assets.h"
#include "managers/ProjectManager.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"

#include <fstream>
#include <memory>


namespace FlatEngine
{
    namespace AnimationManager
    {
        std::map<std::string, void (*)(GameObject*, LuaManager::LuaParameter)> cppAnimationEventFunctions = std::map<std::string, void (*)(GameObject*, LuaManager::LuaParameter)>();

        void AddCPPAnimationEventFunction(std::string functionName, void (*eventFunction)(GameObject*, LuaManager::LuaParameter))
        {
            if (functionName != "" && cppAnimationEventFunctions.count(functionName) == 0)
            {
                std::pair<std::string, void (*)(GameObject*, LuaManager::LuaParameter)> newFunctionPair = { functionName, eventFunction };
                cppAnimationEventFunctions.emplace(newFunctionPair);
            }
        }

        void CreateNewAnimationFile(std::string filename, std::string path)
        {		
            std::string filePath = "";

            if (path == "")
            {
                filePath = Assets::assetManager.GetDir("projectDir") + "/animations/" + filename + ".anm";
            }
            else
            {
                filePath = path + "/" + filename + ".anm";
            }

            AnimationData animData = AnimationData();
            animData.name = filename;
            animData.path = filePath;
            SaveAnimationFile(animData, filePath);

            FL::ProjectManager::loadedProject.loadedAnimationPath = filePath;
            FL::ProjectManager::SaveCurrentProject();

        }

        void SaveAnimationFile(const AnimationData& animData, std::string path)
        {		
            std::ofstream fileObject;
            std::ifstream ifstream(path);

            // Delete old contents of the file
            fileObject.open(path, std::ofstream::out | std::ofstream::trunc);
            fileObject.close();

            // Opening file in append mode
            fileObject.open(path, std::ios::app);


            // json eventProps = json::array();
            // for (std::shared_ptr<EventProp> eventProp : animData->eventProps)
            // {
            // 	json jsonData = {
            // 		{ "functionName", eventProp->functionName },
            // 		{ "time", eventProp->time },
            // 		{ "b_cppEvent", eventProp->b_cppEvent },
            // 		{ "b_luaEvent", eventProp->b_luaEvent }
            // 	};

            // 	json parameters = {
            // 		{ "string", eventProp->parameters.e_string },
            // 		{ "int", eventProp->parameters.e_int },
            // 		{ "float", eventProp->parameters.e_float },
            // 		{ "double", eventProp->parameters.e_double },
            // 		{ "long", eventProp->parameters.e_long },
            // 		{ "bool", eventProp->parameters.e_boolean },
            // 		{ "vector2X", eventProp->parameters.e_Vector2.x },
            // 		{ "vector2Y", eventProp->parameters.e_Vector2.y },
            // 	};

            // 	jsonData.push_back({ "parameters", parameters });

            // 	std::string data = jsonData.dump();
            // 	eventProps.push_back(json::parse(data));
            // }
                    
            json transformProps = json::array();
            for (const auto& prop : animData.props)
            {		
                TransformProp* transformProp = static_cast<TransformProp*>(prop.get());

                json jsonData = {
                    { "positionInterpType", (int)transformProp->positionInterpType },
                    { "scaleInterpType", (int)transformProp->scaleInterpType },
                    { "rotationInterpType", (int)transformProp->rotationInterpType },											
                    { "time", transformProp->time },		
                    { "xPosition", transformProp->position.x },
                    { "yPosition", transformProp->position.y },
                    { "zPosition", transformProp->position.z },
                    { "xRotation", transformProp->rotation.x },
                    { "yRotation", transformProp->rotation.y },
                    { "zRotation", transformProp->rotation.z },
                    { "xScale", transformProp->scale.x },
                    { "yScale", transformProp->scale.y },
                    { "zScale", transformProp->scale.z },
                    { "b_posAnimated", transformProp->b_posAnimated },
                    { "b_scaleAnimated", transformProp->b_scaleAnimated },
                    { "b_rotationAnimated", transformProp->b_rotationAnimated }
                };
                std::string data = jsonData.dump();
                transformProps.push_back(json::parse(data));
            }
        
            // json spriteProps = json::array();
            // for (std::shared_ptr<SpriteProp> spriteProp : animData->spriteProps)
            // {
            // 	json jsonData = {
            // 		{ "interpType", spriteProp->interpType },
            // 		{ "speed", spriteProp->speed },
            // 		{ "time", spriteProp->time },
            // 		{ "path", spriteProp->path },
            // 		{ "xOffset", spriteProp->xOffset },
            // 		{ "yOffset", spriteProp->yOffset },
            // 		{ "tintColorX", spriteProp->tintColor.x },
            // 		{ "tintColorY", spriteProp->tintColor.y },
            // 		{ "tintColorZ", spriteProp->tintColor.z },
            // 		{ "tintColorW", spriteProp->tintColor.w },
            // 		{ "b_instantTintChange", spriteProp->b_instantTintChange },
            // 		{ "b_pathAnimated", spriteProp->b_pathAnimated },
            // 		{ "b_offsetAnimated", spriteProp->b_offsetAnimated },
            // 		{ "b_scaleAnimated", spriteProp->b_scaleAnimated },
            // 		{ "b_tintColorAnimated", spriteProp->b_tintColorAnimated }
            // 	};

            // 	std::string data = jsonData.dump();
            // 	spriteProps.push_back(json::parse(data));
            // }
            
            // json cameraProps = json::array();
            // for (std::shared_ptr<S_Camera> cameraProp : animData->cameraProps)
            // {
            // 	json jsonData = {
            // 		{ "time", cameraProp->time },
            // 		{ "b_isPrimaryCamera", cameraProp->b_isPrimaryCamera }
            // 	};
            // 	std::string data = jsonData.dump();
            // 	cameraProps.push_back(json::parse(data));
            // }

            // json canvasProps = json::array();
            // for (std::shared_ptr<S_Canvas> canvasProp : animData->canvasProps)
            // {
            // 	json jsonData = {
            // 		{ "time", canvasProp->time }
            // 	};
            // 	std::string data = jsonData.dump();
            // 	canvasProps.push_back(json::parse(data));
            // }
            
            // json audioProps = json::array();
            // for (std::shared_ptr<S_Audio> audioProp : animData->audioProps)
            // {
            // 	json jsonData = {
            // 		{ "time", audioProp->time },
            // 		{ "soundName", audioProp->soundName },
            // 		{ "b_stopAllOtherSounds", audioProp->b_stopAllOtherSounds }
            // 	};
            // 	std::string data = jsonData.dump();
            // 	audioProps.push_back(json::parse(data));
            // }
            
            // json textProps = json::array();
            // for (std::shared_ptr<S_Text> textProp : animData->textProps)
            // {
            // 	json jsonData = {
            // 		{ "time", textProp->time },
            // 		{ "fontPath", textProp->fontPath },
            // 		{ "text", textProp->text },
            // 		{ "xOffset", textProp->xOffset },
            // 		{ "yOffset", textProp->yOffset },
            // 		{ "tintColorX", textProp->tintColor.x },
            // 		{ "tintColorY", textProp->tintColor.y },
            // 		{ "tintColorZ", textProp->tintColor.z },
            // 		{ "tintColorW", textProp->tintColor.w },
            // 		{ "b_instantTintChange", textProp->b_instantTintChange },
            // 		{ "b_fontPathAnimated", textProp->b_fontPathAnimated },
            // 		{ "b_textAnimated", textProp->b_textAnimated },
            // 		{ "b_tintColorAnimated", textProp->b_tintColorAnimated },
            // 		{ "b_offsetAnimated", textProp->b_offsetAnimated }
            // 	};
            // 	std::string data = jsonData.dump();
            // 	textProps.push_back(json::parse(data));
            // }
        
            // json characterControllerProps = json::array();
            // for (std::shared_ptr<S_CharacterController> characterControllerProp : animData->characterControllerProps)
            // {
            // 	json jsonData = {
            // 		{ "time", characterControllerProp->time },
            // 		{ "b_isActive", characterControllerProp->b_isActive }
            // 	};
            // 	std::string data = jsonData.dump();
            // 	characterControllerProps.push_back(json::parse(data));
            // }

            json animProps = json::object({
                // { "event", eventProps },
                { "transform", transformProps },
                // { "sprite", spriteProps },
                // { "camera", cameraProps },
                // { "canvas", canvasProps },
                // { "audio", audioProps },
                // { "text", textProps },
                // { "characterController", characterControllerProps }
            });

            json animationData = json::object({
                { "name", animData.name },
                { "length", animData.length },
                { "b_loop", animData.b_loop },
                { "b_startAtOrigin", animData.b_startAtOrigin },
                { "animationProperties", animProps }
            });
            
            json newFileObject = json::object({ {"animation", animationData } });
            fileObject << newFileObject.dump(4).c_str() << std::endl;	
            fileObject.close();
        }

        AnimationData LoadAnimationFile(std::string path)
        {
            AnimationData animProps = AnimationData();
            animProps.path = path;
            animProps.props = std::vector<std::shared_ptr<AnimationProperty>>();

            std::ofstream fileObject;
            std::ifstream ifstream(path);
            fileObject.open(path, std::ios::in);
            std::string fileContent = "";

            if (fileObject.good())
            {
                std::string line;
                while (!ifstream.eof()) 
                {
                    std::getline(ifstream, line);
                    fileContent.append(line + "\n");
                }
            }

            fileObject.close();

            if (fileObject.good())
            {
                json fileContentJson = json::parse(fileContent);

                if (fileContentJson.contains("animation"))
                {				
                    json animationJson = fileContentJson["animation"];
                    std::string animName = JsonHelper::CheckJsonString(animationJson, "name", "name");
                    animProps.name = animName;
                    animProps.length = JsonHelper::CheckJsonFloat(animationJson, "length", animName);
                    animProps.b_loop = JsonHelper::CheckJsonBool(animationJson, "b_loop", animName);
                    animProps.b_startAtOrigin = JsonHelper::CheckJsonBool(animationJson, "b_startAtOrigin", animName);
                    
                    // json eventProps = animationJson["animationProperties"]["event"];
                    // for (int i = 0; i < eventProps.size(); i++)
                    // {
                    // 	try
                    // 	{
                    // 		std::shared_ptr<EventProp> frame = std::make_shared<EventProp>();
                    // 		frame->name = "Event";
                    // 		frame->functionName = CheckJsonString(eventProps.at(i), "functionName", animName);
                    // 		frame->time = CheckJsonFloat(eventProps.at(i), "time", animName);
                    // 		frame->b_cppEvent = CheckJsonBool(eventProps.at(i), "b_cppEvent", animName);
                    // 		frame->b_luaEvent = CheckJsonBool(eventProps.at(i), "b_luaEvent", animName);

                    // 		json parameters = eventProps.at(i)["parameters"];
                    // 		EventPropFunctionParam parameter;
                    // 		parameter.e_string = CheckJsonString(parameters, "string", animName);
                    // 		parameter.e_int = CheckJsonInt(parameters, "int", animName);
                    // 		parameter.e_float = CheckJsonFloat(parameters, "float", animName);
                    // 		parameter.e_long = CheckJsonLong(parameters, "long", animName);
                    // 		parameter.e_double = CheckJsonDouble(parameters, "double", animName);
                    // 		parameter.e_boolean = CheckJsonBool(parameters, "bool", animName);
                    // 		parameter.e_Vector2 = Vector2(CheckJsonFloat(parameters, "vector2X", animName), CheckJsonFloat(parameters, "vector2Y", animName));

                    // 		frame->parameters = parameter;
                    
                    // 		animProps.eventProps.push_back(frame);
                    // 	}
                    // 	catch (const json::out_of_range& e)
                    // 	{
                    // 		Logger::log.Err("{}", e.what());
                    // 	}
                    // }
                    
                    json transformProps = animationJson["animationProperties"]["transform"];
                    for (int i = 0; i < transformProps.size(); i++)
                    {
                        try
                        {
                            std::shared_ptr<TransformProp> frame = std::make_shared<TransformProp>();						
                            frame->positionInterpType = (InterpType)JsonHelper::CheckJsonInt(transformProps.at(i), "positionInterpType", animName);
                            frame->scaleInterpType = (InterpType)JsonHelper::CheckJsonInt(transformProps.at(i), "scaleInterpType", animName);
                            frame->rotationInterpType = (InterpType)JsonHelper::CheckJsonInt(transformProps.at(i), "rotationInterpType", animName);						
                            frame->time = JsonHelper::CheckJsonFloat(transformProps.at(i), "time", animName);						
                            frame->position = Vector3(JsonHelper::CheckJsonFloat(transformProps.at(i), "xPosition", animName),JsonHelper::CheckJsonFloat(transformProps.at(i), "yPosition", animName), JsonHelper::CheckJsonFloat(transformProps.at(i), "zPosition", animName));
                            frame->rotation = Vector3(JsonHelper::CheckJsonFloat(transformProps.at(i), "xRotation", animName), JsonHelper::CheckJsonFloat(transformProps.at(i), "yRotation", animName), JsonHelper::CheckJsonFloat(transformProps.at(i), "zRotation", animName));
                            frame->scale = Vector3(JsonHelper::CheckJsonFloat(transformProps.at(i), "xScale", animName), JsonHelper::CheckJsonFloat(transformProps.at(i), "yScale", animName), JsonHelper::CheckJsonFloat(transformProps.at(i), "zScale", animName));
                            frame->b_posAnimated = JsonHelper::CheckJsonBool(transformProps.at(i), "b_posAnimated", animName);
                            frame->b_scaleAnimated = JsonHelper::CheckJsonBool(transformProps.at(i), "b_scaleAnimated", animName);
                            frame->b_rotationAnimated = JsonHelper::CheckJsonBool(transformProps.at(i), "b_rotationAnimated", animName);
                            animProps.props.push_back(std::move(frame));
                        }
                        catch (const json::out_of_range& e)
                        {
                            Logger::log.Err("{}", e.what());
                        }
                    }
                    
                    // json spriteProps = animationJson["animationProperties"]["sprite"];
                    // for (int i = 0; i < spriteProps.size(); i++)
                    // {
                    // 	try
                    // 	{
                    // 		std::shared_ptr<SpriteProp> frame = std::make_shared<SpriteProp>();
                    // 		frame->name = "Sprite";
                    // 		frame->interpType = (InterpType)CheckJsonInt(spriteProps.at(i), "interpType", animName);
                    // 		frame->speed = CheckJsonFloat(spriteProps.at(i), "speed", animName);
                    // 		frame->time = CheckJsonFloat(spriteProps.at(i), "time", animName);
                    // 		frame->xOffset = CheckJsonFloat(spriteProps.at(i), "xOffset", animName);
                    // 		frame->yOffset = CheckJsonFloat(spriteProps.at(i), "yOffset", animName);
                    // 		frame->path = CheckJsonString(spriteProps.at(i), "path", animName);
                    // 		frame->b_instantTintChange = CheckJsonBool(spriteProps.at(i), "b_instantTintChange", animName);
                    // 		frame->tintColor = Vector4(
                    // 			CheckJsonFloat(spriteProps.at(i), "tintColorX", animName),
                    // 			CheckJsonFloat(spriteProps.at(i), "tintColorY", animName),
                    // 			CheckJsonFloat(spriteProps.at(i), "tintColorZ", animName),
                    // 			CheckJsonFloat(spriteProps.at(i), "tintColorW", animName)
                    // 		);
                    // 		frame->b_pathAnimated = CheckJsonBool(spriteProps.at(i), "b_pathAnimated", animName);
                    // 		frame->b_scaleAnimated = CheckJsonBool(spriteProps.at(i), "b_scaleAnimated", animName);
                    // 		frame->b_offsetAnimated = CheckJsonBool(spriteProps.at(i), "b_offsetAnimated", animName);
                    // 		frame->b_tintColorAnimated = CheckJsonBool(spriteProps.at(i), "b_tintColorAnimated", animName);
                    // 		animProps.spriteProps.push_back(frame);
                    // 	}
                    // 	catch (const json::out_of_range& e)
                    // 	{
                    // 		Logger::log.Err("{}", e.what());
                    // 	}
                    // }
                    
                    // json cameraProps = animationJson["animationProperties"]["camera"];
                    // for (int i = 0; i < cameraProps.size(); i++)
                    // {
                    // 	try
                    // 	{
                    // 		std::shared_ptr<S_Camera> frame = std::make_shared<S_Camera>();
                    // 		frame->name = "Camera";
                    // 		frame->time = CheckJsonFloat(cameraProps.at(i), "time", animName);
                    // 		frame->b_isPrimaryCamera = CheckJsonBool(cameraProps.at(i), "b_isPrimaryCamera", animName);
                    // 		animProps.cameraProps.push_back(frame);
                    // 	}
                    // 	catch (const json::out_of_range& e)
                    // 	{
                    // 		Logger::log.Err("{}", e.what());
                    // 	}
                    // }
                    
                    // json canvasProps = animationJson["animationProperties"]["canvas"];
                    // for (int i = 0; i < canvasProps.size(); i++)
                    // {
                    // 	try
                    // 	{
                    // 		std::shared_ptr<S_Canvas> frame = std::make_shared<S_Canvas>();
                    // 		frame->name = "Canvas";
                    // 		frame->time = CheckJsonFloat(canvasProps.at(i), "time", animName);
                    // 		animProps.canvasProps.push_back(frame);
                    // 	}
                    // 	catch (const json::out_of_range& e)
                    // 	{
                    // 		Logger::log.Err("{}", e.what());
                    // 	}
                    // }
                    
                    // json audioProps = animationJson["animationProperties"]["audio"];
                    // for (int i = 0; i < audioProps.size(); i++)
                    // {
                    // 	try
                    // 	{
                    // 		std::shared_ptr<S_Audio> frame = std::make_shared<S_Audio>();
                    // 		frame->name = "Audio";
                    // 		frame->time = CheckJsonFloat(audioProps.at(i), "time", animName);
                    // 		frame->soundName = CheckJsonString(audioProps.at(i), "soundName", animName);
                    // 		frame->b_stopAllOtherSounds = CheckJsonBool(audioProps.at(i), "b_stopAllOtherSounds", animName);
                    // 		animProps.audioProps.push_back(frame);
                    // 	}
                    // 	catch (const json::out_of_range& e)
                    // 	{
                    // 		Logger::log.Err("{}", e.what());
                    // 	}
                    // }
                    
                    // json textProps = animationJson["animationProperties"]["text"];
                    // for (int i = 0; i < textProps.size(); i++)
                    // {
                    // 	try
                    // 	{
                    // 		std::shared_ptr<S_Text> frame = std::make_shared<S_Text>();
                    // 		frame->name = "Text";
                    // 		frame->time = CheckJsonFloat(textProps.at(i), "time", animName);
                    // 		frame->fontPath = CheckJsonString(textProps.at(i), "fontPath", frame->name);
                    // 		frame->text = CheckJsonString(textProps.at(i), "text", frame->name);
                    // 		frame->tintColor = Vector4(
                    // 			CheckJsonFloat(textProps.at(i), "tintColorX", frame->name),
                    // 			CheckJsonFloat(textProps.at(i), "tintColorY", frame->name),
                    // 			CheckJsonFloat(textProps.at(i), "tintColorZ", frame->name),
                    // 			CheckJsonFloat(textProps.at(i), "tintColorW", frame->name)
                    // 		);
                    // 		frame->b_fontPathAnimated = CheckJsonBool(textProps.at(i), "b_fontPathAnimated", animName);
                    // 		frame->b_textAnimated = CheckJsonBool(textProps.at(i), "b_textAnimated", animName);
                    // 		frame->b_tintColorAnimated = CheckJsonBool(textProps.at(i), "b_tintColorAnimated", animName);
                    // 		frame->b_offsetAnimated = CheckJsonBool(textProps.at(i), "b_offsetAnimated", animName);
                    // 		animProps.textProps.push_back(frame);
                    // 	}
                    // 	catch (const json::out_of_range& e)
                    // 	{
                    // 		Logger::log.Err("{}", e.what());
                    // 	}
                    // }
                    
                    // json characterControllerProps = animationJson["animationProperties"]["characterController"];
                    // for (int i = 0; i < characterControllerProps.size(); i++)
                    // {
                    // 	try
                    // 	{
                    // 		std::shared_ptr<S_CharacterController> frame = std::make_shared<S_CharacterController>();
                    // 		frame->name = "CharacterController";
                    // 		frame->time = CheckJsonFloat(characterControllerProps.at(i), "time", animName);
                    // 		frame->b_isActive = CheckJsonBool(characterControllerProps.at(i), "b_isActive", animName);
                    // 		animProps.characterControllerProps.push_back(frame);
                    // 	}
                    // 	catch (const json::out_of_range& e)
                    // 	{
                    // 		Logger::log.Err("{}", e.what());
                    // 	}
                    // }
                }
            }

            return animProps;
        }
    }
}