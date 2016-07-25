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

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    float scaleFactor = visibleSize.height / visibleSize.width;

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create("CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(MainScene::menuCloseCallback, this));

    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));


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
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "MainScene" splash screen"
    auto spriteBackground = Sprite::create("MainBackground.png");
    
    // position the sprite on the center of the screen
    spriteBackground->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(spriteBackground, 0);

	// Net Manager
	std::unique_ptr<NetManager> netManager(new NetManager());
	_NetManager = std::move(netManager);
	_NetManager->Init("192.168.1.5", "5000");

	// Initialize buffers to null
	_cSendBuffer[0] = '\0';
	_cRecvBuffer[0] = '\0';

	this->scheduleUpdate();

	// Register for touch events
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	

	_iPlayerOneX = visibleSize.width / 2;
	_iPlayerOneY = visibleSize.height / 2;
	_iPlayerTwoX = 0;
	_iPlayerTwoY = 0;

	_PlayerOne = cocos2d::Sprite::create("playerone.png");
	_PlayerOne->setPosition(cocos2d::Vec2(_iPlayerOneX, _iPlayerOneY));
	this->addChild(_PlayerOne);
    return true;
}

void MainScene::update(float dt)
{
	_NetManager->ReadData(_cRecvBuffer, MAX_BUFFER_SIZE_RECV);
	if (strcmp(_cRecvBuffer, "0") != 0)
	{
		CCLOG("Server response: %s", _cRecvBuffer);

		/* Player One */
		if (strcmp(_cRecvBuffer, "{uid:001,value:w}") == 0)
		{
			CCLOG("PlayerOne moving Y++");
			_iPlayerOneY += 5;
		}
		else if (strcmp(_cRecvBuffer, "{uid:001,value:a}\n") == 0)
		{
			_iPlayerOneX--;
		}
		else if (strcmp(_cRecvBuffer, "{uid:001,value:s}\n") == 0)
		{
			_iPlayerOneY--;
		}
		else if (strcmp(_cRecvBuffer, "{uid:001,value:d}\n") == 0)
		{
			_iPlayerOneX++;
		}

		/* Player Two */
		if (strcmp(_cRecvBuffer, "{uid:002,value:w}\n") == 0)
		{
			_iPlayerTwoY++;
		}
		else if (strcmp(_cRecvBuffer, "{uid:002,value:a}\n") == 0)
		{
			_iPlayerTwoX--;
		}
		else if (strcmp(_cRecvBuffer, "{uid:002,value:s}\n") == 0)
		{
			_iPlayerTwoY--;
		}
		else if (strcmp(_cRecvBuffer, "{uid:002,value:d}\n") == 0)
		{
			_iPlayerTwoX++;
		}


		UpdatePlayerOne(_iPlayerOneX, _iPlayerOneY);
		UpdatePlayerTwo(_iPlayerTwoX, _iPlayerTwoY);
	}	
}

bool MainScene::onTouchBegan(Touch* touch, Event* event)
{
	return true;
}

void MainScene::onTouchEnded(Touch* touch, Event* event)
{
	CCLOG("Sending message...");
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