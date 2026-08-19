#pragma once
#include "components/Component.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"
#include "tools/Vector4.h"


namespace FlatEngine 
{
	const int MAX_CANVAS_LAYERS = 100;

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

