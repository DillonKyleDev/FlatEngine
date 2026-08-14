#include "managers/Assets.h"
#include "managers/Controls.h"
#include "tools/FileHelper.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"
#include "tools/Time.h"

#include <fstream>


namespace FlatEngine
{
    namespace Controls
    {
        std::vector<MappingContext> mappingContexts = std::vector<MappingContext>();
        std::vector<std::string> keyBindingsAvailable = std::vector<std::string>();
        std::vector<SDL_Joystick*> gamepads = std::vector<SDL_Joystick*>();
        std::string selectedMappingContextName = "";


		MappingContext::MappingContext()
		{
			m_name = "";		
			m_inputsByBinding = std::map<std::string, std::shared_ptr<InputMapping>>();
			m_bindingsByInput = std::map<std::string, std::string>();
			m_remapTimeoutTime = 0;
			m_remapStartTime = 0;
			m_actionToRemap = "";
			m_b_waitingForRemap = false;
		}

		MappingContext::~MappingContext()
		{
		}

		std::string MappingContext::GetData()
		{
			json mappings;

			for (std::pair<std::string, std::shared_ptr<InputMapping>> inputMapping : m_inputsByBinding)
			{
				std::string keyString = inputMapping.first;
				std::string downAction = inputMapping.second->pressActions.downAction.actionName;
				std::string upAction = inputMapping.second->pressActions.upAction.actionName;
				std::string holdAction = inputMapping.second->pressActions.holdAction.actionName;
				std::string doubleAction = inputMapping.second->pressActions.doubleAction.actionName;

				json actions = 
				{					
					{ "downAction", downAction },	
					{ "upAction", upAction },
					{ "holdAction", holdAction },
					{ "doubleAction", doubleAction },
				};

				mappings.emplace(keyString, actions);
			}

			json jsonData = {
				{ "name", m_name },
				{ "mappings", mappings }
			};

			std::string data = jsonData.dump(4);		
			return data;
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

		void MappingContext::AddKeyBinding(std::string keyBinding, std::string actionName, PressType pressType)
		{
			if (actionName != "")
			{
				// Remove existing binding using that actionName if it exists.
				if (m_bindingsByInput.count(actionName))
				{
					std::string existingKeyBind = m_bindingsByInput.at(actionName);
					if (existingKeyBind != keyBinding)
					{
						if (m_inputsByBinding.count(existingKeyBind))
						{
							std::shared_ptr<InputMapping> inputMap = m_inputsByBinding.at(existingKeyBind);
							switch (pressType)
							{
							case PressType_Down:
							{							
								inputMap->pressActions.downAction.actionName = "";
								break;
							}
							case PressType_Up:
							{							
								inputMap->pressActions.upAction.actionName = "";
								break;
							}
							case PressType_Hold:
							{							
								inputMap->pressActions.holdAction.actionName = "";
								break;
							}
							case PressType_Double:
							{							
								inputMap->pressActions.doubleAction.actionName = "";
								break;
							}
							default:
								break;
							}
							Logger::log.Info("No duplicate action names allowed in the same Mapping Context. Previous keybinding with that action name removed.");
						}
					}
				}

				std::shared_ptr<InputMapping> inputMap = nullptr;

				if (m_inputsByBinding.count(keyBinding) > 0)
				{
					inputMap = m_inputsByBinding.at(keyBinding);
				}
				else
				{
					inputMap = std::make_shared<InputMapping>();
					inputMap->keyCode = keyBinding;
				}

				inputMap->pressActions.downAction.keyCode = keyBinding;
				inputMap->pressActions.upAction.keyCode = keyBinding;
				inputMap->pressActions.holdAction.keyCode = keyBinding;
				inputMap->pressActions.doubleAction.keyCode = keyBinding;

				std::string oldActionName = "";

				switch (pressType)
				{
				case PressType_Down:
				{
					oldActionName = inputMap->pressActions.downAction.actionName;
					inputMap->pressActions.downAction.actionName = actionName;
					inputMap->pressActions.downAction.keyCode = keyBinding;
					break;
				}
				case PressType_Up:
				{
					oldActionName = inputMap->pressActions.upAction.actionName;
					inputMap->pressActions.upAction.actionName = actionName;
					inputMap->pressActions.upAction.keyCode = keyBinding;
					break;
				}
				case PressType_Hold:
				{
					oldActionName = inputMap->pressActions.holdAction.actionName;
					inputMap->pressActions.holdAction.actionName = actionName;
					inputMap->pressActions.holdAction.keyCode = keyBinding;
					break;
				}
				case PressType_Double:
				{
					oldActionName = inputMap->pressActions.doubleAction.actionName;
					inputMap->pressActions.doubleAction.actionName = actionName;
					inputMap->pressActions.doubleAction.keyCode = keyBinding;
					break;
				}
				default:
					break;
				}
				
				if (m_inputsByBinding.count(keyBinding) == 0)
				{
					std::pair<std::string, std::shared_ptr<InputMapping>> bindingPair = { keyBinding, inputMap };
					m_inputsByBinding.emplace(bindingPair);
				}


				std::pair<std::string, std::string> actionPair = { actionName, keyBinding };

				// erase old inputAction pair from m_inputsByAction if the action name has changed
				if (oldActionName != actionName && m_bindingsByInput.count(oldActionName))
				{
					m_bindingsByInput.erase(oldActionName);
				}			
				
				if (m_bindingsByInput.count(actionName))
				{
					m_bindingsByInput.at(actionName) = keyBinding;
				}
				else
				{
					m_bindingsByInput.emplace(actionPair);
				}
			}
			else if (m_inputsByBinding.count(keyBinding) > 0)
			{
				std::shared_ptr<InputMapping> inputMap = m_inputsByBinding.at(keyBinding);
				std::string oldActionName = "";

				switch (pressType)
				{
				case PressType_Down:
				{
					oldActionName = inputMap->pressActions.downAction.actionName;
					inputMap->pressActions.downAction.actionName = "";
					inputMap->pressActions.downAction.keyCode = "";
					break;
				}
				case PressType_Up:
				{
					oldActionName = inputMap->pressActions.upAction.actionName;
					inputMap->pressActions.upAction.actionName = "";
					inputMap->pressActions.upAction.keyCode = "";
					break;
				}
				case PressType_Hold:
				{
					oldActionName = inputMap->pressActions.holdAction.actionName;
					inputMap->pressActions.holdAction.actionName = "";
					inputMap->pressActions.upAction.keyCode = "";
					break;
				}
				case PressType_Double:
				{
					oldActionName = inputMap->pressActions.doubleAction.actionName;
					inputMap->pressActions.doubleAction.actionName = "";
					inputMap->pressActions.doubleAction.keyCode = "";
					break;
				}
				default:
					break;
				}

				if (m_bindingsByInput.count(actionName))
				{
					m_bindingsByInput.erase(actionName);
				}
			}
		}

		bool MappingContext::FireEvent(std::string keyBinding, SDL_Event event, PressType pressType)
		{
			std::string actionName = "";

			if (m_b_waitingForRemap && !RemapTimedOut(Time::Time()))
			{			
				m_b_waitingForRemap = false;
				Controls::SaveMappingContext(m_path, *this);
			}
			else if (RemapTimedOut(Time::Time()))
			{
				m_b_waitingForRemap = false;
			}

			if (m_inputsByBinding.count(keyBinding) > 0)
			{
				std::shared_ptr<InputMapping> inputMap = m_inputsByBinding.at(keyBinding);
				switch (pressType)
				{
				case PressType_Down:
				{
					if (inputMap->pressActions.downAction.actionName != "" || inputMap->pressActions.holdAction.actionName != "")
					{
						inputMap->pressActions.downAction.event = event;
						inputMap->pressActions.downAction.b_fired = true;
						inputMap->pressActions.holdAction.event = event;
						inputMap->pressActions.holdAction.b_fired = true;
						return true;
					}
					break;
				}
				case PressType_Up:
				{
					bool b_actionFound = false;
					if (inputMap->pressActions.upAction.actionName != "")
					{
						inputMap->pressActions.upAction.event = event;
						inputMap->pressActions.upAction.b_fired = true;
						b_actionFound = true;
					}
					if (inputMap->pressActions.holdAction.actionName != "")
					{
						inputMap->pressActions.holdAction.event = SDL_Event();
						inputMap->pressActions.holdAction.b_fired = false;
						b_actionFound = true;
					}

					return b_actionFound;
					break;
				}
				default:
					break;
				}
			}

			return false;
		}

		void MappingContext::UnFireEvent(std::string keyBinding)
		{
			if (m_inputsByBinding.count(keyBinding) > 0)
			{			
				std::shared_ptr<InputMapping> inputMap = m_inputsByBinding.at(keyBinding);

				inputMap->pressActions.downAction.b_fired = false;
				inputMap->pressActions.downAction.event = SDL_Event();
				inputMap->pressActions.upAction.b_fired = false;
				inputMap->pressActions.upAction.event = SDL_Event();
				inputMap->pressActions.doubleAction.b_fired = false;
				inputMap->pressActions.doubleAction.event = SDL_Event();
			}
		}

		SDL_Event MappingContext::GetInputActionEvent(std::string actionName)
		{
			if (m_bindingsByInput.count(actionName) > 0)
			{
				std::string keyBinding = m_bindingsByInput.at(actionName);

				if (m_inputsByBinding.count(keyBinding))
				{
					std::shared_ptr<InputMapping> inputMap = m_inputsByBinding.at(keyBinding);

					if (inputMap->pressActions.downAction.actionName == actionName)
					{
						return inputMap->pressActions.downAction.event;
					}
					else if (inputMap->pressActions.upAction.actionName == actionName)
					{
						return inputMap->pressActions.upAction.event;
					}
					else if (inputMap->pressActions.holdAction.actionName == actionName)
					{
						return inputMap->pressActions.holdAction.event;
					}
					else if (inputMap->pressActions.doubleAction.actionName == actionName)
					{
						return inputMap->pressActions.doubleAction.event;
					}
				}
			}	

			return SDL_Event();
		}

		bool MappingContext::ActionPressed(std::string actionName)
		{
			if (m_bindingsByInput.count(actionName) > 0)
			{
				std::string keyBinding = m_bindingsByInput.at(actionName);

				if (m_inputsByBinding.count(keyBinding))
				{
					std::shared_ptr<InputMapping> inputMap = m_inputsByBinding.at(keyBinding);

					if (inputMap->pressActions.downAction.actionName == actionName)
					{
						return inputMap->pressActions.downAction.b_fired;
					}
					else if (inputMap->pressActions.upAction.actionName == actionName)
					{
						return inputMap->pressActions.upAction.b_fired;
					}
					else if (inputMap->pressActions.holdAction.actionName == actionName)
					{
						return inputMap->pressActions.holdAction.b_fired;
					}
					else if (inputMap->pressActions.doubleAction.actionName == actionName)
					{
						return inputMap->pressActions.doubleAction.b_fired;
					}
				}
			}

			return false;
		}

		std::map<std::string, std::shared_ptr<InputMapping>> MappingContext::GetInputActions()
		{
			return m_inputsByBinding;
		}

		std::vector<std::shared_ptr<InputMapping>> MappingContext::GetInputMappingsLua()
		{
			std::vector<std::shared_ptr<InputMapping>> inputActions = std::vector<std::shared_ptr<InputMapping>>();

			for (std::pair<std::string, std::shared_ptr<InputMapping>> inputAction : m_inputsByBinding)
			{
				InputMapping newMapping;
				inputActions.push_back(inputAction.second);
			}

			return inputActions;
		}

		std::shared_ptr<InputMapping> MappingContext::GetInputMapping(std::string actionName)
		{
			if (m_bindingsByInput.count(actionName) > 0)
			{
				std::string keyBinding = m_bindingsByInput.at(actionName);

				if (m_inputsByBinding.count(keyBinding))
				{
					std::shared_ptr<InputMapping> inputMap = m_inputsByBinding.at(keyBinding);

					if (inputMap->pressActions.downAction.actionName == actionName)
					{
						return inputMap;
					}
					else if (inputMap->pressActions.upAction.actionName == actionName)
					{
						return inputMap;
					}
					else if (inputMap->pressActions.holdAction.actionName == actionName)
					{
						return inputMap;
					}
					else if (inputMap->pressActions.doubleAction.actionName == actionName)
					{
						return inputMap;
					}
				}
			}

			return nullptr;
		}

		void MappingContext::SetWaitingForRemap(bool b_waiting)
		{
			m_b_waitingForRemap = b_waiting;
		}

		bool MappingContext::WaitingForRemap()
		{
			return m_b_waitingForRemap;
		}

		void MappingContext::SetActionToRemap(std::string actionToRemap)
		{
			m_actionToRemap = actionToRemap;
		}

		std::string MappingContext::GetActionToRemap()
		{
			return m_actionToRemap;
		}

		void MappingContext::SetRemapStartTime(Uint32 startTime)
		{
			m_remapStartTime = startTime;
		}

		bool MappingContext::RemapTimedOut(Uint32 currentTime)
		{
			return !(m_remapTimeoutTime != 0 && m_remapStartTime + m_remapTimeoutTime > currentTime);
		}

		void MappingContext::SetRemapTimeoutTime(Uint32 timeoutTime)
		{
			m_remapTimeoutTime = timeoutTime;
		}

		Uint32 MappingContext::GetRemapTimeoutTime()
		{
			return m_remapTimeoutTime;
		}

		bool MappingContext::InputActionNameTaken(std::string actionName, std::string keyCode)
		{
			if (m_bindingsByInput.count(actionName))
			{
				return m_bindingsByInput.at(actionName) != keyCode;
			}
			return false;
		}
        
        void SaveMappingContext(std::string path, MappingContext context)
        {
            std::ofstream fileObject;
            std::ifstream ifstream(path);

            // Delete old contents of the file
            fileObject.open(path, std::ofstream::out | std::ofstream::trunc);
            fileObject.close();

            // Opening file in append mode
            fileObject.open(path, std::ios::app);

            fileObject << context.GetData().c_str() << std::endl;
            fileObject.close();

            InitializeMappingContexts();
        }

        void AddMappingContext(std::string path)
        {
            MappingContext newContext = MappingContext();

            json contextData = JsonHelper::LoadFileData(path);
            if (contextData != nullptr)
            {
                newContext.SetName(JsonHelper::CheckJsonString(contextData, "name", "MappingContext"));
                newContext.SetPath(path);

                if (newContext.GetName() == "")
                {
                    newContext.SetName(FileHelper::GetFilenameFromPath(path));
                }

                auto mappings = contextData["mappings"];
                std::string errorMessage = "";
                for (std::string possibleBinding : keyBindingsAvailable)
                {
                    auto pressActions = mappings[possibleBinding];

                    if (pressActions != nullptr)
                    {
                        std::string downAction = JsonHelper::CheckJsonString(pressActions, "downAction", newContext.GetName());
                        if (downAction != "")
                        {
                            newContext.AddKeyBinding(possibleBinding, downAction, PressType::PressType_Down);
                        }
                        std::string upAction = JsonHelper::CheckJsonString(pressActions, "upAction", newContext.GetName());
                        if (upAction != "")
                        {
                            newContext.AddKeyBinding(possibleBinding, upAction, PressType::PressType_Up);
                        }
                        std::string holdAction = JsonHelper::CheckJsonString(pressActions, "holdAction", newContext.GetName());
                        if (holdAction != "")
                        {
                            newContext.AddKeyBinding(possibleBinding, holdAction, PressType::PressType_Hold);
                        }
                        std::string doubleAction = JsonHelper::CheckJsonString(pressActions, "doubleAction", newContext.GetName());
                        if (doubleAction != "")
                        {
                            newContext.AddKeyBinding(possibleBinding, doubleAction, PressType::PressType_Double);
                        }
                    }
                }

                mappingContexts.push_back(newContext);
            }
        }

        void InitializeMappingContexts()
        {
            mappingContexts.clear();

            // Get available input keycodes from MappingContext.h
            for (std::pair<long, std::string> inputKeycode : F_MappedMouseCodes)
            {
                keyBindingsAvailable.push_back(inputKeycode.second);
            }
            for (std::pair<long, std::string> inputKeycode : F_MappedKeyboardCodes)
            {
                keyBindingsAvailable.push_back(inputKeycode.second);
            }
            for (std::pair<long, std::string> inputKeycode : F_MappedXInputButtonCodes)
            {
                keyBindingsAvailable.push_back(inputKeycode.second);
            }
            for (std::pair<long, std::string> inputKeycode : F_MappedXInputDPadCodes)
            {
                keyBindingsAvailable.push_back(inputKeycode.second);
            }
            for (std::pair<long, std::string> inputKeycode : F_MappedXInputAnalogCodes)
            {
                keyBindingsAvailable.push_back(inputKeycode.second);
            }


            std::vector<std::string> mappingContextFiles = std::vector<std::string>();
            mappingContextFiles = FileHelper::FindAllFilesWithExtension(Assets::assetManager.GetDir("projectDir"), ".mpc");

            for (std::string path : mappingContextFiles)
            {
                AddMappingContext(path);
            }

            AddMappingContext("../engine/mappingContexts/EngineContext.mpc");
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
            AddMappingContext(filePath);
        }

        void HandleContextEvents(MappingContext& context, SDL_Event event, std::vector<std::string> &firedKeys)
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

                    if (event.jaxis.value > -JOYSTICK_DEAD_ZONE && event.jaxis.value < JOYSTICK_DEAD_ZONE)
                    {
                        event.jaxis.value = 0;
                    }					

                    context.FireEvent(key, event, PressType::PressType_Down);
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
                    Logger::log.Info("{}", key);

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
                    context.SetRemapStartTime(FlatEngine::Time::Time());
                    context.SetWaitingForRemap(true);
                    context.SetActionToRemap(inputAction);
                    context.SetRemapTimeoutTime(timeoutTime);
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