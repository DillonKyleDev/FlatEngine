#pragma once
#include "box2d.h"
#include "Vector2.h"


namespace FlatEngine
{
	class Physics
	{
	public:

		struct BodyProps {
			b2BodyType type = b2_staticBody;
			Vector2 position = Vector2(0,0);	
			float rotation = 0;
			Vector2 dimensions = Vector2(1.0f, 1.0f);
			float density = 1;
			float friction = 0.3f;		
		};

		Physics();
		~Physics();

		void Init();
		void Shutdown();
		void Update(float deltaTime);

		void CreateBody(BodyProps bodyProps, b2BodyId& bodyID);
		void CreateBox(BodyProps bodyProps, b2BodyId& bodyID);
		void DestroyBody(b2BodyId bodyID);
		void RecreateBody(BodyProps bodyProps, b2BodyId& bodyID);

	private:
		b2WorldId m_worldID;

	};
}
