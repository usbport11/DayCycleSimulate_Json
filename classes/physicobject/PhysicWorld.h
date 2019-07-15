#ifndef physicworldH
#define physicworldH

#include "../physicobject/ObjectContactListener.h"

class MPhysicWorld
{
private:
	//global physic
	b2Vec2 Gravity;
	float32 timeStep;
	int velocityIterations;
	int positionIterations;
	b2World* World;
	MObjectContactListener OCL;
	
public:
	MPhysicWorld();
	b2World* GetWorld();
	bool Initialize();
	void Step();
	void Close();
};

#endif
