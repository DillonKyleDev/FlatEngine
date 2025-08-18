#include "Canvas.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Transform.h"


namespace FlatEngine 
{
	Canvas::Canvas(long myID, long parentID)
	{
		SetType(T_Canvas);
		SetID(myID);
		SetParentID(parentID);
		m_layerNumber = 0;
		m_b_blocksLayers = true;
		m_width = 20;
		m_height = 10;
		m_activeEdges = Vector4();
	}

	Canvas::~Canvas()
	{
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
			LogError("Canvas width and height must be positive values.");
		}
	}

	void Canvas::CalculateActiveEdges()
	{
		Transform* transform = GetParent()->GetTransform();
		Vector2 position = transform->GetAbsolutePosition();
		Vector2 scale = transform->GetScale();

		float activeLeft = F_gameViewCenter.x + ((position.x - (m_width / 2 * scale.x)) * F_gameViewGridStep.x);
		float activeRight = F_gameViewCenter.x + ((position.x + (m_width / 2 * scale.x)) * F_gameViewGridStep.x);
		float activeTop = F_gameViewCenter.y - ((position.y + (m_height / 2 * scale.y)) * F_gameViewGridStep.x);
		float activeBottom = F_gameViewCenter.y - ((position.y - (m_height / 2 * scale.y)) * F_gameViewGridStep.x);

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