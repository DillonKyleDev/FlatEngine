#pragma once
#include "components/Component.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"
#include "tools/Vector4.h"
#include "Types.h"


namespace FlatEngine 
{
	const int MAX_CANVAS_LAYERS = 100;

	enum Pivot {
		Pivot_Center,
		Pivot_Left,
		Pivot_Right,
		Pivot_Top,
		Pivot_Bottom,
		Pivot_TopLeft,
		Pivot_TopRight,
		Pivot_BottomRight,
		Pivot_BottomLeft
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
	const std::unordered_map<std::string, Pivot> PivotFromString = {
		{ "Center",       Pivot_Center },
		{ "Left",         Pivot_Left },
		{ "Right",        Pivot_Right },
		{ "Top",          Pivot_Top },
		{ "Bottom",       Pivot_Bottom },
		{ "Top Left",     Pivot_TopLeft },
		{ "Top Right",    Pivot_TopRight },
		{ "Bottom Right", Pivot_BottomRight },
		{ "Bottom Left",  Pivot_BottomLeft }
	};


	struct CanvasPlacement {
		Pivot pivot = Pivot_Center;		
		Vector2 percent = Vector2(0.5f);
		Vector2 pixel;
		Vector2 dimensions;
		Vector2 offset;
		
		json GetData()
		{
			json jsonData = { 
				{ "pivot", PivotStrings[pivot] },
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

			pivot = GetTypeFromString(PivotFromString, JsonHelper::CheckJsonString(jsonData, "pivot", "Canvas Placement"));
			percent = Vector2(JsonHelper::CheckJsonFloat(jsonData, "xPercent", name), JsonHelper::CheckJsonFloat(jsonData, "yPercent", name));
			pixel = Vector2(JsonHelper::CheckJsonFloat(jsonData, "xPixel", name), JsonHelper::CheckJsonFloat(jsonData, "yPixel", name));

			UpdatePivotOffset();
		}

		void SetPivot(Pivot newPivot)
		{
			pivot = newPivot;
			UpdatePivotOffset();
		}

		void UpdatePivotOffset()
		{
			Vector2 center = Vector2(dimensions.x / 2, dimensions.y / 2);

			switch (pivot)
			{
			case Pivot_Center:      offset = center; break;
			case Pivot_Left:        offset = Vector2(center.x - (dimensions.x / 2), center.y);	break; 
			case Pivot_Right:       offset = Vector2(center.x + (dimensions.x / 2), center.y); break;	
			case Pivot_Top:         offset = Vector2(center.x, center.y - (dimensions.y / 2)); break; 
			case Pivot_Bottom: 	    offset = Vector2(center.x, center.y + (dimensions.y / 2)); break; 
			case Pivot_TopLeft: 	offset = Vector2(center.x - (dimensions.x / 2), center.y - (dimensions.y / 2)); break; 
			case Pivot_TopRight:    offset = Vector2(center.x + (dimensions.x / 2), center.y - (dimensions.y / 2)); break;	
			case Pivot_BottomLeft:  offset = Vector2(center.x - (dimensions.x / 2), center.y + (dimensions.y / 2));	break; 
			case Pivot_BottomRight: offset = Vector2(center.x + (dimensions.x / 2), center.y + (dimensions.y / 2));	break; 
			default: break;
			}
		}

		std::string GetPivotString()
		{
			return PivotStrings[pivot];
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

	private:
		int m_layerNumber;
		bool m_b_blocksLayers;
		Vector2 m_dimensions;
		Vector4 m_activeEdges;
	};
}