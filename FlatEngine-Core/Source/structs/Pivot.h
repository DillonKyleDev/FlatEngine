#pragma once
#include "tools/Vector2.h"

#include <string>
#include <unordered_map>


namespace FlatEngine
{
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
}