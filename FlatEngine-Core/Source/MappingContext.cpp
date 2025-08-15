#include "MappingContext.h"
#include "FlatEngine.h"

#include <SDL.h>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine 
{
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
						case PT_Down:
						{							
							inputMap->pressActions.downAction.actionName = "";
							break;
						}
						case PT_Up:
						{							
							inputMap->pressActions.upAction.actionName = "";
							break;
						}
						case PT_Hold:
						{							
							inputMap->pressActions.holdAction.actionName = "";
							break;
						}
						case PT_Double:
						{							
							inputMap->pressActions.doubleAction.actionName = "";
							break;
						}
						}
						LogString("No duplicate action names allowed in the same Mapping Context. Previous keybinding with that action name removed.");
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
			case PT_Down:
			{
				oldActionName = inputMap->pressActions.downAction.actionName;
				inputMap->pressActions.downAction.actionName = actionName;
				inputMap->pressActions.downAction.keyCode = keyBinding;
				break;
			}
			case PT_Up:
			{
				oldActionName = inputMap->pressActions.upAction.actionName;
				inputMap->pressActions.upAction.actionName = actionName;
				inputMap->pressActions.upAction.keyCode = keyBinding;
				break;
			}
			case PT_Hold:
			{
				oldActionName = inputMap->pressActions.holdAction.actionName;
				inputMap->pressActions.holdAction.actionName = actionName;
				inputMap->pressActions.holdAction.keyCode = keyBinding;
				break;
			}
			case PT_Double:
			{
				oldActionName = inputMap->pressActions.doubleAction.actionName;
				inputMap->pressActions.doubleAction.actionName = actionName;
				inputMap->pressActions.doubleAction.keyCode = keyBinding;
				break;
			}
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
			case PT_Down:
			{
				oldActionName = inputMap->pressActions.downAction.actionName;
				inputMap->pressActions.downAction.actionName = "";
				inputMap->pressActions.downAction.keyCode = "";
				break;
			}
			case PT_Up:
			{
				oldActionName = inputMap->pressActions.upAction.actionName;
				inputMap->pressActions.upAction.actionName = "";
				inputMap->pressActions.upAction.keyCode = "";
				break;
			}
			case PT_Hold:
			{
				oldActionName = inputMap->pressActions.holdAction.actionName;
				inputMap->pressActions.holdAction.actionName = "";
				inputMap->pressActions.upAction.keyCode = "";
				break;
			}
			case PT_Double:
			{
				oldActionName = inputMap->pressActions.doubleAction.actionName;
				inputMap->pressActions.doubleAction.actionName = "";
				inputMap->pressActions.doubleAction.keyCode = "";
				break;
			}
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

		if (m_b_waitingForRemap && !RemapTimedOut(GetEngineTime()))
		{			
			m_b_waitingForRemap = false;
			SaveMappingContext(m_path, *this);
		}
		else if (RemapTimedOut(GetEngineTime()))
		{
			m_b_waitingForRemap = false;
		}

		if (m_inputsByBinding.count(keyBinding) > 0)
		{
			std::shared_ptr<InputMapping> inputMap = m_inputsByBinding.at(keyBinding);
			switch (pressType)
			{
			case PT_Down:
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
			case PT_Up:
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
}