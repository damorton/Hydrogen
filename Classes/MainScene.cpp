#include "MainScene.h"
#include "SimpleAudioEngine.h"
#include "layer/UILayer.h"
#include "physics3d/CCPhysics3DWorld.h"
#include "physics3d/CCPhysics3D.h"

static cocos2d::Scene *_gPhysicsScene = nullptr;

USING_NS_CC;

Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    _gPhysicsScene = scene;

    Physics3DWorld* world = scene->getPhysics3DWorld();
    world->setDebugDrawEnable(false);
    
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
    
    
    Terrain::DetailMap r("Terrain/dirt.jpg"),g("Terrain/Grass2.jpg",10),b("Terrain/road.jpg"),a("Terrain/GreenSkin.jpg",20);
    
    Terrain::TerrainData data("Terrain/heightmap129.jpg","Terrain/alphamap.png",r,g,b,a, Size(32,32), 20.0f, 1.0f);
    _terrain = Terrain::create(data,Terrain::CrackFixedType::SKIRT);
    _terrain->setMaxDetailMapAmount(4);
    _terrain->setDrawWire(false);
    _terrain->setSkirtHeightRatio(3);
    _terrain->setLODDistance(64,128,192);
    _terrain->setCameraMask((unsigned short)CameraFlag::USER1);
    
    //create terrain
    std::vector<float> heidata = _terrain->getHeightData();
    auto size = _terrain->getTerrainSize();
    Physics3DColliderDes colliderDes;
    colliderDes.shape = Physics3DShape::createHeightfield(size.width, size.height, &heidata[0], 1.0f, _terrain->getMinHeight(), _terrain->getMaxHeight(), true, false, true);
    auto collider = Physics3DCollider::create(&colliderDes);
    auto component = Physics3DComponent::create(collider);
    _terrain->addComponent(component);
    component->syncNodeToPhysics();
    component->setSyncFlag(Physics3DComponent::PhysicsSyncFlag::NONE);
    
    // Add terrain to game world container
    _GameWorld->addChild(_terrain);
    
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
    
    //_GameWorld->addChild(_drawGrid);
    

    // Ambient lighting
    cocos2d::AmbientLight *ambientLight = AmbientLight::create(cocos2d::Color3B::WHITE);
    _GameWorld->addChild( ambientLight );
    
    // Game World Camera
    _Camera = Camera::createPerspective(60, (GLfloat)_WinSize.width/_WinSize.height, 1, 1000);
    _Camera->setPosition3D(Vec3(0, 45, 60));
    _Camera->setCameraFlag(cocos2d::CameraFlag::USER1);
    _GameWorld->addChild(_Camera);

    // Set this scenes debug physics camera
    _gPhysicsScene->setPhysics3DDebugCamera(_Camera);
    
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
    _Orc = Sprite3D::create("orc.c3b");
    _Orc->setScale(0.4f);
    _Orc->setRotation3D(Vec3(0, 180, 0));
    _Orc->setPositionY(_terrain->getHeight(_Orc->getPositionX(), _Orc->getPositionZ()));
    _GameWorld->addChild(_Orc);
    _Camera->lookAt(_Orc->getPosition3D(), Vec3(0, 1, 0));

    
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
            //_Orc->setPosition3D(_Orc->getPosition3D() + Vec3(0, 10, 0));
		}
	}
    
    // Update camera to follow player character
    _Camera->setPosition3D(_Orc->getPosition3D() + Vec3(0, 45, 60));
    //_Camera->lookAt(_Orc->getPosition3D(), Vec3(0, 1, 0));
    
    
    
    // Move player
    if(_OrcState == ORC_FORWARD)
    {
        Vec3 curPos = _Orc->getPosition3D();
        Vec3 newFaceDir = _targetPos - curPos;
        newFaceDir.y = 0.0f;
        newFaceDir.normalize();
        Vec3 offset = newFaceDir * 25.0f * dt;
        curPos+=offset;
        _Orc->setPosition3D(curPos);
        
        // transform player position to world coord
        auto playerPos = _Orc->getPosition3D();
        auto playerModelMat = _Orc->getParent()->getNodeToWorldTransform();
        playerModelMat.transformPoint(&playerPos);
        Vec3 Normal;
        float player_h = _terrain->getHeight(playerPos.x, playerPos.z,&Normal);
        if (Normal.isZero())//check the player whether is out of the terrain
        {
            player_h = playerPos.y;
        }
        else
        {
            player_h += 0;
        }
        _Orc->setPositionY(player_h);
        Quaternion q2;
        q2.createFromAxisAngle(Vec3(0,1,0),(float)-M_PI,&q2);
        
        Quaternion headingQ;
        headingQ.createFromAxisAngle(_headingAxis,_headingAngle,&headingQ);
        _Orc->setRotationQuat(headingQ * q2);
    }
    
    // Check if orc is at destination
    Vec2 player_pos =Vec2(_Orc->getPositionX(),_Orc->getPositionZ());
    Vec2 targetPos = Vec2(_targetPos.x,_targetPos.z);
    auto dist = player_pos.distance(targetPos);
    if(dist<1)
    {
        _OrcState = ORC_IDLE;
    }
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
    
    
    
    
    // Player movement
    cocos2d::Vec2 touchLocation = touch->getLocation();
    PrintLocation("Touch location", touchLocation.x, touchLocation.y, 0);
    
    // Use the touch location to find an origin point and a direction for a ray cast
    cocos2d::Vec3 nearPosition(touchLocation.x, touchLocation.y, 0.0f);
    cocos2d::Vec3 farPosition(touchLocation.x, touchLocation.y, 1.0f);
    
    CCLOG("Before unproject");
    PrintLocation("Near position", nearPosition.x, nearPosition.y, nearPosition.z);
    PrintLocation("Far position", farPosition.x, farPosition.y, farPosition.z);
    
    auto size = Director::getInstance()->getWinSize();
    _Camera->unprojectGL(size, &nearPosition, &nearPosition);
    _Camera->unprojectGL(size, &farPosition, &farPosition);
    
    CCLOG("After unproject");
    PrintLocation("Near position", nearPosition.x, nearPosition.y, nearPosition.z);
    PrintLocation("Far position", farPosition.x, farPosition.y, farPosition.z);
    
    cocos2d::Vec3 direction(farPosition - nearPosition);
    direction.normalize();
    PrintLocation("Direction", direction.x, direction.y, direction.z);
    
    CCLOG("Creating raycast");
    Vec3 collisionPoint;
    bool isInTerrain = _terrain->getIntersectionPoint(Ray(nearPosition, direction), collisionPoint);
    
    
    if(!isInTerrain)
    {
        CCLOG("Raycast hit the player");
        _OrcState = ORC_IDLE;
    }
    else
    {
        direction = collisionPoint - _Orc->getPosition3D();
        direction.y = 0;
        direction.normalize();
        _headingAngle = -1 * acos(direction.dot(Vec3(0,0,-1)));
        _headingAngle += 135;
        direction.cross(direction,Vec3(0,0,-1), &_headingAxis);
        _targetPos = collisionPoint;
        // forward();
        _OrcState = ORC_FORWARD;
        
    }
    event->stopPropagation();
}

void MainScene::PrintLocation(std::string message, float x, float y, float z)
{
    CCLOG("%s X:%f, Y:%f, Z:%f", message.c_str(), x, y, z);
}