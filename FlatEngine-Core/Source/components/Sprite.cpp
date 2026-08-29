#include "components/Canvas.h"
#include "components/Sprite.h"
#include "managers/Assets.h"
#include "tools/FileHelper.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"
#include <memory>


namespace FlatEngine
{
	Sprite::Sprite(long ownerID)
	{
		SetType(ComponentType_Sprite);
		SetOwnerID(ownerID);
		mesh = Mesh(ownerID);
		m_offset = Vector2();
		m_textureWidth = 0;
		m_textureHeight = 0;
		m_path = "";
		m_tintColor = Vector4(1);	
		m_pivot = std::make_shared<Pivot>();	
		mesh.canvasPlacement.pivot = m_pivot;
	}

	Sprite::~Sprite()
	{
		// RemoveTexture();
	}

	json Sprite::GetData(bool b_IDOverride)
	{
		json componentJson = { 
			{ "path",            m_path },	
			{ "xOffset",         m_offset.x },
			{ "yOffset",         m_offset.y },
			{ "tintColorX",      m_tintColor.x },
			{ "tintColorY",      m_tintColor.y },
			{ "tintColorZ",      m_tintColor.z },
			{ "tintColorW",      m_tintColor.w },
			{ "pivotType",     	 m_pivot->GetPivotString() },
			{ "canvasPlacement", mesh.canvasPlacement.GetData() }	
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
			mesh.canvasPlacement.PutData(componentJson.at("canvasPlacement"), objectName);		
		m_pivot->type = GetTypeFromString(PivotFromString, JsonHelper::CheckJsonString(componentJson, "pivotType", objectName));
					
		SetTintColor(Vector4(
			JsonHelper::CheckJsonFloat(componentJson, "tintColorX", objectName),
			JsonHelper::CheckJsonFloat(componentJson, "tintColorY", objectName),
			JsonHelper::CheckJsonFloat(componentJson, "tintColorZ", objectName),
			JsonHelper::CheckJsonFloat(componentJson, "tintColorW", objectName)
		));
		std::string path = JsonHelper::CheckJsonString(componentJson, "path", objectName);
		if (path != "" && !FileHelper::DoesFileExist(path))		
			Logger::log.Err("Sprite file at \"{}\" not found for GameObject: {}.", path, objectName);		
		else if (path != "")		
			SetTexture(path);						
		
		SetOffset(Vector2(JsonHelper::CheckJsonFloat(componentJson, "xOffset", objectName), JsonHelper::CheckJsonFloat(componentJson, "yOffset", objectName)));
    }

	void Sprite::SetTexture(std::string newPath)
	{		
		RemoveTexture();

		if (newPath != "")
		{				
			mesh.CreateUniformBuffers();
			mesh.SetMaterial("fl_unlit");
			mesh.SetModel("../engine/models/quad.obj");					
			mesh.CreateResources();	

			m_path = newPath;
			std::map<uint32_t, Texture>& meshTextures = mesh.GetTextures();
			if (meshTextures.count(0) && meshTextures.at(0).LoadFromFile(m_path))
			{
				mesh.CreateResources();	
				m_textureWidth = meshTextures.at(0).GetWidth();
				m_textureHeight = meshTextures.at(0).GetHeight();
				m_pivot->dimensions = Vector2(m_textureWidth, m_textureHeight);
				mesh.SetTextureDimensions(Vector2(m_textureWidth, m_textureHeight));
			}
			else
			{
				// Set broken texture
				meshTextures.at(0).LoadFromFile(Assets::assetManager.GetFailedToLoadImagePath());				

				if (m_textureWidth == 0 || m_textureHeight == 0)
				{
					m_textureWidth = 50;
					m_textureHeight = 50;
					m_pivot->dimensions = Vector2(m_textureWidth, m_textureHeight);	
					mesh.SetTextureDimensions(Vector2(m_textureWidth, m_textureHeight));
				}

				Logger::log.Err("Sprite::SetTexture() - Texture could not be loaded.");
			}		
			
			m_pivot->UpdatePivotOffset();
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

	void Sprite::RemoveTexture()
	{
		m_path = "";
		mesh.Cleanup();
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
		return &mesh.canvasPlacement;
	}

	std::shared_ptr<Pivot> Sprite::GetPivot()
	{
		return m_pivot;
	}
}
