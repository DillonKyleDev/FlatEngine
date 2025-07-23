#include "Collider.h"
#include "FlatEngine.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "RigidBody.h"
#include "GameObject.h"
#include "Transform.h"
#include "RayCast.h"

#include <cmath>


namespace FlatEngine
{
	void BoxBoxRayCastCallback(RayCast* rayCast, Collider* collidedWith)
	{				
	}


	Collider::Collider(long myID, long parentID)
	{		
		SetID(myID);
		SetParentID(parentID);
		
		m_activeOffset = Vector2(0, 0);
		m_previousPosition = Vector2(0, 0);
		m_centerGrid = Vector2(0, 0);
		m_previousGridStep = 0;
		m_nextCenterGrid = Vector2(0, 0);
		m_centerCoord = Vector2(0, 0);
		m_nextCenterCoord = Vector2(0, 0);
		m_activeRadiusScreen = 0;
		m_activeRadiusGrid = 0;
		m_activeLayer = 0;
		m_b_isColliding = false;
		m_b_isContinuous = true;
		m_b_isSolid = true;
		m_b_showActiveRadius = false;

		m_b_cornerCollided = false;
		m_b_sideCollided = false;
		m_impactNormal = Vector2();
	}

	Collider::~Collider()
	{
	}

	bool Collider::CheckForCollision(Collider* collider1, Collider* collider2)
	{
		bool b_colliding = false;
		Vector2 collider1Center = collider1->GetCenterGrid();
		Vector2 collider2Center = collider2->GetCenterGrid();
		float depth = 100;
		Vector2 collisionNormal = Vector2(0, 0);
		std::vector<Vector2> box1HitPositions = std::vector<Vector2>();
		std::vector<Vector2> box2HitPositions = std::vector<Vector2>();
		// Calculate center distance with pythag
		float rise = std::abs(collider1Center.y - collider2Center.y);
		float run = std::abs(collider1Center.x - collider2Center.x);
		float centerDistance = std::sqrt((rise * rise) + (run * run));

		// If they are close enough to check for collision ( actually colliding if they are both CircleColliders )
		if (centerDistance < collider1->GetActiveRadiusGrid() + collider2->GetActiveRadiusGrid())
		{
			if (collider1->GetTypeString() == "BoxCollider" && collider2->GetTypeString() == "BoxCollider")
			{
				BoxCollider* boxCol1 = static_cast<BoxCollider*>(collider1);
				BoxCollider* boxCol2 = static_cast<BoxCollider*>(collider2);

				b_colliding = CheckForCollisionBoxBoxSAT(boxCol1, boxCol2, box1HitPositions, box2HitPositions, collisionNormal, depth);
			}
			else if (collider1->GetTypeString() == "CircleCollider" && collider2->GetTypeString() == "BoxCollider")
			{
				CircleCollider* circleCol = static_cast<CircleCollider*>(collider1);
				BoxCollider* boxCol = static_cast<BoxCollider*>(collider2);

				//b_colliding = Collider::CheckForCollisionBoxCircleSAT(boxCol, circleCol);
			}
			else if (collider1->GetTypeString() == "BoxCollider" && collider2->GetTypeString() == "CircleCollider")
			{
				BoxCollider* boxCol = static_cast<BoxCollider*>(collider1);
				CircleCollider* circleCol = static_cast<CircleCollider*>(collider2);

				//b_colliding = Collider::CheckForCollisionBoxCircleSAT(boxCol, circleCol);
			}
			else if (collider1->GetTypeString() == "CircleCollider" && collider2->GetTypeString() == "CircleCollider")
			{
				b_colliding = true;
			}
		}


		if (b_colliding)
		{			
			collider1->SetColliding(true);
			collider2->SetColliding(true);
			bool b_collider1Added2 = collider1->AddCollidingObject(collider2);
			bool b_collider2Added1 = collider2->AddCollidingObject(collider1);
			Transform* transform1 = collider1->GetParent()->GetTransform();
			Transform* transform2 = collider2->GetParent()->GetTransform();
			RigidBody* rb1 = collider1->GetParent()->GetRigidBody();
			RigidBody* rb2 = collider2->GetParent()->GetRigidBody();
			Vector2 v1Initial = rb1->GetVelocity();
			Vector2 v2Initial = rb2->GetVelocity();
			Vector2 v1Final = Vector2();
			Vector2 v2Final = Vector2();
			float m1 = rb1->GetMass();
			float m2 = rb2->GetMass();
			v1Final = (v1Initial * ((m1 - m2) / (m1 + m2))) + (v2Initial * 2 * (m2 / (m1 + m2)));
			v2Final = (v1Initial * 2 * (m1 / (m1 + m2))) - (v2Initial * ((m1 - m2) / (m1 + m2)));			

			if (!rb1->IsStatic())
			{
				float distribution = 0.5;
				if (rb2->IsStatic())
				{
					distribution = 1;
				}

				Vector2 difference = transform1->GetTruePosition() - transform2->GetTruePosition();
				if (difference.Dot(collisionNormal) > 0)
				{
					transform1->Move(collisionNormal * depth * distribution);
				}
				else
				{
					transform1->Move(collisionNormal * depth * -distribution);
				}
			}

			if (!rb2->IsStatic())
			{
				float distribution = 0.5;
				if (rb1->IsStatic())
				{
					distribution = 1;
				}

				Vector2 difference = transform2->GetTruePosition() - transform1->GetTruePosition();
				if (difference.Dot(collisionNormal) > 0)
				{
					transform2->Move(collisionNormal * depth * distribution);
				}
				else
				{
					transform2->Move(collisionNormal * depth * -distribution);
				}
			}
		
			if (b_collider1Added2 && box1HitPositions.size())
			{			
				for (Vector2 box1Hit : box1HitPositions)
				{					
					Vector2 impactToCenter = (transform1->GetTruePosition() - box1Hit);
					Vector2 movingForce = v1Final.ProjectedOnto(impactToCenter);
					float rotatingForce = (v1Final - movingForce).GetMagnitude();
					float direction = 1;
					if (v1Final.CrossKResult(impactToCenter) < 0)
					{
						direction = -1;
					}

					//rb1->AddForce(movingForce, movingForce.GetMagnitude());
					if (rotatingForce > 0.001)
					{
						//rb1->AddTorque(rotatingForce, direction);
					}					
				}
			}

			if (b_collider2Added1 && box2HitPositions.size())
			{			
				for (Vector2 box2Hit : box2HitPositions)
				{
					Vector2 impactToCenter = (transform2->GetTruePosition() - box2Hit);
					Vector2 movingForce = v2Final.ProjectedOnto(impactToCenter);
					float rotatingForce = (v2Final - movingForce).GetMagnitude();
					float direction = 1;
					if (v2Final.CrossKResult(impactToCenter) < 0)
					{
						direction = -1;
					}

					//rb2->AddForce(movingForce, movingForce.GetMagnitude());
					if (rotatingForce > 0.001)
					{
						//rb2->AddTorque(rotatingForce, direction);
					}
				}
			}
		}

		// For Collider events - Fire OnActiveCollision while there is a collision happening
		if (collider1->GetType() == ComponentTypes::T_BoxCollider)
		{
			CallLuaCollisionFunction(collider1->GetParent(), collider2, LuaEventFunction::OnBoxCollision);
		}
		else if (collider1->GetType() == ComponentTypes::T_CircleCollider)
		{
			CallLuaCollisionFunction(collider1->GetParent(), collider2, LuaEventFunction::OnCircleCollision);
		}
		// For Collider events - Fire OnActiveCollision while there is a collision happening
		if (collider2->GetType() == ComponentTypes::T_BoxCollider)
		{
			CallLuaCollisionFunction(collider2->GetParent(), collider1, LuaEventFunction::OnBoxCollision);
		}
		else if (collider2->GetType() == ComponentTypes::T_CircleCollider)
		{
			CallLuaCollisionFunction(collider2->GetParent(), collider1, LuaEventFunction::OnCircleCollision);
		}

		return b_colliding;
	}

	double Collider::GetDistanceToLine(Vector2 starting, Vector2 ending, Vector2 point)
	{
		Vector2 insideVector = ending - starting;
		Vector2 testVector = point - starting;
		double dotOverDot = ((double)testVector.Dot(insideVector)) / ((double)insideVector.Dot(insideVector));
		Vector2 projectedVector = insideVector * dotOverDot;

		double distance = (double)(projectedVector - testVector).GetMagnitude();
		if (distance < 0.001)
		{
			distance = 0.001;
		}

		return distance;
	}

	bool Collider::IsPointProjectedInside(Vector2 starting, Vector2 ending, Vector2 point)
	{
		Vector2 insideVector = ending - starting;
		Vector2 testVector = point - starting;
		float dotOverDot = (testVector.Dot(insideVector)) / (insideVector.Dot(insideVector));
		Vector2 projectedVector = insideVector * dotOverDot;
		bool b_projectedInside = (projectedVector.GetMagnitude() <= insideVector.GetMagnitude() && (projectedVector.x * insideVector.x >= 0) && (projectedVector.y * insideVector.y >= 0));

		return b_projectedInside;
	}

	bool Collider::CheckForCollisionBoxBoxSAT(BoxCollider* boxCol1, BoxCollider* boxCol2, std::vector<Vector2>& box1HitPositions, std::vector<Vector2>& box2HitPositions, Vector2& collisionNormal, float& depth)
	{
		std::vector<int> box1CornersInsideBox2 = std::vector<int>();
		std::vector<int> box2CornersInsideBox1 = std::vector<int>();
		Vector2* box1Corners = boxCol1->GetNextCorners();
		Vector2* box2Corners = boxCol2->GetNextCorners();


		for (int i = 0; i < 4; i++)
		{
			if (IsPointProjectedInside(box1Corners[0], box1Corners[1], box2Corners[i]))
			{
				box2CornersInsideBox1.push_back(i);
			}
		}
		if (box2CornersInsideBox1.size())
		{
			for (int i = 0; i < 4; i++)
			{
				if (IsPointProjectedInside(box1Corners[1], box1Corners[2], box2Corners[i]))
				{
					for (int savedIndex : box2CornersInsideBox1)
					{
						if (i == savedIndex)
						{
							box2HitPositions.push_back(box2Corners[i]);
						}
					}
				}
			}
			if (box2HitPositions.size() == 0)
			{
				boxCol2->m_b_cornerCollided = false;
			}
			else if (box2HitPositions.size() == 1)
			{
				boxCol2->m_b_cornerCollided = true;
				// Test each point that we know is inside the other collider to see what edge it is closest to, then find the normal using that vector.
				int indexOfClosestEdge = 0;
				int nextIndex = 0;
				for (int i = 0; i < 4; i++)
				{
					int nextCornerIndex = i + 1;
					if (i == 3)
					{
						nextCornerIndex = 0;
					}

					float distance = GetDistanceToLine(box1Corners[i], box1Corners[nextCornerIndex], box2HitPositions.back());

					if (distance < depth)
					{						
						depth = distance;
						indexOfClosestEdge = i;
						nextIndex = nextCornerIndex;
					}
				}

				if (depth > 1)
				{
					depth = 1;
				}
				Vector2 parallelToCollision = box1Corners[indexOfClosestEdge] - box1Corners[nextIndex];
				collisionNormal = parallelToCollision.Rotate(90).Normalize();
				return true;
			}
			else if (box2HitPositions.size() == 2)
			{
				boxCol2->m_b_cornerCollided = false;
				boxCol2->m_b_sideCollided = true;
				std::vector<double> point1Distances = std::vector<double>();
				std::vector<double> point2Distances = std::vector<double>();
				double closestDistance = 100;
				double smallestDistanceDifference = 0.01;

				for (int i = 0; i < 4; i++)
				{
					int nextCornerIndex = i + 1;
					if (i == 3)
					{
						nextCornerIndex = 0;
					}

					point1Distances.push_back(GetDistanceToLine(box1Corners[i], box1Corners[nextCornerIndex], box2HitPositions[0]));
					point2Distances.push_back(GetDistanceToLine(box1Corners[i], box1Corners[nextCornerIndex], box2HitPositions[1]));
				}

				// if 2 points are colliding, it's likely they are close to the same distance away from the line of collision
				for (double distance1 : point1Distances)
				{
					for (double distance2 : point2Distances)
					{
						double difference = distance1 - distance2;

						if (difference < 0)
						{
							difference *= -1;
						}

						if (difference <= smallestDistanceDifference && distance1 < closestDistance)
						{
							smallestDistanceDifference = difference;
							closestDistance = distance1;
						}
					}
				}

				depth = closestDistance;
				Vector2 parallelToCollision = box2HitPositions[0] - box2HitPositions[1];
				collisionNormal = parallelToCollision.Rotate(90).Normalize();
				if (depth > 1)
				{
					depth = 1;
				}
				return true;
			}
		}
		
	

		for (int i = 0; i < 4; i++)
		{
			if (IsPointProjectedInside(box2Corners[0], box2Corners[1], box1Corners[i]))
			{
				box1CornersInsideBox2.push_back(i);
			}
		}
		if (box1CornersInsideBox2.size())
		{
			for (int i = 0; i < 4; i++)
			{
				if (IsPointProjectedInside(box2Corners[1], box2Corners[2], box1Corners[i]))
				{
					for (int savedIndex : box1CornersInsideBox2)
					{
						if (i == savedIndex)
						{
							box1HitPositions.push_back(box1Corners[i]);
						}
					}
				}
			}
			if (box1HitPositions.size() == 0)
			{
				boxCol1->m_b_cornerCollided = false;
			}
			if (box1HitPositions.size() == 1)
			{
				boxCol1->m_b_cornerCollided = true;
				// Test each point that we know is inside the other collider to see what edge it is closest to, then find the normal using that vector.
				int indexOfClosestEdge = 0;
				int nextIndex = 0;
				for (int i = 0; i < 4; i++)
				{
					int nextCornerIndex = i + 1;
					if (i == 3)
					{
						nextCornerIndex = 0;
					}

					float distance = GetDistanceToLine(box2Corners[i], box2Corners[nextCornerIndex], box1HitPositions.back());

					if (distance <= depth)
					{
						depth = distance;
						indexOfClosestEdge = i;
						nextIndex = nextCornerIndex;
					}
				}

				if (depth > 1)
				{
					depth = 1;
				}
				Vector2 parallelToCollision = box2Corners[indexOfClosestEdge] - box2Corners[nextIndex];
				collisionNormal = parallelToCollision.Rotate(90).Normalize();
			}
			else if (box1HitPositions.size() == 2)
			{
				boxCol1->m_b_cornerCollided = false;
				boxCol1->m_b_sideCollided = true;
				std::vector<double> point1Distances = std::vector<double>();
				std::vector<double> point2Distances = std::vector<double>();
				double closestDistance = 100;
				double smallestDistanceDifference = 0.01;

				for (int i = 0; i < 4; i++)
				{
					int nextCornerIndex = i + 1;
					if (i == 3)
					{
						nextCornerIndex = 0;
					}

					point1Distances.push_back(GetDistanceToLine(box2Corners[i], box2Corners[nextCornerIndex], box1HitPositions[0]));
					point2Distances.push_back(GetDistanceToLine(box2Corners[i], box2Corners[nextCornerIndex], box1HitPositions[1]));
				}

				// if 2 points are colliding, it's likely they are close to the same distance away from the line of collision
				for (double distance1 : point1Distances)
				{
					for (double distance2 : point2Distances)
					{
						double difference = distance1 - distance2;

						if (difference < 0)
						{
							difference *= -1;
						}

						if (difference <= smallestDistanceDifference && distance1 < closestDistance)
						{
							smallestDistanceDifference = difference;
							closestDistance = distance1;
						}
					}
				}

				depth = closestDistance;
				if (depth > 1)
				{
					depth = 1;
				}
				Vector2 parallelToCollision = box1HitPositions[0] - box1HitPositions[1];
				collisionNormal = parallelToCollision.Rotate(90).Normalize();
			}
		}		

		return (box1HitPositions.size() || box2HitPositions.size());
	}

	bool Collider::CheckForCollisionBoxRayCastSAT(BoxCollider* boxCol, RayCast* rayCast)
	{
		bool b_colliding = true;
		Vector2* boxCorners = boxCol->GetCorners();
		Vector2 castPoint = rayCast->GetPoint();

		if (!IsPointProjectedInside(boxCorners[0], boxCorners[1], castPoint))
		{
			b_colliding = false;
		}
		if (!IsPointProjectedInside(boxCorners[1], boxCorners[2], castPoint))
		{
			b_colliding = false;
		}

		if (b_colliding)
		{
			rayCast->OnHit(boxCol);
		}

		return b_colliding;
	}

	bool Collider::CheckForCollisionBoxCircleSAT(BoxCollider* boxCol, CircleCollider* circleCol, std::vector<Vector2>& boxHitPositions, std::vector<Vector2>& circleHitPositions)
	{
		bool b_colliding = true;
		Vector2* boxCorners = boxCol->GetCorners();		

		// Do this later
		/*if (!IsPointProjectedInside(boxCorners[0], boxCorners[1], castPoint))
		{
			b_colliding = false;
		}
		if (!IsPointProjectedInside(boxCorners[1], boxCorners[2], castPoint))
		{
			b_colliding = false;
		}

		if (b_colliding)
		{
			rayCast->OnHit(boxCol);
		}*/

		return b_colliding;
	}

	bool Collider::IsColliding()
	{
		return m_b_isColliding;
	}

	void Collider::SetColliding(bool b_isColliding)
	{
		m_b_isColliding = b_isColliding;
	}

	void Collider::UpdatePreviousPosition()
	{
		Transform* transform = GetParent()->GetTransform();
		m_previousPosition = transform->GetTruePosition();
	}

	Vector2 Collider::GetPreviousPosition()
	{
		return m_previousPosition;
	}

	void Collider::SetPreviousPosition(Vector2 prevPos)
	{
		m_previousPosition = prevPos;
	}

	Vector2 Collider::GetPreviousCenterPoint()
	{
		return m_previousCenterPoint;
	}

	void Collider::SetPreviousCenterPoint(Vector2 centerPoint)
	{
		m_previousCenterPoint = centerPoint;
	}

	float Collider::GetPreviousGridStep()
	{
		return m_previousGridStep;
	}

	void Collider::SetPreviousGridStep(float prevGridStep)
	{
		m_previousGridStep = prevGridStep;
	}

	bool Collider::HasMoved()
	{
		Transform* transform = GetParent()->GetTransform();
		Vector2 position = transform->GetPosition();

		return (m_previousPosition.x != position.x || m_previousPosition.y != position.y);
	}

	// Returns true if successfully added for the first time
	bool Collider::AddCollidingObject(Collider* collidedWith)
	{		
		for (GameObject* object : m_collidingObjects)
		{	
			if (object->GetID() == collidedWith->GetParent()->GetID())
			{
				return false;
			}
		}		
		m_collidingObjects.push_back(collidedWith->GetParent());

		for (GameObject *object : m_collidingLastFrame)
		{
			// Leave function if the object has already fired OnCollisionEnter() (in the lastframe);
			if (object->GetID() == collidedWith->GetParent()->GetID())
			{
				return false;
			}
		}

		// else fire it now. (upon initially adding the object to m_collidingObjects for the first time)
		if (static_cast<Component*>(this)->GetType() == ComponentTypes::T_BoxCollider)
		{			
			CallLuaCollisionFunction(GetParent(), collidedWith, LuaEventFunction::OnBoxCollisionEnter);
		}
		if (static_cast<Component*>(this)->GetType() == ComponentTypes::T_CircleCollider)
		{			
			CallLuaCollisionFunction(GetParent(), collidedWith, LuaEventFunction::OnCircleCollisionEnter);
		}

		return true;
	}

	std::vector<GameObject*> Collider::GetCollidingObjects()
	{
		return m_collidingObjects;
	}

	void Collider::ClearCollidingObjects()
	{
		// Check which objects have left collision state since last frame
		for (GameObject *collidedLastFrame : m_collidingLastFrame)
		{		
			bool b_objectStillColliding = false;

			for (GameObject *collidedThisFrame : m_collidingObjects)
			{
				if (collidedLastFrame->GetID() == collidedThisFrame->GetID())
				{
					b_objectStillColliding = true;
				}
			}

			// Fire OnLeave if not colliding
			if (!b_objectStillColliding)
			{
				for (BoxCollider* boxCollider : collidedLastFrame->GetBoxColliders())
				{
					CallLuaCollisionFunction(GetParent(), boxCollider, LuaEventFunction::OnBoxCollisionLeave);
				}
				for (CircleCollider* circleCollider : collidedLastFrame->GetCircleColliders())
				{
					CallLuaCollisionFunction(GetParent(), circleCollider, LuaEventFunction::OnCircleCollisionLeave);
				}
			}
		}

		// Save colliding objects for next frame
		m_collidingLastFrame = m_collidingObjects;
		m_collidingObjects.clear();
	}

	void Collider::SetActiveOffset(Vector2 offset)
	{
		m_activeOffset = offset;
	}

	void Collider::SetActiveLayer(int layer)
	{
		if (layer >= 0)
		{
			m_activeLayer = layer;
		}
		else
		{
			LogError("Collider active layer must be an integer greater than 0.");
		}
	}

	int Collider::GetActiveLayer()
	{
		return m_activeLayer;
	}

	void Collider::SetActiveRadiusScreen(float radius)
	{
		m_activeRadiusScreen = radius;
	}

	Vector2 Collider::GetActiveOffset()
	{
		return m_activeOffset;
	}

	float Collider::GetActiveRadiusScreen()
	{
		return m_activeRadiusScreen;
	}

	void Collider::SetActiveRadiusGrid(float radius)
	{
		m_activeRadiusGrid = radius;
	}

	float Collider::GetActiveRadiusGrid()
	{
		return m_activeRadiusGrid;
	}

	void  Collider::SetShowActiveRadius(bool b_showActiveRadius)
	{
		m_b_showActiveRadius = b_showActiveRadius;
	}

	bool  Collider::GetShowActiveRadius()
	{
		return m_b_showActiveRadius;
	}

	void Collider::ResetCollisions()
	{
		ClearCollidingObjects();
		SetColliding(false);

		m_b_cornerCollided = false;
		m_b_sideCollided = false;
		m_impactNormal = Vector2(0, 0);
	}

	void Collider::SetCenterGrid(Vector2 newCenter)
	{
		m_centerGrid = newCenter;
	}

	Vector2 Collider::GetCenterGrid()
	{
		return m_centerGrid;
	}

	void Collider::SetCenterCoord(Vector2 newCenter)
	{
		m_centerCoord = newCenter;
	}

	Vector2 Collider::GetCenterCoord()
	{
		return m_centerCoord;
	}

	void Collider::SetNextCenterGrid(Vector2 nextCenter)
	{
		m_nextCenterGrid = nextCenter;
	}

	Vector2 Collider::GetNextCenterGrid()
	{
		return m_nextCenterGrid;
	}

	void Collider::SetNextCenterCoord(Vector2 nextCenter)
	{
		m_nextCenterCoord = nextCenter;
	}

	Vector2 Collider::GetNextCenterCoord()
	{
		return m_nextCenterCoord;
	}

	void Collider::SetIsContinuous(bool b_isContinuous)
	{
		m_b_isContinuous = b_isContinuous;
	}

	bool Collider::IsContinuous()
	{
		return m_b_isContinuous;
	}

	void Collider::SetIsSolid(bool b_isSolid)
	{
		m_b_isSolid = b_isSolid;
	}

	bool Collider::IsSolid()
	{
		return m_b_isSolid;
	}

	void Collider::SetRotation(float newRotation)
	{
		m_rotation = newRotation;
	}

	void Collider::UpdateRotation()
	{
		m_rotation = GetParent()->GetTransform()->GetRotation();
	}

	float Collider::GetRotation()
	{
		return GetParent()->GetTransform()->GetRotation();;
	}
}
