#ifndef LAYER_UILAYER_H_
#define LAYER_UILAYER_H_

#include "cocos2d.h"
#include "objects/Fireball.h"

class UILayer : public cocos2d::Layer {
public:
    virtual bool init();
    static UILayer *create();    
    void menuCloseCallback(cocos2d::Ref* pSender);
    
private:
    cocos2d::Size _VisibleSize;
    cocos2d::Vec2 _Origin;
};

#endif
