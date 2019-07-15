#include "../../stdafx.h"
#include "DayCycle2.h"

MDayCycle2::MDayCycle2()
{
	Day = true;
	SceneSize = glm::vec2(0, 0);
	DayTime[0] = 6;
	DayTime[1] = 0;
	HoursCount[HC_DAY] = 0;
	HoursCount[HC_NIGHT] = 0;
	HoursInDay = 0;
	MinutesInHour = 0;
	DayHours = 0;
	DayStartHour = 0;
	SunSize = glm::vec2(0.32f, 0.32f);
	SunCenter = glm::vec2(-0.16, 4);
	SunStart = SunCenter;
	SunOffset = glm::vec2(0, 0);
	SunVelocity = 0;
}

MDayCycle2::~MDayCycle2()
{
	SceneSize = glm::vec2(0, 0);
	DayTime[0] = 0;
	DayTime[1] = 0;
	HoursCount[HC_DAY] = 0;
	HoursCount[HC_NIGHT] = 0;
	HoursInDay = 0;
	MinutesInHour = 0;
	DayHours = 0;
	DayStartHour = 0;
	SunSize = glm::vec2(0, 0);
	SunVelocity = 0;
}

bool MDayCycle2::Initialize()
{
	if(SceneSize.x <= 0 || SceneSize.y <= 0)
	{
		LogFile<<"DayCycle2: Wrong scene size"<<endl;
		return false;
	}
	if(SunSize.x <= 0 || SunSize.y <= 0)
	{
		LogFile<<"DayCycle2: Wrong sun size"<<endl;
		return false;
	}
	if(!Timer.SetLimit(TM_LIMIT))
	{
		LogFile<<"DayCycle2: Can't set timer limit'"<<endl;
		return false;
	}
	//precheck
	if(!HoursCount[HC_DAY]) HoursCount[HC_DAY] = CNT_DAY_HOURS;
	if(!HoursCount[HC_NIGHT]) HoursCount[HC_NIGHT] = CNT_NIGHT_HOURS;
	if(!HoursInDay) HoursInDay = CNT_HOURS;
	if(!MinutesInHour) MinutesInHour = CNT_MINUTES;
	if(!DayHours) DayHours = CNT_DAY_HOURS;
	if(!DayStartHour) DayStartHour = DAY_START;
	
	SunVelocity = (SceneSize.x + SunSize.x) / (DayHours * MinutesInHour);
	SunStart = glm::vec2(-(SunSize.x / 2), SceneSize.y - 2);
	
	//cout<<HoursInDay<<" "<<MinutesInHour<<" "<<DayHours<<" "<<DayStartHour<<" "<<HoursCount[HC_DAY]<<" "<<HoursCount[HC_NIGHT]<<endl;
	
	return true;
}

void MDayCycle2::StartCycle()
{
	Timer.Start();
}

void MDayCycle2::StopCycle()
{
	Timer.Stop();
}

bool MDayCycle2::SetSunSize(float SizeX, float SizeY)
{
	if(SizeX <= 0 || SizeY <= 0)
	{
		LogFile<<"DayCycle2: Wrong sun size"<<endl;
		return false;
	}
	SunSize = glm::vec2(SizeX, SizeY);
	
	return true;
}

bool MDayCycle2::SetSceneSize(float SizeX, float SizeY)
{
	if(SizeX <= 0 || SizeY <= 0)
	{
		LogFile<<"DayCycle2: Wrong scene size"<<endl;
		return false;
	}
	SceneSize = glm::vec2(SizeX, SizeY);
	
	return true;
}

bool MDayCycle2::SetTimeParameters(unsigned int inHoursInDay, unsigned int inMinutesInHour, unsigned int inDayHours, unsigned int inDayStartHour)
{
	if(!inHoursInDay || !inMinutesInHour || !inDayHours || !inDayStartHour)
	{
		LogFile<<"DayCycle2: Some of time values is NULL"<<endl;
		return false;
	}
	if(inDayHours >= inHoursInDay)
	{
		LogFile<<"DayCycle2: Wrong day hours count"<<endl;
		return false;
	}
	if(inDayStartHour >= inHoursInDay || inDayStartHour + inDayHours >= inHoursInDay)
	{
		LogFile<<"DayCycle2: Wrong day start hour"<<endl;
		return false;
	}
	HoursInDay = inHoursInDay;
	MinutesInHour = inMinutesInHour;
	DayHours = inDayHours;
	DayStartHour = inDayStartHour;
	
	return true;
}

bool MDayCycle2::SetDayTime(unsigned int Hours, unsigned int Minutes)
{
	if(SunVelocity <= 0)
	{
		LogFile<<"DayCycle2: Wrong sun velocity"<<endl;
		return false;
	}
	if(Hours >= HoursInDay || Minutes >= MinutesInHour)
	{
		LogFile<<"DayCycle2: Wrong input h/m parameters"<<endl;
		return false;
	}
	
	DayTime[DT_HOUR] = Hours;
	DayTime[DT_MINUTE] = Minutes;
	
	//sun center recalculate
	if(DayTime[DT_HOUR] >= DayStartHour && DayTime[DT_HOUR] < DayStartHour + DayHours)
	{
		SunCenter.x = SunStart.x + ((DayTime[DT_HOUR] - DayStartHour) * MinutesInHour + DayTime[DT_MINUTE]) * SunVelocity;
		SunCenter.y = SunStart.y + sin(SunCenter.x / ((SceneSize.x + SunSize.x) / PI));
	}
	else 
	{
		SunCenter = glm::vec2(SunCenter.x, SunStart.y + sin(SunCenter.x / ((SceneSize.x + SunSize.x) / PI)));
	}
	SunOffset = SunCenter - SunStart;
	
	return true;
}

bool MDayCycle2::IsDay()
{
	return Day;
}

void MDayCycle2::TimeStep()
{
	DayTime[DT_MINUTE] ++;
	if(DayTime[DT_MINUTE] >= MinutesInHour)
	{
		DayTime[DT_HOUR] ++;
		DayTime[DT_MINUTE] = 0;
	}
	if(DayTime[DT_HOUR] >= HoursInDay)
	{
		DayTime[DT_HOUR] = 0;
		DayTime[DT_MINUTE] = 0;
	}
	if(DayTime[DT_HOUR] >= DayStartHour && DayTime[DT_HOUR] < DayStartHour + DayHours) Day = true;
	else
	{
		Day = false;
		SunCenter = glm::vec2(SunCenter.x, SunStart.y + sin(SunCenter.x / ((SceneSize.x + SunSize.x) / PI)));
		SunOffset = SunCenter - SunStart;
	}
}

bool MDayCycle2::Time()
{
	if(Timer.Time())
	{
		TimeStep();
		if(Day)
		{
			SunCenter.x += SunVelocity;
			SunCenter.y = SunStart.y + sin(SunCenter.x / ((SceneSize.x + SunSize.x) / PI));
			SunOffset = SunCenter - SunStart;
		}
		Timer.Start();
		//cout<<DayTime[DT_HOUR]<<":"<<DayTime[DT_MINUTE]<<" ("<<SunCenter.x<<" "<<SunCenter.y<<")"<<endl;
		return true;
	}
	
	return false;
}

float MDayCycle2::GetSunVelocity()
{
	return SunVelocity;
}

glm::vec2 MDayCycle2::GetSunOffset()
{
	return SunOffset;
}

glm::vec2 MDayCycle2::GetSunCenter()
{
	return SunCenter;
}
