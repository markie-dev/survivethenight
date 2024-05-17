/// \file Object.h
/// \brief Interface for the game object class CObject.

#ifndef __L4RC_GAME_OBJECT_H__
#define __L4RC_GAME_OBJECT_H__

#include "GameDefines.h"
#include "SpriteRenderer.h"
#include "Common.h"
#include "Component.h"
#include "SpriteDesc.h"
#include "BaseObject.h"
#include "EventTimer.h"

/// \brief The game object. 
///
/// The abstract representation of an object. `CObjectManager` is a friend of
/// this class so that it can access any private members as needed to manage
/// the objects without the need for reader and set functions for each private
/// or protected member variable. This class must contain public member
/// functions `move()` and `draw()` to move and draw the object, respectively.

class CObject:
  public CCommon,
  public LBaseObject
{
  friend class CObjectManager; ///< Object manager needs access so it can manage.

  protected:
    float m_fRadius = 0; ///< Bounding circle radius.

    float m_fSpeed = 0; ///< Speed.
    float m_fRotSpeed = 0; ///< Rotational speed.
    Vector2 m_vVelocity; ///< Velocity.
    bool m_bStatic = true; ///< Is static (does not move).
    bool m_bIsTarget = true; ///< Is a target.
    bool m_bIsBullet = false; ///< Is a bullet.
    bool m_bIsTurret = false; ///< Is a turret.
    bool m_bIsActivity = false;
    bool m_bIsHouse = false;
    bool m_bIsTree = false;
    bool m_bIsRadio = false;
    eSprite spriteType = eSprite::Background;
    bool m_bIsShop = false;

    LEventTimer* m_pGunFireEvent = nullptr; ///< Gun fire event.
    
    virtual void CollisionResponse(const Vector2&, float,
      CObject* = nullptr); ///< Collision response.
    virtual void ActiveCollisionResponse(const Vector2&, float,
        CObject* = nullptr); ///< Collision response.
    virtual void DeathFX(); ///< Death special effects.

    const Vector2 GetViewVector() const; ///< Compute view vector.

  public:
    CObject(eSprite, const Vector2&); ///< Constructor.
    virtual ~CObject(); ///< Destructor.

    void move(); ///< Move object.
    void draw(); ///< Draw object.

    const bool isBullet() const; ///< Is a bullet.
    const bool isTurret() const; ///< Is a turret.
    const bool isActivity() const;
    const bool isRadio() const;

    eSprite getSpriteType();
    void setSpriteType(eSprite es);

}; //CObject

#endif //__L4RC_GAME_OBJECT_H__