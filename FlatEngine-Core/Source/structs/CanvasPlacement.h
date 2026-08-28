#pragma once
#include "structs/Pivot.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"
#include "Types.h"

#include <memory>


namespace FlatEngine
{
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
				{ "yPixel", pixel.y },
				{ "zPosition", zPosition },
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
			zPosition = JsonHelper::CheckJsonFloat(jsonData, "zPosition", name);
			pivot->UpdatePivotOffset();
		}
	};
}