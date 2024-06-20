#pragma once

#include <string>

class DResource
{
protected:
	std::string  m_name;

public:
	DResource(std::string name);
	virtual ~DResource();

	virtual void Load(std::string file);
	virtual void Release();

	virtual void Draw() {}
};

#define SAFE_DELETE(p)  { if(p) delete (p); (p) = NULL; }
