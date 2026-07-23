#include "components/Body.h"
#include "render/SceneView.h"
#include "shapes/Box.h"
#include "tools/Numbers.h"


namespace FlatEngine
{
	Box::Box(Body* parentBody) : Shape(parentBody)
	{
		m_shapeProps.shape = Shape::ShapeType::BS_Box;
		m_shapeProps.dimensions = Vector2(2.0f, 2.0f);		
	}

	void Box::SetDimensions(Vector2 dimensions)
	{
		if (dimensions.x != 0 && dimensions.y != 0)
		{			
			m_shapeProps.dimensions = dimensions;
			RecreateShape();
		}
	}

	void Box::UpdateCorners()
	{
		float rotationOffset = Numbers::RadiansToDegrees(b2Rot_GetAngle(m_shapeProps.rotationOffset));
		float rotation = m_parentBody->GetRotation() + rotationOffset;
		float width = m_shapeProps.dimensions.x;
		float height = m_shapeProps.dimensions.y;
		Vector2 offset = m_shapeProps.positionOffset;

		Vector2 topLeft = Vector2::Rotate(Vector2(-width / 2, height / 2) + offset, rotation);
		Vector2 topRight = Vector2::Rotate(Vector2(+width / 2, height / 2) + offset, rotation);
		Vector2 bottomRight = Vector2::Rotate(Vector2(+width / 2, -height / 2) + offset, rotation);
		Vector2 bottomLeft = Vector2::Rotate(Vector2(-width / 2, -height / 2) + offset, rotation);

		Vector2 position = m_parentBody->GetPosition();
		Vector2 corners[4] =
		{
			position + topLeft,
			position + topRight,
			position + bottomRight,
			position + bottomLeft
		};
		SetCorners(corners);
	}

	void Box::SetCorners(Vector2 corners[4])
	{
		m_corners[0] = corners[0];
		m_corners[1] = corners[1];
		m_corners[2] = corners[2];
		m_corners[3] = corners[3];
	}

	Vector2* Box::GetCornersWorld()
	{
		return m_corners;
	}

	std::vector<Vector2> Box::GetCornersScreen()
	{
		std::vector<Vector2> corners = {
			SceneView::Scene_ConvertWorldToScreen(m_corners[0]),
			SceneView::Scene_ConvertWorldToScreen(m_corners[1]),
			SceneView::Scene_ConvertWorldToScreen(m_corners[2]),
			SceneView::Scene_ConvertWorldToScreen(m_corners[3]),
		};

		return corners;
	}
	void Box::SetCornerRadius(float cornerRadius)
	{		
		m_shapeProps.cornerRadius = cornerRadius;
		RecreateShape();
	}
}
