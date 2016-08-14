#include "MainScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = MainScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    _VisibleSize = Director::getInstance()->getVisibleSize();
    _Origin = Director::getInstance()->getVisibleOrigin();
    
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create("CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(MainScene::menuCloseCallback, this));

    closeItem->setPosition(Vec2(_Origin.x + _VisibleSize.width - closeItem->getContentSize().width/2 ,
                                _Origin.y + closeItem->getContentSize().height/2));


    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithTTF("Main", "fonts/Roboto-Regular.ttf", 24);
    label->setColor(Color3B::BLACK);

    // position the label on the center of the screen
    label->setPosition(Vec2(_Origin.x + _VisibleSize.width/2,
                            _Origin.y + _VisibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "MainScene" splash screen"
    auto spriteBackground = Sprite::create("GameBackground.png");
    
    // position the sprite on the center of the screen
    spriteBackground->setPosition(Vec2(_VisibleSize.width/2 + _Origin.x, _VisibleSize.height/2 + _Origin.y));

    // add the sprite as a child to this layer
    //sthis->addChild(spriteBackground, 0);

	// Net Manager
	std::unique_ptr<NetManager> netManager(new NetManager());
	_NetManager = std::move(netManager);
	_NetManager->Init("gameserver.bitbosh.com", "5000");

	// Initialize buffers to null
	_cSendBuffer[0] = '\0';
	_cRecvBuffer[0] = '\0';

	this->scheduleUpdate();

	// Register for touch events
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	

	_iPlayerOneX = _VisibleSize.width / 2;
	_iPlayerOneY = _VisibleSize.height / 2;
	_iPlayerTwoX = 0;
	_iPlayerTwoY = 0;
	
	_PlayerOne = cocos2d::Sprite::create("playerone.png");
	_PlayerOne->setPosition(Vec2( _VisibleSize.width / 2 + _Origin.x, _Origin.y + _PlayerOne->getContentSize().height ));
	this->addChild(_PlayerOne);

	// FireballA
	_FireballA = Fireball::create("fireball/fireballA.png");
	_iFireballAX = _FireballA->getContentSize().width / 2;
	_iFireballAY = 0;
	_FireballA->setPosition(cocos2d::Vec2(_Origin.x + _iFireballAX, _Origin.y + _iFireballAY));
	this->addChild(_FireballA);

	// FireballB
	_FireballB = Fireball::create("fireball/fireballB.png");
	_iFireballBX = _VisibleSize.width - _FireballB->getContentSize().width / 2;
	_iFireballBY = 0;
	_FireballB->setPosition(cocos2d::Vec2(_Origin.x + _iFireballBX, _Origin.y + _iFireballBY));
	this->addChild(_FireballB);
    
    auto s = Director::getInstance()->getWinSize();
    auto camera = Camera::createOrthographic(s.width, s.height, 1, 1000);
    
    // set parameters for camera
    camera->setPosition3D(Vec3(0, 0, 50));
    camera->lookAt(Vec3(0, 0, 0), Vec3(0, 1, 0));
    
    this->addChild(camera); //add camera to the scene

    // 3D Sprite
    auto sprite = Sprite3D::create("orc.c3b"); //c3b file, created with the FBX-converter
    sprite->setScale(10.0f); //sets the object scale in float
    sprite->setPosition(Vec2(_VisibleSize.width/2 + _Origin.x, _VisibleSize.height/2 + _Origin.y)); //sets sprite position
    this->addChild(sprite,1); //adds sprite to scene, z-index: 1

    //rotate around the X axis
    auto rotation = RotateBy::create(15, Vec3(0, 360, 0));
    //our sprite object runs the action
    sprite->runAction(RepeatForever::create(rotation));
    
    auto animation = Animation3D::create("orc.c3b");
    
    if (animation)
    {
        auto animate = Animate3D::create(animation);
        sprite->runAction(RepeatForever::create(animate));
    }
    
    return true;
}

void MainScene::update(float dt)
{
	/*
	// Move FireballA forward on touch
	CCLOG("Updating fireballAs position");
	_iFireballAY++;
	_FireballA->setPosition(cocos2d::Vec2(_iFireballAX, _iFireballAY));

	// Send a message to the server about fireballA
	CCLOG("Sending message to the server");
	std::string message = "{uid:001,value:w}";

	sprintf(_cSendBuffer, "%s", message.c_str());
	_NetManager->SendData(_cSendBuffer);
	*/

	_NetManager->ReadData(_cRecvBuffer, MAX_BUFFER_SIZE_RECV);
	if (strcmp(_cRecvBuffer, "0") != 0)
	{		
		// Update fireballBs position according to the server
		if (strcmp(_cRecvBuffer, "{uid:002,value:w}") == 0)
		{
			CCLOG("Server response: %s", _cRecvBuffer);
			CCLOG("Updating fireballB position");
			_iFireballBY += 10;
			_FireballB->setPosition(cocos2d::Vec2(_Origin.x + _iFireballBX, _Origin.y + _iFireballBY));
		}		

		//UpdatePlayerOne(_iPlayerOneX, _iPlayerOneY);
		//UpdatePlayerTwo(_iPlayerTwoX, _iPlayerTwoY);
	}	
}

bool MainScene::onTouchBegan(Touch* touch, Event* event)
{
	return true;
}

void MainScene::onTouchEnded(Touch* touch, Event* event)
{
	// Move FireballA forward on touch
	CCLOG("Updating fireballAs position");
	_iFireballAY += 10;
	_FireballA->setPosition(cocos2d::Vec2(_Origin.x + _iFireballAX, _Origin.y + _iFireballAY));

	// Send a message to the server about fireballA
	CCLOG("Sending message to the server");
	std::string message = "{uid:001,value:w}";

	sprintf(_cSendBuffer, "%s", message.c_str());
	_NetManager->SendData(_cSendBuffer);
}

void MainScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void MainScene::UpdatePlayerOne(int x, int y)
{
	_PlayerOne->setPosition(cocos2d::Vec2(x, y));
}

void MainScene::UpdatePlayerTwo(int x, int y)
{

}