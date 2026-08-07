#pragma once
#include "components/Component.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"


namespace FlatEngine
{
	enum LightType {
		LightType_None,
		LightType_Directional,		
		LightType_Point
	};
	class Light : public Component
	{
	public:
		Light(long ownerID = -1);
		json GetData(bool b_IDOverride = false);
		void PutData(json componentJson, std::string objectName);

		void SetLightType(LightType lightType);
		LightType GetLightType();
		void SetDirection(Vector3 direction);
		Vector3 GetDirection();
		void SetColor(Vector4 color);
		Vector4 GetColor();

	private:
		LightType m_lightType;
		Vector3 m_direction;
		Vector4 m_color;
	};
}
