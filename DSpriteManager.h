#pragma once

#include "DManager.h"
#include "DSprite.h"

class DSpriteManager : public DManager
{
public:
	DSpriteManager();
	virtual ~DSpriteManager();

	DSprite *Create(std::string name, std::string file);
};

