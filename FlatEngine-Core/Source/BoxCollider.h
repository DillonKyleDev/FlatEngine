#pragma once
#include "Component.h"
#include "CircleCollider.h"
#include "FlatEngine.h"
#include "Collider.h"
#include "Vector2.h"
#include "Vector4.h"

#include <string>


namespace FlatEngine
{
	class GameObject;
	class RigidBody;

	class BoxCollider : public Collider
	{
		friend class Collider;
		friend class CircleCollider;

	public:
		BoxCollider(long myID = -1, long parentID = -1);
		~BoxCollider();
		std::string GetData();

		bool IsTileMapCollider();
		void SetTileMapCollider(bool b_tileMapCollider);
		void SetActiveDimensions(float width, float height);
		float GetActiveWidth();
		float GetActiveHeight();
		//Vector4(activeTop, activeRight, activeBottom, activeLeft)
		void SetActiveEdges(Vector4 edges);
		Vector4 GetActiveEdges();
		void UpdateActiveEdges(float gridstep, Vector2 viewportCenter);
		void UpdateNormals(float gridstep, Vector2 viewportCenter);
		void UpdateCorners(float gridstep, Vector2 viewportCenter);
		void UpdateCenter(float gridstep, Vector2 viewportCenter);
		void SetCorners(Vector2 corners[4]);
		void SetNextCorners(Vector2 corners[4]);
		Vector2* GetCorners();
		Vector2* GetNextCorners();
		void SetNormals(Vector2 normals[4]);
		Vector2* GetNormals();
		void RecalculateBounds(float gridstep = 0, Vector2 viewportCenter = Vector2());

	private:		
		bool m_b_isTileMapCollider;
		float m_activeWidth;
		float m_activeHeight;

		// Current
		float m_activeLeft;
		float m_activeRight;
		float m_activeBottom;
		float m_activeTop;

		// Next
		float m_nextActiveLeft;
		float m_nextActiveRight;
		float m_nextActiveBottom;
		float m_nextActiveTop;

		Vector4 m_activeEdges;
		bool m_b_activeEdgesSet;
		Vector2 m_corners[4];
		Vector2 m_nextCorners[4];
		Vector2 m_normals[4];

		float m_previousRotation;
		float m_lastGridStep;
		Vector2 m_lastViewportCenter;
	};
}