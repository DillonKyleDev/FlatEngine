#include "CircleCollider.h"
#include "FlatEngine.h"
#include "RigidBody.h"
#include "GameObject.h"
#include "Transform.h"


namespace FlatEngine {

	CircleCollider::CircleCollider(long myID, long parentID) : Collider(myID, parentID)
	{
		SetType(T_CircleCollider);
		SetActiveRadiusGrid(1.5);
		m_b_activeEdgesSet = false;
		m_activeLeft = 0;
		m_activeRight = 0;
		m_activeBottom = 0;
		m_activeTop = 0;
		m_nextActiveLeft = 0;
		m_nextActiveRight = 0;
		m_nextActiveBottom = 0;
		m_nextActiveTop = 0;
	}

	CircleCollider::~CircleCollider()
	{
	}

	std::string CircleCollider::GetData()
	{
		json jsonData = {
			{ "type", "CircleCollider" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "_isContinuous", IsContinuous()},
			{ "_isSolid", IsSolid() },
			{ "activeLayer", GetActiveLayer() },
			{ "activeRadius", GetActiveRadiusGrid() },
			{ "_showActiveRadius", GetShowActiveRadius()},
			{ "activeOffsetX", GetActiveOffset().x },
			{ "activeOffsetY", GetActiveOffset().y },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void CircleCollider::UpdateCenter(float gridStep, Vector2 centerPoint)
	{
		Transform* transform = GetParent()->GetTransform();
		Vector2 position = transform->GetTruePosition();
		float activeRadius = GetActiveRadiusGrid();
		Vector2 activeOffset = GetActiveOffset();

		float xCenter = centerPoint.x + (position.x + activeOffset.x) * gridStep;
		float yCenter = centerPoint.y - (position.y + activeOffset.y) * gridStep;
		
		SetCenterGrid(Vector2(position.x + activeOffset.x, position.y + activeOffset.y));
		SetCenterCoord(Vector2(xCenter, yCenter));
	}

	// Just based on actual pixel locations (0,0) being the top left of the window
	// You can use it for either game view or scene view, you just need the correct center location of whichever you choose
	void CircleCollider::UpdateActiveEdges(float gridStep, Vector2 centerPoint)
	{
		// Only if the activeEdges has not been set or if the velocity is not 0 do we update the active edges
		bool b_shouldUpdate = false;

		GameObject *parent = GetParent();
		Transform* transform = nullptr;
		Vector2 scale = Vector2(1, 1);
		float activeRadius = GetActiveRadiusGrid();

		if (parent != nullptr)
		{
			transform = parent->GetTransform();
		}
		if (transform != nullptr)
		{
			scale = transform->GetScale();
		}

		// Accounts for any scene view scrolling / panning / zooming
		if (GetPreviousCenterPoint() != centerPoint)
		{
			b_shouldUpdate = true;
			SetPreviousCenterPoint(centerPoint);
		}
		if (GetPreviousGridStep() != gridStep)
		{
			b_shouldUpdate = true;
			SetPreviousGridStep(gridStep);
		}

		RigidBody* rigidBody;
		if (parent != nullptr && parent->HasComponent("RigidBody"))
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
			RigidBody* rigidBody = parent->GetRigidBody();
			Transform* transform = GetParent()->GetTransform();
			Vector2 scale = transform->GetScale();
			Vector2 activeOffset = GetActiveOffset();

			// For visual representation ( screen space values )
			SetCenterGrid(Vector2(transform->GetTruePosition().x + activeOffset.x, transform->GetTruePosition().y + activeOffset.y));

			m_activeLeft = centerPoint.x + (GetCenterGrid().x - (activeRadius * scale.x)) * gridStep;
			m_activeTop = centerPoint.y + (-GetCenterGrid().y - (activeRadius * scale.y)) * gridStep;
			m_activeRight = centerPoint.x + (GetCenterGrid().x + (activeRadius * scale.x)) * gridStep;
			m_activeBottom = centerPoint.y + (-GetCenterGrid().y + (activeRadius * scale.y)) * gridStep;

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

			m_nextActiveLeft = GetNextCenterGrid().x - (activeRadius * scale.x);
			m_nextActiveTop = GetNextCenterGrid().y + (activeRadius * scale.y);
			m_nextActiveRight = GetNextCenterGrid().x + (activeRadius * scale.x);
			m_nextActiveBottom = GetNextCenterGrid().y - (activeRadius * scale.y);

			SetNextCenterCoord(Vector2(m_nextActiveLeft + (m_nextActiveRight - m_nextActiveLeft) / 2, m_nextActiveTop + (m_nextActiveBottom - m_nextActiveTop) / 2));
			m_b_activeEdgesSet = true;
		}

		UpdatePreviousPosition();
	}

	void CircleCollider::RecalculateBounds(float gridStep, Vector2 centerPoint)
	{
		UpdateCenter(gridStep, centerPoint);
		UpdateActiveEdges(gridStep, centerPoint);
	}
}
