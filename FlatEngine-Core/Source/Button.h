#pragma once
#include "Component.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Animation.h"

#include <memory>
#include <string>

namespace FlatEngine
{
	class GameObject;

	class Button : public Component
	{
	public:
		Button(long myID = -1, long parentID = -1);
		~Button();
		std::string GetData();

		void SetActiveDimensions(float width, float height);
		void SetActiveOffset(Vector2 offset);
		void SetActiveLayer(int layer);
		int GetActiveLayer();
		float GetActiveWidth();
		float GetActiveHeight();
		Vector2 GetActiveOffset();
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
		void SetFunctionParams(std::shared_ptr<Animation::S_Event> params);
		void SetFunctionParamsLua(Animation::S_Event params);
		std::shared_ptr<Animation::S_Event> GetFunctionParams();
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

	private:
		bool m_b_mouseIsOver;
		bool m_b_hasMouseOverFired;
		float m_activeWidth;
		float m_activeHeight;
		Vector4 m_activeEdges;
		Vector2 m_activeOffset;
		int m_activeLayer;
		std::shared_ptr<Animation::S_Event> m_functionParams;
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