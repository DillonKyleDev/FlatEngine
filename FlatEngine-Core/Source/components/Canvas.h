#pragma once
#include "components/Component.h"
#include "tools/JsonHelper.h"
#include "tools/Vector4.h"


namespace FlatEngine 
{
	class Canvas : public Component
	{
	public:
		Canvas(long ownerID = -1);
		json GetData(bool b_IDOverride = false);
		void PutData(json componentJson, std::string objectName);

		float GetWidth();
		float GetHeight();
		void SetDimensions(float width, float height);
		void CalculateActiveEdges();
		Vector4 GetActiveEdges();
		void SetLayerNumber(int layerNumber);
		int GetLayerNumber();
		void SetBlocksLayers(bool b_blocksLayers);
		bool GetBlocksLayers();

	private:
		int m_layerNumber;
		bool m_b_blocksLayers;
		float m_width;
		float m_height;
		Vector4 m_activeEdges;
	};
}

