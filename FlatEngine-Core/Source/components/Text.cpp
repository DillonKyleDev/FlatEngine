#include "components/Text.h"
#include "GameObject.h"
#include "managers/Assets.h"
#include "tools/FileHelper.h"
#include "tools/Logger.h"


namespace FlatEngine
{
	Text::Text(long ownerID)
	{
		SetType(ComponentType_Text);
		SetOwnerID(ownerID);
		m_textureWidth = 0;
		m_textureHeight = 0;		
		m_fontSize = 40;
		m_font = nullptr;
		m_offset = Vector2(0, 0);
		m_tintColor = Vector4(1,1,1,1);
		m_text = "Sample Text";
		m_renderOrder = 0;
		m_texture = std::make_shared<Texture>();
		m_white = { (Uint8)255, (Uint8)255, (Uint8)255, (Uint8)255 };
		m_transparent = { (Uint8)0, (Uint8)0, (Uint8)0, (Uint8)0 };
		LoadText();
	}

	Text::~Text()
	{
		m_font = nullptr;
		TTF_CloseFont(m_font);
	}

	json Text::GetData(bool b_IDOverride)
	{
		json componentJson = {
			{ "fontPath",        m_fontPath },
			{ "text",            m_text },
			{ "fontSize",        m_fontSize },
			{ "tintColorX",      m_tintColor.x },
			{ "tintColorY",      m_tintColor.y },
			{ "tintColorZ",      m_tintColor.z },
			{ "tintColorW",      m_tintColor.w },
			{ "xOffset",         m_offset.x },
			{ "yOffset",         m_offset.y },
			{ "renderOrder",     m_renderOrder },
			{ "canvasPlacement", m_canvasPlacement.GetData() }	
		};
		componentJson.update(Component::GetData(b_IDOverride));
		
		return componentJson;
	}

	void Text::PutData(json componentJson, std::string objectName)
	{
		if (componentJson.empty())		
			return;	
		
        Component::PutData(componentJson, objectName);

		if (JsonHelper::JsonContains(componentJson, "canvasPlacement", objectName))		
			m_canvasPlacement.PutData(componentJson.at("canvasPlacement"), objectName);		

		SetFontPath(JsonHelper::CheckJsonString(componentJson, "fontPath", objectName));
		SetFontSize(JsonHelper::CheckJsonInt(componentJson, "fontSize", objectName));
		SetColor(Vector4(
			JsonHelper::CheckJsonFloat(componentJson, "tintColorX", objectName),
			JsonHelper::CheckJsonFloat(componentJson, "tintColorY", objectName),
			JsonHelper::CheckJsonFloat(componentJson, "tintColorZ", objectName),
			JsonHelper::CheckJsonFloat(componentJson, "tintColorW", objectName)
		));
		SetText(JsonHelper::CheckJsonString(componentJson, "text", objectName));
		SetRenderOrder(JsonHelper::CheckJsonInt(componentJson, "renderOrder", objectName));
		SetOffset(Vector2(JsonHelper::CheckJsonFloat(componentJson, "xOffset", objectName), JsonHelper::CheckJsonFloat(componentJson, "yOffset", objectName)));
		
		LoadText();
    }

	void Text::LoadText()
	{
		if (m_text == "")
			m_text = "Sample Text";

		if (m_font != nullptr && m_texture != nullptr)
		{
			m_texture->LoadFromRenderedText(m_text, m_white, m_font);
			m_textureWidth = m_texture->GetWidth();
			m_textureHeight = m_texture->GetHeight();
			m_canvasPlacement.dimensions = Vector2(m_textureWidth, m_textureHeight);
			m_canvasPlacement.UpdatePivotOffset();
		}		
	}

	void Text::SetRenderOrder(int order)
	{
		m_renderOrder = order;
	}

	int Text::GetRenderOrder()
	{
		return m_renderOrder;
	}

	std::shared_ptr<Texture> Text::GetTexture()
	{
		return m_texture;
	}

	void Text::SetFontPath(std::string path)
	{
		m_fontPath = FileHelper::DoesFileExist(path) ? path : Assets::assetManager.GetFilePath("mainFont");
		if (m_fontSize <= 0)
				m_fontSize = 40;
		
		m_font = TTF_OpenFont(m_fontPath.c_str(), m_fontSize);				
		LoadText();
	}

	std::string Text::GetFontPath()
	{
		return m_fontPath;
	}

	void Text::SetColor(Vector4 newColor)
	{
		m_tintColor = newColor;
	}

	Vector4 Text::GetColor()
	{
		return m_tintColor;
	}

	void Text::SetText(std::string newText)
	{
		m_text = newText;
		if (m_font != nullptr)
		{
			LoadText();
		}
	}

	std::string Text::GetText()
	{
		return m_text;
	}

	void Text::SetFontSize(int fontSize)
	{
		if (fontSize > 0 && m_fontPath != "")
		{
			TTF_CloseFont(m_font);
			m_fontSize = fontSize;
			m_font = TTF_OpenFont(m_fontPath.c_str(), m_fontSize);
			LoadText();
		}
	}

	int Text::GetFontSize()
	{
		return m_fontSize;
	}

	void Text::SetOffset(Vector2 newOffset)
	{
		m_offset = newOffset;
	}

	Vector2 Text::GetOffset()
	{
		return m_offset;
	}

	CanvasPlacement* Text::GetCanvasPlacement()
	{
		return &m_canvasPlacement;
	}
}