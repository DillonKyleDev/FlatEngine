#pragma once
#include "components/Canvas.h"
#include "components/Component.h"
#include "components/Mesh.h"
#include "tools/Vector2.h"
#include "tools/Vector4.h"


namespace FlatEngine
{
	class Sprite : public Component
	{
	public:
		Sprite(long ownerID = -1);
		~Sprite();
		json GetData(bool b_IDOverride = false);
		void PutData(json componentJson, std::string objectName);

		void SetTexture(std::string path);
		VkDescriptorSet GetTexture();
		void SetScale(Vector2 newScale);
		Vector2 GetScale();
		int GetTextureWidth();
		int GetTextureHeight();
		std::string GetPath();
		void SetRenderOrder(int order);
		int GetRenderOrder();
		void SetOffset(Vector2 offset);
		Vector2 GetOffset();
		void RemoveTexture();
		void SetTintColor(Vector4 newTintColor);
		Vector4 GetTintColor();
		void SetAlpha(float);
		float GetAlpha();
		CanvasPlacement* GetCanvasPlacement();
		
		Mesh mesh;

	private:		
		CanvasPlacement m_canvasPlacement;
		int m_renderOrder;
		int m_textureWidth;
		int m_textureHeight;
		Vector2 m_scale;
		Vector2 m_offset;
		std::string m_path;
		Vector4 m_tintColor;
	};
}