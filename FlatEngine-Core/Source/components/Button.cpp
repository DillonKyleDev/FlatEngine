#include "components/Button.h"
#include "components/Transform.h"
#include "GameObject.h"
#include "GuiCore.h"
#include "managers/Assets.h"
#include "managers/SceneManager.h"
#include "physics/Shape2D.h"
#include "render/SceneView.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"
#include "tools/Vector3.h"
#include <types.h>

#include <variant>


namespace FlatEngine
{
	Button::Button(long ownerID)
	{
		SetType(ComponentType_Button);
		SetOwnerID(ownerID);
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
			{ "shapeType", ShapeType2DStrings[(int)shapeType] },
			{ "functionName", functionName },
			{ "b_luaFunction", m_b_luaFunction },
			{ "b_leftClick", m_b_leftClick },
			{ "b_rightClick", m_b_rightClick },
			{ "canvasPlacement", m_canvasPlacement.GetData() }
		};
		componentJson.update(Component::GetData(b_IDOverride));
		componentJson["shapeData"] = std::visit([](auto&& sData) { return sData.GetData(); }, shapeData);

		json parameters = parameterContainer.GetData();
		
		componentJson.push_back({ "functionParameters", parameters });

		return componentJson;
	}

	void Button::PutData(json componentJson, std::string objectName)
	{
		json shapeDataJson = json::object();
		if (!componentJson.empty() && JsonHelper::JsonContains(componentJson, "shapeData", objectName))		
			shapeDataJson = componentJson.at("shapeData");

		shapeType = GetTypeFromString<ShapeType2D>(ShapeType2DFromString, JsonHelper::CheckJsonString(componentJson, "shapeType", objectName));

		switch (shapeType)
		{
			case ShapeType2D_Box:     shapeData = BoxShape2DData();     renderShapes.push_back(CreateQuadObject()); break; 
			case ShapeType2D_Circle:  shapeData = CircleShape2DData();  renderShapes.push_back(CreateCircleObject());  break; 
			case ShapeType2D_Capsule: shapeData = CapsuleShape2DData(); renderShapes = CreateCapsuleObject(); break; 
			case ShapeType2D_Polygon: shapeData = PolygonShape2DData(); renderShapes = CreatePolygonObject(); break; 			
			case ShapeType2D_Chain:   shapeData = ChainShape2DData();   renderShapes = CreateChainObject(); break;
			default: break;
		}
		std::visit([shapeDataJson, objectName](auto&& sData) { sData.PutData(shapeDataJson, objectName); }, shapeData);

		if (componentJson == json::object())		
			return;	
		
        Component::PutData(componentJson, objectName);	

		json canvasPlacementJson = json::object();
		if (JsonHelper::JsonContains(componentJson, "canvasPlacement", objectName))		
			canvasPlacementJson = componentJson.at("canvasPlacement");
		m_canvasPlacement.PutData(canvasPlacementJson, objectName);		

		parameterContainer.PutData(componentJson, objectName);

		functionName = JsonHelper::CheckJsonString(componentJson, "functionName", objectName);
		SetIsCPP(JsonHelper::CheckJsonBool(componentJson, "b_luaFunction", objectName));						
		SetLeftClick(JsonHelper::CheckJsonBool(componentJson, "b_leftClick", objectName));
		SetRightClick(JsonHelper::CheckJsonBool(componentJson, "b_rightClick", objectName));	
    }

	bool Button::CheckForMouseOver(Vector2 mousePos)
	{
		bool b_mouseOver = false;
		GameObject* owner = SceneManager::loadedScene.GetObjectByID(GetOwnerID());
		Canvas* canvas = owner->GetFirstCanvas();
		Transform* ownerTransform = owner->Get<Transform>();
		Vector3 ownerPos = ownerTransform->GetPosition();
		Vector2 worldPos = canvas->GetMousePosOnCanvas(mousePos);
		b2Vec2 worldPoint = { worldPos.x, worldPos.y };

		std::visit([this, canvas, mousePos, &b_mouseOver, ownerTransform, ownerPos, worldPoint](auto&& sData)
		{
			using T = std::decay_t<decltype(sData)>;

			if constexpr (std::is_same_v<T, BoxShape2DData>)
			{	
				b2Polygon polygon = b2MakeBox(sData.dimensions.x * 0.5f, sData.dimensions.y * 0.5f);												
				b2Transform b2transform;				
				b2transform.p = { ownerPos.x, ownerPos.y };
				b2transform.q = b2MakeRot(Numbers::DegreesToRadians(ownerTransform->GetRotation().z));
				
				b2Vec2 localPoint = b2InvTransformPoint(b2transform, worldPoint);				
				b_mouseOver = b2PointInPolygon(&polygon, localPoint);
			}
			if constexpr (std::is_same_v<T, CircleShape2DData>)
			{
				b2Circle circle;
				circle.center = { 0.0f, 0.0f };
				circle.radius = sData.radius;

				b2Transform ownerXf;
				ownerXf.p = { ownerPos.x, ownerPos.y };
				ownerXf.q = b2MakeRot(Numbers::DegreesToRadians(ownerTransform->GetRotation().z));

				b2Transform localXf;
				localXf.p = { sData.offset.x, sData.offset.y };
				localXf.q = sData.rotationOffset;

				b2Transform xf = b2MulTransforms(ownerXf, localXf);

				b2Vec2 localPoint = b2InvTransformPoint(xf, worldPoint);
				b_mouseOver = b2PointInCircle(&circle, localPoint);
			}
			else if constexpr (std::is_same_v<T, CapsuleShape2DData>)
			{
				b2Capsule capsule;
				float halfLen = sData.length * 0.5f;
				if (sData.b_horizontal) {
					capsule.center1 = { -halfLen, 0.0f };
					capsule.center2 = {  halfLen, 0.0f };
				} else {
					capsule.center1 = { 0.0f, -halfLen };
					capsule.center2 = { 0.0f,  halfLen };
				}
				capsule.radius = sData.radius;

				b2Transform ownerXf;
				ownerXf.p = { ownerPos.x, ownerPos.y };
				ownerXf.q = b2MakeRot(Numbers::DegreesToRadians(ownerTransform->GetRotation().z));

				b2Transform localXf;
				localXf.p = { sData.offset.x, sData.offset.y };
				localXf.q = sData.rotationOffset;

				b2Transform xf = b2MulTransforms(ownerXf, localXf);

				b2Vec2 localPoint = b2InvTransformPoint(xf, worldPoint);
				b_mouseOver = b2PointInCapsule(&capsule, localPoint);
			}
			else if constexpr (std::is_same_v<T, PolygonShape2DData>)
			{
				std::vector<b2Vec2> b2Points;
				b2Points.reserve(sData.points.size());
				for (auto& p : sData.points) b2Points.push_back({ p.x, p.y });

				b2Hull hull = b2ComputeHull(b2Points.data(), (int)b2Points.size());
				b2Polygon polygon = b2MakePolygon(&hull, sData.cornerRadius);

				b2Transform xf;
				xf.p = { ownerPos.x, ownerPos.y };
				xf.q = b2MakeRot(Numbers::DegreesToRadians(ownerTransform->GetRotation().z));

				b2Vec2 localPoint = b2InvTransformPoint(xf, worldPoint);
				b_mouseOver = b2PointInPolygon(&polygon, localPoint);
			}
			else if constexpr (std::is_same_v<T, ChainShape2DData>)
			{
				// Box2D has no built-in point-in-chain test — chains are one-sided edge
				// loops/strips, not solid areas. If b_isLoop, treat the outline as a
				// polygon for hover-testing purposes (approximation, not physically
				// identical to how the chain actually collides in the sim).
				if (sData.b_isLoop)
				{
					std::vector<b2Vec2> b2Points;
					b2Points.reserve(sData.points.size());
					for (auto& p : sData.points) b2Points.push_back({ p.x, p.y });

					b2Hull hull = b2ComputeHull(b2Points.data(), (int)b2Points.size());
					b2Polygon polygon = b2MakePolygon(&hull, 0.0f);

					b2Transform xf;
					xf.p = { ownerPos.x, ownerPos.y };
					xf.q = b2MakeRot(Numbers::DegreesToRadians(ownerTransform->GetRotation().z));

					b2Vec2 localPoint = b2InvTransformPoint(xf, worldPoint);
					b_mouseOver = b2PointInPolygon(&polygon, localPoint);
				}
				else
				{
					// Open chain (not a loop) has no interior — decide separately
					// whether hover should mean "near any segment" (distance-to-segment
					// check) rather than "inside," since there's no inside to test.
					b_mouseOver = false;
				}
			}
		}, shapeData);

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
		Vector3 canvasPos = canvas->GetOwningObject()->Get<Transform>()->GetAbsolutePosition();
		Vector3 scale = ownerTransform->GetScale();				
		ownerTransform->SetPosition(canvas->GetCanvasPlacementPosition(&m_canvasPlacement, SceneView::finalImageSize));						
	}

	// only do this after changing a shape setting because they don't really change that often and don't have a physics step
	void Button::UpdateRenderShapes()
	{
		UpdateButtonTransform(); 

		Transform* ownerTransform = SceneManager::loadedScene.Get<Transform>(GetOwnerID());		
		Canvas* canvas = GetOwningObject()->GetFirstCanvas();

		if (ownerTransform == nullptr || canvas == nullptr)
			return;
		
		Vector3 ownerPos = ownerTransform->GetPosition();
		Vector3 ownerRot = ownerTransform->GetRotation();
		Vector3 ownerScale = ownerTransform->GetScale();
		Vector3 canvasPos = canvas->GetOwningObject()->Get<Transform>()->GetAbsolutePosition();		

		Transform renderTransform = Transform(*ownerTransform);
		Vector2 pivotOffset = m_canvasPlacement.pivot->offset;
		pivotOffset = Vector2(pivotOffset.x * ownerScale.x / GuiCore::texturePixelsPerGridSpace, pivotOffset.y * ownerScale.y / GuiCore::texturePixelsPerGridSpace * -1.0f);				
		Vector3 buttonPos = ownerTransform->GetPosition();						
		Vector3 renderOffset = Vector3(pivotOffset, m_canvasPlacement.zPosition);
		renderTransform.SetPosition(buttonPos + renderOffset);

		if (renderShapes.size() == 0)
			return;
		
		std::string activeString = IsActive() ? "Active" : "Inactive";
		std::string color = "canvasButton" + activeString;

		std::visit([this, renderTransform, ownerTransform, ownerScale, color](auto&& sData)
		{
			using T = std::decay_t<decltype(sData)>;

			if constexpr (std::is_same_v<T, BoxShape2DData>)
			{					
				this->renderShapes[0].transform = renderTransform;
				this->renderShapes[0].transform.SetScale(Vector3(ownerScale.x * sData.dimensions.x, ownerScale.y * sData.dimensions.y, 1));
				this->m_canvasPlacement.pivot->dimensions = sData.dimensions * GuiCore::texturePixelsPerGridSpace;
				this->renderShapes[0].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
			}
			else if constexpr (std::is_same_v<T, CircleShape2DData>)
			{
				// Transform renderTransform;						
				// renderTransform.SetPosition(Vector3(ownerPos.x + sData.offset.x, ownerPos.y + sData.offset.y, ownerPos.z));
				// renderTransform.SetScale(Vector3(sData.radius, sData.radius, 1));
				// this->renderShapes[0].transform = renderTransform;
			}
			else if constexpr (std::is_same_v<T, CapsuleShape2DData>)
			{
				// if (this->renderShapes.size() != 8)											
				// 	return;					
								
				// float center1Value = ((sData.length / 2) - sData.radius) * -1;
				// float center2Value = (sData.length / 2) - sData.radius;
				// b2Vec2 offset = b2Vec2(sData.offset.x, sData.offset.y);
				// Vector2 center1 = b2Vec2(sData.b_horizontal ? center1Value : 0, sData.b_horizontal ? 0 : center1Value);
				// Vector2 center2 = b2Vec2(sData.b_horizontal ? center2Value : 0, sData.b_horizontal ? 0 : center2Value);				
				// Vector2 center1World = Vector2(b2Body_GetWorldPoint(m_bodyID, offset + b2Vec2(center1.x, center1.y)));
				// Vector2 center2World = Vector2(b2Body_GetWorldPoint(m_bodyID, offset + b2Vec2(center2.x, center2.y)));
				// Vector2 difference = center2World - center1World;
				// Vector2 diffN = Vector2::Normalize(difference);
				// Vector2 diffNR = diffN * sData.radius;
				// Vector2 diffPerp = Vector2::Rotate(diffNR, 90);
				// Vector2 flippedDiffPerp = Vector2::Rotate(diffNR, -90);
				
				// // circles
				// this->renderShapes[0].transform.SetPosition(Vector3(center1World, ownerPos.z));
				// this->renderShapes[0].transform.SetScale(Vector3(sData.radius, sData.radius, 1));
				// this->renderShapes[1].transform.SetPosition(Vector3(center2World, ownerPos.z));
				// this->renderShapes[1].transform.SetScale(Vector3(sData.radius, sData.radius, 1));
				// // Lines
				// this->renderShapes[2].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center1World - diffNR, ownerPos.z), Vector3(center1World + diffNR, ownerPos.z));				
				// this->renderShapes[3].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center2World - diffNR, ownerPos.z), Vector3(center2World + diffNR, ownerPos.z));				
				// this->renderShapes[4].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center1World - diffPerp, ownerPos.z), Vector3(center1World + diffPerp, ownerPos.z));				
				// this->renderShapes[5].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center2World - diffPerp, ownerPos.z), Vector3(center2World + diffPerp, ownerPos.z));				
				// this->renderShapes[6].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center1World + diffPerp, ownerPos.z), Vector3(center1World + diffPerp + difference, ownerPos.z));												
				// this->renderShapes[7].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center1World + flippedDiffPerp, ownerPos.z), Vector3(center1World + flippedDiffPerp + difference, ownerPos.z));									

				// std::string shapeString = "capsule";
				// Vector4 color = Assets::assetManager.GetColor(shapeString + postfix);
				// Vector4 colorLight = Assets::assetManager.GetColor(shapeString + postfix + "Light");

				// this->renderShapes[0].mesh.SetUBOVec4("color", color);
				// this->renderShapes[1].mesh.SetUBOVec4("color", color);
				// this->renderShapes[2].mesh.SetUBOVec4("color", color);
				// this->renderShapes[3].mesh.SetUBOVec4("color", color);
				// this->renderShapes[4].mesh.SetUBOVec4("color", color);
				// this->renderShapes[5].mesh.SetUBOVec4("color", color);
				// this->renderShapes[6].mesh.SetUBOVec4("color", color);
				// this->renderShapes[7].mesh.SetUBOVec4("color", color);		
			}
			else if constexpr (std::is_same_v<T, PolygonShape2DData>)
			{
				// if (this->renderShapes.size() < sData.points.size())
				// {
				// 	int diff = sData.points.size() - this->renderShapes.size();
				// 	for (int i = 0; i < diff; i++)
				// 	{
				// 		this->renderShapes.push_back(CreateLineObject());
				// 	}
				// }

				// for (int p = 0; p < sData.points.size(); p++)
				// {
				// 	int pNext = p == sData.points.size() - 1 ? 0 : p + 1;
				// 	b2Vec2 pointStart = b2Body_GetWorldPoint(m_bodyID, b2Vec2(sData.points[p].x, sData.points[p].y));
				// 	Vector3 startPos = Vector3(pointStart, ownerPos.z);
				// 	b2Vec2 pointEnd = b2Body_GetWorldPoint(m_bodyID, b2Vec2(sData.points[pNext].x, sData.points[pNext].y));
				// 	Vector3 endPos = Vector3(pointEnd, ownerPos.z);

				// 	this->renderShapes[p].transform = SceneView::GetLineTransformForStartEndPos(startPos, endPos);
				// }
			}
			else if constexpr (std::is_same_v<T, ChainShape2DData>)
			{
				if (this->renderShapes.size() < sData.points.size())
				{
					int diff = sData.points.size() - this->renderShapes.size();
					for (int i = 0; i < diff; i++)
					{
						this->renderShapes.push_back(CreateLineObject());
					}
				}

				for (int p = 0; p < sData.points.size(); p++)
				{
					if (p == sData.points.size() - 1)
					{
						if (sData.b_isLoop)
						{
							this->renderShapes[p].mesh.SetActive(true);
						}
						else 
						{
							this->renderShapes[p].mesh.SetActive(false);
							return;
						}
					}

					// int pNext = p == sData.points.size() - 1 ? 0 : p + 1;
					// b2Vec2 pointStart = b2Body_GetWorldPoint(m_bodyID, b2Vec2(sData.points[p].x, sData.points[p].y));
					// Vector3 startPos = Vector3(pointStart, ownerPos.z);
					// b2Vec2 pointEnd = b2Body_GetWorldPoint(m_bodyID, b2Vec2(sData.points[pNext].x, sData.points[pNext].y));
					// Vector3 endPos = Vector3(pointEnd, ownerPos.z);
					// this->renderShapes[p].transform = SceneView::GetLineTransformForStartEndPos(startPos, endPos);

					// std::string color = "chain";
					// if ((p == 0 || p == this->renderShapes.size() - 2) && !sData.b_isLoop)
					// 	color = "chainEndSegments";
					// this->renderShapes[p].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color + postfix));
				}
			}
		}, shapeData);
	}
}