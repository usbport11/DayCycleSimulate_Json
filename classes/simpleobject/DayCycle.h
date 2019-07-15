#ifndef daycycleH
#define daycycleH

#include "../system/Timer2.h"
#include "Object.h"

#define CB_SUN 0
#define CB_MOON 1
#define LTID 2001
#define STID 2002

class MDayCycle
{
private:
	glm::vec2 SceneSize;
	float SunStartHeight;
	float SunSpeed;
	MObject* pSun;
	MTimer2 SunTimer;
	MTimer2 LifeTimer;
	unsigned int BodyLifeTime[2];
	unsigned char CurrentBody;
	stQuad Vertex;
	glm::vec2 SunSize;
public:
	MDayCycle();
	bool Initialize(glm::vec2 inSceneSize, MObject* inpSun, float inSunStartHeight);
	void Start();
	void Stop();
	unsigned char DayStep();
	void Close();
};

#endif
