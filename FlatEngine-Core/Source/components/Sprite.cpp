#include "components/Canvas.h"
#include "components/Sprite.h"
#include "managers/Assets.h"
#include "tools/FileHelper.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"


namespace FlatEngine
{
	Sprite::Sprite(long ownerID)
	{
		SetType(ComponentType_Sprite);
		SetOwnerID(ownerID);
		mesh = Mesh(ownerID);
		m_textureWidth = 0;
		m_textureHeight = 0;
		m_scale = Vector2(1);		
		m_path = "";
		m_tintColor = Vector4(1);
		m_renderOrder = 0;
	}

	Sprite::~Sprite()
	{
		RemoveTexture();
	}

	json Sprite::GetData(bool b_IDOverride)
	{
		json componentJson = { 
			{ "path",            m_path },
			{ "xScale",          m_scale.x },
			{ "yScale",          m_scale.y },			
			{ "xOffset",         m_offset.x },
			{ "yOffset",         m_offset.y },
			{ "tintColorX",      m_tintColor.x },
			{ "tintColorY",      m_tintColor.y },
			{ "tintColorZ",      m_tintColor.z },
			{ "tintColorW",      m_tintColor.w },
			{ "renderOrder",     m_renderOrder },
			{ "canvasPlacement", m_canvasPlacement.GetData() }	
		};
		componentJson.update(Component::GetData(b_IDOverride));
		
		return componentJson;
	}

	void Sprite::PutData(json componentJson, std::string objectName)
	{
		if (componentJson.empty())		
			return;	
		
        Component::PutData(componentJson, objectName);

		if (JsonHelper::JsonContains(componentJson, "canvasPlacement", objectName))		
			m_canvasPlacement.PutData(componentJson.at("canvasPlacement"), objectName);		
				
		SetScale(Vector2(JsonHelper::CheckJsonFloat(componentJson, "xScale", objectName), JsonHelper::CheckJsonFloat(componentJson, "yScale", objectName)));
		SetRenderOrder(JsonHelper::CheckJsonInt(componentJson, "renderOrder", objectName));
		SetTintColor(Vector4(
			JsonHelper::CheckJsonFloat(componentJson, "tintColorX", objectName),
			JsonHelper::CheckJsonFloat(componentJson, "tintColorY", objectName),
			JsonHelper::CheckJsonFloat(componentJson, "tintColorZ", objectName),
			JsonHelper::CheckJsonFloat(componentJson, "tintColorW", objectName)
		));
		std::string path = JsonHelper::CheckJsonString(componentJson, "path", objectName);
		if (!FileHelper::DoesFileExist(path))
		{
			Logger::log.Err("Sprite file not found for GameObject: {}. This may lead to unexpected behavior.  \npath: {}", objectName, path);
		}
		else
		{
			SetTexture(path);
		}		
		
		SetOffset(Vector2(JsonHelper::CheckJsonFloat(componentJson, "xOffset", objectName), JsonHelper::CheckJsonFloat(componentJson, "yOffset", objectName)));
    }

	void Sprite::SetTexture(std::string newPath)
	{		
		// mesh.CleanupTextures();

		if (newPath != "")
		{				
			mesh.CreateUniformBuffers();
			mesh.SetMaterial("fl_unlit");
			mesh.SetModel("../engine/models/plane.obj");					
			mesh.CreateResources();	

			m_path = newPath;
			std::map<uint32_t, Texture>& meshTextures = mesh.GetTextures();
			if (meshTextures.count(0) && meshTextures.at(0).LoadFromFile(m_path))
			{
				mesh.CreateResources();	
				m_textureWidth = meshTextures.at(0).GetWidth();
				m_textureHeight = meshTextures.at(0).GetHeight();

				m_offset = Vector2((float)m_textureWidth / 2, (float)m_textureHeight / 2);				
				m_canvasPlacement.dimensions = Vector2(m_textureWidth, m_textureHeight);			
			}
			else
			{
				// Set broken texture
				meshTextures.at(0).LoadFromFile(Assets::assetManager.GetFailedToLoadImagePath());				

				if (m_textureWidth == 0 || m_textureHeight == 0)
				{
					m_textureWidth = 50;
					m_textureHeight = 50;
					SetOffset(Vector2(25, 25));
					m_canvasPlacement.dimensions = Vector2(m_textureWidth, m_textureHeight);	
				}

				Logger::log.Err("Sprite::SetTexture() - Texture could not be loaded.");
			}		
			
			m_canvasPlacement.UpdatePivotOffset();
		}
	}

	void Sprite::SetOffset(Vector2 newOffset)
	{
		m_offset = newOffset;
	}

	Vector2 Sprite::GetOffset()
	{
		return m_offset;
	}

	VkDescriptorSet Sprite::GetTexture()
	{
		return mesh.GetTextures().at(0).GetTexture();
	}

	void Sprite::SetScale(Vector2 newScale)
	{		
		if (newScale.x != 0 && newScale.y != 0)
		{
			m_scale = newScale;
		}
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
		// mesh.CleanupTextures();
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
			Logger::log.Err("Sprite:SetAlpha() - Alpha value must be a float between 0 and 1.");
		}
	}

	float Sprite::GetAlpha()
	{
		return m_tintColor.w;
	}

	CanvasPlacement* Sprite::GetCanvasPlacement()
	{
		return &m_canvasPlacement;
	}
}
