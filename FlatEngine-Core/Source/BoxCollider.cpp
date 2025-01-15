#include "BoxCollider.h"
#include "RigidBody.h"
#include "GameObject.h"
#include "Transform.h"

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
			{ "_isStatic", IsStatic() },
			{ "activeLayer", GetActiveLayer() },
			{ "_showActiveRadius", GetShowActiveRadius()},
			{ "_isComposite", IsComposite() },
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
			m_activeWidth = width;
			m_activeHeight = height;
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
	void BoxCollider::UpdateActiveEdges(float gridStep, Vector2 viewportCenter)
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

		if (b_shouldUpdate)
		{
			m_b_activeEdgesSet = true;
			RigidBody* rigidBody = parent->GetRigidBody();
			Transform* transform = GetParent()->GetTransform();
			Vector2 scale = transform->GetScale();
			Vector2 activeOffset = GetActiveOffset();

			// For visual representation ( screen space values )
			SetCenterGrid(Vector2(transform->GetTruePosition().x + activeOffset.x, transform->GetTruePosition().y + activeOffset.y));

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
		float cos_a = cosf(GetRotation() * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
		float sin_a = sinf(GetRotation() * 2.0f * (float)M_PI / 360.0f);
		Transform* transform = GetParent()->GetTransform();
		Vector2 scale = transform->GetScale();
		Vector2 center = GetCenterCoord();

		// Corners without rotation
		Vector2 topLeft = { m_activeLeft, m_activeTop };
		Vector2 bottomRight = { m_activeRight, m_activeBottom };
		Vector2 topRight = { m_activeRight, m_activeTop };
		Vector2 bottomLeft = { m_activeLeft, m_activeBottom };

		if (GetRotation() != 0)
		{
			// Corners with rotation
			topLeft = ImRotate(Vector2(-m_activeWidth * gridStep / 2 * scale.x, -m_activeHeight * gridStep / 2 * scale.y), cos_a, sin_a);
			topRight = ImRotate(Vector2(+m_activeWidth * gridStep / 2 * scale.x, -m_activeHeight * gridStep / 2 * scale.y), cos_a, sin_a);
			bottomRight = ImRotate(Vector2(+m_activeWidth * gridStep / 2 * scale.x, +m_activeHeight * gridStep / 2 * scale.y), cos_a, sin_a);
			bottomLeft = ImRotate(Vector2(-m_activeWidth * gridStep / 2 * scale.x, +m_activeHeight * gridStep / 2 * scale.y), cos_a, sin_a);

			Vector2 newCorners[4] =
			{
				Vector2(center.x + topLeft.x, center.y + topLeft.y),
				Vector2(center.x + topRight.x, center.y + topRight.y),
				Vector2(center.x + bottomRight.x, center.y + bottomRight.y),
				Vector2(center.x + bottomLeft.x, center.y + bottomLeft.y),
			};

			SetCorners(newCorners);
		}
		else
		{
			Vector2 newCorners[4] =
			{
				topLeft,
				topRight,
				bottomRight,
				bottomLeft
			};
			SetCorners(newCorners);
		}

		// Calculate activeRadius with pythag
		float rise = std::abs(topLeft.y - GetCenterCoord().y);
		float run = std::abs(topLeft.x - GetCenterCoord().x);
		SetActiveRadiusScreen(std::sqrt((rise * rise) + (run * run)));
		SetActiveRadiusGrid(std::sqrt((rise * rise) + (run * run)) / gridStep);

		// For collision detection
		Vector2 nextTopLeft = { m_nextActiveLeft, m_nextActiveTop };
		Vector2 nextTopRight = { m_nextActiveRight, m_nextActiveTop };
		Vector2 nextBottomRight = { m_nextActiveRight, m_nextActiveBottom };
		Vector2 nextBottomLeft = { m_nextActiveLeft, m_nextActiveBottom };

		Vector2 newNextCorners[4] =
		{
			nextTopLeft,
			nextTopRight,
			nextBottomRight,
			nextBottomLeft
		};
		SetNextCorners(newNextCorners);
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

	Vector2* BoxCollider::GetCorners()
	{
		return m_corners;
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
		UpdateActiveEdges(gridStep, viewportCenter);
	}
}
