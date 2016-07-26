#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "net\NetManager.h"
#include "GameDefinitions.h"
#include "objects/Fireball.h"

class MainScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);

	void update(float dt) override;

	virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);

	void UpdatePlayerOne(int x, int y);
	void UpdatePlayerTwo(int x, int y);

private:
	std::unique_ptr<NetManager> _NetManager;

	char _cSendBuffer[MAX_BUFFER_SIZE_SEND];
	char _cRecvBuffer[MAX_BUFFER_SIZE_RECV];
	int _iPlayerOneX, _iPlayerOneY;
	int _iPlayerTwoX, _iPlayerTwoY;
	cocos2d::Sprite *_PlayerOne;
	Fireball *_FireballA;
	Fireball *_FireballB;
	int _iFireballAX, _iFireballAY;
	int _iFireballBX, _iFireballBY;
};

#endif // __MAIN_SCENE_H__
