#include "Sprite.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "AssetManager.h"


namespace FlatEngine
{
	Sprite::Sprite(long myID, long parentID)
	{
		SetType(T_Sprite);
		SetID(myID);
		SetParentID(parentID);
		m_texture = Texture();
		m_textureWidth = 0;
		m_textureHeight = 0;
		m_scale = Vector2(1, 1);
		m_pivotPoint = Pivot::PivotCenter;
		m_pivotOffset = Vector2(0, 0);
		m_offset = Vector2(0, 0);
		m_path = "";
		m_tintColor = Vector4(1, 1, 1, 1);
		m_renderOrder = F_maxSpriteLayers / 2;
	}

	Sprite::~Sprite()
	{
		RemoveTexture();
	}

	std::string Sprite::GetData()
	{
		json jsonData = { 
			{ "type", "Sprite" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "path", m_path },
			{ "xScale", m_scale.x },
			{ "yScale", m_scale.y },
			{ "pivotPoint", GetPivotPointString() },
			{ "xOffset", m_offset.x },
			{ "yOffset", m_offset.y },
			{ "tintColorX", m_tintColor.x },
			{ "tintColorY", m_tintColor.y },
			{ "tintColorZ", m_tintColor.z },
			{ "tintColorW", m_tintColor.w },
			{ "renderOrder", m_renderOrder }
,		};
		std::string data = jsonData.dump();
		return data;
	}

	void Sprite::SetTexture(std::string newPath)
	{
		RemoveTexture();

		if (newPath != "")
		{						
			m_path = newPath;
			if (m_texture.LoadFromFile(m_path))
			{
				m_textureWidth = m_texture.GetWidth();
				m_textureHeight = m_texture.GetHeight();

				// Set pivot point to the center of the texture by default
				m_offset = { (float)m_textureWidth / 2, (float)m_textureHeight / 2 };
				m_pivotOffset = m_offset;
			}
			else
			{
				// Set broken texture Texture
				m_texture.LoadFromFile(F_AssetManager.GetFailedToLoadImagePath());
				if (m_textureWidth == 0 || m_textureHeight == 0)
				{
					m_textureWidth = 50;
					m_textureHeight = 50;
					SetOffset(Vector2(25, 25));
				}

				LogError("Sprite::SetTexture() - Texture could not be loaded.");
			}
		}
	}

	void Sprite::SetOffset(Vector2 newOffset)
	{
		m_offset = newOffset;
		SetScale(m_scale);
	}

	Vector2 Sprite::GetOffset()
	{
		return m_offset;
	}

	SDL_Texture* Sprite::GetTexture()
	{
		return m_texture.GetTexture();
	}

	void Sprite::SetScale(Vector2 newScale)
	{
		m_scale = newScale;		
	}

	Vector2 Sprite::GetScale()
	{
		return m_scale;
	}

	int Sprite::GetTextureWidth()
	{
		return m_textureWidth;
	}

	int Sprite::GetTextureHeight()
	{
		return m_textureHeight;
	}

	std::string Sprite::GetPath()
	{
		return m_path;
	}

	void Sprite::SetRenderOrder(int order)
	{
		m_renderOrder = order;
	}

	int Sprite::GetRenderOrder()
	{
		return m_renderOrder;
	}

	void Sprite::RemoveTexture()
	{
		m_path = "";
		m_texture.FreeTexture();
	}

	void Sprite::SetPivotPoint(Pivot newPivot)
	{
		m_pivotPoint = newPivot;
		UpdatePivotOffset();
	}

	void Sprite::SetPivotPoint(std::string newPivot)
	{
		for (int i = 0; i < F_PivotStrings->size(); i++)
		{
			if (newPivot == F_PivotStrings[i])
			{
				m_pivotPoint = Pivot(i);
				UpdatePivotOffset();
				return;
			}
		}

		m_pivotPoint = Pivot::PivotCenter;
		UpdatePivotOffset();
	}

	void Sprite::UpdatePivotOffset()
	{
		Vector2 centeredOffset = Vector2((float)m_textureWidth / 2, (float)m_textureHeight / 2);

		switch (m_pivotPoint)
		{
		case Pivot::PivotCenter:
		{
			m_pivotOffset = centeredOffset;
			break;
		}
		case Pivot::PivotLeft:
		{
			m_pivotOffset = Vector2(centeredOffset.x - ((float)m_textureWidth / 2), centeredOffset.y);
			break;
		}
		case Pivot::PivotRight:
		{
			m_pivotOffset = Vector2(centeredOffset.x + ((float)m_textureWidth / 2), centeredOffset.y);
			break;
		}
		case Pivot::PivotTop:
		{
			m_pivotOffset = Vector2(centeredOffset.x, centeredOffset.y - ((float)m_textureHeight / 2));
			break;
		}
		case Pivot::PivotBottom:
		{
			m_pivotOffset = Vector2(centeredOffset.x, centeredOffset.y + ((float)m_textureHeight / 2));
			break;
		}

		case Pivot::PivotTopLeft:
		{
			m_pivotOffset = Vector2(centeredOffset.x - ((float)m_textureWidth / 2), centeredOffset.y - ((float)m_textureHeight / 2));
			break;
		}
		case Pivot::PivotTopRight:
		{
			m_pivotOffset = Vector2(centeredOffset.x + ((float)m_textureWidth / 2), centeredOffset.y - ((float)m_textureHeight / 2));
			break;
		}
		case Pivot::PivotBottomLeft:
		{
			m_pivotOffset = Vector2(centeredOffset.x - ((float)m_textureWidth / 2), centeredOffset.y + ((float)m_textureHeight / 2));
			break;
		}
		case Pivot::PivotBottomRight:
		{
			m_pivotOffset = Vector2(centeredOffset.x + ((float)m_textureWidth / 2), centeredOffset.y + ((float)m_textureHeight / 2));
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

	Pivot Sprite::GetPivotPoint()
	{
		return m_pivotPoint;
	}

	std::string Sprite::GetPivotPointString()
	{
		return F_PivotStrings[m_pivotPoint];
	}

	void Sprite::SetPivotOffset(Vector2 newPivotOffset)
	{
		m_pivotOffset = newPivotOffset;
	}

	Vector2 Sprite::GetPivotOffset()
	{
		return m_pivotOffset;
	}

	// for r, g, b, a, enter floats between 0.0f - 1.0f
	void Sprite::SetTintColor(Vector4 newTintColor)
	{
		m_tintColor = newTintColor;
	}

	Vector4 Sprite::GetTintColor()
	{
		return m_tintColor;
	}
	void Sprite::SetAlpha(float alpha)
	{
		if (alpha <= 1 && alpha >= 0)
		{
			m_tintColor.w = alpha;
		}
		else
		{
			LogError("Sprite:SetAlpha() - Alpha value must be a float between 0 and 1.");
		}
	}

	float Sprite::GetAlpha()
	{
		return m_tintColor.w;
	}
}
