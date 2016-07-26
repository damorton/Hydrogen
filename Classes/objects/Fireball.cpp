#include "objects\Fireball.h"

Fireball::~Fireball() {}

Fireball* Fireball::create(const std::string& fileName)
{
	Fireball *pInstance = new Fireball();
	pInstance->initWithFile(fileName);
	if (pInstance && pInstance->init())
	{
		pInstance->autorelease();

		CCLOG("Fireball created");
		return pInstance;
	}
	CC_SAFE_DELETE(pInstance);
	return nullptr;
}

bool Fireball::init()
{ 	
	return true;
}