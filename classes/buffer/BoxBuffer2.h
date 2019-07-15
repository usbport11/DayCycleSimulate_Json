#ifndef boxbuffer2
#define boxbuffer2

#include "../system/FileJSON.h"

#define TXP_LBA 0.0f
#define TXP_LTA 0.0625f
#define TXP_RTA 0.125f
#define TXP_RBA 0.1875f
#define TXP_LBC 0.25f
#define TXP_LTC 0.3125f
#define TXP_RTC 0.375f
#define TXP_RBC 0.4375f
#define TXP_BF 0.5f
#define TXP_LF 0.5625f
#define TXP_TF 0.625f
#define TXP_RF 0.6875f
#define TXP_CF 0.75f
#define TXP_NA0 0.8125f
#define TXP_NA1 0.875f
#define TXP_NA2 0.9375f

#define TXP_CENTER 12
#define TXP_TOTAL 16
#define MAX_POS 10

struct stFixtureData
{
	int Position[2];
	int Size[2];
};

struct stTexturedBox
{
	int vp[2];//offset in 2d world grid
	int tp;//offset in big texture
};

struct stTexturedBlock
{
	vector<stTexturedBox> TexturedBoxes;
	vector<stFixtureData> FixtureData;
	unsigned int BlockEnd;
};

class MBoxBuffer2
{
protected:
	unsigned int BindNumber;
	glm::vec2 TextureSize;
	glm::vec2 UnitSize;
	GLuint TextureId;
	GLuint BufferId;
	vector<stUVQuad> Buffer;
	map<unsigned char, const float> Type_Offset;//defaults {block code, offset in texture}
	map<unsigned int, vector<stTexturedBlock> > Type_Tempates;//{height of template, array of blocks}
	MFileJSON FileJSON;
	
	void FillUnits(glm::vec2 Pos, unsigned int SizeX, unsigned SizeY, glm::vec2 texStart);
	void FillVerticalUnits(glm::vec2 Pos, unsigned int Size, glm::vec2 texStart);
	void FillHorizontalUnits(glm::vec2 Pos, unsigned int Size, glm::vec2 texStart);
public:
	MBoxBuffer2();
	bool Initialize(GLuint inTextureId, glm::vec2 inUnitSize);
	void AddOneQuad(glm::vec2 Pos, glm::vec2 texStart);
	void AddFullBox(glm::vec2 Pos, unsigned int Size[2]);
	void AddLevelEdges(glm::vec2 EdgeSize, glm::vec2 GroundPos, glm::vec2 GroundSize);
	void SetBindNumber(unsigned char Number);
	void Draw();
	void Begin();
	void End();
	void Clear();
	void Close();
	bool LoadTempltesFromFile(const char* FileName);
	bool Generate();
	bool Dispose();
};

#endif
