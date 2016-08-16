#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "layer/UILayer.h"

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
    _WinSize = Director::getInstance()->getWinSize();
    
    // Add UI Layer to Main Scene
    _UILayer = UILayer::create();
    this->addChild(_UILayer, 1);
    
    
    // Game World Container
    _GameWorld = cocos2d::Node::create();
    this->addChild(_GameWorld);
    
    // Game World grid
    _drawGrid = DrawNode3D::create();
    int iNumberOfGrids = 50;
    //draw x
    for( int j =-iNumberOfGrids; j<=iNumberOfGrids ;j++)
    {
        _drawGrid->drawLine(Vec3(-iNumberOfGrids*5, 0, 5*j),Vec3(iNumberOfGrids*5,0,5*j),Color4F(1,0,0,1));
    }
    //draw z
    for( int j =-iNumberOfGrids; j<=iNumberOfGrids ;j++)
    {
        _drawGrid->drawLine(Vec3(5*j, 0, -iNumberOfGrids*5),Vec3(5*j,0,iNumberOfGrids*5),Color4F(0,0,1,1));
    }
    //draw y
    _drawGrid->drawLine(Vec3(0, 0, 0),Vec3(0,50,0),Color4F(0,1,0,1));
    _GameWorld->addChild(_drawGrid);
    
    // Ambient lighting
    cocos2d::AmbientLight *ambientLight = AmbientLight::create(cocos2d::Color3B::WHITE);
    _GameWorld->addChild( ambientLight );
    
    // Game World Camera
    _Camera = Camera::createPerspective(60, (GLfloat)_WinSize.width/_WinSize.height, 1, 1000);
    _Camera->setPosition3D(Vec3(0, 130, 130));
    _Camera->setCameraFlag(cocos2d::CameraFlag::USER1);
    _GameWorld->addChild(_Camera);
    
    
	// Network Manager
    // TODO: Needs to a singleton object
	std::unique_ptr<NetManager> netManager(new NetManager());
	_NetManager = std::move(netManager);
	_NetManager->Init("gameserver.bitbosh.com", "5000");

	// Initialize buffers to null
	_cSendBuffer[0] = '\0';
	_cRecvBuffer[0] = '\0';

	// Register for touch events
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(MainScene::onTouchMoved, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    
    // 3D Sprite
    _Orc = Sprite3D::create("orc.c3b"); // c3b file, created with the FBX-converter
    _Orc->setPosition3D(Vec3(0, 0, 0));
    _Orc->setRotation3D(Vec3( 0, 180, 0));
    _GameWorld->addChild(_Orc); // Adds orc sprite to scene, z-index: 1
    
    auto animation = Animation3D::create("orc.c3b");
    if (animation)
    {
        auto animate = Animate3D::create(animation);
        _Orc->runAction(RepeatForever::create(animate));
    }
    
    // FireballA
    _FireballA = Fireball::create("fireball/fireballA.png");
    auto rotateFireball = RotateBy::create(15, Vec3(0, 360, 0));
    _FireballA->runAction(RepeatForever::create(rotateFireball));
    _iFireballAX = _FireballA->getContentSize().width / 2;
    _iFireballAY = 0;
    _FireballA->setPosition(cocos2d::Vec2(_Origin.x + _iFireballAX, _Origin.y + _iFireballAY));
    _UILayer->addChild(_FireballA);
    CCLOG("Position 3D %f, %f, %f", _FireballA->getPosition3D().y, _FireballA->getPosition3D().y, _FireballA->getPosition3D().z);
    
    // FireballB
    _FireballB = Fireball::create("fireball/fireballB.png");
    _iFireballBX = _VisibleSize.width - _FireballB->getContentSize().width / 2;
    _iFireballBY = 0;
    _FireballB->setPosition(cocos2d::Vec2(_Origin.x + _iFireballBX, _Origin.y + _iFireballBY));
    _UILayer->addChild(_FireballB);
    
    _Camera->lookAt(_Orc->getPosition3D(), Vec3(0, 1, 0));
    //_Camera->lookAt(_FireballB->getPosition3D(), Vec3(0, 1, 0));
    
    _GameWorld->setCameraMask((unsigned short) cocos2d::CameraFlag::USER1);
    

    // Schedule an update for the Main Scene
    this->scheduleUpdate();
    return true;
}

void MainScene::update(float dt)
{
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
            _Orc->setPosition3D(_Orc->getPosition3D() + Vec3(0, 0, -10));
		}
	}
    
    // Update camera to follow player character
    _Camera->setPosition3D(_Orc->getPosition3D() + Vec3(0, 130, 130));
    _Camera->lookAt(_Orc->getPosition3D(), Vec3(0, 1, 0));
}

bool MainScene::onTouchBegan(Touch* touch, Event* event)
{
	return true;
}

void MainScene::onTouchMoved(Touch* touch, Event* event)
{
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