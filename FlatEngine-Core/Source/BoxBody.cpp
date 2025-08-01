#include "BoxBody.h"
#include "GameObject.h"
#include "Transform.h"
#include "FlatEngine.h"



namespace FlatEngine
{
	BoxBody::BoxBody(long myID, long parentID) : Body::Body(myID, parentID)
	{
		SetType(T_BoxBody);
		m_bodyProps.position = GetParent()->GetTransform()->GetTruePosition();
		m_bodyProps.dimensions = Vector2(2.0f, 2.0f);
		m_bodyProps.type = b2_staticBody;		
		m_bodyProps.density = 1.0f;
		m_bodyProps.friction = 0.3f;
		F_Physics->CreateBody(m_bodyProps, m_bodyID);
	}

	BoxBody::~BoxBody()
	{
	}

	void BoxBody::UpdateDimensions()
	{
		SetDimensions(m_bodyProps.dimensions);
	}

	void BoxBody::SetDimensions(Vector2 dimensions)
	{
		if (dimensions.x != 0 && dimensions.y != 0)
		{
			Vector2 scale = GetParent()->GetTransform()->GetScale();
			m_bodyProps.dimensions = Vector2(dimensions.x * scale.x, dimensions.y * scale.y);
			RecreateBody();
		}
		else
		{
			LogError("BoxBody dimensions must not be 0.");
		}
	}

	void BoxBody::UpdateCorners()
	{
		Transform* transform = GetParent()->GetTransform();
		Vector2 scale = transform->GetScale();
		float rotation = RadiansToDegrees(GetRotation());
		float width = m_bodyProps.dimensions.x;
		float height = m_bodyProps.dimensions.y;

		Vector2 topLeft = Vector2::Rotate(Vector2(-width / 2 * scale.x, height / 2 * scale.y), rotation);
		Vector2 topRight = Vector2::Rotate(Vector2(+width / 2 * scale.x, height / 2 * scale.y), rotation);
		Vector2 bottomRight = Vector2::Rotate(Vector2(+width / 2 * scale.x, -height / 2 * scale.y), rotation);
		Vector2 bottomLeft = Vector2::Rotate(Vector2(-width / 2 * scale.x, -height / 2 * scale.y), rotation);

		Vector2 position = GetPosition();
		Vector2 corners[4] =
		{
			position + topLeft,
			position + topRight,
			position + bottomRight,
			position + bottomLeft
		};
		SetCorners(corners);
	}

	void BoxBody::SetCorners(Vector2 corners[4])
	{
		m_corners[0] = corners[0];
		m_corners[1] = corners[1];
		m_corners[2] = corners[2];
		m_corners[3] = corners[3];
	}

	Vector2* BoxBody::GetCornersWorld()
	{
		return m_corners;
	}

	std::vector<Vector2> BoxBody::GetCornersScreen()
	{
		std::vector<Vector2> corners = {
			ConvertWorldToScreen(m_corners[0], F_sceneViewCenter, F_sceneViewGridStep.x),
			ConvertWorldToScreen(m_corners[1], F_sceneViewCenter, F_sceneViewGridStep.x),
			ConvertWorldToScreen(m_corners[2], F_sceneViewCenter, F_sceneViewGridStep.x),
			ConvertWorldToScreen(m_corners[3], F_sceneViewCenter, F_sceneViewGridStep.x),
		};

		return corners;
	}
}