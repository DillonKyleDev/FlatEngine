#include "components/Canvas.h"
#include "components/Transform.h"
#include "GameObject.h"
#include "render/GameView.h"
#include "tools/Logger.h"


namespace FlatEngine 
{
	Canvas::Canvas(long myID, long parentObjectID)
	{
		SetType(ComponentType_Canvas);
		SetID(myID);
		SetParentObjectID(parentObjectID);
		m_layerNumber = 0;
		m_b_blocksLayers = true;
		m_width = 20;
		m_height = 10;
		m_activeEdges = Vector4();
	}

	std::string Canvas::GetData()
	{
		json jsonData = {
			{ "type", "Canvas" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "width", m_width },
			{ "height", m_height },
			{ "layerNumber", m_layerNumber },
			{ "_blocksLayers", m_b_blocksLayers },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	    void Canvas::PutData(json componentJson)
	{
		std::string objectName = "Canvas GameObject";
		std::string type = JsonHelper::CheckJsonString(componentJson, "type", objectName);
		long componentID = JsonHelper::CheckJsonLong(componentJson, "id", objectName);
		bool b_isCollapsed = JsonHelper::CheckJsonBool(componentJson, "_isCollapsed", objectName);
		bool b_isActive = JsonHelper::CheckJsonBool(componentJson, "_isActive", objectName);
		SetActive(b_isActive);
		SetCollapsed(b_isCollapsed);
		SetDimensions(JsonHelper::CheckJsonFloat(componentJson, "width", objectName), JsonHelper::CheckJsonFloat(componentJson, "height", objectName));
		SetLayerNumber(JsonHelper::CheckJsonInt(componentJson, "layerNumber", objectName));
		SetBlocksLayers(JsonHelper::CheckJsonBool(componentJson, "_blocksLayers", objectName));
    }

	float Canvas::GetWidth()
	{
		return m_width;
	}

	float Canvas::GetHeight()
	{
		return m_height;
	}

	void Canvas::SetDimensions(float newWidth, float newHeight)
	{
		if (newWidth >= 0 && newHeight >= 0)
		{
			m_width = newWidth;
			m_height = newHeight;
		}
		else
		{
			Logger::log.Err("Canvas width and height must be positive values.");
		}
	}

	void Canvas::CalculateActiveEdges()
	{
		Transform* transform = GetParentObject()->Get<Transform>();
		Vector3 position = transform->GetAbsolutePosition();
		Vector3 scale = transform->GetScale();

		float activeLeft = GameView::gameViewCenter.x + ((position.x - (m_width / 2 * scale.x)) * GameView::gameViewGridStep);
		float activeRight = GameView::gameViewCenter.x + ((position.x + (m_width / 2 * scale.x)) * GameView::gameViewGridStep);
		float activeTop = GameView::gameViewCenter.y - ((position.y + (m_height / 2 * scale.y)) * GameView::gameViewGridStep);
		float activeBottom = GameView::gameViewCenter.y - ((position.y - (m_height / 2 * scale.y)) * GameView::gameViewGridStep);

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