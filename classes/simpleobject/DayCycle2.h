#ifndef daycycle2H
#define daycycle2H

#include "../system/Timer3.h"

#define PI 3.14159265
#define DAY_START 6
#define CNT_DAY_HOURS 12
#define CNT_NIGHT_HOURS 12
#define CNT_MINUTES 60
#define CNT_HOURS 24
#define HC_DAY 0
#define HC_NIGHT 1
#define DT_HOUR 0
#define DT_MINUTE 1
#define TM_LIMIT 1000
#define ONE_FRAME 0.16

class MDayCycle2
{
private:
	unsigned int HoursInDay;
	unsigned int MinutesInHour;
	unsigned int DayHours;
	unsigned int DayStartHour;
	
	float SunVelocity;
	glm::vec2 SunSize;
	glm::vec2 SunCenter;
	glm::vec2 SunStart;
	glm::vec2 SunOffset;
	glm::vec2 SceneSize;
	MTimer3 Timer;
	unsigned int HoursCount[2];//day/night
	unsigned int DayTime[2];//hours/mimutes
	bool Day;
	void TimeStep();
public:
	MDayCycle2();
	~MDayCycle2();
	bool Initialize();
	void StartCycle();
	void StopCycle();
	bool SetSunSize(float SizeX, float SizeY);
	bool SetSceneSize(float SizeX, float SizeY);
	bool SetTimeParameters(unsigned int inHoursInDay, unsigned int inMinutesInHour, unsigned int inDayHours, unsigned int inDayStartHour);
	bool SetHoursCount(bool Day, unsigned int Hours);
	bool SetDayTime(unsigned int Hours, unsigned int Minutes);
	bool IsDay();
	bool Time();
	float GetSunVelocity();
	glm::vec2 GetSunCenter();
	glm::vec2 GetSunOffset();
};

#endif
