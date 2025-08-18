#include "Button.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animation.h"


namespace FlatEngine
{
	Button::Button(long myID, long parentID)
	{
		SetType(T_Button);
		SetID(myID);
		SetParentID(parentID);
		m_b_mouseIsOver = false;
		m_b_hasMouseOverFired = false;
		m_activeWidth = 5;
		m_activeHeight = 3;
		m_activeOffset = Vector2(0, 0);
		m_activeLayer = 0;
		m_functionParams = std::make_shared<Animation::S_Event>();
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

	Button::~Button()
	{
	}

	std::string Button::GetData()
	{
		json jsonData = {
			{ "type", "Button" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "activeWidth", m_activeWidth },
			{ "activeHeight", m_activeHeight },
			{ "activeOffsetX", m_activeOffset.x },
			{ "activeOffsetY", m_activeOffset.y },
			{ "activeLayer", m_activeLayer },
			{ "functionName", m_functionParams->functionName },
			{ "_cppEvent", m_functionParams->b_cppEvent },
			{ "_luaEvent", m_functionParams->b_luaEvent },
			{ "_leftClick", m_b_leftClick },
			{ "_rightClick", m_b_rightClick },
		};

		json parameters = {			
			{ "string", m_functionParams->parameters.e_string },
			{ "int", m_functionParams->parameters.e_int },
			{ "float", m_functionParams->parameters.e_float },
			{ "double", m_functionParams->parameters.e_double },
			{ "long", m_functionParams->parameters.e_long },
			{ "bool", m_functionParams->parameters.e_boolean },
			{ "vector2X", m_functionParams->parameters.e_Vector2.x },
			{ "vector2Y", m_functionParams->parameters.e_Vector2.y },
		};
		
		jsonData.push_back({ "functionParameters", parameters });

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
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
			LogError("The active width and height must be larger than 0.");
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
			LogError("Button active layer must be an larger than 0.");
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
		Transform* transform = GetParent()->GetTransform();
		
		if (transform != nullptr)
		{
			Vector2 position = transform->GetAbsolutePosition();
			Vector2 scale = transform->GetAbsoluteScale();

			float activeLeft = F_gameViewCenter.x + ((position.x - (m_activeWidth / 2 * scale.x) + m_activeOffset.x * scale.x) * F_gameViewGridStep.x);
			float activeRight = F_gameViewCenter.x + ((position.x + (m_activeWidth / 2 * scale.x) + m_activeOffset.x * scale.x) * F_gameViewGridStep.x);
			float activeTop = F_gameViewCenter.y - ((position.y + (m_activeHeight / 2 * scale.y) + m_activeOffset.y * scale.y) * F_gameViewGridStep.x);
			float activeBottom = F_gameViewCenter.y - ((position.y - (m_activeHeight / 2 * scale.y) + m_activeOffset.y * scale.y) * F_gameViewGridStep.x);

			m_activeEdges = Vector4(activeTop, activeRight, activeBottom, activeLeft);
		}
	}

	Vector4 Button::GetActiveEdges()
	{
		return m_activeEdges;
	}

	void Button::SetFunctionName(std::string functionName)
	{
		m_functionParams->functionName = functionName;
	}

	std::string Button::GetFunctionName()
	{
		return m_functionParams->functionName;
	}

	void Button::SetIsCPP(bool b_isCPP)
	{
		m_functionParams->b_cppEvent = b_isCPP;
	}

	void Button::SetIsLua(bool b_isLua)
	{
		m_functionParams->b_luaEvent = b_isLua;
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

	void Button::SetFunctionParams(std::shared_ptr<Animation::S_Event> params)
	{
		m_functionParams = params;
	}

	void Button::SetFunctionParamsLua(Animation::S_Event params)
	{		
		m_functionParams->name = params.name;
		m_functionParams->functionName = params.functionName;
		m_functionParams->b_cppEvent = params.b_cppEvent;
		m_functionParams->b_luaEvent = params.b_luaEvent;
		m_functionParams->time = params.time;
		m_functionParams->b_fired = params.b_fired;
		m_functionParams->parameters = params.parameters;
	}

	std::shared_ptr<Animation::S_Event> Button::GetFunctionParams()
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
		m_onMouseEnterCallback(GetParent());
	}

	void Button::OnMouseLeave()
	{
		m_onMouseLeaveCallback(GetParent());
	}

	void Button::OnMouseOver()
	{
		m_onMouseOverCallback(GetParent());
	}

	void Button::OnLeftClick()
	{
		m_onLeftClickCallback(GetParent());
	}

	void Button::OnRightClick()
	{
		m_onRightClickCallback(GetParent());
	}

}
