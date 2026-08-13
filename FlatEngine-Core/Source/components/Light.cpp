#include "components/Light.h"


namespace FlatEngine
{
	Light::Light(long ownerID)
	{
		SetType(ComponentType_Light);
		SetOwnerID(ownerID);

		m_lightType = LightType::LightType_Point;
		m_direction = Vector3(0, 0, 1);
		m_color = Vector4(1);
	}
	
	json Light::GetData(bool b_IDOverride)
	{
		json componentJson = {
			{ "directionX", m_direction.x },
			{ "directionY", m_direction.y },
			{ "directionZ", m_direction.z },
			{ "colorX", m_color.x },
			{ "colorY", m_color.y },
			{ "colorZ", m_color.z },
			{ "colorW", m_color.w },
			{ "lightType", (int)m_lightType }
		};
		componentJson.update(Component::GetData(b_IDOverride));

		return componentJson;
	}

	void Light::PutData(json componentJson, std::string objectName)
	{
		if (componentJson.empty())		
			return;	
		
        Component::PutData(componentJson, objectName);

		Vector3 lightDirection = Vector3(JsonHelper::CheckJsonFloat(componentJson, "directionX", objectName), JsonHelper::CheckJsonFloat(componentJson, "directionY", objectName), JsonHelper::CheckJsonFloat(componentJson, "directionZ", objectName));
		Vector4 lightColor = Vector4(JsonHelper::CheckJsonFloat(componentJson, "colorX", objectName), JsonHelper::CheckJsonFloat(componentJson, "colorY", objectName), JsonHelper::CheckJsonFloat(componentJson, "colorZ", objectName), JsonHelper::CheckJsonFloat(componentJson, "colorW", objectName));
		LightType lightType = (LightType)(JsonHelper::CheckJsonInt(componentJson, "lightType", objectName));

		SetDirection(lightDirection);
		SetColor(lightColor);
		SetLightType(lightType);
    }


	void Light::SetLightType(LightType lightType)
	{
		m_lightType = lightType;
	}

	LightType Light::GetLightType()
	{
		return m_lightType;
	}

	void Light::SetDirection(Vector3 direction)
	{
		m_direction = direction;
	}

	Vector3 Light::GetDirection()
	{
		return m_direction;
	}

	void Light::SetColor(Vector4 color)
	{
		m_color = color;
	}

	Vector4 Light::GetColor()
	{
		return m_color;
	}
}
