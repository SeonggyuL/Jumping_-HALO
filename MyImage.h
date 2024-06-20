#pragma once

#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus")
using namespace Gdiplus;

class CMyImage
{
protected:
	Image *m_pImage;

	UINT    m_width;
	UINT    m_height;

public:
	CMyImage(void);
	~CMyImage(void);


	void Load(char *lpstrFile);
	void Draw(Graphics *g, int x, int y);
	void DrawCenter(Graphics *g, int x, int y, int xCenter, int yCenter);
	void Draw(Graphics *g, int x, int y, int width, int height);
	void Draw();

	void Draw(Graphics *g, int dstX, int dstY, int srcX, int srcY, int width, int height);
};
