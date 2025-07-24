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
	}

	Collider::~Collider()
	{
	}

	bool Collider::CheckForCollision(Collider* collider1, Collider* collider2)
	{
		bool b_colliding = false;
		Vector2 collider1Center = collider1->GetCenterGrid();
		Vector2 collider2Center = collider2->GetCenterGrid();
		float depth = FLT_MAX;
		Vector2 collisionNormal = Vector2(0, 0);
		// Calculate center distance with pythag
		float rise = std::abs(collider1Center.y - collider2Center.y);
		float run = std::abs(collider1Center.x - collider2Center.x);
		float centerDistance = std::sqrt((rise * rise) + (run * run));
		Vector2 difference = Vector2();
		CircleCollider* circleCollider = nullptr;
		float radius = 0;
		Vector2 contactPoint1 = Vector2();
		Vector2 contactPoint2 = Vector2();
		int contactCount = 0;

		// If they are close enough to check for collision ( actually colliding if they are both CircleColliders )
		if (centerDistance < collider1->GetActiveRadiusGrid() + collider2->GetActiveRadiusGrid())
		{
			if (collider1->GetTypeString() == "BoxCollider" && collider2->GetTypeString() == "BoxCollider")
			{
				BoxCollider* boxCol1 = static_cast<BoxCollider*>(collider1);
				BoxCollider* boxCol2 = static_cast<BoxCollider*>(collider2);

				b_colliding = CheckForCollisionBoxBoxSAT(boxCol1, boxCol2, collisionNormal, depth);
				if (b_colliding)
				{
					FindContactPointsPolygonPolygon(boxCol1->GetNextCorners(), boxCol2->GetNextCorners(), contactPoint1, contactPoint2, contactCount);
				}
			}
			else if (collider1->GetTypeString() == "CircleCollider" && collider2->GetTypeString() == "BoxCollider")
			{
				CircleCollider* circleCol = static_cast<CircleCollider*>(collider1);
				BoxCollider* boxCol = static_cast<BoxCollider*>(collider2);

				b_colliding = Collider::CheckForCollisionBoxCircleSAT(boxCol, circleCol, collisionNormal, depth);

				if (b_colliding)
				{
					radius = circleCol->GetActiveRadiusGrid();
					difference = boxCol->GetParent()->GetTransform()->GetTruePosition() - circleCol->GetParent()->GetTransform()->GetTruePosition();
					circleCollider = circleCol;
				}
			}
			else if (collider1->GetTypeString() == "BoxCollider" && collider2->GetTypeString() == "CircleCollider")
			{
				BoxCollider* boxCol = static_cast<BoxCollider*>(collider1);
				CircleCollider* circleCol = static_cast<CircleCollider*>(collider2);

				b_colliding = Collider::CheckForCollisionBoxCircleSAT(boxCol, circleCol, collisionNormal, depth);

				if (b_colliding)
				{
					radius = circleCol->GetActiveRadiusGrid();
					difference = boxCol->GetParent()->GetTransform()->GetTruePosition() - circleCol->GetParent()->GetTransform()->GetTruePosition();
					circleCollider = circleCol;
				}
			}
			else if (collider1->GetTypeString() == "CircleCollider" && collider2->GetTypeString() == "CircleCollider")
			{
				depth = collider1->GetActiveRadiusGrid() + collider2->GetActiveRadiusGrid() - centerDistance;
				collisionNormal = Vector2::Normalize(collider1Center - collider2Center);
				b_colliding = true;

				if (b_colliding)
				{
					radius = collider1->GetActiveRadiusGrid();
					difference = collider2->GetParent()->GetTransform()->GetTruePosition() - collider1->GetParent()->GetTransform()->GetTruePosition();
					circleCollider = static_cast<CircleCollider*>(collider1);
				}
			}
		}

		if (b_colliding && circleCollider != nullptr)
		{
			if (difference.Dot(collisionNormal) > 0)
			{
				contactPoint1 = circleCollider->GetParent()->GetTransform()->GetTruePosition() + (collisionNormal * radius);
			}
			else
			{
				contactPoint1 = circleCollider->GetParent()->GetTransform()->GetTruePosition() - (collisionNormal * radius);
			}

			contactCount = 1;
		}

		if (b_colliding)
		{			
			collider1->SetColliding(true);
			collider2->SetColliding(true);
			collider1->AddCollidingObject(collider2);
			collider2->AddCollidingObject(collider1);
			Transform* transform1 = collider1->GetParent()->GetTransform();
			Transform* transform2 = collider2->GetParent()->GetTransform();
			RigidBody* rb1 = collider1->GetParent()->GetRigidBody();
			RigidBody* rb2 = collider2->GetParent()->GetRigidBody();
			Vector2 v1Initial = rb1->GetVelocity();
			Vector2 v2Initial = rb2->GetVelocity();
			Vector2 v1Final = Vector2();
			Vector2 v2Final = Vector2();
			float angularV1Initial = rb1->GetAngularVelocity();
			float angularV2Initial = rb2->GetAngularVelocity();
			float angularV1Final = 0;
			float angularV2Final = 0;
			float I1Inv = rb1->GetIInv();
			float I2Inv = rb2->GetIInv();
			Vector2 r1Perp = Vector2();
			Vector2 r2Perp = Vector2();
			Vector2 angularToLinear1 = Vector2();
			Vector2 angularToLinear2 = Vector2();

			if (contactCount == 2)
			{
				r1Perp = Vector2::Rotate(collisionNormal, 90);
				r2Perp = Vector2::Rotate(collisionNormal, 90);
			}
			else
			{
				r1Perp = Vector2::Rotate(contactPoint1 - rb1->GetNextPosition(), 90);
				r2Perp = Vector2::Rotate(contactPoint1 - rb2->GetNextPosition(), 90);
			}
			angularToLinear1 = r1Perp * angularV1Initial;
			angularToLinear2 = r2Perp * angularV2Initial;
			float m1 = rb1->GetMass();
			float m2 = rb2->GetMass();
			float m1Inv = rb1->GetMassInv();
			float m2Inv = rb2->GetMassInv();
			float e1 = rb1->GetRestitution();
			float e2 = rb2->GetRestitution();
			Vector2 relativeVelocity = (v1Initial + angularToLinear1) - (v2Initial + angularToLinear2);
			float contactVelocityMagnitude = relativeVelocity.Dot(collisionNormal);

			float numerator = -(1 + (e1 * e2)) * contactVelocityMagnitude;
			float denominator = m1Inv + m2Inv + (r1Perp.Dot(collisionNormal) * r1Perp.Dot(collisionNormal) * I1Inv) + (r2Perp.Dot(collisionNormal) * r2Perp.Dot(collisionNormal) * I2Inv);

			float impulseMagnitude = numerator / denominator;
			if (impulseMagnitude < 0)
			{
				impulseMagnitude *= -1;
			}

			Vector2 impulse = collisionNormal * impulseMagnitude;

			if (!rb1->IsStatic())
			{
				float distribution = m2 / (m1 + m2);
				if (rb2->IsStatic())
				{
					distribution = 1;
				}

				Vector2 difference = transform1->GetTruePosition() - transform2->GetTruePosition();
				if (difference.Dot(collisionNormal) > 0)
				{
					transform1->Move(collisionNormal * depth * distribution);
					v1Final = v1Initial + (impulse * m1Inv);
					angularV1Final = angularV1Initial + (r1Perp.CrossKResult(impulse) * I1Inv);
				}
				else
				{
					transform1->Move(collisionNormal * depth * -distribution);
					v1Final = v1Initial - (impulse * m1Inv);
					angularV1Final = angularV1Initial + (r1Perp.CrossKResult(impulse) * I1Inv);
				}

				rb1->SetVelocity(v1Final);
				rb1->SetAngularVelocity(angularV1Final);
			}

			if (!rb2->IsStatic())
			{
				float distribution = m1 / (m1 + m2);
				if (rb1->IsStatic())
				{
					distribution = 1;
				}

				Vector2 difference = transform2->GetTruePosition() - transform1->GetTruePosition();
				if (difference.Dot(collisionNormal) > 0)
				{
					transform2->Move(collisionNormal * depth * distribution);
					v2Final = v2Initial + (impulse * m2Inv);
					angularV2Final = angularV2Initial + (r2Perp.CrossKResult(impulse) * I2Inv);
				}
				else
				{
					transform2->Move(collisionNormal * depth * -distribution);
					v2Final = v2Initial - (impulse * m2Inv);
					angularV2Final = angularV2Initial + (r2Perp.CrossKResult(impulse) * I2Inv);
				}

				rb2->SetVelocity(v2Final);
				rb2->SetAngularVelocity(angularV2Final);
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

	float Collider::GetDistanceToLine(Vector2 starting, Vector2 ending, Vector2 point)
	{
		Vector2 insideVector = ending - starting;
		Vector2 testVector = point - starting;
		Vector2 projectedVector = testVector.ProjectedOnto(insideVector);		

		return (projectedVector - testVector).GetMagnitude();
	}

	void Collider::FindContactPointsPolygonPolygon(std::vector<Vector2> vertices1, std::vector<Vector2> vertices2, Vector2& contact1, Vector2& contact2, int& contactCount)
	{		
		float shortestDistanceFrom1 = FLT_MAX;
		float shortestDistanceFrom2 = FLT_MAX;

		for (int i = 0; i < vertices1.size(); i++)
		{
			for (int j = 0; j < vertices2.size(); j++)
			{
				float distance = GetDistanceToLine(vertices1[i], vertices1[fmod(i + 1, vertices1.size())], vertices2[j]);

				if (distance < shortestDistanceFrom1)
				{
					shortestDistanceFrom1 = distance;
					contact1 = vertices2[j];
					contactCount = 1;
				}
				else if (distance == shortestDistanceFrom1)
				{
					contact2 = vertices2[j];
					contactCount = 2;
				}
			}
		}
		for (int i = 0; i < vertices2.size(); i++)
		{
			for (int j = 0; j < vertices1.size(); j++)
			{
				float distance = GetDistanceToLine(vertices2[i], vertices2[fmod(i + 1, vertices2.size())], vertices1[j]);

				if (distance < shortestDistanceFrom2)
				{
					shortestDistanceFrom2 = distance;
					contact1 = vertices1[j];
					contactCount = 1;
				}
				else if (distance == shortestDistanceFrom2)
				{
					contact2 = vertices1[j];
					contactCount = 2;
				}
			}
		}
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

	bool Collider::CheckForCollisionBoxBoxSAT(BoxCollider* boxCol1, BoxCollider* boxCol2, Vector2& collisionNormal, float& depth)
	{
		std::vector<Vector2> box1Vertices = boxCol1->GetNextCorners();
		std::vector<Vector2> box2Vertices = boxCol2->GetNextCorners();
		float minBox1 = FLT_MAX;
		float maxBox1 = -FLT_MAX;
		float minBox2 = FLT_MAX;
		float maxBox2 = -FLT_MAX;
		Vector2 axis = Vector2();
		float overlapDepth = 0;

		for (int i = 0; i < box1Vertices.size(); i++)
		{
			minBox1 = FLT_MAX;
			maxBox1 = -FLT_MAX;
			minBox2 = FLT_MAX;
			maxBox2 = -FLT_MAX;
			Vector2 start = box1Vertices[i];
			Vector2 end = box1Vertices[fmod((i + 1), box1Vertices.size())];
			axis = end - start;

			ProjectVerticesOntoAxis(box1Vertices, axis, minBox1, maxBox1);
			ProjectVerticesOntoAxis(box2Vertices, axis, minBox2, maxBox2);

			if (minBox1 >= maxBox2 || minBox2 >= maxBox1)
			{
				return false;
			}

			overlapDepth = Min(maxBox2 - minBox1, maxBox1 - minBox2);

			if (overlapDepth < depth)
			{
				depth = overlapDepth;
				collisionNormal = Vector2::Normalize(axis);
			}
		}

		for (int i = 0; i < box2Vertices.size(); i++)
		{
			minBox1 = FLT_MAX;
			maxBox1 = -FLT_MAX;
			minBox2 = FLT_MAX;
			maxBox2 = -FLT_MAX;
			Vector2 start = box2Vertices[i];
			Vector2 end = box2Vertices[fmod((i + 1), box2Vertices.size())];
			axis = end - start;

			ProjectVerticesOntoAxis(box1Vertices, axis, minBox1, maxBox1);
			ProjectVerticesOntoAxis(box2Vertices, axis, minBox2, maxBox2);

			if (minBox1 >= maxBox2 || minBox2 >= maxBox1)
			{
				return false;
			}

			overlapDepth = Min(maxBox2 - minBox1, maxBox1 - minBox2);

			if (overlapDepth < depth)
			{
				depth = overlapDepth;
				collisionNormal = Vector2::Normalize(axis);
			}
		}

		return true;
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

	bool Collider::CheckForCollisionBoxCircleSAT(BoxCollider* boxCol, CircleCollider* circleCol, Vector2& collisionNormal, float& depth)
	{		
		std::vector<Vector2> vertices = boxCol->GetNextCorners();		
		Vector2 circlePos = circleCol->GetParent()->GetTransform()->GetTruePosition();
		float minBox = FLT_MAX;
		float maxBox = -FLT_MAX;
		float minCircle = FLT_MAX;
		float maxCircle = -FLT_MAX;
		Vector2 axis = Vector2();
		float overlapDepth = 0;		

		for (int i = 0; i < vertices.size(); i++)
		{
			minBox = FLT_MAX;
			maxBox = -FLT_MAX;
			Vector2 start = vertices[i];
			Vector2 end = vertices[fmod((i + 1), vertices.size())];
			axis = end - start;

			ProjectVerticesOntoAxis(vertices, axis, minBox, maxBox);
			ProjectCircleOntoAxis(circleCol, axis, minCircle, maxCircle);

			if (minBox >= maxCircle || minCircle >= maxBox)
			{
				return false;
			}

			overlapDepth = Min(maxCircle - minBox, maxBox - minCircle);

			if (overlapDepth < depth)
			{
				depth = overlapDepth;
				collisionNormal = Vector2::Normalize(axis);
			}
		}

		minBox = FLT_MAX;
		maxBox = -FLT_MAX;
		int closestPointIndex = FindClosestVertexToPoint(vertices, circlePos);
		Vector2 closesPoint = vertices[closestPointIndex];
		axis = closesPoint - circlePos;

		ProjectVerticesOntoAxis(vertices, axis, minBox, maxBox);
		ProjectCircleOntoAxis(circleCol, axis, minCircle, maxCircle);

		if (minBox >= maxCircle || minCircle >= maxBox)
		{
			return false;
		}

		overlapDepth = Min(maxCircle - minBox, maxBox - minCircle);

		if (overlapDepth < depth)
		{
			depth = overlapDepth;
			collisionNormal = Vector2::Normalize(axis);
		}

		return true;
	}

	int Collider::FindClosestVertexToPoint(std::vector<Vector2> vertices, Vector2 point)
	{
		float min = FLT_MAX;
		int index = -1;
		for (int i = 0; i < vertices.size(); i++)
		{
			float distance = (vertices[i] - point).GetMagnitude();
			if (distance < min)
			{
				min = distance;
				index = i;
			}
		}

		return index;
	}

	void Collider::ProjectCircleOntoAxis(CircleCollider* circle, Vector2 axis, float& min, float& max)
	{
		Vector2 leftEdge = circle->GetParent()->GetTransform()->GetTruePosition() + (Vector2::Normalize(axis) * -circle->GetActiveRadiusGrid());
		Vector2 rightEdge = circle->GetParent()->GetTransform()->GetTruePosition() + (Vector2::Normalize(axis) * circle->GetActiveRadiusGrid());
		min = leftEdge.Dot(Vector2::Normalize(axis));
		max = rightEdge.Dot(Vector2::Normalize(axis));

		if (min > max)
		{
			float temp = min;
			min = max;
			max = temp;
		}
	}

	void Collider::ProjectVerticesOntoAxis(std::vector<Vector2> vertices, Vector2 axis, float& min, float& max)
	{		
		for (Vector2 vertex : vertices)
		{
			float projection = vertex.Dot(Vector2::Normalize(axis));
			if (projection < min)
			{
				min = projection;
			}
			if (projection > max)
			{
				max = projection;
			}
		}
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
