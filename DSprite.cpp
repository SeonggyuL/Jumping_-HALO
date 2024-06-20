#include "DSprite.h"


#define IMAGE_PATH  ".\\Data\\Image\\"

DSprite::DSprite(std::string name) : DResource(name)
{
	m_pImage = NULL;
}


DSprite::~DSprite()
{
	Release();
}


void DSprite::Load(std::string file)
{
	if (m_pImage)
		delete m_pImage;

	std::string path = IMAGE_PATH;
	path = path + file;

	WCHAR pathfile[MAX_PATH];
	MultiByteToWideChar(CP_ACP, NULL, path.data(), -1, pathfile, MAX_PATH);


	m_pImage = Image::FromFile(pathfile);
}

void DSprite::Release()
{
	if (m_pImage)
		delete m_pImage;

	m_pImage = NULL;
}

void DSprite::Draw(Graphics *g)
{
//	if (m_pImage->GetType() != ImageType::ImageTypeUnknown)
//	  MessageBox(NULL, "Sprite Draw", "Draw", MB_OK);

	g->DrawImage(m_pImage, 200, 200);
}
