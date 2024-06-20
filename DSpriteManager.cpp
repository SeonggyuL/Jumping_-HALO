#include "DSpriteManager.h"


DSpriteManager::DSpriteManager()
{
}


DSpriteManager::~DSpriteManager()
{
}


DSprite *DSpriteManager::Create(std::string name, std::string file)
{
	DSprite *pRes = (DSprite *)Find(name);
	if (pRes != NULL)
		return pRes;

	pRes = new DSprite(name);
	m_mapResource[name] = (DResource *)pRes;

	pRes->Load(file);

	return pRes;
}