#include "Text.h"
#include "FlatEngine.h"
#include "GameObject.h"


namespace FlatEngine
{
	Text::Text(long myID, long parentID)
	{
		SetType(T_Text);
		SetID(myID);
		SetParentID(parentID);
		m_fontPath = GetFilePath("cinzelBlack");
		m_fontSize = 40;
		m_font = TTF_OpenFont(m_fontPath.c_str(), m_fontSize);
		m_offset = Vector2(0, 0);
		m_pivotPoint = PivotCenter;
		m_pivotOffset = Vector2(0, 0); 
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

	std::string Text::GetData()
	{
		json jsonData = {
			{ "type", "Text" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "fontPath", m_fontPath },
			{ "text", m_text },
			{ "fontSize", m_fontSize },
			{ "pivotPoint", GetPivotPointString() },
			{ "tintColorX", m_tintColor.x },
			{ "tintColorY", m_tintColor.y },
			{ "tintColorZ", m_tintColor.z },
			{ "tintColorW", m_tintColor.w },
			{ "xOffset", m_offset.x },
			{ "yOffset", m_offset.y },
			{ "renderOrder", m_renderOrder },
		};
		std::string data = jsonData.dump();
		return data;
	}

	void Text::LoadText()
	{
		if (m_text != "" && m_font != nullptr && m_fontSize > 0 && m_texture != nullptr)
		{
			m_texture->LoadFromRenderedText(m_text, m_white, m_font);
			m_pivotOffset = Vector2((float)m_texture->GetWidth() / 2, (float)m_texture->GetHeight() / 2);
		}
		else if (m_texture != nullptr)
		{
			if (m_font == nullptr)
			{
				TTF_CloseFont(m_font);
				LogError("Font not valid in \"" + GetParent()->GetName() + "\" Text component.");
			}
			m_texture->FreeTexture();
			m_pivotOffset = Vector2(0,0);
			m_offset = m_pivotOffset;
		}

		UpdatePivotOffset();
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
		m_fontPath = path;
		if (path != "" && m_fontSize > 0)
		{
			m_font = TTF_OpenFont(m_fontPath.c_str(), m_fontSize);
			LoadText();
		}
		else
		{
			TTF_CloseFont(m_font);
			m_font = nullptr;
			if (m_texture != nullptr)
			{
				m_texture->FreeTexture();
			}
			LogError("Font not valid in \"" + GetParent()->GetName() + "\" Text component.");
		}
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

	void Text::SetPivotPoint(Pivot newPivot)
	{
		m_pivotPoint = newPivot;
		UpdatePivotOffset();
	}

	void Text::SetPivotPoint(std::string newPivot)
	{
		for (int i = 0; i < F_PivotStrings->size(); i++)
		{
			if (newPivot == F_PivotStrings[i])
			{
				m_pivotPoint = Pivot(i);
				return;
			}
		}

		m_pivotPoint = Pivot::PivotCenter;	
		LogError("Pivot point string not valid. Use 'Pivot<Direction>': ie. PivotLeft, PivotRight, PivotCenter");
		UpdatePivotOffset();
	}

	void Text::SetPivotPointLua(std::string newPivot)
	{
		SetPivotPoint(newPivot);
	}

	Pivot Text::GetPivotPoint()
	{
		return m_pivotPoint;
	}

	std::string Text::GetPivotPointString()
	{
		return F_PivotStrings[m_pivotPoint];
	}

	void Text::SetPivotOffset(Vector2 newPivotOffset)
	{
		m_pivotOffset = newPivotOffset;
	}

	Vector2 Text::GetPivotOffset()
	{
		return m_pivotOffset;
	}

	void Text::UpdatePivotOffset()
	{
		if (m_texture != nullptr)
		{
			Vector2 centeredOffset = Vector2((float)m_texture->GetWidth() / 2, (float)m_texture->GetHeight() / 2);

			switch (m_pivotPoint)
			{
			case Pivot::PivotCenter:
			{
				m_pivotOffset = centeredOffset;
				break;
			}
			case Pivot::PivotLeft:
			{
				m_pivotOffset = Vector2(centeredOffset.x - (m_texture->GetWidth() / 2), centeredOffset.y);
				break;
			}
			case Pivot::PivotRight:
			{
				m_pivotOffset = Vector2(centeredOffset.x + (m_texture->GetWidth() / 2), centeredOffset.y);
				break;
			}
			case Pivot::PivotTop:
			{
				m_pivotOffset = Vector2(centeredOffset.x, centeredOffset.y - (m_texture->GetHeight() / 2));
				break;
			}
			case Pivot::PivotBottom:
			{
				m_pivotOffset = Vector2(centeredOffset.x, centeredOffset.y + (m_texture->GetHeight() / 2));
				break;
			}

			case Pivot::PivotTopLeft:
			{
				m_pivotOffset = Vector2(centeredOffset.x - (m_texture->GetWidth() / 2), centeredOffset.y - (m_texture->GetHeight() / 2));
				break;
			}
			case Pivot::PivotTopRight:
			{
				m_pivotOffset = Vector2(centeredOffset.x + (m_texture->GetWidth() / 2), centeredOffset.y - (m_texture->GetHeight() / 2));
				break;
			}
			case Pivot::PivotBottomLeft:
			{
				m_pivotOffset = Vector2(centeredOffset.x - (m_texture->GetWidth() / 2), centeredOffset.y + (m_texture->GetHeight() / 2));
				break;
			}
			case Pivot::PivotBottomRight:
			{
				m_pivotOffset = Vector2(centeredOffset.x + (m_texture->GetWidth() / 2), centeredOffset.y + (m_texture->GetHeight() / 2));
				break;
			}
			default:
			{
				m_pivotOffset = m_offset;
				break;
			}
			}

			m_offset = m_pivotOffset;
		}
	}
}