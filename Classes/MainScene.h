#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "net/NetworkSingleton.h"
#include "GameDefinitions.h"
#include "objects/Fireball.h"
#include "layer/UILayer.h"

NS_CC_BEGIN

/**
 * Copy DrawNode for 3D geometry drawing.
 */
class DrawNode3D : public cocos2d::Node
{
public:
    /** creates and initialize a DrawNode3D node */
    static cocos2d::DrawNode3D* create();
    
    /**
     * Draw 3D Line
     */
    void drawLine(const cocos2d::Vec3 &from, const cocos2d::Vec3 &to, const Color4F &color);
    
    /**
     * Draw 3D cube
     * @param point to a vertex array who has 8 element.
     *        vertices[0]:Left-top-front,
     *        vertices[1]:Left-bottom-front,
     *        vertices[2]:Right-bottom-front,
     *        vertices[3]:Right-top-front,
     *        vertices[4]:Right-top-back,
     *        vertices[5]:Right-bottom-back,
     *        vertices[6]:Left-bottom-back,
     *        vertices[7]:Left-top-back.
     * @param color
     */
    void drawCube(cocos2d::Vec3* vertices, const Color4F &color);
    
    /** Clear the geometry in the node's buffer. */
    void clear();
    
    /**
     * @js NA
     * @lua NA
     */
    const BlendFunc& getBlendFunc() const;
    
    /**
     * @code
     * When this function bound into js or lua,the parameter will be changed
     * In js: var setBlendFunc(var src, var dst)
     * @endcode
     * @lua NA
     */
    void setBlendFunc(const BlendFunc &blendFunc);
    
    void onDraw(const cocos2d::Mat4& transform, uint32_t flags);
    
    // Overrides
    virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
    
CC_CONSTRUCTOR_ACCESS:
    DrawNode3D();
    virtual ~DrawNode3D();
    virtual bool init() override;
    
protected:
    struct V3F_C4B
    {
        cocos2d::Vec3     vertices;
        Color4B  colors;
    };
    void ensureCapacity(int count);
    
    GLuint      _vao;
    GLuint      _vbo;
    
    int         _bufferCapacity;
    GLsizei     _bufferCount;
    V3F_C4B*    _buffer;
    
    BlendFunc   _blendFunc;
    cocos2d::CustomCommand _customCommand;
    
    bool        _dirty;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(DrawNode3D);
};

NS_CC_END

class MainScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);

	void update(float dt) override;

	virtual bool onTouchBegan(cocos2d::Touch*, cocos2d::Event*);
    virtual void onTouchMoved(cocos2d::Touch*, cocos2d::Event*);
	virtual void onTouchEnded(cocos2d::Touch*, cocos2d::Event*);

private:
    
    cocos2d::Size _VisibleSize;
    cocos2d::Vec2 _Origin;
    cocos2d::Size _WinSize;
    
    UILayer *_UILayer;
    cocos2d::Node *_GameWorld;
    
	std::unique_ptr<NetManager> _NetManager;

	char _cSendBuffer[MAX_BUFFER_SIZE_SEND];
	char _cRecvBuffer[MAX_BUFFER_SIZE_RECV];
	
    cocos2d::Sprite3D *_Orc;

    Fireball *_FireballA;
    Fireball *_FireballB;
    int _iFireballAX, _iFireballAY;
    int _iFireballBX, _iFireballBY;
    
    cocos2d::Camera *_Camera;
    
    cocos2d::DrawNode3D *_drawGrid;
};

#endif // __MAIN_SCENE_H__



