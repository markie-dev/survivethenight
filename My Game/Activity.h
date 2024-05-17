/// \file Activity.h
/// \brief Interface for the bullet object class CBullet.

#ifndef __L4RC_GAME_ACTIVITY_H__
#define __L4RC_GAME_ACTIVITY_H__

#include "Object.h"

class CActivity : public CObject {
protected:
    virtual void CollisionResponse(const Vector2&, float,
        CObject* = nullptr); ///< Collision response.

public:
    CActivity(eSprite t, const Vector2& p); ///< Constructor.
    void UpdatePos(const Vector2&);
    void HideActivity();
}; //CBullet

#endif