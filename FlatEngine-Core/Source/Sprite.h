#pragma once
#include "Component.h"
#include "Texture.h"
#include "Vector2.h"
#include "Vector4.h"

#include <SDL.h>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	class Sprite : public Component
	{
	public:
		Sprite(long myID = -1, long parentID = -1);
		~Sprite();
		std::string GetData();

		void SetTexture(std::string path);
		//SDL_Texture* GetTexture();
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
