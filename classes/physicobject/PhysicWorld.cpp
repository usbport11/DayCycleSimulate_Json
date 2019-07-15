#include "../../stdafx.h"
#include "PhysicWorld.h"

MPhysicWorld::MPhysicWorld()
{
	World = NULL;
}

b2World* MPhysicWorld::GetWorld()
{
	return World;
}

bool MPhysicWorld::Initialize()
{
	Gravity = b2Vec2(0.0, -20.0);
	timeStep = 1.0f / 60.0f;
	velocityIterations = 6;//6
	positionIterations = 2;//2
	try
	{
		World = new b2World(Gravity);
	}
	catch(bad_alloc& ba)
	{
		LogFile<<"PhysicWorld: Can't allocate memory"<<endl;
		return false;
	}
    World->SetContactListener(&OCL);
	
	return true;
}

void MPhysicWorld::Step()
{
	World->Step(timeStep, velocityIterations, positionIterations);
}

void MPhysicWorld::Close()
{
	if(World) delete World;
}
