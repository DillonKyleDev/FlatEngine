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
		m_luaFunctionName = "";
		m_luaFunctionParams = std::make_shared<Animation::S_Event>();
		m_b_leftClick = true;
		m_b_rightClick = false;
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
			{ "luaFunctionName", m_luaFunctionName },
			{ "_leftClick", m_b_leftClick },
			{ "_rightClick", m_b_rightClick },
		};

		json parameters = {			
			{ "string", m_luaFunctionParams->parameters.e_string },
			{ "int", m_luaFunctionParams->parameters.e_int },
			{ "float", m_luaFunctionParams->parameters.e_float },
			{ "double", m_luaFunctionParams->parameters.e_double },
			{ "long", m_luaFunctionParams->parameters.e_long },
			{ "bool", m_luaFunctionParams->parameters.e_boolean },
			{ "vector2X", m_luaFunctionParams->parameters.e_Vector2.x },
			{ "vector2Y", m_luaFunctionParams->parameters.e_Vector2.y },
		};
		
		jsonData.push_back({ "luaFunctionParameters", parameters });

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
			Vector2 position = transform->GetTruePosition();
			Vector2 scale = transform->GetScale();

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

	void Button::SetLuaFunctionName(std::string functionName)
	{
		m_luaFunctionName = functionName;
	}

	std::string Button::GetLuaFunctionName()
	{
		return m_luaFunctionName;
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

	void Button::SetLuaFunctionParams(std::shared_ptr<Animation::S_Event> params)
	{
		m_luaFunctionParams = params;
	}

	void Button::SetLuaFunctionParamsLua(Animation::S_Event params)
	{		
		m_luaFunctionParams->name = params.name;
		m_luaFunctionParams->functionName = params.functionName;
		m_luaFunctionParams->time = params.time;
		m_luaFunctionParams->b_fired = params.b_fired;
		m_luaFunctionParams->parameters = params.parameters;
	}

	std::shared_ptr<Animation::S_Event> Button::GetLuaFunctionParams()
	{
		return m_luaFunctionParams;
	}
}
