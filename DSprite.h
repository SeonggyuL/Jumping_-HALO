#pragma once

#include "DResource.h"

#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus")
using namespace Gdiplus;


class DSprite : public DResource
{
protected:
	Image *m_pImage;

public:
	DSprite(std::string name);
	virtual ~DSprite();

	void Load(std::string file);
	void Release();

	void Draw(Graphics *g);
};

