#pragma once
#include "components/Component.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"
#include "tools/Vector4.h"
#include "Types.h"

#include <ext/matrix_float4x4.hpp>
#include <memory>


namespace FlatEngine 
{
	const int MAX_CANVAS_LAYERS = 100;

	enum PivotType {
		PivotType_Center,
		PivotType_Left,
		PivotType_Right,
		PivotType_Top,
		PivotType_Bottom,
		PivotType_TopLeft,
		PivotType_TopRight,
		PivotType_BottomRight,
		PivotType_BottomLeft
	};
	const std::string PivotStrings[9] = {
		"Center",
		"Left",
		"Right",
		"Top",
		"Bottom",
		"Top Left",
		"Top Right",
		"Bottom Right",
		"Bottom Left"
	};
	const std::unordered_map<std::string, PivotType> PivotFromString = {
		{ "Center",       PivotType_Center },
		{ "Left",         PivotType_Left },
		{ "Right",        PivotType_Right },
		{ "Top",          PivotType_Top },
		{ "Bottom",       PivotType_Bottom },
		{ "Top Left",     PivotType_TopLeft },
		{ "Top Right",    PivotType_TopRight },
		{ "Bottom Right", PivotType_BottomRight },
		{ "Bottom Left",  PivotType_BottomLeft }
	};

	struct Pivot {
		PivotType type = PivotType_Center;
		Vector2 offset;
		Vector2 dimensions;	

		void SetPivot(PivotType newType)
		{
			type = newType;
			UpdatePivotOffset();
		}

		void UpdatePivotOffset()
		{
			switch (type)
			{
			case PivotType_Center:      offset = Vector2(); break;
			case PivotType_Left:        offset = Vector2( dimensions.x / 2,  0); break; 
			case PivotType_Right:       offset = Vector2(-dimensions.x / 2,  0); break;	
			case PivotType_Top:         offset = Vector2( 0               ,  dimensions.y / 2); break; 
			case PivotType_Bottom: 	    offset = Vector2( 0               , -dimensions.y / 2); break; 
			case PivotType_TopLeft: 	offset = Vector2( dimensions.x / 2,  dimensions.y / 2); break; 
			case PivotType_TopRight:    offset = Vector2(-dimensions.x / 2,  dimensions.y / 2); break;	
			case PivotType_BottomLeft:  offset = Vector2( dimensions.x / 2, -dimensions.y / 2); break; 
			case PivotType_BottomRight: offset = Vector2(-dimensions.x / 2, -dimensions.y / 2); break; 
			default: break;
			}
		}

		std::string GetPivotString()
		{
			return PivotStrings[type];
		}
	};

	struct CanvasPlacement {
		std::shared_ptr<Pivot> pivot = std::make_shared<Pivot>();
		Vector2 percent = Vector2(0.5f);
		Vector2 pixel;			
		float zPosition = 0.01f;
		
		json GetData()
		{
			json jsonData = { 
				{ "pivotType", PivotStrings[pivot->type] },
				{ "xPercent", percent.x },
				{ "yPercent", percent.y },
				{ "xPixel", pixel.x },		
				{ "yPixel", pixel.y }
			};
		
			return jsonData;
		}

		void PutData(json jsonData, std::string name)
		{
			if (jsonData.empty())
				return;

			pivot->type = GetTypeFromString(PivotFromString, JsonHelper::CheckJsonString(jsonData, "pivotType", "Canvas Placement"));
			percent = Vector2(JsonHelper::CheckJsonFloat(jsonData, "xPercent", name), JsonHelper::CheckJsonFloat(jsonData, "yPercent", name));
			pixel = Vector2(JsonHelper::CheckJsonFloat(jsonData, "xPixel", name), JsonHelper::CheckJsonFloat(jsonData, "yPixel", name));

			pivot->UpdatePivotOffset();
		}
	};

	class Canvas : public Component
	{
	public:
		Canvas(long ownerID = -1);
		json GetData(bool b_IDOverride = false);
		void PutData(json componentJson, std::string objectName);

		Vector2 GetDimensions();
		void SetDimensions(Vector2 dimensions);
		void CalculateActiveEdges();
		Vector4 GetActiveEdges();
		void SetLayerNumber(int layerNumber);
		int GetLayerNumber();
		void SetBlocksLayers(bool b_blocksLayers);
		bool GetBlocksLayers();
		Vector3 GetCanvasPlacementPosition(CanvasPlacement* canvasPlacement, Vector2 imageSize, Vector2 textureScale);
		glm::mat4 GetProjection();
		float GetPixelsPerGridSpace();
		void SetPixelsPerGridSpace(float pixels);				

	private:
		float m_pixelsPerGridSpace;	
		int m_layerNumber;
		bool m_b_blocksLayers;
		Vector2 m_dimensions;
		Vector4 m_activeEdges;			
	};
}