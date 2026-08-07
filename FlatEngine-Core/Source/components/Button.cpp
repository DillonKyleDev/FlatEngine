#include "components/Button.h"
#include "components/Transform.h"
#include "GameObject.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"
#include "tools/Vector2.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"


namespace FlatEngine
{
	Button::Button(long ownerID)
	{
		SetType(ComponentType_Button);
		SetOwnerID(ownerID);
		m_b_mouseIsOver = false;
		m_b_hasMouseOverFired = false;
		m_activeWidth = 5;
		m_activeHeight = 3;
		m_activeOffset = Vector2(0, 0);
		m_activeLayer = 0;
		m_functionParams = LuaManager::LuaParameter();
		m_functionName = "";
		m_b_luaFunction = true;
		m_b_cppFunction = false;
		m_b_leftClick = true;
		m_b_rightClick = false;

		m_onMouseEnterCallback = nullptr;
		m_b_onMouseEnterCallbackSet = false;
		m_onMouseLeaveCallback = nullptr;
		m_b_onMouseLeaveCallbackSet = false;
		m_onMouseOverCallback = nullptr;
		m_b_onMouseOverCallbackSet = false;
		m_onLeftClickCallback = nullptr;
		m_b_onLeftClickCallbackSet = false;
		m_onRightClickCallback = nullptr;
		m_b_onRightClickCallbackSet = false;
	}

	json Button::GetData(bool b_IDOverride)
	{
		json jsonData = {
			{ "type", (int)GetType() },
			{ "b_isCollapsed", IsCollapsed() },
			{ "b_isActive", IsActive() },
			{ "activeWidth", m_activeWidth },
			{ "activeHeight", m_activeHeight },
			{ "activeOffsetX", m_activeOffset.x },
			{ "activeOffsetY", m_activeOffset.y },
			{ "activeLayer", m_activeLayer },
			{ "functionName", m_functionName },
			{ "b_luaFunction", m_b_luaFunction },
			{ "b_cppFunction", m_b_cppFunction },
			{ "b_leftClick", m_b_leftClick },
			{ "b_rightClick", m_b_rightClick },
		};

		json parameters = m_functionParams.GetData();
		
		jsonData.push_back({ "functionParameters", parameters });

		return jsonData;
	}

	void Button::PutData(json componentJson, std::string objectName)
	{
        Component::PutData(componentJson, objectName);	

		LuaManager::LuaParameter parameter;
		if (JsonHelper::JsonContains(componentJson, "functionParameters", objectName))
			parameter.PutData(componentJson.at("functionParameters"), objectName);
		SetFunctionParams(parameter);

		SetActiveDimensions(JsonHelper::CheckJsonFloat(componentJson, "activeWidth", objectName), JsonHelper::CheckJsonFloat(componentJson, "activeHeight", objectName));
		SetActiveOffset(Vector2(JsonHelper::CheckJsonFloat(componentJson, "activeOffsetX", objectName), JsonHelper::CheckJsonFloat(componentJson, "activeOffsetY", objectName)));
		SetActiveLayer(JsonHelper::CheckJsonInt(componentJson, "activeLayer", objectName));
		SetFunctionName(JsonHelper::CheckJsonString(componentJson, "functionName", objectName));
		SetIsCPP(JsonHelper::CheckJsonBool(componentJson, "b_cppEvent", objectName));
		SetIsLua(JsonHelper::CheckJsonBool(componentJson, "b_luaEvent", objectName));								
		SetLeftClick(JsonHelper::CheckJsonBool(componentJson, "b_leftClick", objectName));
		SetRightClick(JsonHelper::CheckJsonBool(componentJson, "b_rightClick", objectName));	
    }

	void Button::SetActiveDimensions(float width, float height)
	{
		if (width >= 0 && height >= 0)
		{
			m_activeWidth = width;
			m_activeHeight = height;
		}
		else
		{
			Logger::log.Err("The active width and height must be larger than 0.");
		}
	}

	void Button::SetActiveOffset(Vector2 offset)
	{
		m_activeOffset = offset;
	}

	void Button::SetActiveLayer(int layer)
	{
		if (layer >= 0)
		{
			m_activeLayer = layer;
		}
		else
		{
			Logger::log.Err("Button active layer must be an larger than 0.");
		}
	}

	int Button::GetActiveLayer()
	{
		return m_activeLayer;
	}

	float Button::GetActiveWidth()
	{
		return m_activeWidth;
	}

	float Button::GetActiveHeight()
	{
		return m_activeHeight;
	}

	Vector2 Button::GetActiveOffset()
	{
		return m_activeOffset;
	}

	void Button::SetMouseIsOver(bool b_isOver)
	{
		m_b_mouseIsOver = b_isOver;
	}

	void Button::SetIsOverFired(bool b_fired)
	{
		m_b_hasMouseOverFired = b_fired;
	}

	bool Button::MouseIsOver()
	{
		return m_b_mouseIsOver;
	}

	void Button::CalculateActiveEdges()
	{
		Transform* transform = GetOwningObject()->Get<Transform>();
		
		if (transform != nullptr)
		{
			Vector3 position = transform->GetAbsolutePosition();
			Vector3 scale = transform->GetAbsoluteScale();
			
			// FIX LATER
			// float activeLeft = F_gameViewCenter.x + ((position.x - (m_activeWidth / 2 * scale.x) + m_activeOffset.x * scale.x) * F_gameViewGridStep.x);
			// float activeRight = F_gameViewCenter.x + ((position.x + (m_activeWidth / 2 * scale.x) + m_activeOffset.x * scale.x) * F_gameViewGridStep.x);
			// float activeTop = F_gameViewCenter.y - ((position.y + (m_activeHeight / 2 * scale.y) + m_activeOffset.y * scale.y) * F_gameViewGridStep.x);
			// float activeBottom = F_gameViewCenter.y - ((position.y - (m_activeHeight / 2 * scale.y) + m_activeOffset.y * scale.y) * F_gameViewGridStep.x);

			// m_activeEdges = Vector4(activeTop, activeRight, activeBottom, activeLeft);
		}
	}

	Vector4 Button::GetActiveEdges()
	{
		return m_activeEdges;
	}

	void Button::SetFunctionName(std::string functionName)
	{
		m_functionParams.name = functionName;
	}

	std::string Button::GetFunctionName()
	{
		return m_functionParams.name;
	}

	void Button::SetIsCPP(bool b_isCPP)
	{
		// m_functionParams.b_cppEvent = b_isCPP;
	}

	void Button::SetIsLua(bool b_isLua)
	{
		// m_functionParams.b_luaEvent = b_isLua;
	}

	void Button::SetLeftClick(bool b_leftClick)
	{
		m_b_leftClick = b_leftClick;
	}

	bool Button::GetLeftClick()
	{
		return m_b_leftClick;
	}

	void Button::SetRightClick(bool b_rightClick)
	{
		m_b_rightClick = b_rightClick;
	}

	bool Button::GetRightClick()
	{
		return m_b_rightClick;
	}

	void Button::SetFunctionParams(LuaManager::LuaParameter params)
	{
		m_functionParams = params;
	}

	void Button::SetFunctionParamsLua(LuaManager::LuaParameter params)
	{		
		m_functionParams = params;
	}

	LuaManager::LuaParameter Button::GetFunctionParams()
	{
		return m_functionParams;
	}

	void Button::SetOnMouseEnterCallback(void (*callback)(GameObject* caller))
	{
		m_onMouseEnterCallback = callback;
		m_b_onMouseEnterCallbackSet = true;
	}

	void Button::SetOnMouseLeaveCallback(void (*callback)(GameObject* caller))
	{
		m_onMouseLeaveCallback = callback;
		m_b_onMouseLeaveCallbackSet = true;
	}

	void Button::SetOnMouseOverCallback(void (*callback)(GameObject* caller))
	{
		m_onMouseOverCallback = callback;
		m_b_onMouseOverCallbackSet = true;
	}

	void Button::SetOnLeftClickCallback(void (*callback)(GameObject* caller))
	{
		m_onLeftClickCallback = callback;
		m_b_onLeftClickCallbackSet = true;
	}

	void Button::SetOnRightClickCallback(void (*callback)(GameObject* caller))
	{
		m_onRightClickCallback = callback;
		m_b_onRightClickCallbackSet = true;
	}

	bool Button::MouseEnterSet()
	{
		return m_b_onMouseEnterCallbackSet;
	}

	bool Button::MouseLeaveSet()
	{
		return m_b_onMouseLeaveCallbackSet;
	}

	bool Button::MouseOverSet()
	{
		return m_b_onMouseOverCallbackSet;
	}

	bool Button::LeftClickSet()
	{
		return m_b_onLeftClickCallbackSet;
	}

	bool Button::RightClickSet()
	{
		return m_b_onRightClickCallbackSet;
	}

	void Button::OnMouseEnter()
	{
		m_onMouseEnterCallback(GetOwningObject());
	}

	void Button::OnMouseLeave()
	{
		m_onMouseLeaveCallback(GetOwningObject());
	}

	void Button::OnMouseOver()
	{
		m_onMouseOverCallback(GetOwningObject());
	}

	void Button::OnLeftClick()
	{
		m_onLeftClickCallback(GetOwningObject());
	}

	void Button::OnRightClick()
	{
		m_onRightClickCallback(GetOwningObject());
	}
}
