#pragma once
#include "components/Component.h"
#include "render/Texture.h"
#include "tools/Vector2.h"
#include "tools/Vector4.h"


namespace FlatEngine
{
	class Sprite : public Component
	{
	public:
		Sprite(long myID = -1, long parentObjectID = -1);
		~Sprite();
		json GetData();
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
		void SetPivotPoint(Pivot newPivot);
		Pivot GetPivotPoint();
		std::string GetPivotPointString();
		void SetPivotOffset(Vector2 newPivotOffset);
		void SetPivotPoint(std::string newPivot);
		void UpdatePivotOffset();
		Vector2 GetPivotOffset();
		void SetTintColor(Vector4 newTintColor);
		Vector4 GetTintColor();
		void SetAlpha(float);
		float GetAlpha();

	private:
		Texture m_texture;
		int m_renderOrder;
		int m_textureWidth;
		int m_textureHeight;
		Vector2 m_scale;
		Pivot m_pivotPoint;
		Vector2 m_pivotOffset;
		Vector2 m_offset;
		std::string m_path;
		Vector4 m_tintColor;
	};
}
