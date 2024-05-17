/// \file Turret.h
/// \brief Interface for the turret object class CTurret.

#ifndef __L4RC_GAME_ZOMBIE_H__
#define __L4RC_GAME_ZOMBIE_H__

#include "Object.h"

/// \brief The turret object. 
///
/// CTurret is the abstract representation of a turret object.

class CZombie : public CObject {
protected:
    Vector2 m_vWanderDirection; ///< Current wander direction.
    const UINT m_nMaxHealth = 4; ///< Maximum health.
    UINT m_nHealth = m_nMaxHealth; ///< Current health.
    bool HasBeenInActivity;
    bool HasBeenShot;
    Vector2 m_vKnockbackVelocity; // Knockback velocity

    void RotateTowards(const Vector2&); ///< Swivel towards position.
    void MoveTowards(const Vector2&);
    virtual void CollisionResponse(const Vector2&, float, CObject* = nullptr); ///< Collision response.
    virtual void DeathFX(); ///< Death special effects.

public:
    CZombie(const Vector2& p); ///< Constructor.
    virtual void move(); ///< Move turret.
    int m_frameCounter = 0;
    bool m_bIsAlternateSprite = false;
    bool m_bIsMoving = false;
}; //CBullet

#endif //__L4RC_GAME_TURRET_H__
#pragma once
