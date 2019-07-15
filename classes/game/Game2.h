#ifndef game2H
#define game2H

#include "../system/Window.h"
#include "../system/Scene.h"
#include "../system/Shader.h"
#include "../image/TextureLoader.h"
#include "../buffer/ObjectBuffer.h"
#include "../buffer/FrameBuffer.h"
#include "../physicobject/Ground.h"
#include "../physicobject/PhysicWorld.h"
#include "../physicobject/MovableObject.h"
#include "../simpleobject/DayCycle2.h"

class MGame2;

typedef void (MGame2::*VoidFunc) ();

class MGame2: public MWindow
{
private:
	VoidFunc pDrawFunc;
	
	//statuses
	bool Pause;
	bool* Key;
	
	//classes
	MScene Scene;
	MShader Shader;
	MTextureLoader TextureLoader;
	MObjectBuffer LocalBuffer;
	MObjectBuffer LightBuffer;
	MObjectBuffer BackgroundBuffer;
	MObject* SkyBodies[2];//0 - sun, 1 - moon
	MDayCycle2* DayCycle2;
	MGround* Ground;
	MPhysicWorld* PhysicWorld;
	MMovableObject* MovableObject;
	
	//textures
	stTexture* txMovableObject;
	stTexture* txTest2;
	stTexture* txTest;
	stTexture* txUnit;
	stTexture* txLight;
	stTexture* txBackground;
	unsigned int txUnit_cnt;
	unsigned int txBackground_cnt;
	unsigned int txOne_cnt;
	stTexture* txBox_repeat;
	stTexture* txBox_norepeat;
	unsigned int txBox_repeat_cnt;
	unsigned int txBox_norepeat_cnt;
	unsigned int txTest2_cnt;
	
	//FBO - lights
	MObject* objLight;
	MFrameBuffer FrameBuffer;
	GLuint lightFBOId;
	GLuint FBOTextureId;
	glm::vec2 lightOffset;
	glm::vec4 ambientColor;
	glm::vec4 whiteColor;
	glm::vec2 sunCenter;
	
	//background
	MObject* objTest;
	MObject* objBackground_Grass;
	MObject* objBackground_Sky;
	MObject* objBackground_Cloud;
	MObject* objBackground_Sun;
	glm::vec2 CloudVelocity;
	glm::vec2 SunVelocity;
	stQuad BuffQuad;
	float Angle;
	
	//draw functions
	void DrawGame();
	
	//local
	void Start();
	void Stop();
	
	//window overload functions
	void OnDraw();
	void OnActivate(WPARAM wParam);
	void OnKeyUp(WPARAM wParam, LPARAM lParam);
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnSize();
public:
	MGame2();
	~MGame2();
	bool Initialize(HINSTANCE hInstance);
	void OnClose();
};

#endif
