#pragma once
#include "components/Component.h"
#include "render/Texture.h"
#include "tools/Vector2.h"
#include "tools/Vector4.h"

#include <SDL_ttf.h>
#include <string>
#include <memory>


namespace FlatEngine
{
	class Text : public Component
	{
	public:
		Text(long myID = -1, long parentObjectID = -1);
		~Text();
		json GetData();
		void PutData(json componentJson, std::string objectName);

		void LoadText();
		void SetRenderOrder(int order);
		int GetRenderOrder();
		std::shared_ptr<Texture> GetTexture();
		void SetFontPath(std::string path);
		std::string GetFontPath();
		void SetFontSize(int fontSize);
		int GetFontSize();
		void SetColor(Vector4 newColor);
		Vector4 GetColor();
		void SetText(std::string text);
		std::string GetText();
		void SetOffset(Vector2 offset);
		Vector2 GetOffset();
		void SetPivotPoint(Pivot newPivot);
		void SetPivotPoint(std::string newPivot);
		void SetPivotPointLua(std::string newPivot);
		Pivot GetPivotPoint();
		std::string GetPivotPointString();
		void SetPivotOffset(Vector2 newPivotOffset);
		Vector2 GetPivotOffset();
		void UpdatePivotOffset();
		
	private:
		TTF_Font* m_font;
		std::string m_fontPath;
		int m_fontSize;
		std::string m_text;
		Vector4 m_tintColor;
		std::shared_ptr<Texture> m_texture;
		Vector2 m_offset;
		Pivot m_pivotPoint;
		Vector2 m_pivotOffset;
		int m_renderOrder;
		SDL_Color m_white;
		SDL_Color m_transparent;
	};
}

