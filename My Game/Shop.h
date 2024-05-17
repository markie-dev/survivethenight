/// \file House.h
/// \brief Interface for the bullet object class CBullet.

#ifndef __L4RC_GAME_SHOP_H__
#define __L4RC_GAME_SHOP_H__

#include "Object.h"

class CShop : public CObject {
protected:
    virtual void CollisionResponse(const Vector2&, float,
        CObject* = nullptr); ///< Collision response.

public:
    CShop(eSprite t, const Vector2& p); ///< Constructor.
}; //CBullet

#endif