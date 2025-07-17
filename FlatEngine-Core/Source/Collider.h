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

		static bool CheckForCollisionBoxCircle(BoxCollider* boxCol, CircleCollider* circleCol);
		static bool CheckForCollisionBoxBox(BoxCollider* boxCol1, BoxCollider* boxCol2);
		static bool CheckForCollisionBoxBoxSAT(BoxCollider* boxCol1, BoxCollider* boxCol2);
		static bool CheckForCollisionBoxRayCastSAT(BoxCollider* boxCol, RayCast* rayCast);
		static bool CheckForCollision(Collider* collider1, Collider* collider2);
		static bool IsPointProjectedInside(Vector2 starting, Vector2 ending, Vector2 point, bool& b_sameDirection, bool& b_oppositeDirection);

		virtual void RecalculateBounds() {};
		virtual void ResetCollisions();		

		void AddCollidingObject(Collider* collidedWith);		
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
		void SetIsStatic(bool b_isStatic);
		bool IsStatic();
		void SetIsSolid(bool b_isSolid);
		bool IsSolid();
		void SetRotation(float rotation);
		void UpdateRotation();
		float GetRotation();
		void SetIsComposite(bool b_isComposite);
		bool IsComposite();

		Vector2 m_collidedPosition;
		Vector2 m_leftCollidedPosition;
		Vector2 m_rightCollidedPosition;
		Vector2 m_bottomCollidedPosition;
		Vector2 m_topCollidedPosition;
		Vector2 m_topRightCollidedPosition;
		Vector2 m_bottomRightCollidedPosition;
		Vector2 m_topLeftCollidedPosition;
		Vector2 m_bottomLeftCollidedPosition;

		float m_rightCollision;
		float m_leftCollision;
		float m_bottomCollision;
		float m_topCollision;

		bool m_b_isCollidingRight;
		bool m_b_isCollidingLeft;
		bool m_b_isCollidingBottom;
		bool m_b_isCollidingTop;

		bool m_b_isCollidingTopRight;
		bool m_b_isCollidingTopLeft;
		bool m_b_isCollidingBottomRight;
		bool m_b_isCollidingBottomLeft;

		bool m_b_rightCollisionStatic;
		bool m_b_leftCollisionStatic;
		bool m_b_bottomCollisionStatic;
		bool m_b_topCollisionStatic;
		bool m_b_bottomLeftCollisionStatic;
		bool m_b_bottomRightCollisionStatic;
		//bool _topLeftCollisionStatic;
		//bool _topRightCollisionStatic;

		bool m_b_rightCollisionSolid;
		bool m_b_leftCollisionSolid;
		bool m_b_bottomCollisionSolid;
		bool m_b_topCollisionSolid;
		bool m_b_bottomLeftCollisionSolid;
		bool m_b_bottomRightCollisionSolid;
		bool m_b_topLeftCollisionSolid;
		bool m_b_topRightCollisionSolid;

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
		bool m_b_isStatic;
		bool m_b_isSolid;
		bool m_b_showActiveRadius;
		bool m_b_isComposite;
	};
}