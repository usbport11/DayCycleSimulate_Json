#include "../../stdafx.h"
#include "Scene.h"

MScene::MScene()
{
	pWindowWidth = NULL;
	pWindowHeight = NULL;
	
	CameraRound = 1000;
	CoordinateScale = 0.01; //1 metre - 100 pixels
	ViewScale = 1.0f;
	ViewOffset = glm::vec2(0.0f, 0.0f);
	ViewCenter = glm::vec2(0.0f, 0.0f);
}

MScene::~MScene()
{
	pWindowWidth = NULL;
	pWindowHeight = NULL;
}

bool MScene::Initialize(int* inpWindowWidth, int* inpWindowHeight)
{
	if(!inpWindowWidth || !inpWindowHeight)
	{
		LogFile<<"Scene: null window size"<<endl;
		return false;
	}
	
	if(*inpWindowWidth < MIN_WINDOW_SIZE_X || *inpWindowHeight < MIN_WINDOW_SIZE_Y)
	{
		LogFile<<"Scene: wrong window size"<<endl;
		return false;
	}

	pWindowWidth = inpWindowWidth;
	pWindowHeight = inpWindowHeight;

	//prepare view
	Projection = glm::ortho(0.0f, (float)*pWindowWidth * CoordinateScale, 0.0f, (float)*pWindowHeight * CoordinateScale, -5.0f, 5.0f);
	CameraPosition = glm::vec3(0.0f, 0.0f, 1.0f);
	CameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
	Model = glm::mat4(1.0f);
	View = glm::lookAt(CameraPosition, CameraDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    StaticMVP = DynamicMVP = Projection * View * Model;
	
	return true;
}

GLfloat* MScene::GetDynamicMVP()
{
	return (GLfloat*)&DynamicMVP;
}

GLfloat* MScene::GetStaticMVP()
{
	return (GLfloat*)&StaticMVP;
}

void MScene::SetViewScale(float inViewScale)
{
	ViewScale = inViewScale;
	ViewCenter = glm::vec2((*pWindowWidth * 0.5f) * (1 + (1.0f - ViewScale)), (*pWindowHeight * 0.5f) * (1 + (1.0f - ViewScale)));
}

void MScene::Size()
{
	if(!pWindowWidth || !pWindowHeight) return;
	cout<<"Size function: "<<*pWindowWidth<<" "<<*pWindowHeight<<endl;
	
	SetViewScale(ViewScale);
	
	//change projection matix and default MVP
	Projection = glm::ortho(0.0f, (float)*pWindowWidth * CoordinateScale, 0.0f, (float)*pWindowHeight * CoordinateScale, -5.0f, 5.0f);
    StaticMVP = Model * Projection * glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	//change viewport
	ViewCenter = glm::vec2((*pWindowWidth >> 1) * CoordinateScale, (*pWindowHeight >> 1) * CoordinateScale);
	glViewport(0, 0, *pWindowWidth, *pWindowHeight);//IMPORTANT!
}

void MScene::ViewAt(glm::vec2 Point)
{
	CameraPosition.x = CameraDirection.x = ViewOffset.x + round((Point.x - ViewCenter.x) * CameraRound) / CameraRound;
	CameraPosition.y = CameraDirection.y = ViewOffset.y + round((Point.y - ViewCenter.y) * CameraRound) / CameraRound;
	View = glm::lookAt(CameraPosition, CameraDirection, glm::vec3(0, 1, 0));
	if(ViewScale != 1.0f) View = glm::scale(View, glm::vec3(ViewScale, ViewScale, 0.0f));
    DynamicMVP = Projection * View * Model;
}
