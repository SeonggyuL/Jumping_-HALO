#include "DManager.h"
#include "DResource.h"


DManager::DManager()
{
}


DManager::~DManager()
{
	std::map<std::string, DResource *>::iterator  it;
	for (it = m_mapResource.begin(); it != m_mapResource.end(); it++)
	{
		DResource *pRes = it->second;
		delete pRes;
	}
	m_mapResource.clear();
}


DResource *DManager::Create(std::string name)
{
	DResource *pRes = Find(name);
	if (pRes != NULL)
		return pRes;

	pRes = new DResource(name);
	m_mapResource[name] = pRes;

	return pRes;
}

DResource *DManager::Find(std::string name)
{
	std::map<std::string, DResource *>::iterator  it;

	it = m_mapResource.find(name);
	if (it == m_mapResource.end())
		return NULL;

	DResource *pRes = it->second;
	return pRes;
}


void DManager::Delete(std::string name)
{
	std::map<std::string, DResource *>::iterator  it;

	it = m_mapResource.find(name);
	if (it != m_mapResource.end())
	{
		DResource *pRes = it->second;
		delete pRes;
		m_mapResource.erase(it);
	}	
}