#include "../../stdafx.h"
#include "DayCycle.h"

MDayCycle::MDayCycle()
{
	BodyLifeTime[CB_SUN] = 24;
	BodyLifeTime[CB_MOON] = 24;
	CurrentBody = CB_SUN;
}

bool MDayCycle::Initialize(glm::vec2 inSceneSize, MObject* inpSun, float inSunStartHeight)
{
	if(!inpSun)
	{
		LogFile<<"DayCycle: Sun object is empty"<<endl;
		return false;
	}
	if(inSceneSize.x <= 0 || inSceneSize.y <= 0)
	{
		LogFile<<"DayCycle: Scene size is wrong"<<endl;
		return false;
	}
	if(inSunStartHeight >= inSceneSize.y)
	{
		LogFile<<"DayCycle: Sun start height is wrong"<<endl;
		return false;
	}
	SceneSize = inSceneSize;
	pSun = inpSun;
	SunStartHeight = inSunStartHeight;
	SunSize = GetQuadSize(pSun->GetVertex());
	SunSpeed = (SceneSize.x + SunSize.x * 2) / BodyLifeTime[CB_SUN];
	cout<<"Sun speed: "<<SunSpeed<<endl;
	
	LifeTimer.Create(LTID, BodyLifeTime[CB_SUN]);
	SunTimer.Create(STID, 1);
	CurrentBody = CB_SUN;
	
	return true;
}

void MDayCycle::Start()
{
	LifeTimer.Start();
	SunTimer.Start();
}

void MDayCycle::Stop()
{
	LifeTimer.Stop();
	SunTimer.Stop();
}

unsigned char MDayCycle::DayStep()
{
	switch(CurrentBody)
	{
		case CB_SUN:
			if(LifeTimer.Time())
			{
				CurrentBody = CB_MOON;
				SunTimer.Stop();
				LifeTimer.Start();
				cout<<"Switch to moon"<<endl;
				return CurrentBody;
			}
			if(SunTimer.Time())
			{
				Vertex = pSun->GetVertex();
				MoveQuad(Vertex, SunSpeed, 0);
				pSun->SetVertex(Vertex);
				SunTimer.Start();
			}
			break;
		case CB_MOON:
			if(LifeTimer.Time())
			{
				SetQuad(Vertex, -SunSize.x, SunStartHeight, SunSize.x, SunSize.y);
				pSun->SetVertex(Vertex);
				CurrentBody = CB_SUN;
				SunTimer.Start();
				LifeTimer.Start();
				cout<<"Switch to sun"<<endl;
				return CurrentBody;
			}
			break;
	}
	
	return CurrentBody;
}

