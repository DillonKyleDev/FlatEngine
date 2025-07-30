#include "BoxCollider.h"
#include "RigidBody.h"
#include "GameObject.h"
#include "Transform.h"
#include "Physics.h"

#include "imgui_internal.h"
#include <cmath>

namespace FlatEngine
{
	BoxCollider::BoxCollider(long myID, long parentID) : Collider(myID, parentID)
	{		
		SetType(T_BoxCollider);
		m_b_isTileMapCollider = false;
		m_activeWidth = 2;
		m_activeHeight = 2;
		m_activeEdges = Vector4(0, 0, 0, 0);
		m_b_activeEdgesSet = false;
		m_activeLeft = 0;
		m_activeRight = 0;
		m_activeBottom = 0;
		m_activeTop = 0;
		m_nextActiveLeft = 0;
		m_nextActiveRight = 0;
		m_nextActiveBottom = 0;
		m_nextActiveTop = 0;
		m_nextCorners;
		m_previousRotation = 0;
		m_lastGridStep = 0;
		m_lastViewportCenter = Vector2();

		m_bodyProps.position = GetParent()->GetTransform()->GetTruePosition();
		m_bodyProps.dimensions = Vector2(m_activeWidth, m_activeHeight);

		F_Physics->CreateBox(m_bodyProps, m_bodyID);
	}

	BoxCollider::~BoxCollider()
	{
	}

	std::string BoxCollider::GetData()
	{
		json jsonData = {
			{ "type", "BoxCollider" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "_isTileMapCollider", m_b_isTileMapCollider },
			{ "activeWidth", m_activeWidth },
			{ "activeHeight", m_activeHeight },
			{ "activeOffsetX", GetActiveOffset().x},
			{ "activeOffsetY", GetActiveOffset().y },
			{ "_isContinuous", IsContinuous()},
			{ "_isSolid", IsSolid() },
			{ "activeLayer", GetActiveLayer() },
			{ "_showActiveRadius", GetShowActiveRadius()}
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	bool BoxCollider::IsTileMapCollider()
	{
		return m_b_isTileMapCollider;
	}

	void BoxCollider::SetTileMapCollider(bool b_isTileMapCollider)
	{
		m_b_isTileMapCollider = b_isTileMapCollider;
	}

	void BoxCollider::SetActiveDimensions(float width, float height)
	{
		if (width >= 0 && height >= 0)
		{
			m_bodyProps.dimensions = Vector2(width, height);
			F_Physics->DestroyBody(m_bodyID);
			F_Physics->CreateBox(m_bodyProps, m_bodyID);

			m_activeWidth = width;
			m_activeHeight = height;
			bool b_forceUpdate = true;
			UpdateActiveEdges(F_sceneViewGridStep.x, F_sceneViewCenter, b_forceUpdate);
		}
		else
		{
			LogError("Active dimensions must be larger than 0.");
		}
	}

	float BoxCollider::GetActiveWidth()
	{
		return m_activeWidth;
	}

	float BoxCollider::GetActiveHeight()
	{
		return m_activeHeight;
	}

	void BoxCollider::SetActiveEdges(Vector4 edges)
	{
		m_activeEdges = edges;
		m_b_activeEdgesSet = true;
	}

	Vector4 BoxCollider::GetActiveEdges()
	{
		return m_activeEdges;
	}

	// Just based on actual pixel locations (0,0) being the top left of the window
	// You can use it for either game view or scene view, you just need the correct center location of whichever you choose
	void BoxCollider::UpdateActiveEdges(float gridStep, Vector2 viewportCenter, bool b_forceUpdate)
	{
		bool b_shouldUpdate = false;

		GameObject* parent = GetParent();
		Transform* transform = nullptr;
		Vector2 scale = Vector2(1, 1);

		if (parent != nullptr)
		{
			transform = parent->GetTransform();
		}
		if (transform != nullptr)
		{
			scale = transform->GetScale();
		}

		// Accounts for any scene view scrolling / panning
		if (GetPreviousCenterPoint() != viewportCenter)
		{
			b_shouldUpdate = true;
			SetPreviousCenterPoint(viewportCenter);
		}
		if (GetPreviousGridStep() != gridStep)
		{
			b_shouldUpdate = true;
			SetPreviousGridStep(gridStep);
		}

		RigidBody* rigidBody;
		if (GetParent() != nullptr && GetParent()->HasComponent("RigidBody"))
		{
			rigidBody = GetParent()->GetRigidBody();
			Vector2 velocity = rigidBody->GetVelocity();

			if (velocity.x != 0 || velocity.y != 0 || !m_b_activeEdgesSet || HasMoved())
			{
				b_shouldUpdate = true;
			}
		}
		else
		{
			if (!m_b_activeEdgesSet || HasMoved())
			{
				b_shouldUpdate = true;
			}
		}

		if (GetParent()->GetTransform()->GetRotation() != m_previousRotation)
		{
			b_shouldUpdate = true;
		}
		m_previousRotation = GetParent()->GetTransform()->GetRotation();

		if (b_shouldUpdate || b_forceUpdate)
		{
			m_b_activeEdgesSet = true;
			RigidBody* rigidBody = parent->GetRigidBody();
			Transform* transform = GetParent()->GetTransform();
			Vector2 scale = transform->GetScale();
			Vector2 activeOffset = GetActiveOffset();			
			Vector2 position = GetB2Position();

			// For visual representation ( screen space values )
			SetCenterGrid(position + activeOffset);

			m_activeLeft = viewportCenter.x + (GetCenterGrid().x - (m_activeWidth * scale.x / 2)) * gridStep;
			m_activeTop = viewportCenter.y + (-GetCenterGrid().y - (m_activeHeight * scale.y / 2)) * gridStep;
			m_activeRight = viewportCenter.x + (GetCenterGrid().x + (m_activeWidth * scale.x / 2)) * gridStep;
			m_activeBottom = viewportCenter.y + (-GetCenterGrid().y + (m_activeHeight * scale.y / 2)) * gridStep;

			SetCenterCoord(Vector2(m_activeLeft + (m_activeRight - m_activeLeft) / 2, m_activeTop + (m_activeBottom - m_activeTop) / 2));

			// For collision detection ( grid space values )
			if (rigidBody != nullptr)
			{
				SetNextCenterGrid(Vector2(rigidBody->GetNextPosition().x + activeOffset.x, rigidBody->GetNextPosition().y + activeOffset.y));
			}
			else
			{
				SetNextCenterGrid(GetCenterGrid());
			}

			m_nextActiveLeft = GetNextCenterGrid().x - (m_activeWidth * scale.x / 2);
			m_nextActiveTop = GetNextCenterGrid().y + (m_activeHeight * scale.y / 2);
			m_nextActiveRight = GetNextCenterGrid().x + (m_activeWidth * scale.x / 2);
			m_nextActiveBottom = GetNextCenterGrid().y - (m_activeHeight * scale.y / 2);

			SetNextCenterCoord(Vector2(m_nextActiveLeft + (m_nextActiveRight - m_nextActiveLeft) / 2, m_nextActiveTop + (m_nextActiveBottom - m_nextActiveTop) / 2));

			UpdateCorners(gridStep, viewportCenter);
		}

		UpdatePreviousPosition();
	}

	void BoxCollider::UpdateNormals(float gridStep, Vector2 centerPoint)
	{
		float cos_a = cosf(GetRotation() * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
		float sin_a = sinf(GetRotation() * 2.0f * (float)M_PI / 360.0f);
		Vector2 scale = GetParent()->GetTransform()->GetScale();
		Vector2 center = GetCenterCoord();

		// Normal vectors without rotation
		Vector2 topNormal = Vector2(0, -m_activeHeight * gridStep * scale.y);
		Vector2 rightNormal = Vector2(+m_activeWidth * gridStep * scale.x, 0);
		Vector2 bottomNormal = Vector2(0, +m_activeHeight * gridStep * scale.y);
		Vector2 leftNormal = Vector2(-m_activeWidth * gridStep * scale.x, 0);

		if (GetRotation() != 0)
		{
			// Normal vectors with rotation
			topNormal = ImRotate(Vector2(0, -m_activeHeight * gridStep * scale.y), cos_a, sin_a);
			rightNormal = ImRotate(Vector2(+m_activeWidth * gridStep * scale.x, 0), cos_a, sin_a);
			bottomNormal = ImRotate(Vector2(0, +m_activeHeight * gridStep * scale.y), cos_a, sin_a);
			leftNormal = ImRotate(Vector2(-m_activeWidth * gridStep * scale.x, 0), cos_a, sin_a);
		}

		Vector2 newNormals[4] =
		{
			Vector2(center.x + topNormal.x, center.y + topNormal.y),
			Vector2(center.x + rightNormal.x, center.y + rightNormal.y),
			Vector2(center.x + bottomNormal.x, center.y + bottomNormal.y),
			Vector2(center.x + leftNormal.x, center.y + leftNormal.y),
		};
		
		SetNormals(newNormals);
	}

	void BoxCollider::UpdateCorners(float gridStep, Vector2 centerPoint)
	{
		Transform* transform = GetParent()->GetTransform();
		Vector2 scale = transform->GetScale();
		float rotation = GetB2Rotation();

		Vector2 topLeft = Vector2::Rotate(Vector2(-m_activeWidth / 2 * scale.x, m_activeHeight / 2 * scale.y), rotation);
		Vector2 topRight = Vector2::Rotate(Vector2(+m_activeWidth / 2 * scale.x, m_activeHeight / 2 * scale.y), rotation);
		Vector2 bottomRight = Vector2::Rotate(Vector2(+m_activeWidth / 2 * scale.x, -m_activeHeight / 2 * scale.y), rotation);
		Vector2 bottomLeft = Vector2::Rotate(Vector2(-m_activeWidth / 2 * scale.x, -m_activeHeight / 2 * scale.y), rotation);

		Vector2 position = GetB2Position();
		Vector2 newCorners[4] =
		{
			position + topLeft,
			position + topRight,
			position + bottomRight,
			position + bottomLeft
		};
		SetCorners(newCorners);

		Vector2 nextPosition = position; 
		if (GetParent()->GetRigidBody() != nullptr)
		{
			nextPosition = GetParent()->GetRigidBody()->GetNextPosition();
		}
		Vector2 nextCorners[4] =
		{
			nextPosition + topLeft,
			nextPosition + topRight,
			nextPosition + bottomRight,
			nextPosition + bottomLeft
		};
		SetNextCorners(nextCorners);


		// Calculate activeRadius with pythag
		float rise = std::abs(topLeft.y - GetCenterCoord().y);
		float run = std::abs(topLeft.x - GetCenterCoord().x);
		SetActiveRadiusScreen(std::sqrt((rise * rise) + (run * run)));
		SetActiveRadiusGrid(std::sqrt((rise * rise) + (run * run)) / gridStep);
	}

	void BoxCollider::UpdateCenter(float gridStep, Vector2 centerPoint)
	{
		Transform* transform = GetParent()->GetTransform();
		Vector2 scale = transform->GetScale();
		Vector2 position = transform->GetTruePosition();
		Vector2 activeOffset = GetActiveOffset();

		m_activeLeft = centerPoint.x + (position.x - (m_activeWidth * scale.x / 2) + activeOffset.x) * gridStep;
		m_activeTop = centerPoint.y + (-position.y - (m_activeHeight * scale.y / 2) + activeOffset.y) * gridStep;
		m_activeRight = centerPoint.x + (position.x + (m_activeWidth * scale.x / 2) + activeOffset.x) * gridStep;
		m_activeBottom = centerPoint.y + (-position.y + (m_activeHeight * scale.y / 2) + activeOffset.y) * gridStep;

		SetCenterCoord(Vector2(m_activeLeft + (m_activeRight - m_activeLeft) / 2, m_activeTop + (m_activeBottom - m_activeTop) / 2));
		SetCenterGrid(Vector2(position.x + activeOffset.x, position.y + activeOffset.y));
	}

	void BoxCollider::SetCorners(Vector2 newCorners[4])
	{
		m_corners[0] = newCorners[0];
		m_corners[1] = newCorners[1];
		m_corners[2] = newCorners[2];
		m_corners[3] = newCorners[3];
	}

	void BoxCollider::SetNextCorners(Vector2 newCorners[4])
	{
		m_nextCorners[0] = newCorners[0];
		m_nextCorners[1] = newCorners[1];
		m_nextCorners[2] = newCorners[2];
		m_nextCorners[3] = newCorners[3];
	}

	std::vector<Vector2> BoxCollider::GetCorners()
	{
		std::vector<Vector2> nextCorners = { m_corners[0], m_corners[1], m_corners[2], m_corners[3], };
		return nextCorners;
	}

	std::vector<Vector2> BoxCollider::GetNextCorners()
	{
		std::vector<Vector2> nextCorners = { m_nextCorners[0], m_nextCorners[1], m_nextCorners[2], m_nextCorners[3], };
		return nextCorners;
	}

	void BoxCollider::SetNormals(Vector2 newNormals[4])
	{
		m_normals[0] = newNormals[0];
		m_normals[1] = newNormals[1];
		m_normals[2] = newNormals[2];
		m_normals[3] = newNormals[3];
	}

	Vector2* BoxCollider::GetNormals()
	{
		return m_normals;
	}

	void BoxCollider::RecalculateBounds(float gridStep, Vector2 viewportCenter)
	{
		//UpdateRotation();
		//UpdateActiveEdges();
		//UpdateCenter();
		//UpdateCorners();
		//UpdateNormals();
		if (gridStep != 0)
		{
			m_lastGridStep = gridStep;
		}
		if (viewportCenter.x != 0 && viewportCenter.y != 0)
		{
			m_lastViewportCenter = viewportCenter;
		}

		UpdateActiveEdges(m_lastGridStep, m_lastViewportCenter);
	}

	b2BodyId BoxCollider::GetBodyID()
	{
		return m_bodyID;
	}
}
