#pragma once
#include "box2d.h"
#include "Vector2.h"


namespace FlatEngine
{
	class Physics
	{
	public:

		struct BodyProps {
			b2BodyDef bodyDef;
			b2BodyType type;
			Vector2 position;	
			Vector2 dimensions;
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

		b2BodyId m_dynamicID;

	private:
		b2WorldId m_worldID;

	};
}
