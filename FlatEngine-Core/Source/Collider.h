#pragma once
#include "Component.h"
#include "Vector2.h"
#include "Physics.h"

#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	class RayCast;
	class BoxCollider;
	class CircleCollider;
	class GameObject;

	class Collider : public Component
	{
		friend class RigidBody;
		friend class BoxCollider;
		friend class CircleCollider;
	public:
		Collider(long myID = -1, long parentID = -1);
		~Collider();

		static bool CheckForCollisionBoxBoxSAT(BoxCollider* boxCol1, BoxCollider* boxCol2, Vector2& collisionNormal, float& depth);
		static bool CheckForCollisionBoxRayCastSAT(BoxCollider* boxCol, RayCast* rayCast);
		static bool CheckForCollisionBoxCircleSAT(BoxCollider* boxCol, CircleCollider* circleCol, Vector2& collisionNormal, float& depth);
		static int FindClosestVertexToPoint(std::vector<Vector2> vertices, Vector2 point);
		static void ProjectCircleOntoAxis(CircleCollider* circle, Vector2 axis, float& min, float& max);
		static void ProjectVerticesOntoAxis(std::vector<Vector2> vertices, Vector2 axis, float& min, float& max);
		static bool CheckForCollision(Collider* collider1, Collider* collider2);
		static bool IsPointProjectedInside(Vector2 starting, Vector2 ending, Vector2 point);
		static float GetDistanceToLine(Vector2 starting, Vector2 ending, Vector2 point);
		static void FindContactPointsPolygonPolygon(std::vector<Vector2> vertices1, std::vector<Vector2> vertices2, Vector2& contact1, Vector2& contact2, int& contactCount);		
		virtual void RecalculateBounds() {};
		virtual void ResetCollisions();		

		bool AddCollidingObject(Collider* collidedWith);		
		bool IsColliding();
		void SetColliding(bool b_isColliding);
		void UpdatePreviousPosition();
		Vector2 GetPreviousPosition();
		void SetPreviousPosition(Vector2 prevPos);
		Vector2 GetPreviousCenterPoint();
		void SetPreviousCenterPoint(Vector2 centerPoint);
		float GetPreviousGridStep();
		void SetPreviousGridStep(float prevGridStep);
		bool HasMoved();
		std::vector<GameObject*> GetCollidingObjects();
		void ClearCollidingObjects();
		void SetActiveOffset(Vector2 offset);
		Vector2 GetActiveOffset();
		void SetActiveLayer(int layer);
		int GetActiveLayer();
		void SetActiveRadiusScreen(float radius);
		float GetActiveRadiusScreen();
		void SetActiveRadiusGrid(float radius);
		float GetActiveRadiusGrid();
		void SetShowActiveRadius(bool b_showActiveRadius);
		bool GetShowActiveRadius();
		void SetCenterGrid(Vector2 newCenter);
		Vector2 GetCenterGrid();
		void SetCenterCoord(Vector2 newCenter);
		Vector2 GetCenterCoord();
		void SetNextCenterGrid(Vector2 nextCenter);
		Vector2 GetNextCenterGrid();
		void SetNextCenterCoord(Vector2 nextCenter);
		Vector2 GetNextCenterCoord();
		void SetIsContinuous(bool b_continuous);
		bool IsContinuous();
		void SetIsSolid(bool b_isSolid);
		bool IsSolid();
		void SetRotation(float rotation);
		void UpdateRotation();
		float GetRotation();

		// Box2D
		void SetB2Position(Vector2 position);
		Vector2 GetB2Position();
		void SetB2Rotation(float rotation);
		float GetB2Rotation();
		b2BodyId GetBodyID();		

	private:
		b2BodyId m_bodyID;
		Physics::BodyProps m_bodyProps;
		 

		std::vector<GameObject*> m_collidingObjects;
		std::vector<GameObject*> m_collidingLastFrame;
		Vector2 m_activeOffset;
		Vector2 m_previousPosition;
		Vector2 m_previousCenterPoint;
		float m_previousGridStep;
		Vector2 m_centerGrid;
		Vector2 m_nextCenterGrid;
		Vector2 m_centerCoord;
		Vector2 m_nextCenterCoord;
		float m_rotation;
		float m_activeRadiusScreen;
		float m_activeRadiusGrid;
		int m_activeLayer;
		bool m_b_isColliding;
		bool m_b_isContinuous;
		bool m_b_isSolid;
		bool m_b_showActiveRadius;
	};
}