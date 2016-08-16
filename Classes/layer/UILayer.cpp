#include "layer/UILayer.h"

UILayer *UILayer::create()
{
    UILayer *pRet = new UILayer();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool UILayer::init()
{
    if ( !cocos2d::Layer::init() )
    {
        return false;
    }
    
    _VisibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    _Origin = cocos2d::Director::getInstance()->getVisibleOrigin();
        
    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = cocos2d::MenuItemImage::create("CloseNormal.png",
                                                    "CloseSelected.png",
                                                    CC_CALLBACK_1(UILayer::menuCloseCallback, this));
    
    closeItem->setPosition(cocos2d::Vec2(_Origin.x + _VisibleSize.width - closeItem->getContentSize().width/2 ,
                                _Origin.y + closeItem->getContentSize().height/2));
    
    
    // create menu, it's an autorelease object
    auto menu = cocos2d::Menu::create(closeItem, NULL);
    menu->setPosition(cocos2d::Vec2::ZERO);
    this->addChild(menu, 1);
    
    /////////////////////////////
    // 3. add your codes below...
    // add "MainScene" splash screen"
    auto spriteBackground = cocos2d::Sprite::create("GameBackground.png");
    
    // position the sprite on the center of the screen
    spriteBackground->setPosition(cocos2d::Vec2(_VisibleSize.width/2 + _Origin.x, _VisibleSize.height/2 + _Origin.y));
    
    // add the sprite as a child to this layer
    //this->addChild(spriteBackground, 0);

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = cocos2d::Label::createWithTTF("UI Layer", "fonts/Roboto-Regular.ttf", 32);
    label->setColor(cocos2d::Color3B::YELLOW);
    
    // position the label on the center of the screen
    label->setPosition(cocos2d::Vec2(_Origin.x + _VisibleSize.width/2,
                            _Origin.y + _VisibleSize.height - label->getContentSize().height));
    
    // add the label as a child to this layer
    this->addChild(label, 1);
        
    CCLOG("UILayer created");
    return true;
}

void UILayer::menuCloseCallback(Ref* pSender)
{
    cocos2d::Director::getInstance()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

