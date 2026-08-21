#include "managers/Assets.h"
#include "managers/Controls.h"
#include "tools/FileHelper.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"
#include "tools/Time.h"
#include <SDL_stdinc.h>
#include <cstdint>

#include "tools/Logger.h"


namespace FlatEngine
{
    namespace Controls
    {
        MappingContext engineContext;
        std::vector<MappingContext> mappingContexts;
        std::vector<std::string> KeyBindingsAvailable;
        std::vector<SDL_Joystick*> gamepads;
        std::string selectedMappingContextName = "";		
        std::vector<SDL_Event> events;	
		std::vector<std::string> firedKeys;	    
        std::vector<std::string> pressedKeys;    

		MappingContext::MappingContext()
		{
			m_name = "";		
			m_remapStartTime = 0;
			m_actionToRemap = "";
			b_waitingForRemap = false;                    
            b_waitingForInput = false;
            tempPressType = PressType_Down;
            tempKeycode = 0;
            tempActionName = "";  
		}

		json MappingContext::GetData()
		{
			json mappings;

			for (auto& inputMapping : m_bindings)
			{
				json keybindJson = json::array(); 

				for (int i = 0; i < inputMapping.second.actions.size(); i++)
                {
					json action = {
						{ "pressType", (int)inputMapping.second.actions[i].pressType },
                        { "actionName", inputMapping.second.actions[i].actionName }
					};
					
                    keybindJson.push_back(action);
                }

				mappings.emplace(inputMapping.first, keybindJson);
			}

			json jsonData = {
				{ "name", m_name },
				{ "mappings", mappings }
			};

			return jsonData;
		}

        void MappingContext::PutData(json jsonData, std::string name)
        {                        
            if (jsonData.empty())
                return;

            m_name = JsonHelper::CheckJsonString(jsonData, "name", name);

            if (JsonHelper::JsonContains(jsonData, "mappings", name))
            {
                auto mappings = jsonData["mappings"];                

                for (auto& [key, keycodeJson] : mappings.items())
                {               
                    for (auto actionJson : keycodeJson)
                    {                   
                        int pressType = JsonHelper::CheckJsonInt(actionJson, "pressType", name); 
                        AddKeyBinding(key, JsonHelper::CheckJsonString(actionJson, "actionName", name), (pressType != -1 ? (PressType)pressType : PressType_Down));                                            
                    }
                }          
            }      
        }

		void MappingContext::SetName(std::string contextName)
		{
			m_name = contextName;
		}

		std::string MappingContext::GetName()
		{
			return m_name;
		}

		void MappingContext::SetPath(std::string filepath)
		{
			m_path = filepath;
		}

		std::string MappingContext::GetPath()
		{
			return m_path;
		}

		void MappingContext::AddKeyBinding(std::string keycode, std::string actionName, PressType pressType)
		{			
			ActionMapping actionMap;
			actionMap.actionName = actionName;
			actionMap.pressType = pressType;			

			if (m_bindings.count(keycode))
			{
				if (m_bindings.at(keycode).PressTypeTaken(pressType))
				{
					RemoveKeyBinding(keycode, pressType);
				}			

				m_bindings.at(keycode).actions.push_back(actionMap);
			}
			else
			{
				InputMapping inputMap;
				inputMap.keycode = keycode;
				inputMap.actions.push_back(actionMap);

				m_bindings.emplace(keycode, inputMap);
			}
		}

		void MappingContext::RemoveKeyBinding(std::string keycode, PressType pressType, std::string actionName)
		{
			if (m_bindings.count(keycode))
			{
				for (auto iter = m_bindings.at(keycode).actions.begin(); iter != m_bindings.at(keycode).actions.end(); iter++)
				{
					if ((*iter).pressType == pressType && (actionName == "" || actionName == (*iter).actionName))
					{
						m_bindings.at(keycode).actions.erase(iter);                                               
						return;
					}
				}
			}
		}

        void MappingContext::AddActiveAction(ActionMapping* actionMapping)
        {
            for (int i = 0; i < m_activeActionMappings.size(); i++)
            {
                if (m_activeActionMappings[i] == actionMapping)
                    return;
            }

            m_activeActionMappings.push_back(actionMapping);
        }

        void MappingContext::RemoveActiveAction(ActionMapping* actionMapping)
        {
            for (auto iter = m_activeActionMappings.begin(); iter != m_activeActionMappings.end(); iter++)
            {
                if (actionMapping->actionName == (*iter)->actionName)
                {
                    m_activeActionMappings.erase(iter);
                    break;
                }
            }
        }

		bool MappingContext::FireEvent(std::string keyBinding, SDL_Event event, PressType pressType)
		{
            pressedKeys.push_back(keyBinding);

			if (b_waitingForRemap && !RemapTimedOut(Time::Time()))
			{			
				b_waitingForRemap = false;
				// RemoveKeyBinding(keyBinding, m_actionToRemap)
				// AddKeyBinding(keyBinding, m_actionToRemap, pressType);
				Controls::SaveMappingContext(m_path, *this);
			}
			else if (RemapTimedOut(Time::Time()))
			{
				b_waitingForRemap = false;
			}

			bool b_actionFound = false;

			if (m_bindings.count(keyBinding))
			{
				InputMapping* inputMap = &m_bindings.at(keyBinding);
				
				switch (pressType)
				{
				case PressType_Down:
				{					
					ActionMapping* downAction = inputMap->GetAction(PressType_Down);
					ActionMapping* holdAction = inputMap->GetAction(PressType_Hold);

					if (downAction != nullptr)
					{
                        AddActiveAction(downAction);
						downAction->event = event;
						downAction->b_fired = true;
						b_actionFound = true;
					}
					if (holdAction != nullptr)
					{
                        AddActiveAction(holdAction);                        
						holdAction->event = event; 
						holdAction->b_fired = true;
						b_actionFound = true;
					}
					break;
				}
				case PressType_Up:
				{
					ActionMapping* upAction = inputMap->GetAction(PressType_Up);
					ActionMapping* holdAction = inputMap->GetAction(PressType_Hold);

					if (upAction != nullptr)
					{
                        AddActiveAction(upAction);                           
						upAction->event = event;
						upAction->b_fired = true;
						b_actionFound = true;
					}
					if (holdAction != nullptr)
					{
                        RemoveActiveAction(holdAction);
						holdAction->event = SDL_Event();
						holdAction->b_fired = false;
						// b_actionFound = true;
					}
					break;
				}
				default:
					break;
				}
			}

			return b_actionFound;
		}

		void MappingContext::UnFireEvent(std::string keyBinding)
		{
			if (m_bindings.count(keyBinding))
			{			
				InputMapping* inputMap = &m_bindings.at(keyBinding);

				for (int i = 0; i < inputMap->actions.size(); i++)
				{                
                    RemoveActiveAction(&inputMap->actions[i]);
                    inputMap->actions[i].b_fired = false;
                    inputMap->actions[i].event = SDL_Event();                    
				}
			}            
		}

		// SDL_Event MappingContext::GetInputActionEvent(std::string actionName)
		// {
			// if (m_bindingsByInput.count(actionName) > 0)
			// {
			// 	std::string keyBinding = m_bindingsByInput.at(actionName);

			// 	if (m_bindings.count(keyBinding))
			// 	{
			// 		std::shared_ptr<InputMapping> inputMap = m_bindings.at(keyBinding);

			// 		if (inputMap->actions.at(PressType_Down).actionName == actionName)
			// 		{
			// 			return inputMap->actions.at(PressType_Down).event;
			// 		}
			// 		else if (inputMap->actions.at(PressType_Up).actionName == actionName)
			// 		{
			// 			return inputMap->actions.at(PressType_Up).event;
			// 		}
			// 		else if (inputMap->actions.at(PressType_Hold).actionName == actionName)
			// 		{
			// 			return inputMap->actions.at(PressType_Hold).event;
			// 		}
			// 		else if (inputMap->actions.at(PressType_Double).actionName == actionName)
			// 		{
			// 			return inputMap->actions.at(PressType_Double).event;
			// 		}
			// 	}
			// }	

			// return SDL_Event();
		// }

        void MappingContext::ResetActiveActionMappings()
        {
            for (auto iter = m_activeActionMappings.begin(); iter != m_activeActionMappings.end(); iter++)
            {
                if ((*iter)->pressType != PressType_Hold || ((*iter)->pressType == PressType_Hold && (*iter)->event.type == SDL_JOYAXISMOTION && (*iter)->event.jaxis.value > -JOYSTICK_DEAD_ZONE && (*iter)->event.jaxis.value < JOYSTICK_DEAD_ZONE))
                {
                    (*iter)->b_fired = false;
                    m_activeActionMappings.erase(iter);
                    break;
                }
            }
        }

		bool MappingContext::ActionPressed(std::string actionName)
		{
			for (ActionMapping* activeAction : m_activeActionMappings)
			{
                if (activeAction == nullptr)
                    break;
                
                if (activeAction->actionName == actionName && activeAction->b_fired)
                {
                    return true;
                }
			}

			return false;
		}

		std::unordered_map<std::string, InputMapping>& MappingContext::GetInputMappings()
		{
			return m_bindings;
		}

		std::vector<InputMapping*> MappingContext::GetInputMappingsLua()
		{
			std::vector<InputMapping*> inputActions;

			for (std::pair<std::string, InputMapping> inputAction : m_bindings)
			{
				InputMapping newMapping;
				inputActions.push_back(&inputAction.second);
			}

			return inputActions;
		}

		// std::shared_ptr<InputMapping> MappingContext::GetInputMapping(std::string actionName)
		// {
		// 	if (m_bindingsByInput.count(actionName) > 0)
		// 	{
		// 		std::string keyBinding = m_bindingsByInput.at(actionName);

		// 		if (m_bindings.count(keyBinding))
		// 		{
		// 			std::shared_ptr<InputMapping> inputMap = m_bindings.at(keyBinding);

		// 			if (inputMap->actions.at(PressType_Down).actionName == actionName)
		// 			{
		// 				return inputMap;
		// 			}
		// 			else if (inputMap->actions.at(PressType_Up).actionName == actionName)
		// 			{
		// 				return inputMap;
		// 			}
		// 			else if (inputMap->actions.at(PressType_Hold).actionName == actionName)
		// 			{
		// 				return inputMap;
		// 			}
		// 			else if (inputMap->actions.at(PressType_Double).actionName == actionName)
		// 			{
		// 				return inputMap;
		// 			}
		// 		}
		// 	}

		// 	return nullptr;
		// }

        Sint16 MappingContext::GetAxis(std::string actionName)
		{
            Sint16 axis = 0;

			for (ActionMapping* activeAction : m_activeActionMappings)
			{
                if (activeAction->event.type == SDL_JOYAXISMOTION && activeAction->actionName == actionName)
                    axis = activeAction->event.jaxis.value;
                // else if (activeAction->event.type == SDL_J)
			}

			return axis;
		}

		void MappingContext::BeginRemap(std::string contextName, std::string actionToRemap)
		{
			m_actionToRemap = actionToRemap;
			b_waitingForRemap = true;
			m_remapStartTime = Time::Time();
		}

		bool MappingContext::WaitingForRemap()
		{
			return b_waitingForRemap;
		}

		bool MappingContext::RemapTimedOut(Uint32 currentTime)
		{
			return m_remapStartTime + REMAP_TIMEOUT_LENGTH > Time::Time();
		}

        
        void SaveMappingContext(std::string path, MappingContext context)
        {
			JsonHelper::WriteJsonToFile(context.GetData(), path);
            InitializeMappingContexts();
        }

        void InitializeMappingContexts()
        {
            mappingContexts.clear();

            if (KeyBindingsAvailable.size() == 0)
            {
                // Get available input keycodes from MappingContext.h
                for (std::pair<long, std::string> inputKeycode : F_MappedMouseCodes)
                {
                    KeyBindingsAvailable.push_back(inputKeycode.second);
                }
                for (std::pair<long, std::string> inputKeycode : F_MappedKeyboardCodes)
                {
                    KeyBindingsAvailable.push_back(inputKeycode.second);
                }
                for (std::pair<long, std::string> inputKeycode : F_MappedXInputButtonCodes)
                {
                    KeyBindingsAvailable.push_back(inputKeycode.second);
                }
                for (std::pair<long, std::string> inputKeycode : F_MappedXInputDPadCodes)
                {
                    KeyBindingsAvailable.push_back(inputKeycode.second);
                }
                for (std::pair<long, std::string> inputKeycode : F_MappedXInputAnalogCodes)
                {
                    KeyBindingsAvailable.push_back(inputKeycode.second);
                }
            }

            std::vector<std::string> mappingContextFiles = std::vector<std::string>();
            mappingContextFiles = FileHelper::FindAllFilesWithExtension(Assets::assetManager.GetDir("projectDir"), ".mpc");

            for (std::string path : mappingContextFiles)
            {               
                json contextData = JsonHelper::LoadFileData(path);
                MappingContext mappingContext;
                mappingContext.PutData(contextData, FileHelper::GetFilenameFromPath(path));
                mappingContext.SetPath(path);
                mappingContexts.push_back(mappingContext);
            }

            json engineContextData = JsonHelper::LoadFileData("../engine/mappingContexts/EngineContext.mpc");
            engineContext.PutData(engineContextData, "EngineContext");
        }

        MappingContext* GetMappingContext(std::string contextName)
        {
            for (std::vector<MappingContext>::iterator iter = mappingContexts.begin(); iter != mappingContexts.end(); iter++)
            {
                if (iter->GetName() == contextName)
                {
                    return &(*iter);
                }
            }

            return nullptr;
        }

        void CreateNewMappingContextFile(std::string fileName, std::string path)
        {
            std::string filePath = "";		
            MappingContext newContext = MappingContext();

            if (path == "")
            {
                filePath = Assets::assetManager.GetDir("projectDir") + "/mappingContexts" + "/" + fileName + ".mpc";
            }
            else
            {
                filePath = path + "/" + fileName + ".mpc";
            }

            newContext.SetPath(filePath);
            newContext.SetName(fileName);
            SaveMappingContext(filePath, newContext);
            mappingContexts.push_back(newContext);
        }

        void HandleContextEvents(MappingContext& context, SDL_Event event)
        {
            // Mouse Events
            if (event.type == SDL_MOUSEMOTION)
            {
                if (F_MappedMouseCodes.count(Mouse_motion))
                {								
                    std::string key = F_MappedMouseCodes.at(Mouse_motion);

                    if (context.FireEvent(key, event, PressType::PressType_Down))
                    {
                        firedKeys.push_back(key);
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {		
                if (F_MappedMouseCodes.count(event.button.button))
                {
                    std::string key = F_MappedMouseCodes.at(event.button.button);

                    if (context.FireEvent(key, event, PressType::PressType_Down))
                    {
                        firedKeys.push_back(key);
                    }
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (F_MappedMouseCodes.count(event.button.button))
                {
                    std::string key = F_MappedMouseCodes.at(event.button.button);

                    if (context.FireEvent(key, event, PressType::PressType_Up))
                    {
                        firedKeys.push_back(key);
                    }
                }
            }
            // Keyboard Keys Down
            if (event.type == SDL_KEYDOWN)
            {
                if (F_MappedKeyboardCodes.count(event.key.keysym.sym))
                {
                    std::string key = F_MappedKeyboardCodes.at(event.key.keysym.sym);

                    if (context.FireEvent(key, event, PressType::PressType_Down))
                    {
                        firedKeys.push_back(key);
                    }
                }
            }
            // Keyboard Keys Up
            else if (event.type == SDL_KEYUP)
            {
                // Clear Mapping Context Events of buttons that are released
                if (F_MappedKeyboardCodes.count(event.key.keysym.sym))
                {
                    std::string key = F_MappedKeyboardCodes.at(event.key.keysym.sym);

                    if (context.FireEvent(key, event, PressType::PressType_Up))
                    {
                        firedKeys.push_back(key);
                    }
                }
            }
            // Axis (analog inputs)
            else if (event.type == SDL_JOYAXISMOTION)
            {
                // Axis (analogs)
                //if (event.jaxis.which == 0)
                //{			
                if (F_MappedXInputAnalogCodes.count(event.jaxis.axis))
                {
                    std::string key = F_MappedXInputAnalogCodes.at(event.jaxis.axis);	

                    if (event.jaxis.value < -JOYSTICK_DEAD_ZONE || event.jaxis.value > JOYSTICK_DEAD_ZONE)
                    {
                        context.FireEvent(key, event, PressType::PressType_Down);
                    }					                    
                    else {
                        context.UnFireEvent(key);
                    }
                }
            }
            // Buttons Down
            else if (event.type == SDL_JOYBUTTONDOWN)
            {
                if (F_MappedXInputButtonCodes.count(event.jbutton.button))
                {
                    std::string key = F_MappedXInputButtonCodes.at(event.jbutton.button);

                    if (context.FireEvent(key, event, PressType::PressType_Down))
                    {					
                        firedKeys.push_back(key);
                    }
                }
            }
            // Buttons Up
            else if (event.type == SDL_JOYBUTTONUP)
            {
                if (F_MappedXInputButtonCodes.count(event.jbutton.button))
                {
                    std::string key = F_MappedXInputButtonCodes.at(event.jbutton.button);

                    if (context.FireEvent(key, event, PressType::PressType_Up))
                    {
                        firedKeys.push_back(key);
                    }
                }
            }
            // Hats
            else if (event.type == SDL_JOYHATMOTION)
            {
                if (F_MappedXInputDPadCodes.count(event.jhat.value))
                {
                    std::string key = F_MappedXInputDPadCodes.at(event.jhat.value);                    

                    if (context.FireEvent(key, event, PressType::PressType_Down))
                    {
                        firedKeys.push_back(key);
                    }
                    // TODO
                }
            }
        }

        void RemapInputAction(std::string contextName, std::string inputAction, Uint32 timeoutTime)
        {
            for (MappingContext &context : mappingContexts)
            {
                if (context.GetName() == contextName)
                {
                    // context.SetRemapStartTime(FlatEngine::Time::Time());
                    // context.SetWaitingForRemap(true);
                    // context.SetActionToRemap(inputAction);
                    // context.SetRemapTimeoutTime(timeoutTime);
                }
            }
        }

        void HandleControllerConnections()
        {
            static int controllersConnected = 0;
            if (SDL_NumJoysticks() != controllersConnected)
            {
                // Clean up old gamepads
                for (SDL_Joystick* gamepad : gamepads)
                {
                    SDL_JoystickClose(gamepad);
                    gamepad = nullptr;
                }

                controllersConnected = SDL_NumJoysticks();
                for (int i = 0; i < controllersConnected; i++)
                {
                    SDL_Joystick* gamepad = SDL_JoystickOpen(i);
                    if (gamepad == nullptr)
                    {
                        Logger::log.Warn("Unable to open game controller! SDL Error: {}\n", SDL_GetError());
                    }
                    else
                    {
                        Logger::log.Trace("Gamepad connected...");
                        gamepads.push_back(gamepad);
                    }
                }
            }
        }
    }
}