#include "BoxBody.h"
#include "GameObject.h"
#include "Transform.h"


namespace FlatEngine
{
	BoxBody::BoxBody(long myID, long parentID) : Body::Body(myID, parentID)
	{
		SetType(T_BoxBody);		
		m_bodyProps.b_isEnabled = IsActive();
		m_bodyProps.shape = Physics::BodyShape::BS_Box;
		m_bodyProps.dimensions = Vector2(2.0f, 2.0f);
		m_bodyProps.position = GetParent()->GetTransform()->GetPosition();
	}

	BoxBody::~BoxBody()
	{
	}

	std::string BoxBody::GetData()
	{
		json jsonData = {
			{ "type", "BoxBody" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },			
			{ "bodyType", (int)m_bodyProps.type },
			{ "_lockedRotation", m_bodyProps.b_lockedRotation },
			{ "_lockedXAxis", m_bodyProps.b_lockedXAxis },
			{ "_lockedYAxis", m_bodyProps.b_lockedYAxis },
			{ "gravityScale", m_bodyProps.gravityScale },
			{ "linearDamping", m_bodyProps.linearDamping },
			{ "angularDamping", m_bodyProps.angularDamping },
			{ "restitution", m_bodyProps.restitution },
			{ "density", m_bodyProps.density },
			{ "friction", m_bodyProps.friction },
			{ "width", m_bodyProps.dimensions.x },
			{ "height", m_bodyProps.dimensions.y },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void BoxBody::SetDimensions(Vector2 dimensions)
	{
		if (dimensions.x != 0 && dimensions.y != 0)
		{			
			m_bodyProps = GetLiveProps();
			m_bodyProps.dimensions = dimensions;
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
		float rotation = GetRotation();
		float width = m_bodyProps.dimensions.x;
		float height = m_bodyProps.dimensions.y;

		Vector2 topLeft = Vector2::Rotate(Vector2(-width / 2, height / 2), rotation);
		Vector2 topRight = Vector2::Rotate(Vector2(+width / 2, height / 2), rotation);
		Vector2 bottomRight = Vector2::Rotate(Vector2(+width / 2, -height / 2), rotation);
		Vector2 bottomLeft = Vector2::Rotate(Vector2(-width / 2, -height / 2), rotation);

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