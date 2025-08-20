#pragma once
#include "Vector2.h"
#include "Vector4.h"

#include <string>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	class Body;

	class Shape
	{
		
		friend class Box;
		friend class Circle;
		friend class Capsule;
		friend class Polygon;
		friend class Chain;

	public:
		enum ShapeType {
			BS_None,
			BS_Box,
			BS_Circle,
			BS_Capsule,
			BS_Polygon,
			BS_Chain
		};

		struct ShapeProps {
			bool b_enableContactEvents = true;
			bool b_enableSensorEvents = true;
			bool b_isSensor = false;
			ShapeType shape = BS_None;
			Vector2 positionOffset = Vector2(0, 0);
			b2Rot rotationOffset = b2MakeRot(0);
			float restitution = 0.3f;
			float density = 1.0f;
			float friction = 0.3f;
			Vector2 dimensions = Vector2(1.0f, 1.0f);
			float radius = 1.0f;
			float capsuleLength = 4.0f;
			bool b_horizontal = false;
			float cornerRadius = 0.0f;
			std::vector<Vector2> points = std::vector<Vector2>();
			bool b_isLoop = false;
			float tangentSpeed = 0.0f;
			float rollingResistance = 0.0f;
		};

		Shape(Body* parentBody);
		~Shape();
		json GetShapeData();
		ShapeType GetShapeType();
		std::string GetShapeString();

		void SetShapeID(b2ShapeId shapeID);
		b2ShapeId GetShapeID();
		void SetChainID(b2ChainId chainID);
		b2ChainId GetChainID();
		Body* GetParentBody();
		b2BodyId GetParentBodyID();
		void SetShapeProps(ShapeProps shapeProps);
		ShapeProps& GetShapeProps();
		void CreateShape();
		void RecreateShape();
		void DestroyShape();
		bool PointInShape(Vector2 point);

		b2CastOutput CastRayAt(b2RayCastInput* rayCastInput);
		b2CastOutput CastShapeAt(b2ShapeCastInput* shapeCastInput);

		void SetIsSensor(bool b_isSensor);
		void SetEnableSensorEvents(bool b_enableSensorEvents);
		void SetEnableContactEvents(bool b_enableContactEvents);
		void SetRestitution(float restitution);
		void SetDensity(float mass);
		void SetFriction(float friction);
		void SetPositionOffset(Vector2 positionOffset);
		void SetRotationOffset(float rotationOffset);

		// for PolygonBody and ChainBody
		virtual void SetPoints(std::vector<Vector2> points) {};
		virtual void UpdatePoints() {};
		bool ShowPoints();
		void SetShowPoints(bool b_showPoints);
		bool IsEditingPoints();
		void SetEditingPoints(bool b_editingPoints);

		void SetDrawInGame(bool b_drawInGame);
		bool DrawInGame();
		void SetInGameDrawColor(Vector4 drawColor);
		Vector4 GetInGameDrawColor();
		void SetInGameDraThickness(float thickness);
		float GetInGameDraThickness();

	private:
		b2ShapeId m_shapeID;
		b2ChainId m_chainID;
		Body* m_parentBody;
		ShapeProps m_shapeProps;
		bool m_b_showPoints;
		bool m_b_editingPoints;
		bool m_b_drawInGame;
		Vector4 m_inGameDrawColor;
		float m_inGameDrawThickness;
	};
}
