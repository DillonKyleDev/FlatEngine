#pragma once
#include "components/Canvas.h"
#include "components/Component.h"
#include "render/Texture.h"
#include "tools/Vector2.h"
#include "tools/Vector4.h"

#include <SDL_ttf.h>
#include <string>
#include <memory>


namespace FlatEngine
{
	const int MAX_FONT_SIZE = 1000;
	
	class Text : public Component
	{
	public:
		Text(long ownerID = -1);
		~Text();
		json GetData(bool b_IDOverride = false);
		void PutData(json componentJson, std::string objectName);

		void LoadText();
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
		CanvasPlacement* GetCanvasPlacement();
		
	private:
		CanvasPlacement m_canvasPlacement;
		int m_textureWidth;
		int m_textureHeight;
		TTF_Font* m_font;
		std::string m_fontPath;
		int m_fontSize;
		std::string m_text;
		Vector4 m_tintColor;
		std::shared_ptr<Texture> m_texture;
		Vector2 m_offset;
		SDL_Color m_white;
		SDL_Color m_transparent;
	};
}

