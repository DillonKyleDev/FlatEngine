#include "components/Button.h"
#include "components/Transform.h"
#include "GameObject.h"
#include "GuiCore.h"
#include "managers/Assets.h"
#include "managers/SceneManager.h"
#include "physics/Shape2D.h"
#include "render/SceneView.h"
#include "structs/SceneRenderObject.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"
#include "tools/Vector3.h"
#include <types.h>


namespace FlatEngine
{
	Button::Button(long ownerID)
	{
		SetType(ComponentType_Button);
		SetOwnerID(ownerID);

		functionName = "";
		shapeType = ShapeType2D_Box;	
		m_b_mouseIsOver = false;
		m_b_hasMouseOverFired = false;
		functionName = "";
		m_b_luaFunction = true;
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
		json componentJson = {
			{ "shapeType",        ShapeType2DStrings[(int)shapeType] },
			{ "functionName",     functionName },
			{ "b_luaFunction",    m_b_luaFunction },
			{ "b_leftClick",      m_b_leftClick },
			{ "b_rightClick",     m_b_rightClick },
			{ "canvasPlacement",  m_canvasPlacement.GetData() },
			{ "boxShapeData",     boxShapeData.GetData() },
			{ "circleShapeData",  circleShapeData.GetData() },
			{ "capsuleShapeData", capsuleShapeData.GetData() },
			{ "polygonShapeData", polygonShapeData.GetData() },
		};
		componentJson.update(Component::GetData(b_IDOverride));

		json parameters = parameterContainer.GetData();
		
		componentJson.push_back({ "functionParameters", parameters });

		return componentJson;
	}

	void Button::PutData(json componentJson, std::string objectName)
	{
		if (componentJson == json::object())		
			return;	
		
        Component::PutData(componentJson, objectName);	

		shapeType = GetTypeFromString<ShapeType2D>(ShapeType2DFromString, JsonHelper::CheckJsonString(componentJson, "shapeType", objectName));
		boxShapeData.PutData(JsonHelper::JsonContains(componentJson, "boxShapeData", objectName) ? componentJson.at("boxShapeData") : json::object(), objectName);
		circleShapeData.PutData(JsonHelper::JsonContains(componentJson, "circleShapeData", objectName) ? componentJson.at("circleShapeData") : json::object(), objectName);
		capsuleShapeData.PutData(JsonHelper::JsonContains(componentJson, "capsuleShapeData", objectName) ? componentJson.at("capsuleShapeData") : json::object(), objectName);
		polygonShapeData.PutData(JsonHelper::JsonContains(componentJson, "polygonShapeData", objectName) ? componentJson.at("polygonShapeData") : json::object(), objectName);
		boxRenderShapes.push_back(CreateQuadObject()); 
		circleRenderShapes.push_back(CreateCircleObject()); 
		capsuleRenderShapes = CreateCapsuleObject(); 
		polygonRenderShapes = CreatePolygonObject();

		json canvasPlacementJson = json::object();
		if (JsonHelper::JsonContains(componentJson, "canvasPlacement", objectName))		
			canvasPlacementJson = componentJson.at("canvasPlacement");
		m_canvasPlacement.PutData(canvasPlacementJson, objectName);		

		json parametersJson = json::object();
		if (JsonHelper::JsonContains(componentJson, "functionParameters", objectName))		
			parametersJson = componentJson.at("functionParameters");
		parameterContainer.PutData(parametersJson, objectName);

		functionName = JsonHelper::CheckJsonString(componentJson, "functionName", objectName);
		SetIsCPP(JsonHelper::CheckJsonBool(componentJson, "b_luaFunction", objectName));						
		SetLeftClick(JsonHelper::CheckJsonBool(componentJson, "b_leftClick", objectName));
		SetRightClick(JsonHelper::CheckJsonBool(componentJson, "b_rightClick", objectName));	

		UpdateButtonTransform();
    }

	bool Button::CheckForMouseOver(Vector2 mousePos)
	{
		bool b_mouseOver = false;
		GameObject* owner = SceneManager::loadedScene.GetObjectByID(GetOwnerID());
		Canvas* canvas = owner->GetFirstCanvas();
		Transform* ownerTransform = owner->Get<Transform>();
		Vector3 ownerPos = ownerTransform->GetPosition();		
		Vector3 ownerScale = ownerTransform->GetAbsoluteScale();
		Vector2 worldPos = canvas->GetMousePosOnCanvas(mousePos);
		b2Vec2 worldPoint = { worldPos.x, worldPos.y };
		Vector2 pivotOffset = m_canvasPlacement.pivot->offset;
		pivotOffset = Vector2(pivotOffset.x / GuiCore::texturePixelsPerGridSpace, pivotOffset.y / GuiCore::texturePixelsPerGridSpace * -1.0f);	
		ownerPos = ownerPos + Vector3(pivotOffset, 0);

		switch (shapeType)
		{
			case ShapeType2D_Box:
			{	
				Vector2 offset = m_canvasPlacement.pivot->offset;
				b2Polygon polygon = b2MakeBox(boxShapeData.dimensions.x * 0.5f, boxShapeData.dimensions.y * 0.5f);												
				b2Transform b2transform;				
				b2transform.p = { ownerPos.x, ownerPos.y };
				b2transform.q = b2MakeRot(Numbers::DegreesToRadians(ownerTransform->GetRotation().z));
				
				b2Vec2 localPoint = b2InvTransformPoint(b2transform, worldPoint);				
				b_mouseOver = b2PointInPolygon(&polygon, localPoint);
				break;
			}
			case ShapeType2D_Circle:
			{
				b2Circle circle;
				circle.center = { 0.0f, 0.0f };
				circle.radius = circleShapeData.radius;

				b2Transform ownerXf;
				ownerXf.p = { ownerPos.x, ownerPos.y };
				ownerXf.q = b2MakeRot(Numbers::DegreesToRadians(ownerTransform->GetRotation().z));

				b2Transform localXf;
				localXf.p = { circleShapeData.offset.x, circleShapeData.offset.y };
				localXf.q = circleShapeData.rotationOffset;

				b2Transform xf = b2MulTransforms(ownerXf, localXf);

				b2Vec2 localPoint = b2InvTransformPoint(xf, worldPoint);
				b_mouseOver = b2PointInCircle(&circle, localPoint);
				break;
			}
			case ShapeType2D_Capsule:
			{
				b2Capsule capsule;
				float halfLen = (capsuleShapeData.length - (capsuleShapeData.radius * 2.0f)) * 0.5f;
				if (capsuleShapeData.b_horizontal) {
					capsule.center1 = { -halfLen, 0.0f };
					capsule.center2 = {  halfLen, 0.0f };
				} else {
					capsule.center1 = { 0.0f, -halfLen };
					capsule.center2 = { 0.0f,  halfLen };
				}
				capsule.radius = capsuleShapeData.radius;

				b2Transform ownerXf;
				ownerXf.p = { ownerPos.x, ownerPos.y };
				ownerXf.q = b2MakeRot(Numbers::DegreesToRadians(ownerTransform->GetRotation().z));

				b2Transform localXf;
				localXf.p = { capsuleShapeData.offset.x, capsuleShapeData.offset.y };
				localXf.q = capsuleShapeData.rotationOffset;

				b2Transform xf = b2MulTransforms(ownerXf, localXf);

				b2Vec2 localPoint = b2InvTransformPoint(xf, worldPoint);
				b_mouseOver = b2PointInCapsule(&capsule, localPoint);
				break;
			}
			case ShapeType2D_Polygon:
			{
				std::vector<b2Vec2> b2Points;
				b2Points.reserve(polygonShapeData.points.size());
				for (auto& p : polygonShapeData.points) b2Points.push_back({ p.x, p.y });

				b2Hull hull = b2ComputeHull(b2Points.data(), (int)b2Points.size());
				b2Polygon polygon = b2MakePolygon(&hull, polygonShapeData.cornerRadius);

				b2Transform xf;
				xf.p = { ownerPos.x, ownerPos.y };
				xf.q = b2MakeRot(Numbers::DegreesToRadians(ownerTransform->GetRotation().z));

				b2Vec2 localPoint = b2InvTransformPoint(xf, worldPoint);
				b_mouseOver = b2PointInPolygon(&polygon, localPoint);
				break;
			}
			default: break;
		}

		return b_mouseOver;
	}

	int Button::GetLayer()
	{
		// TODO
		return 0;
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

	CanvasPlacement* Button::GetCanvasPlacement()
	{
		return &m_canvasPlacement;
	}

	void Button::UpdateButtonTransform()
	{
		Transform* ownerTransform = SceneManager::loadedScene.Get<Transform>(GetOwnerID());
		Canvas* canvas = GetOwningObject()->GetFirstCanvas();					
		ownerTransform->SetPosition(canvas->GetCanvasPlacementPosition(&m_canvasPlacement, SceneView::finalImageSize));		
		
		UpdateRenderShapes();
	}

	void Button::UpdateRenderShapes()
	{	
		Transform* ownerTransform = SceneManager::loadedScene.Get<Transform>(GetOwnerID());		
		Canvas* canvas = GetOwningObject()->GetFirstCanvas();

		if (ownerTransform == nullptr || canvas == nullptr)
			return;
						
		Vector3 ownerRot = ownerTransform->GetAbsoluteRotation();	
		Vector3 canvasPos = canvas->GetOwningObject()->Get<Transform>()->GetAbsolutePosition();		
		
		Vector2 pivotOffset = m_canvasPlacement.pivot->offset;
		pivotOffset = Vector2(pivotOffset.x / GuiCore::texturePixelsPerGridSpace, pivotOffset.y / GuiCore::texturePixelsPerGridSpace * -1.0f);				
		Vector3 buttonPos = ownerTransform->GetPosition();						
		Vector3 renderOffset = Vector3(pivotOffset, m_canvasPlacement.zPosition);		
		Vector3 renderPosition = canvasPos + buttonPos + renderOffset;
		
		std::string activeString = IsActive() ? "Active" : "Inactive";
		std::string color = "canvasButton" + activeString;

		switch (shapeType)
		{
			case ShapeType2D_Box:
			{					
				if (boxRenderShapes.size() != 1)
				{
					boxRenderShapes.clear();
					boxRenderShapes.push_back(CreateQuadObject());
				}	

				boxRenderShapes[0].transform.SetPosition(renderPosition);
				boxRenderShapes[0].transform.SetScale(Vector3(boxShapeData.dimensions.x, boxShapeData.dimensions.y, 1));	
				boxRenderShapes[0].transform.SetRotation(ownerRot);				
				boxRenderShapes[0].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
				m_canvasPlacement.pivot->dimensions = boxShapeData.dimensions * GuiCore::texturePixelsPerGridSpace;
				m_canvasPlacement.pivot->UpdatePivotOffset();
				break;
			}
			case ShapeType2D_Circle:
			{
				if (circleRenderShapes.size() != 1)
				{
					circleRenderShapes.clear();
					circleRenderShapes.push_back(CreateCircleObject());
				}	

				circleRenderShapes[0].transform.SetPosition(renderPosition);	
				circleRenderShapes[0].transform.SetScale(Vector3(circleShapeData.radius, circleShapeData.radius, 1));					
				circleRenderShapes[0].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
				m_canvasPlacement.pivot->dimensions = Vector2(circleShapeData.radius * GuiCore::texturePixelsPerGridSpace * 2.0f);	
				m_canvasPlacement.pivot->UpdatePivotOffset();
				break;			
			}
			case ShapeType2D_Capsule:
			{
				if (capsuleRenderShapes.size() != 8)											
				{
					capsuleRenderShapes.clear();
					capsuleRenderShapes = CreateCapsuleObject();
				}					
								
				float center1Value = ((capsuleShapeData.length / 2) - capsuleShapeData.radius) * -1;
				float center2Value = (capsuleShapeData.length / 2) - capsuleShapeData.radius;				
				b2Vec2 b2center1 = b2Vec2(capsuleShapeData.b_horizontal ? center1Value : 0, capsuleShapeData.b_horizontal ? 0 : center1Value);
				b2Vec2 b2center2 = b2Vec2(capsuleShapeData.b_horizontal ? center2Value : 0, capsuleShapeData.b_horizontal ? 0 : center2Value);				
				
				b2Transform b2transform;				
				b2transform.p = b2Vec2(0,0);
				b2transform.q = b2MakeRot(Numbers::DegreesToRadians(-ownerRot.z));
				Vector2 center1 = b2Vec2(renderPosition.x, renderPosition.y) + b2InvTransformPoint(b2transform, b2center1);	
				Vector2 center2 = b2Vec2(renderPosition.x, renderPosition.y) + b2InvTransformPoint(b2transform, b2center2);	

				Vector2 difference = center2 - center1;
				Vector2 diffN = Vector2::Normalize(difference);
				Vector2 diffNR = diffN * capsuleShapeData.radius;
				Vector2 diffPerp = Vector2::Rotate(diffNR, 90);
				Vector2 flippedDiffPerp = Vector2::Rotate(diffNR, -90);				

				// circles
				capsuleRenderShapes[0].transform.SetPosition(Vector3(center1, renderPosition.z));
				capsuleRenderShapes[1].transform.SetPosition(Vector3(center2, renderPosition.z));
				capsuleRenderShapes[0].transform.SetScale(Vector3(capsuleShapeData.radius, capsuleShapeData.radius, 1));
				capsuleRenderShapes[1].transform.SetScale(Vector3(capsuleShapeData.radius, capsuleShapeData.radius, 1));				
				// Lines		
				capsuleRenderShapes[2].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center1 - diffNR, renderPosition.z), Vector3(center1 + diffNR, renderPosition.z));				
				capsuleRenderShapes[3].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center2 - diffNR, renderPosition.z), Vector3(center2 + diffNR, renderPosition.z));				
				capsuleRenderShapes[4].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center1 - diffPerp, renderPosition.z), Vector3(center1 + diffPerp, renderPosition.z));				
				capsuleRenderShapes[5].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center2 - diffPerp, renderPosition.z), Vector3(center2 + diffPerp, renderPosition.z));				
				capsuleRenderShapes[6].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center1 + diffPerp, renderPosition.z), Vector3(center1 + diffPerp + difference, renderPosition.z));												
				capsuleRenderShapes[7].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center1 + flippedDiffPerp, renderPosition.z), Vector3(center1 + flippedDiffPerp + difference, renderPosition.z));									
				// Colors
				capsuleRenderShapes[0].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
				capsuleRenderShapes[1].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
				capsuleRenderShapes[2].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
				capsuleRenderShapes[3].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
				capsuleRenderShapes[4].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
				capsuleRenderShapes[5].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
				capsuleRenderShapes[6].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
				capsuleRenderShapes[7].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));	

				float length = capsuleShapeData.length + (capsuleShapeData.radius * 2.0f) * GuiCore::texturePixelsPerGridSpace;
				float width = capsuleShapeData.radius * 2.0f;

				m_canvasPlacement.pivot->dimensions = Vector2(capsuleShapeData.b_horizontal ? length : width, capsuleShapeData.b_horizontal ? width : length);
				m_canvasPlacement.pivot->UpdatePivotOffset();
				break;	
			}
			case ShapeType2D_Polygon:
			{
				if (polygonRenderShapes.size() < polygonShapeData.points.size())
				{
					int diff = polygonShapeData.points.size() - polygonRenderShapes.size();
					for (int i = 0; i < diff; i++)
					{
						polygonRenderShapes.push_back(CreateLineObject());
					}
				}

				for (int p = 0; p < polygonShapeData.points.size(); p++)
				{
					int pNext = p == polygonShapeData.points.size() - 1 ? 0 : p + 1;
					b2Vec2 pointStart = b2Vec2(renderPosition.x + polygonShapeData.points[p].x, renderPosition.y + polygonShapeData.points[p].y);
					Vector3 startPos = Vector3(pointStart, renderPosition.z);
					b2Vec2 pointEnd = b2Vec2(renderPosition.x + polygonShapeData.points[pNext].x, renderPosition.y + polygonShapeData.points[pNext].y);
					Vector3 endPos = Vector3(pointEnd, renderPosition.z);

					polygonRenderShapes[p].transform = SceneView::GetLineTransformForStartEndPos(startPos, endPos);
					polygonRenderShapes[p].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));	
				}
				break;
			}
			default: break;
		}
	}

	std::vector<SceneRenderObject>& Button::GetRenderShapes()
	{		
		switch (shapeType)
		{
			case ShapeType2D_Box:     return boxRenderShapes;
			case ShapeType2D_Circle:  return circleRenderShapes;
			case ShapeType2D_Capsule: return capsuleRenderShapes;
			case ShapeType2D_Polygon: return polygonRenderShapes;
			default: 				  return boxRenderShapes;
		}
	}
}