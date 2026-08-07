#pragma once
#include "physics/PhysicsManager.h"
#include "render/SceneView.h"
#include "tools/JsonHelper.h"
#include "tools/Numbers.h"
#include "tools/Vector2.h"
#include "tools/Vector4.h"

#include <id.h>
#include <math_functions.h>
#include <string>
#include <variant>


namespace FlatEngine
{
	class Body2D;

	struct BoxShapeData {
		b2ShapeId shapeID;
		Vector2 offset = Vector2();
		b2Rot rotationOffset = b2MakeRot(0);
		Vector2 dimensions = Vector2(1.0f, 1.0f);
		float cornerRadius = 0.0f;

		BoxShapeData(b2ShapeId ID = b2_nullShapeId) { shapeID = ID; }
		json GetData() 
		{
			json jsonData = {
				{ "xOffset", offset.x },
				{ "yOffset", offset.y },
				{ "rotationOffsetCos", rotationOffset.c },
				{ "rotationOffsetSin", rotationOffset.s },
				{ "width", dimensions.x },
				{ "height", dimensions.y },
				{ "cornerRadius", cornerRadius }		
			};
			return jsonData;
		}
		void PutData(json jsonData, std::string name)
		{
			offset = Vector2(JsonHelper::CheckJsonFloat(jsonData, "xOffset", name), JsonHelper::CheckJsonFloat(jsonData, "yOffset", name));
			rotationOffset.c = JsonHelper::CheckJsonFloat(jsonData, "rotationOffsetCos", name);
			rotationOffset.s = JsonHelper::CheckJsonFloat(jsonData, "rotationOffsetSin", name);
			dimensions.x = JsonHelper::CheckJsonFloat(jsonData, "width", name);
			dimensions.y = JsonHelper::CheckJsonFloat(jsonData, "height", name);
			cornerRadius = JsonHelper::CheckJsonLong(jsonData, "cornerRadius", name);
		}

		void SetDimensions(Vector2 newDimensions)
		{
			if (newDimensions.x > 0 && newDimensions.y > 0)
			{			
				dimensions = newDimensions;					
				PhysicsManager::physics2D.RecreateShape(static_cast<Shape*>(b2Shape_GetUserData(shapeID)));
			}
		}
		void SetCornerRadius(float newCornerRadius)
		{		
			if (newCornerRadius >= 0)
			{
				cornerRadius = newCornerRadius;
				PhysicsManager::physics2D.RecreateShape(static_cast<Shape*>(b2Shape_GetUserData(shapeID)));
			}
		}
		void SetOffset(Vector2 newOffset)
		{
			offset = newOffset;
			PhysicsManager::physics2D.RecreateShape(static_cast<Shape*>(b2Shape_GetUserData(shapeID)));	
		}
		void SetRotationOffset(float newRotationOffset)
		{
			rotationOffset = b2MakeRot(Numbers::DegreesToRadians(Transform::ClampRotation(newRotationOffset)));	
			PhysicsManager::physics2D.RecreateShape(static_cast<Shape*>(b2Shape_GetUserData(shapeID)));		
		}
	};

	struct CircleShapeData {
		b2ShapeId shapeID;
		Vector2 offset = Vector2();
		b2Rot rotationOffset = b2MakeRot(0);
		float radius = 1.0f;

		CircleShapeData(b2ShapeId ID = b2_nullShapeId) { shapeID = ID; }
		json GetData() 
		{
			json jsonData = {
				{ "xOffset", offset.x },
				{ "yOffset", offset.y },
				{ "rotationOffsetCos", rotationOffset.c },
				{ "rotationOffsetSin", rotationOffset.s },
				{ "radius", radius }		
			};
			return jsonData;
		}
		void PutData(json jsonData, std::string name)
		{
			offset = Vector2(JsonHelper::CheckJsonFloat(jsonData, "xOffset", name), JsonHelper::CheckJsonFloat(jsonData, "yOffset", name));
			rotationOffset.c = JsonHelper::CheckJsonFloat(jsonData, "rotationOffsetCos", name);
			rotationOffset.s = JsonHelper::CheckJsonFloat(jsonData, "rotationOffsetSin", name);
			radius = JsonHelper::CheckJsonLong(jsonData, "radius", name);
		}

		void SetRadius(float newRadius)
		{
			if (radius > 0)
			{			
				radius = newRadius;
				PhysicsManager::physics2D.RecreateShape(static_cast<Shape*>(b2Shape_GetUserData(shapeID)));
			}
		}
		void SetOffset(Vector2 newOffset)
		{
			offset = newOffset;
			PhysicsManager::physics2D.RecreateShape(static_cast<Shape*>(b2Shape_GetUserData(shapeID)));	
		}
		void SetRotationOffset(float newRotationOffset)
		{
			rotationOffset = b2MakeRot(Numbers::DegreesToRadians(Transform::ClampRotation(newRotationOffset)));		
			PhysicsManager::physics2D.RecreateShape(static_cast<Shape*>(b2Shape_GetUserData(shapeID)));	
		}
	};

	struct CapsuleShapeData {
		b2ShapeId shapeID;
		Vector2 offset = Vector2();
		b2Rot rotationOffset = b2MakeRot(0);
		bool b_horizontal = false;
		float length = 4.0f;
		float radius = 1.0f;

		CapsuleShapeData(b2ShapeId ID = b2_nullShapeId) { shapeID = ID; }
		json GetData() 
		{
			json jsonData = {
				{ "xOffset", offset.x },
				{ "yOffset", offset.y },
				{ "rotationOffsetCos", rotationOffset.c },
				{ "rotationOffsetSin", rotationOffset.s },
				{ "b_horizontal", b_horizontal },
				{ "length", length },
				{ "radius", radius }		
			};
			return jsonData;
		}
		void PutData(json jsonData, std::string name)
		{
			offset = Vector2(JsonHelper::CheckJsonFloat(jsonData, "xOffset", name), JsonHelper::CheckJsonFloat(jsonData, "yOffset", name));
			rotationOffset.c = JsonHelper::CheckJsonFloat(jsonData, "rotationOffsetCos", name);
			rotationOffset.s = JsonHelper::CheckJsonFloat(jsonData, "rotationOffsetSin", name);
			b_horizontal = JsonHelper::CheckJsonBool(jsonData, "b_horizontal", name);
			length = JsonHelper::CheckJsonFloat(jsonData, "length", name);
			radius = JsonHelper::CheckJsonLong(jsonData, "radius", name);
		}

		void SetLength(float newLength)
		{
			if (newLength <= 0)
			{
				Logger::log.Err("Capsule::SetCapsuleLength() : length must be greater than 0.0f."); 
				return;
			}

			length = newLength;	

			if (newLength < radius * 2)		
				radius = newLength / 2;		
			
			PhysicsManager::physics2D.RecreateShape(static_cast<Shape*>(b2Shape_GetUserData(shapeID)));
		}
		void SetRadius(float newRadius)
		{
			if (newRadius <= 0)
			{
				Logger::log.Err("Capsule::SetRadius() : radius must be greater than 0.0f."); 
				return; 
			}

			radius = newRadius;	

			if (radius > length / 2)	
				length = radius * 2;	

			PhysicsManager::physics2D.RecreateShape(static_cast<Shape*>(b2Shape_GetUserData(shapeID)));
		}
		void SetOffset(Vector2 newOffset)
		{
			offset = newOffset;
			PhysicsManager::physics2D.RecreateShape(static_cast<Shape*>(b2Shape_GetUserData(shapeID)));	
		}
		void SetRotationOffset(float newRotationOffset)
		{
			rotationOffset = b2MakeRot(Numbers::DegreesToRadians(Transform::ClampRotation(newRotationOffset)));	
			PhysicsManager::physics2D.RecreateShape(static_cast<Shape*>(b2Shape_GetUserData(shapeID)));		
		}
	};

	struct PolygonShapeData {
		b2ShapeId shapeID;
		float cornerRadius = 0.0f;
		std::vector<Vector2> points = { {3.0f, -2.0f}, {0.0f, 2.0f}, {-3.0f, -2.0f} };
		bool b_showPoints;
		bool b_editingPoints;

		PolygonShapeData(b2ShapeId ID = b2_nullShapeId) { shapeID = ID; }
		json GetData() 
		{
			json pointArray = json::array();
			for (Vector2 point : points)
			{
				json pointData = {
					{ "xPos", point.x },
					{ "yPos", point.y }
				};
				pointArray.push_back(pointData);
			}

			json jsonData = {
				{ "points", pointArray },
				{ "cornerRadius", cornerRadius }		
			};
			return jsonData;
		}
		void PutData(json jsonData, std::string name)
		{
			for (int i = 0; i < jsonData.at("points").size(); i++)
			{
				try
				{
					json pointsJson = jsonData.at("points").at(i);
					Vector2 point = Vector2(JsonHelper::CheckJsonFloat(pointsJson, "xPos", name), JsonHelper::CheckJsonFloat(pointsJson, "yPos", name));
					points.push_back(point);
				}
				catch (const json::out_of_range& e)
				{
					Logger::log.Err("{}", e.what());
				}
			}

			cornerRadius = JsonHelper::CheckJsonLong(jsonData, "cornerRadius", name);
		}

		void SetCornerRadius(float newCornerRadius)
		{		
			if (newCornerRadius >= 0)
			{
				cornerRadius = newCornerRadius;
				PhysicsManager::physics2D.RecreateShape(static_cast<Shape*>(b2Shape_GetUserData(shapeID)));
			}
		}
	};

	struct ChainShapeData {
		b2ChainId chainID;
		b2ShapeId shapeID;
		bool b_isLoop = false;
		std::vector<Vector2> points = { {4.0f, -2.0f}, {4.0f, 0.0f}, {-4.0f, 0.0f}, {-4.0f, -2.0f} };	
		bool b_showPoints;
		bool b_editingPoints;	
		
		ChainShapeData(b2ChainId ID = b2_nullChainId, b2ShapeId shapeID = b2_nullShapeId) { chainID = ID; }
		json GetData() 
		{
			json pointArray = json::array();
			for (Vector2 point : points)
			{
				json pointData = {
					{ "xPos", point.x },
					{ "yPos", point.y }
				};
				pointArray.push_back(pointData);
			}

			json jsonData = {
				{ "points", pointArray },				
				{ "b_isLoop", b_isLoop }					
			};
			return jsonData;
		}
		void PutData(json jsonData, std::string name)
		{
			for (int i = 0; i < jsonData.at("points").size(); i++)
			{
				try
				{
					json pointsJson = jsonData.at("points").at(i);
					Vector2 point = Vector2(JsonHelper::CheckJsonFloat(pointsJson, "xPos", name), JsonHelper::CheckJsonFloat(pointsJson, "yPos", name));
					points.push_back(point);
				}
				catch (const json::out_of_range& e)
				{
					Logger::log.Err("{}", e.what());
				}
			}

			b_isLoop = JsonHelper::CheckJsonBool(jsonData, "b_isLoop", name);			
		}
	};

	enum ShapeType {
		ShapeType_None,
		ShapeType_Box,
		ShapeType_Circle,
		ShapeType_Capsule,
		ShapeType_Polygon,
		ShapeType_Chain
	};
	const std::vector<std::string> ShapeTypeStrings = {
		"None",
		"Box",
		"Circle",
		"Capsule",
		"Polygon",
		"Chain"
	};

	class Shape
	{
	public:
		Shape(long ownerID);	
		json GetData();
		void PutData(json jsonData, std::string name);
		ShapeType GetType();		
		void SetShapeID(b2ShapeId shapeID);
		void SetChainID(b2ChainId chainID);
		const b2ShapeId GetShapeID();
		const b2ChainId GetChainID();
		const long GetOwnerID();
		const b2BodyId Getb2BodyID();

		void SetIsSensor(bool b_isSensor);
		void SetEnableSensorEvents(bool b_enableSensorEvents);
		void SetEnableContactEvents(bool b_enableContactEvents);
		void SetDensity(float density);
		void SetFriction(float friction);
		void SetRestitution(float restitution);		
		void SetRotationOffset(float rotationOffset);
		void SetTangentSpeed(float tangentSpeed);
		void SetRollingResistance(float rollingResistance);

		bool PointInShape(Vector2 point);
		b2CastOutput CastRayAt(b2RayCastInput* rayCastInput);
		b2CastOutput CastShapeAt(b2ShapeCastInput* shapeCastInput);

		ShapeType type;
		b2Polygon polygon;
		b2Circle circle;
		b2Capsule capsule;		
		std::variant<BoxShapeData, CircleShapeData, PolygonShapeData, CapsuleShapeData, ChainShapeData> shapeData;
		bool b_enableContactEvents;
		bool b_enableSensorEvents;
		bool b_isSensor;
		float restitution;
		float density;
		float friction;
		float tangentSpeed;
		float rollingResistance;
		bool b_drawInGame;
		Vector4 inGameDrawColor;
		float inGameDrawThickness;
		
	private:
		b2ShapeId m_shapeID;
		b2ChainId m_chainID;
		long m_ownerID;
	};
}
