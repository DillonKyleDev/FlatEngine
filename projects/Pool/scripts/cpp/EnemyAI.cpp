#include "EnemyAI.h"
#include "Body.h"
#include "DistanceJoint.h"
#include "PrismaticJoint.h"
#include "RevoluteJoint.h"
#include "MouseJoint.h"
#include "WheelJoint.h"
#include "MotorJoint.h"
#include "WeldJoint.h"
#include "FlatEngine.h"
#include "Physics.h"
#include "MappingContext.h"



namespace FlatEngine
{
	EnemyAI::EnemyAI()
	{

	}

	EnemyAI::~EnemyAI()
	{

	}

	void EnemyAI::Awake()
	{

	}

	void EnemyAI::Start()
	{
		//TagList rayCastCanHit = TagList();
		//rayCastCanHit.SetCollides("Player", true);
		//rayCastCanHit.SetTag("Ball", true);
		//rayCastCanHit.UpdateBits();
		//Body hit;
		//b2CastOutput output = CastRay(Vector2(0), Vector2(1, 0), 1, rayCastCanHit, hit, true);

		//if (output.hit)
		//{
		//	LogString("HIT " + hit.GetParent()->GetName());
		//}



		if (GetObjectByName("BodyB") != nullptr)
		{
			Body* bodyA = GetParent()->GetBody();
			Body* bodyB = GetObjectByName("BodyB")->GetBody();

			//DistanceJoint distanceJoint = DistanceJoint();
			//distanceJoint.CreateJoint(bodyA, bodyB);

			//PrismaticJoint prismaticJoint = PrismaticJoint();
			//prismaticJoint.CreateJoint(bodyA, bodyB);

			//RevoluteJoint revoluteJoint = RevoluteJoint();
			//revoluteJoint.CreateJoint(bodyA, bodyB);

			//MouseJoint mouseJoint = MouseJoint();
			//mouseJoint.CreateJoint(bodyA, bodyB);

			//WeldJoint weldJoint = WeldJoint();
			//weldJoint.CreateJoint(bodyA, bodyB);

			//MotorJoint motorJoint = MotorJoint();
			//motorJoint.CreateJoint(bodyA, bodyB);

			//WheelJoint wheelJoint = WheelJoint();
			//wheelJoint.CreateJoint(bodyA, bodyB);
		}


	}

	void EnemyAI::Update()
    {
		if (GetMappingContext("MC_Player")->ActionPressed("IA_Up"))
		{
			GetObjectByName("BodyB")->GetBody()->ApplyForceToCenter(Vector2(0,10000));
		}
		if (GetMappingContext("MC_Player")->ActionPressed("IA_Down"))
		{
			GetObjectByName("BodyB")->GetBody()->ApplyForceToCenter(Vector2(0, -10000));
		}
		if (GetMappingContext("MC_Player")->ActionPressed("IA_Left"))
		{
			GetObjectByName("BodyB")->GetBody()->ApplyForceToCenter(Vector2(-10000, 0));
		}
		if (GetMappingContext("MC_Player")->ActionPressed("IA_Right"))
		{
			GetObjectByName("BodyB")->GetBody()->ApplyForceToCenter(Vector2(10000, 0));
		}
	}
}