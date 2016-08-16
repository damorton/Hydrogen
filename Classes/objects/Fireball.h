#ifndef OBJECTS_FIREBALL_H
#define OBJECTS_FIREBALL_H

#include "cocos2d.h"

class Fireball : public cocos2d::Sprite
{
public:
	virtual ~Fireball();
	static Fireball *create(const std::string& fileName);
protected:
	bool init();
};

#endif