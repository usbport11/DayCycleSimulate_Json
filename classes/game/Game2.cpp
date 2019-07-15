#include "../../stdafx.h"
#include "Game2.h"

MGame2::MGame2():MWindow()
{
	Pause = true;
    Key = new bool [256];
    memset(Key, 0, 256);
	
	pDrawFunc = NULL;
	txUnit = NULL;
	
	txTest = NULL;
	txLight = NULL;
	objLight = NULL;
	ambientColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);//0.3,0.3,0.7,0.7
	whiteColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	lightOffset = glm::vec2(0.1, 0.1);
	
	objTest = NULL;
	objBackground_Grass = NULL;
	objBackground_Sky = NULL;
	objBackground_Cloud = NULL;
	objBackground_Sun = NULL;
	txBackground = NULL;
	CloudVelocity = glm::vec2(0.01f, 0.0f);
	SunVelocity = glm::vec2(0.001f, 0.0f);
	Angle = 0.0f;
	
	txBox_repeat = NULL;
	txBox_norepeat = NULL;
	txTest2 = NULL;
	
	SkyBodies[0] = SkyBodies[1] = NULL;
	DayCycle2 = NULL;
	Ground = NULL;
	PhysicWorld = NULL;
	MovableObject = NULL;
}

MGame2::~MGame2()
{
	pDrawFunc = NULL;
}

bool MGame2::Initialize(HINSTANCE hInstance)
{	
	if(!CreateMainWindow(hInstance)) return false;
	if(!Scene.Initialize(&WindowWidth, &WindowHeight)) return false;

    LogFile<<"Game2: Initialize"<<endl;
    
    if(!WindowInitialized)
    {
    	LogFile<<"Game2: Window was not initialized"<<endl;
    	return 0;
	}
	
	//randomize
    LogFile<<"Game2: randomize rand by time"<<endl; 
    srand(time(NULL));
    
    //prepare shaders
	if(!Shader.CreateShaderProgram("shaders/main2.vertexshader.glsl", "shaders/main2.fragmentshader.glsl")) return false;
	if(!Shader.PrepareShaderValues()) return false;
    
	//frame buffer
	if(!FrameBuffer.Initialize(WindowWidth, WindowHeight)) return false;
	
	//buffer with background objects
	if(!(txMovableObject = TextureLoader.LoadTexture("textures/tex19.png", 1, 1, 0, txOne_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	if(!(txTest2 = TextureLoader.LoadTexture("textures/sun_moon.png", 1, 1, 0, txOne_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	if(!(txTest = TextureLoader.LoadTexture("textures/line0.png", 1, 1, 0, txOne_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	if(!(txUnit = TextureLoader.LoadTexture("textures/mark.png", 1, 1, 0, txOne_cnt, GL_NEAREST, GL_CLAMP_TO_EDGE))) return false;
	
	//box bufffer
	/*
	if(!BoxBuffer2.Initialize(txTest->Id, glm::vec2(0.32f, 0.32f))) return false;
	if(!BoxBuffer2.LoadTempltesFromFile("configs/blocks_begin_1.json")) return false;
	if(!BoxBuffer2.LoadTempltesFromFile("configs/blocks_begin_2.json")) return false;
	if(!BoxBuffer2.Generate()) return false;
	*/
	
	//objTest = new MObject;
	//BackgroundBuffer.Initialize(GL_STATIC_DRAW);
	//BackgroundBuffer.AddObject(objTest, 1,1,2,2, 0,0,1,1, txTest2[5].Id);
	//BackgroundBuffer.DisposeAll();
	
	//world
	PhysicWorld = new MPhysicWorld;
	if(!PhysicWorld->Initialize()) return false;
	//ground
	Ground = new MGround;
	if(!Ground->InitializeGround(PhysicWorld->GetWorld(), txTest->Id, glm::vec2(0.32f, 0.32f))) return false;
	if(!Ground->GenerateGround(3)) return false;
	
	SkyBodies[0] = new MObject;
	SkyBodies[1] = new MObject;
	MovableObject = new MMovableObject;
	if(!LocalBuffer.Initialize(GL_STREAM_DRAW)) return false;
	if(!LocalBuffer.AddObject(SkyBodies[0], -0.32f, 4, 0.32f, 0.32f, 0, 0, 0.5, 1, txTest2->Id)) return false;
	if(!LocalBuffer.AddObject(MovableObject, 2.0f, 2.0f, 0.32f, 0.32f, 0, 0, 1, 1, txMovableObject->Id)) return false;
		if(!MovableObject->SetAnimations(4, 4)) return false;
    	if(!MovableObject->SetAnimationType(ANM_DEAD, ANMT_ONEWAY)) return false;
		if(!MovableObject->SetAnimationType(ANM_JUMP, ANMT_ONEWAY)) return false;
		if(!MovableObject->SetAnimationType(ANM_MOVE, ANMT_LOOP)) return false;
		if(!MovableObject->SetAnimationType(ANM_STAY, ANMT_LOOP)) return false;
		if(!MovableObject->SetCurrentAnimation(ANM_STAY)) return false;
		MovableObject->SetAnimationControl(true);
		MovableObject->StartAnimation();
		if(!MovableObject->AddPhysics(PhysicWorld->GetWorld(), b2_dynamicBody, false, true, OT_MOVABLE, OT_BOUNDARY, b2Vec2(0.02f, 0.02f))) return false;//false, true
		if(!MovableObject->SetMoveParameters(0.05f, 0.6f, 2.0f)) return false;//0.05,0.9,2.0
		if(!MovableObject->SetMoveKeys(VK_LEFT, VK_RIGHT, VK_UP)) return false;
	if(!LocalBuffer.DisposeAll()) return false;
	
	//buffer with light objects
	if(!(txLight = TextureLoader.LoadTexture("textures/tex06.png", 1, 1, 0, txOne_cnt, GL_NEAREST, GL_CLAMP_TO_BORDER))) return false;//04
	objLight = new MObject;
	if(!LightBuffer.Initialize(GL_STATIC_DRAW)) return false;
	if(!LightBuffer.AddObject(objLight, 0, 0, 3, 3, 0, 0, 1, 1, txLight->Id)) return false;
	if(!LightBuffer.DisposeAll()) return false;
	
	//day cycle
	DayCycle2 = new MDayCycle2;
	if(!DayCycle2->SetSceneSize(8, 6)) return false;
	if(!DayCycle2->SetSunSize(0.32f, 0.32f)) return false;
	if(!DayCycle2->Initialize()) return false;
	if(!DayCycle2->SetDayTime(6, 0)) return false;
	ambientColor[3] = 1.0f + DayCycle2->GetSunOffset().y * 4;
	cout<<"Start ambient color: "<<ambientColor[3]<<" "<<DayCycle2->GetSunOffset().y<<endl;
	DayCycle2->StartCycle();
	
	pDrawFunc = &MGame2::DrawGame;
	Start();
    
    return true;
}

void MGame2::Start()
{
    Pause = false;
}

void MGame2::Stop()
{
    Pause = true;
}

void MGame2::DrawGame()
{
	if(!Pause)
	{
		//daycycle
		if(DayCycle2->Time() && DayCycle2->IsDay())
		{
			SkyBodies[0]->SetVertex(DayCycle2->GetSunCenter(), glm::vec2(0.16, 0.16));
			LocalBuffer.UpdateObject(SkyBodies[0]);
			ambientColor[3] = 1.0f + DayCycle2->GetSunOffset().y * 4;//lighta(5 - 1) / suny(1 - 0)
		}
		//world step
		PhysicWorld->Step();
		MovableObject->Move();
		Scene.ViewAt(GetQuadCenter(MovableObject->GetVertex()));
		//update buffer objects
		LocalBuffer.UpdateAll();
	}
	
	//prepare shader uniforms
	glUseProgram(Shader.ProgramId);
	glUniform1i(Shader.MainTextureId, 0);
	glUniform1i(Shader.LightTextureId, 1);
	glUniform2f(Shader.ResolutionId, WindowWidth, WindowHeight);
	glUniform4fv(Shader.AmbientColorId, 1, &ambientColor[0]);//&whiteColor[0]);// &ambientColor[0]
	
	glEnable(GL_BLEND);
	
	//all window + light circle into texture
	
	//FrameBuffer.Begin();
	//glUniform4fv(Shader.AmbientColorId, 1, &whiteColor[0]);
	//LightBuffer.Begin();
	//LightBuffer.DrawAll();
	//LightBuffer.End();
	
	
	FrameBuffer.End();//IMPORTANT!
	LocalBuffer.Begin();
		glUniformMatrix4fv(Shader.MVPId, 1, GL_FALSE, Scene.GetStaticMVP());
		LocalBuffer.DrawObject(SkyBodies[0]);
		glUniformMatrix4fv(Shader.MVPId, 1, GL_FALSE, Scene.GetDynamicMVP());
		LocalBuffer.DrawObject(MovableObject);
		Ground->Draw();
		FrameBuffer.Bind(1);
		//BackgroundBuffer.DrawAll();
		//LocalBuffer.DrawAll();
	LocalBuffer.End();
	glDisable(GL_BLEND);
}

void MGame2::OnDraw()
{
	if(pDrawFunc) ((*this).*(pDrawFunc))();
}

void MGame2::OnActivate(WPARAM wParam)
{
	switch(LOWORD(wParam))
	{
		case WA_ACTIVE:
		case WA_CLICKACTIVE:
			if(pDrawFunc == &MGame2::DrawGame) Start();
			LogFile<<"Game2: window activated!"<<endl;
			break;
		case WA_INACTIVE:
			if(pDrawFunc == &MGame2::DrawGame) Stop();
			LogFile<<"Game2: window deactivated!"<<endl;
			break;
	}
}

void MGame2::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	Key[wParam] = 1;

	if(pDrawFunc == &MGame2::DrawGame)
	{
		if(Key[VK_ESCAPE])
		{
			SendMessage(m_hWnd, WM_DESTROY, 0, 0);
			return;
		}
	}
}

void MGame2::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	Key[wParam] = 0;
	if(pDrawFunc == &MGame2::DrawGame)
	{
	}
}

void MGame2::OnClose()
{
	Stop();
	pDrawFunc = NULL;
	LogFile<<"Game2: Stopped."<<endl;
	
	if(Ground)
	{
		Ground->Close();
		delete Ground;
	}
	if(MovableObject)
	{
		MovableObject->RemovePhysics();
		delete MovableObject;
	}
	//world must be deleted last!
	if(PhysicWorld)
	{
		PhysicWorld->Close();
		delete PhysicWorld;
	}
	
	FrameBuffer.Close();
	BackgroundBuffer.Close();
	if(objTest) delete objTest;
	if(objBackground_Cloud) delete objBackground_Cloud;
	if(objBackground_Sun) delete objBackground_Sun;
	if(objBackground_Grass) delete objBackground_Grass;
	if(objBackground_Sky) delete objBackground_Sky;
	
	LocalBuffer.Close();
	LogFile<<"Game2: Local buffer free"<<endl;
	
	if(SkyBodies[0]) delete SkyBodies[0];
	if(SkyBodies[1]) delete SkyBodies[1];
	if(DayCycle2)
	{
		DayCycle2->StopCycle();
		delete DayCycle2;
	}
	
	LightBuffer.Close();
	if(objLight) delete objLight;
	
	TextureLoader.DeleteTexture(txMovableObject, txOne_cnt);
	TextureLoader.DeleteTexture(txUnit, txOne_cnt);
	TextureLoader.DeleteTexture(txTest, txOne_cnt);
	TextureLoader.DeleteTexture(txTest2, txOne_cnt);
	TextureLoader.DeleteTexture(txLight, txOne_cnt);
	/*
	TextureLoader.DeleteTexture(txBackground, txBackground_cnt);
	TextureLoader.DeleteTexture(txBox_repeat, txBox_repeat_cnt);
	TextureLoader.DeleteTexture(txBox_norepeat, txBox_norepeat_cnt);
	*/
	TextureLoader.Close();
	
	Shader.Close();
	
	LogFile<<"Game2: Shaders free"<<endl;
	
	if(Key) delete [] Key;
	LogFile<<"Game2: Keys free"<<endl;
}

void MGame2::OnSize()
{
	if(pDrawFunc == NULL) return;
	cout<<"Size function!"<<endl;
	
	Scene.Size();
}
