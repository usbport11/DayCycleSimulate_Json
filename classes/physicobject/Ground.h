#ifndef groundH
#define groundH

#include "../buffer/BoxBuffer2.h"

#define FRICTION_SIZE 0.0f

class MGround: public MBoxBuffer2
{
private:
	b2World* pWorld;
	b2BodyDef BoxBodyDef;
	b2FixtureDef FixtureDef;
	b2Body* BoxBody;
	b2PolygonShape Box;
	vector<b2Fixture*> Fixtures;
	glm::vec2 HalfSize;
	void AddFixture(glm::vec2 HalfSize, glm::vec2 Center);
	void AddFixedFixture(glm::vec2 Start);
	void ClearFixtures();
public:
	MGround();
	bool InitializeGround(b2World* inpWorld, GLuint inTextureId, glm::vec2 inUnitSize);
	bool GenerateGround(unsigned int inSliceCount);
	void CloseGround();
};

#endif
