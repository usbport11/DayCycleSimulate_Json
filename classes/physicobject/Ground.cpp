#include "../../stdafx.h"
#include "Ground.h"

MGround::MGround():MBoxBuffer2()
{
	pWorld = NULL;
}

bool MGround::InitializeGround(b2World* inpWorld, GLuint inTextureId, glm::vec2 inUnitSize)
{
	if(!inpWorld)
	{
		LogFile<<"Ground: World is NULL"<<endl;
		return false;
	}

	if(!Initialize(inTextureId, inUnitSize)) return false;
	if(!LoadTempltesFromFile("configs/blocks_begin_1.json")) return false;
	if(!LoadTempltesFromFile("configs/blocks_begin_2.json")) return false;
	HalfSize.x = UnitSize.x / 2;
	HalfSize.y = UnitSize.y / 2;
	
	pWorld = inpWorld;
	BoxBodyDef.position.Set(0.0f, 0.0f);
	BoxBodyDef.type = b2_staticBody;
	BoxBody = pWorld->CreateBody(&BoxBodyDef);
	
	return true;
}

bool MGround::GenerateGround(unsigned int inSliceCount)
{
	Clear();
	ClearFixtures();
	
	if(!pWorld)
	{
		LogFile<<"Ground: World is NULL"<<endl;
		return false;
	}
	
	if(!inSliceCount || inSliceCount > 5)
	{
		LogFile<<"Ground: Wrong slice count"<<endl;
		return false;
	}

	unsigned int Rand;
	float OffsetX;
	unsigned int LastEnd = 1;
	unsigned int SliceCount = inSliceCount;
	unsigned int TempatesCount = Type_Tempates.size();
	unsigned int BlocksLimit;
	stTexturedBlock* pRandBlock;
	map<unsigned int, vector<stTexturedBlock> >::iterator it;
	glm::vec2 Position;
	bool WasMerge = false;
	vector<stFixtureData> AllFixtures;
	stFixtureData Fixture;
	
	if(!TempatesCount)
	{
		LogFile<<"Ground: no templates founded"<<endl;
		return false;
	}
	
	//here no intellectual fixture creating (merging into a polygon by edge boxes)
	for(unsigned int i=0; i<SliceCount; i++)
	{
		OffsetX = i * MAX_POS;
		//check that previous slice end type (height) existed
		//this check need be placed out of this cycle!
		it = Type_Tempates.find(LastEnd);
		if(it == Type_Tempates.end())
		{
			LogFile<<"Ground: Can't find this end type in templates"<<endl;
			return false;
		}
		//creating visual part
		Rand = rand() % Type_Tempates[LastEnd].size();
		pRandBlock = &Type_Tempates[LastEnd][Rand];
		for(unsigned int j=0; j<pRandBlock->TexturedBoxes.size(); j++)
		{
			Position = glm::vec2(OffsetX + pRandBlock->TexturedBoxes[j].vp[0], pRandBlock->TexturedBoxes[j].vp[1]);
			//adding one box with texture with 1 line (y offset = 0) - will be changed
			AddOneQuad(Position, glm::vec2(Type_Offset[pRandBlock->TexturedBoxes[j].tp], 0));
			//create temporary array for all fixtures
		}
		for(unsigned int j=0; j<pRandBlock->FixtureData.size(); j++)
		{
			AllFixtures.push_back(pRandBlock->FixtureData[j]);
			AllFixtures[AllFixtures.size() - 1].Position[0] += OffsetX;
		}
		
		//set end of slice type (height). it used in next cycle step
		LastEnd = pRandBlock->BlockEnd;
	}
	//create fixtures with merge test
	Fixture = AllFixtures[0];
	for(unsigned int i=0; i<AllFixtures.size(); i++)
	{
		//cout<<i<<": "<<AllFixtures[i].Position[0]<<" "<<AllFixtures[i].Position[1]<<" "<<AllFixtures[i].Size[0]<<" "<<AllFixtures[i].Size[1]<<endl;
		//cout<<"=================================="<<endl;
		if(AllFixtures[i].Size[1] == AllFixtures[i + 1].Size[1] && AllFixtures[i].Position[1] == AllFixtures[i + 1].Position[1]) 
		{
			Fixture.Size[0] += AllFixtures[i + 1].Size[0];
			WasMerge = true;
		}
		else
		{
			AddFixture(glm::vec2(Fixture.Size[0] * 0.5f * UnitSize.x, Fixture.Size[1] * 0.5f * UnitSize.y), 
				glm::vec2((Fixture.Position[0] * 2 + Fixture.Size[0]) * UnitSize.x * 0.5f, (Fixture.Position[1] * 2 + Fixture.Size[1]) * UnitSize.y * 0.5f));
			Fixture = AllFixtures[i + 1];
			WasMerge = false;
		}
	}
	if(WasMerge)
	{
		AddFixture(glm::vec2(Fixture.Size[0] * 0.5f * UnitSize.x, Fixture.Size[1] * 0.5f * UnitSize.y), 
			glm::vec2((Fixture.Position[0] * 2 + Fixture.Size[0]) * UnitSize.x * 0.5f, (Fixture.Position[1] * 2 + Fixture.Size[1]) * UnitSize.y * 0.5f));
	}
	AllFixtures.clear();
	
	if(!Dispose()) return false;
	
	return true;
}

void MGround::AddFixture(glm::vec2 HalfSize, glm::vec2 Center)
{
	Box.SetAsBox(HalfSize.x, HalfSize.y, b2Vec2(Center.x, Center.y), 0.0f);
	Fixtures.push_back(BoxBody->CreateFixture(&Box, 0.0f));
	cout<<"Center:("<<Center.x<<", "<<Center.y<<") HalfSize:("<<HalfSize.x<<", "<<HalfSize.y<<")"<<endl;
}

void MGround::AddFixedFixture(glm::vec2 Start)
{
	glm::vec2 FixSize = glm::vec2(0.005f, 0.005f);
	b2Vec2 Vertices[8];
	//bt bl
	Vertices[0].Set(Start.x, Start.y + FixSize.y);
	Vertices[1].Set(Start.x + FixSize.x, Start.y);
	//br bt
	Vertices[2].Set(Start.x + UnitSize.x - FixSize.x, Start.y);
	Vertices[3].Set(Start.x + UnitSize.x, Start.y + FixSize.y);
	//tr tl
	Vertices[4].Set(Start.x + UnitSize.x, Start.y + UnitSize.y - FixSize.y);
	Vertices[5].Set(Start.x + UnitSize.x - FixSize.x, Start.y + UnitSize.y);
	//tr tl
	Vertices[6].Set(Start.x + FixSize.x, Start.y + UnitSize.y);
	Vertices[7].Set(Start.x, Start.y + UnitSize.y - FixSize.y);
	
	b2PolygonShape polygonShape;
	polygonShape.Set(Vertices, 8);
	Fixtures.push_back(BoxBody->CreateFixture(&polygonShape, 0.0f));
}

void MGround::ClearFixtures()
{
	if(!BoxBody) return;
	for(int i=0; i<Fixtures.size(); i++)
		BoxBody->DestroyFixture(Fixtures[i]);
	Fixtures.clear();
}

void MGround::CloseGround()
{
	//graphic part
	Clear();
	Close();
	if(BoxBody) pWorld->DestroyBody(BoxBody);
	//physic part
	ClearFixtures();
}
