#include "components/Canvas.h"
#include "components/Transform.h"
#include "GameObject.h"
#include "render/GameView.h"
#include "tools/Logger.h"
#include "tools/Vector2.h"


namespace FlatEngine 
{
	Canvas::Canvas(long ownerID)
	{
		SetType(ComponentType_Canvas);
		SetOwnerID(ownerID);
		m_layerNumber = 0;
		m_b_blocksLayers = true;
		m_dimensions = Vector2(20, 10);
		m_activeEdges = Vector4();
	}

	json Canvas::GetData(bool b_IDOverride)
	{
		json componentJson = {
			{ "width", m_dimensions.x },
			{ "height", m_dimensions.y },
			{ "layerNumber", m_layerNumber },
			{ "b_blocksLayers", m_b_blocksLayers }
		};
		componentJson.update(Component::GetData(b_IDOverride));

		return componentJson;
	}

	void Canvas::PutData(json componentJson, std::string objectName)
	{
		if (componentJson.empty())		
			return;	
		
        Component::PutData(componentJson, objectName);

		SetDimensions(Vector2(JsonHelper::CheckJsonFloat(componentJson, "width", objectName), JsonHelper::CheckJsonFloat(componentJson, "height", objectName)));
		SetLayerNumber(JsonHelper::CheckJsonInt(componentJson, "layerNumber", objectName));
		SetBlocksLayers(JsonHelper::CheckJsonBool(componentJson, "b_blocksLayers", objectName));
    }

	Vector2 Canvas::GetDimensions()
	{
		return m_dimensions;
	}

	void Canvas::SetDimensions(Vector2 setDimensions)
	{
		if (m_dimensions.x >= 0 && m_dimensions.y >= 0)
		{
			m_dimensions = setDimensions;
		}
		else
		{
			Logger::log.Err("Canvas width and height must be positive values.");
		}
	}

	void Canvas::CalculateActiveEdges()
	{
		Transform* transform = GetOwningObject()->Get<Transform>();
		Vector3 position = transform->GetAbsolutePosition();
		Vector3 scale = transform->GetScale();

		float activeLeft = GameView::gameViewCenter.x + ((position.x - (m_dimensions.x / 2 * scale.x)) * GameView::gameViewGridStep);
		float activeRight = GameView::gameViewCenter.x + ((position.x + (m_dimensions.x / 2 * scale.x)) * GameView::gameViewGridStep);
		float activeTop = GameView::gameViewCenter.y - ((position.y + (m_dimensions.y / 2 * scale.y)) * GameView::gameViewGridStep);
		float activeBottom = GameView::gameViewCenter.y - ((position.y - (m_dimensions.y / 2 * scale.y)) * GameView::gameViewGridStep);

		m_activeEdges = Vector4(activeTop, activeRight, activeBottom, activeLeft);
	}

	Vector4 Canvas::GetActiveEdges()
	{
		return m_activeEdges;
	}

	void Canvas::SetLayerNumber(int newLayerNumber)
	{
		m_layerNumber = newLayerNumber;
	}

	int Canvas::GetLayerNumber()
	{
		return m_layerNumber;
	}

	void Canvas::SetBlocksLayers(bool b_blocksLayers)
	{
		m_b_blocksLayers = b_blocksLayers;
	}

	bool Canvas::GetBlocksLayers()
	{
		return m_b_blocksLayers;
	}
}