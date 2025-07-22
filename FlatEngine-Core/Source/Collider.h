#pragma once
#include "Component.h"
#include "Vector2.h"

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
	public:
		Collider(long myID = -1, long parentID = -1);
		~Collider();

		static bool CheckForCollisionBoxBoxSAT(BoxCollider* boxCol1, BoxCollider* boxCol2, std::vector<Vector2>& box1HitPositions, std::vector<Vector2>& box2HitPositions);
		static bool CheckForCollisionBoxRayCastSAT(BoxCollider* boxCol, RayCast* rayCast);
		static bool CheckForCollisionBoxCircleSAT(BoxCollider* boxCol, CircleCollider* circleCol, std::vector<Vector2>& boxHitPositions, std::vector<Vector2>& circleHitPositions);
		static bool CheckForCollision(Collider* collider1, Collider* collider2);
		static bool IsPointProjectedInside(Vector2 starting, Vector2 ending, Vector2 point);

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

		float m_collisionDepth;
		bool m_b_cornerCollided;
		Vector2 m_impactNormal;
		bool m_b_sideCollided;

	private:
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