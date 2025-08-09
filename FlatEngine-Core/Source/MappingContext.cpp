#include "MappingContext.h"
#include "FlatEngine.h"

#include <SDL.h>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine 
{
	Vector2 InputMapping::GetMouseMotionWorld() 
	{ 
		return ConvertScreenToWorld(Vector2((float)event.motion.x, (float)event.motion.y));
	}

	Vector2 InputMapping::GetMouseMotionScreen() 
	{
		return Vector2((float)event.motion.x, (float)event.motion.y); 
	}

	MappingContext::MappingContext()
	{
		m_name = "";		
		m_inputsByBinding = std::map<std::string, std::shared_ptr<InputMapping>>();
		m_inputsByAction = std::map<std::string, std::shared_ptr<InputMapping>>();
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
		json jsonData = {
			{ "name", m_name }
		};

		for (std::pair<std::string, std::shared_ptr<InputMapping>> inputMapping : m_inputsByAction)
		{
			std::string keyString = inputMapping.second->keyCode;
			std::string inputAction = inputMapping.first;
			if (inputAction != "")
			{
				jsonData.emplace(keyString, inputAction);
			}
		}

		std::string data = jsonData.dump();		
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

	void MappingContext::AddKeyBinding(std::string keyBinding, std::string actionName)
	{
		if (actionName != "")
		{
			std::shared_ptr<InputMapping> inputMap = std::make_shared<InputMapping>();
			inputMap->keyCode = keyBinding;
			inputMap->actionName = actionName;

			std::pair<std::string, std::shared_ptr<InputMapping>> bindingPair = { keyBinding, inputMap };
			std::pair<std::string, std::shared_ptr<InputMapping>> actionPair = { actionName, inputMap };

			// Add by binding
			if (m_inputsByBinding.count(keyBinding) > 0)
			{
				std::string oldActionName = m_inputsByBinding.at(keyBinding)->actionName;
				m_inputsByBinding.at(keyBinding) = inputMap;

				// erase old inputAction pair from m_inputsByAction if the action name has changed
				if (oldActionName != actionName)
				{
					m_inputsByAction.erase(oldActionName);
				}
			}
			else
			{
				m_inputsByBinding.emplace(bindingPair);
			}
			// Add by action
			if (m_inputsByAction.count(actionName) > 0)
			{
				m_inputsByAction.at(actionName) = inputMap;
			}
			else
			{
				m_inputsByAction.emplace(actionPair);
			}
		}
		else if (m_inputsByBinding.count(keyBinding) > 0)
		{
			std::string oldActionName = m_inputsByBinding.at(keyBinding)->actionName;
			m_inputsByBinding.erase(keyBinding);
			m_inputsByAction.erase(oldActionName);
		}
	}

	bool MappingContext::FireEvent(std::string keyBinding, SDL_Event event)
	{
		std::string actionName = "";

		if (m_b_waitingForRemap && !RemapTimedOut(GetEngineTime()))
		{
			AddKeyBinding(keyBinding, m_actionToRemap);
			m_b_waitingForRemap = false;
			SaveMappingContext(m_path, *this);
		}
		else if (RemapTimedOut(GetEngineTime()))
		{
			m_b_waitingForRemap = false;
		}

		if (m_inputsByBinding.count(keyBinding) > 0)
		{						
			m_inputsByBinding.at(keyBinding)->event = event;
			m_inputsByBinding.at(keyBinding)->b_fired = true;
			return true;
		}

		return false;
	}

	void MappingContext::UnFireEvent(std::string keyBinding)
	{
		std::string actionName = "null";

		if (m_inputsByBinding.count(keyBinding) > 0)
		{			
			m_inputsByBinding.at(keyBinding)->b_fired = false;
		}
	}

	bool MappingContext::Fired(std::string actionName)
	{
		if (m_inputsByAction.count(actionName))
		{
			return m_inputsByAction.at(actionName)->b_fired;
		}
		else
		{
			return false;
		}
	}

	void MappingContext::ClearInputActionEvent(std::string keyBinding)
	{
		if (m_inputsByBinding.count(keyBinding) > 0)
		{
			m_inputsByBinding.at(keyBinding)->event = SDL_Event();
		}
	}

	// Get more detailed event information
	SDL_Event MappingContext::GetInputAction(std::string actionName)
	{
		SDL_Event inputEvent = SDL_Event();

		if (m_inputsByAction.count(actionName) > 0)
		{
			inputEvent = m_inputsByAction.at(actionName)->event;
		}

		return inputEvent;
	}

	bool MappingContext::ActionPressed(std::string actionName)
	{
		return GetInputAction(actionName).type != 0;
	}

	std::map<std::string, std::shared_ptr<InputMapping>> MappingContext::GetInputActions()
	{
		return m_inputsByAction;
	}

	std::vector<std::shared_ptr<InputMapping>> MappingContext::GetInputMappingsLua()
	{
		std::vector<std::shared_ptr<InputMapping>> inputActions = std::vector<std::shared_ptr<InputMapping>>();

		for (std::pair<std::string, std::shared_ptr<InputMapping>> inputAction : m_inputsByAction)
		{
			InputMapping newMapping;
			inputActions.push_back(inputAction.second);
		}

		return inputActions;
	}

	std::shared_ptr<InputMapping> MappingContext::GetInputMapping(std::string actionName)
	{
		if (m_inputsByAction.count(actionName) > 0)
		{
			return m_inputsByAction.at(actionName);
		}

		else return nullptr;
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
}