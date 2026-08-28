#pragma once
#include "components/Component.h"
#include "structs/SceneRenderObject.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"
#include "tools/Vector4.h"

#include <ext/matrix_float4x4.hpp>


namespace FlatEngine 
{
	const int MAX_CANVAS_LAYERS = 1000;

	class CanvasPlacement;

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
		Vector3 GetCanvasPlacementPosition(CanvasPlacement* canvasPlacement, Vector2 imageSize);
		glm::mat4 GetProjection();
		float GetPixelsPerGridSpace();
		void SetPixelsPerGridSpace(float pixels);
		SceneRenderObject* GetRenderObject();		
		void UpdateRenderShapes();		

	private:
		SceneRenderObject m_renderOutline;
		float m_screenPixelsPerGridSpace; // Changes how many world grid spaces stretch across the canvas view.. Set equal to GuiCore::texturePixelsPerGridSpace for a 1:1 pixel representation in the Canvas view.
		int m_layerNumber;
		bool m_b_blocksLayers;
		Vector2 m_dimensions;
		Vector4 m_activeEdges;			
	};
}