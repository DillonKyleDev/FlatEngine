#include "physics/Collision2D.h"
#include "tools/Numbers.h"
#include "tools/Vector2.h"

#include <vector>


namespace FlatEngine
{
    namespace Collision2D
	{
		// For Mouse button collisions - Vector4 objectA(top, right, bottom, left), Vector4 objectB(top, right, bottom, left)
		bool AreCollidingViewport(Vector4 ObjectA, Vector4 ObjectB)
		{
			float A_TopEdge = ObjectA.z;
			float A_RightEdge = ObjectA.y;
			float A_BottomEdge = ObjectA.x;
			float A_LeftEdge = ObjectA.w;

			float B_TopEdge = ObjectB.x;
			float B_RightEdge = ObjectB.y;
			float B_BottomEdge = ObjectB.z;
			float B_LeftEdge = ObjectB.w;

			return (A_LeftEdge < B_RightEdge && A_RightEdge > B_LeftEdge && A_TopEdge > B_BottomEdge && A_BottomEdge < B_TopEdge);
		}

		int FindClosestVertexToPoint(std::vector<Vector2> vertices, Vector2 point)
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

		bool IsPointProjectedInside(Vector2 starting, Vector2 ending, Vector2 point)
		{
			Vector2 insideVector = ending - starting;
			Vector2 testVector = point - starting;
			float dotOverDot = (testVector.Dot(insideVector)) / (insideVector.Dot(insideVector));
			Vector2 projectedVector = insideVector * dotOverDot;
			bool b_projectedInside = (projectedVector.GetMagnitude() <= insideVector.GetMagnitude() && (projectedVector.x * insideVector.x >= 0) && (projectedVector.y * insideVector.y >= 0));

			return b_projectedInside;
		}
		
		void ProjectVerticesOntoAxis(std::vector<Vector2> vertices, Vector2 axis, float& min, float& max)
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

		// void ProjectCircleOntoAxis(CircleCollider* circle, Vector2 axis, float& min, float& max)
		// {
		// 	Vector2 leftEdge = circle->GetParent()->GetTransform()->GetTruePosition() + (Vector2::Normalize(axis) * -circle->GetActiveRadiusGrid());
		// 	Vector2 rightEdge = circle->GetParent()->GetTransform()->GetTruePosition() + (Vector2::Normalize(axis) * circle->GetActiveRadiusGrid());
		// 	min = leftEdge.Dot(Vector2::Normalize(axis));
		// 	max = rightEdge.Dot(Vector2::Normalize(axis));

		// 	if (min > max)
		// 	{
		// 		float temp = min;
		// 		min = max;
		// 		max = temp;
		// 	}
		// }

		bool CheckForCollisionBoxBoxSAT(std::vector<Vector2> verticesA, std::vector<Vector2> verticesB, Vector2& collisionNormal, float& depth)
		{
			float minBox1 = FLT_MAX;
			float maxBox1 = -FLT_MAX;
			float minBox2 = FLT_MAX;
			float maxBox2 = -FLT_MAX;
			Vector2 axis = Vector2();
			float overlapDepth = 0;

			for (int i = 0; i < verticesA.size(); i++)
			{
				minBox1 = FLT_MAX;
				maxBox1 = -FLT_MAX;
				minBox2 = FLT_MAX;
				maxBox2 = -FLT_MAX;
				Vector2 start = verticesA[i];
				Vector2 end = verticesA[fmod((i + 1), verticesA.size())];
				axis = Vector2::Normalize(end - start);

				ProjectVerticesOntoAxis(verticesA, axis, minBox1, maxBox1);
				ProjectVerticesOntoAxis(verticesB, axis, minBox2, maxBox2);

				if (minBox1 >= maxBox2 || minBox2 >= maxBox1)
				{
					return false;
				}

				overlapDepth = Numbers::Min(maxBox2 - minBox1, maxBox1 - minBox2);

				if (overlapDepth < depth)
				{
					depth = overlapDepth;
					collisionNormal = axis;
				}
			}

			for (int i = 0; i < verticesB.size(); i++)
			{
				minBox1 = FLT_MAX;
				maxBox1 = -FLT_MAX;
				minBox2 = FLT_MAX;
				maxBox2 = -FLT_MAX;
				Vector2 start = verticesB[i];
				Vector2 end = verticesB[fmod((i + 1), verticesB.size())];
				axis = Vector2::Normalize(end - start);

				ProjectVerticesOntoAxis(verticesA, axis, minBox1, maxBox1);
				ProjectVerticesOntoAxis(verticesB, axis, minBox2, maxBox2);

				if (minBox1 >= maxBox2 || minBox2 >= maxBox1)
				{
					return false;
				}

				overlapDepth = Numbers::Min(maxBox2 - minBox1, maxBox1 - minBox2);

				if (overlapDepth < depth)
				{
					depth = overlapDepth;
					collisionNormal = axis;
				}
			}

			return true;
		}

		// bool CheckForCollisionBoxRayCastSAT(std::vector<Vector2> verts, RayCast* rayCast)
		// {
		// 	bool b_colliding = true;
		// 	Vector2 castPoint = rayCast->GetPoint();

		// 	if (!IsPointProjectedInside(verts[0], verts[1], castPoint))
		// 	{
		// 		b_colliding = false;
		// 	}
		// 	if (!IsPointProjectedInside(verts[1], verts[2], castPoint))
		// 	{
		// 		b_colliding = false;
		// 	}

		// 	if (b_colliding)
		// 	{
		// 		rayCast->OnHit(boxCol);
		// 	}

		// 	return b_colliding;
		// }

		// bool CheckForCollisionBoxCircleSAT(std::vector<Vector2> vertices, CircleCollider* circleCol, Vector2& collisionNormal, float& depth)
		// {						
		// 	Vector2 circlePos = circleCol->GetParent()->GetTransform()->GetTruePosition();
		// 	float minBox = FLT_MAX;
		// 	float maxBox = -FLT_MAX;
		// 	float minCircle = FLT_MAX;
		// 	float maxCircle = -FLT_MAX;
		// 	Vector2 axis = Vector2();
		// 	float overlapDepth = 0;		

		// 	for (int i = 0; i < vertices.size(); i++)
		// 	{
		// 		minBox = FLT_MAX;
		// 		maxBox = -FLT_MAX;
		// 		Vector2 start = vertices[i];
		// 		Vector2 end = vertices[fmod((i + 1), vertices.size())];
		// 		axis = Vector2::Normalize(end - start);

		// 		ProjectVerticesOntoAxis(vertices, axis, minBox, maxBox);
		// 		ProjectCircleOntoAxis(circleCol, axis, minCircle, maxCircle);

		// 		if (minBox >= maxCircle || minCircle >= maxBox)
		// 		{
		// 			return false;
		// 		}

		// 		overlapDepth = Numbers::Min(maxCircle - minBox, maxBox - minCircle);

		// 		if (overlapDepth < depth)
		// 		{
		// 			depth = overlapDepth;
		// 			collisionNormal = axis;
		// 		}
		// 	}

		// 	minBox = FLT_MAX;
		// 	maxBox = -FLT_MAX;
		// 	int closestPointIndex = FindClosestVertexToPoint(vertices, circlePos);
		// 	Vector2 closesPoint = vertices[closestPointIndex];
		// 	axis = Vector2::Normalize(closesPoint - circlePos);

		// 	ProjectVerticesOntoAxis(vertices, axis, minBox, maxBox);
		// 	ProjectCircleOntoAxis(circleCol, axis, minCircle, maxCircle);

		// 	if (minBox >= maxCircle || minCircle >= maxBox)
		// 	{
		// 		return false;
		// 	}

		// 	overlapDepth = Numbers::Min(maxCircle - minBox, maxBox - minCircle);

		// 	if (overlapDepth < depth)
		// 	{
		// 		depth = overlapDepth;
		// 		collisionNormal = axis;
		// 	}

		// 	return true;
		// }
	}
}