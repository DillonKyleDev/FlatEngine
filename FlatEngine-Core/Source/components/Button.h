#pragma once
#include "components/Canvas.h"
#include "components/Component.h"
#include "managers/LuaManager.h"
#include "physics/Shape2D.h"
#include "structs/SceneRenderObject.h"
#include "tools/JsonHelper.h"

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

		bool CheckForMouseOver(Vector2 mousePos);
		int GetLayer();
		void SetMouseIsOver(bool b_isOver);
		void SetIsOverFired(bool b_fired);
		bool MouseIsOver();
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

		CanvasPlacement* GetCanvasPlacement();
		void UpdateButtonTransform();
		void UpdateRenderShapes();
		std::vector<SceneRenderObject>& GetRenderShapes();

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
		std::string functionName;
		ShapeType2D shapeType;		
		BoxShape2DData boxShapeData;
		CircleShape2DData circleShapeData;
		CapsuleShape2DData capsuleShapeData;
		PolygonShape2DData polygonShapeData;
		std::vector<SceneRenderObject> boxRenderShapes;
		std::vector<SceneRenderObject> circleRenderShapes;
		std::vector<SceneRenderObject> capsuleRenderShapes;
		std::vector<SceneRenderObject> polygonRenderShapes;

	private:
		CanvasPlacement m_canvasPlacement;		
		bool m_b_mouseIsOver;
		bool m_b_hasMouseOverFired;		
		bool m_b_luaFunction;
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