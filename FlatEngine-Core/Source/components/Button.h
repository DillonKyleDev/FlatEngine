#pragma once
#include "components/Component.h"
#include "managers/LuaManager.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"
#include "tools/Vector4.h"

#include <string>


namespace FlatEngine
{
	class GameObject;

	class Button : public Component
	{
	public:
		Button(long ownerID = -1);
		json GetData(bool b_IDOverride = false);
		void PutData(json componentJson, std::string objectName);

		void SetOffset(Vector2 offset);	
		Vector2 GetOffset();	
		int GetLayer();
		Vector2 GetDimensions();
		void SetDimensions(Vector2 dimensions);
		
		void SetMouseIsOver(bool b_isOver);
		void SetIsOverFired(bool b_fired);
		bool MouseIsOver();
		//ImVec4(activeTop, activeRight, activeBottom, activeLeft)
		void CalculateActiveEdges();
		Vector4 GetActiveEdges();
		void SetFunctionName(std::string functionName);
		std::string GetFunctionName();
		void SetIsCPP(bool b_isCPP);
		void SetIsLua(bool b_isLua);
		void SetFunctionParams(LuaManager::LuaParameter params);
		void SetFunctionParamsLua(LuaManager::LuaParameter params);
		LuaManager::LuaParameter GetFunctionParams();
		void SetLeftClick(bool b_leftClick);
		bool GetLeftClick();
		void SetRightClick(bool b_rightClick);
		bool GetRightClick();

		void SetOnMouseEnterCallback(void (*callback)(GameObject* caller));
		void SetOnMouseLeaveCallback(void (*callback)(GameObject* caller));
		void SetOnMouseOverCallback(void (*callback)(GameObject* caller));
		void SetOnLeftClickCallback(void (*callback)(GameObject* caller));
		void SetOnRightClickCallback(void (*callback)(GameObject* caller));

		bool MouseEnterSet();
		bool MouseLeaveSet();
		bool MouseOverSet();
		bool LeftClickSet();
		bool RightClickSet();

		void OnMouseEnter();
		void OnMouseLeave();
		void OnMouseOver();
		void OnLeftClick();
		void OnRightClick();

		LuaManager::LuaParameterContainer parameterContainer;

	private:
		bool m_b_mouseIsOver;
		bool m_b_hasMouseOverFired;
		Vector2 m_dimensions;
		Vector4 m_activeEdges;
		Vector2 m_offset;		
		std::string m_functionName;
		LuaManager::LuaParameter m_functionParams;	
		bool m_b_luaFunction;
		bool m_b_cppFunction;
		bool m_b_leftClick;
		bool m_b_rightClick;
		// for C++ scripting
		void (*m_onMouseEnterCallback)(GameObject*);
		bool m_b_onMouseEnterCallbackSet;
		void (*m_onMouseLeaveCallback)(GameObject*);
		bool m_b_onMouseLeaveCallbackSet;
		void (*m_onMouseOverCallback)(GameObject*);
		bool m_b_onMouseOverCallbackSet;
		void (*m_onLeftClickCallback)(GameObject*);
		bool m_b_onLeftClickCallbackSet;
		void (*m_onRightClickCallback)(GameObject*);
		bool m_b_onRightClickCallbackSet;
	};
}