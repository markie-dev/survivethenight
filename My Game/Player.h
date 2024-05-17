/// \file Player.h
/// \brief Interface for the player object class CPlayer.

#ifndef __L4RC_GAME_PLAYER_H__
#define __L4RC_GAME_PLAYER_H__

#include "Object.h"

/// \brief The player object. 
///
/// The abstract representation of the player object. The player differs from
/// the other objects in the game in that it moves in respond to device inputs.

class CPlayer: public CObject{
  protected:  
    const UINT m_nMaxHealth = 12; ///< Maximum health.
    UINT m_nHealth = m_nMaxHealth; ///< Current health.

    bool m_bStrafeLeft = false; ///< Strafe left.
    bool m_bStrafeRight = false; ///< Strafe right.
    bool m_bStrafeBack = false; ///< Strafe back.
    bool m_bIsWalking = false; ///< Is walking.
    Vector2 m_vKnockbackVelocity; // Knockback velocity
    int m_nHungerCount = 0;
    
    virtual void CollisionResponse(const Vector2&, float, CObject* = nullptr); ///< Collision response.
    virtual void DeathFX(); ///< Death special effects.

  public:
    CPlayer(const Vector2& p); ///< Constructor.
    int m_frameCounter = 0;
    bool m_bIsAlternateSprite = false;

    virtual void move(); ///< Move player object.

    void SetSpeed(const float speed); ///< Set speed.
    void SetRotSpeed(const float speed); ///< Set rotational velocity.
    void SetRotation(float degrees);
    float DegToRad(float degrees);

    void StrafeLeft(); ///< Strafe left.
    void StrafeRight(); ///< Strafe right.
    void StrafeBack(); ///< Strafe back.
    void SetWalking(bool isWalking); ///< Set walking.
    
    const Vector2& GetPos() const; ///< Get position.
    UINT GetHealth() const; ///< Get health.
    void SetHealth(UINT health); ///< Set health.
    float GetRotation() const;
    float GetRotSpeed() const;
    int GetHungerCount() const;
    void SetHungerCount(int count);
}; //CPlayer

#endif //__L4RC_GAME_PLAYER_H__
