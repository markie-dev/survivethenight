/// \file Turret.cpp
/// \brief Code for the turret object class CTurret.

#include "Zombie.h"
#include "ComponentIncludes.h"
#include "ObjectManager.h"
#include "TileManager.h"
#include "Player.h"
#include "Helpers.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Object.h"
#include <iostream>

/// Create and initialize a turret object given its position.
/// \param p Position of turret.

CZombie::CZombie(const Vector2& p) : CObject(eSprite::Zombie2, p) {
    m_bStatic = false; //turrets are static
    m_bIsTurret = true; //flag for collision detection
    m_bIsActivity = false;
    HasBeenInActivity = false;
    HasBeenShot = false;
    m_vWanderDirection = Vector2(-300, 900);
    m_vWanderDirection.Normalize();
} //constructor

/// Rotate the turret and fire the gun at at the closest available target if
/// there is one, and rotate the turret at a constant speed otherwise.

void CZombie::move() {
    m_frameCounter++;
    if (m_pPlayer && m_bIsTurret) {
        if (m_frameCounter % 25 == 0) {
            if (m_bIsAlternateSprite) {
                m_nSpriteIndex = (UINT)eSprite::Zombie2; // Alternate sprite 1
                m_bIsAlternateSprite = false;
            }
            else {
                m_nSpriteIndex = (UINT)eSprite::Zombie2; // Alternate sprite 2
                m_bIsAlternateSprite = true;
            }
        }
    }

    // Apply a fraction of the knockback velocity
    const float knockbackFraction = 0.1f; // Adjust this to control the speed of the knockback
    m_vPos += m_vKnockbackVelocity * knockbackFraction;

    // Reduce the knockback velocity
    m_vKnockbackVelocity *= (1.0f - knockbackFraction);

    if (m_pPlayer && (HasBeenInActivity == true || HasBeenShot == true)) { // Safety check
        const float r = ((CZombie*)m_pPlayer)->m_fRadius; // Player radius
        if (m_pTileManager->Visible(m_vPos, m_pPlayer->m_vPos, r)) // Player visible
        {
            // Rotate towards the player
            RotateTowards(m_pPlayer->m_vPos);

            // Move towards the player
            MoveTowards(m_pPlayer->m_vPos);
        }
        else {
            // Rotate towards the wander direction
            RotateTowards(m_vPos + m_vWanderDirection);

            // Wander around
            MoveTowards(m_vPos + m_vWanderDirection);
        }
    }
    else {
        // Change direction every 100 frames
        /*static int counter = 0;
        if (counter++ % 100 == 0) {
            m_vWanderDirection = Vector2(rand() - RAND_MAX / 2, rand() - RAND_MAX / 2);
            m_vWanderDirection.Normalize();
        }*/

        // Rotate towards the wander direction
        RotateTowards(m_vPos + m_vWanderDirection);

        // Wander around
        MoveTowards(m_vPos + m_vWanderDirection);
    }

    m_fRoll += 0.2f * m_fRotSpeed * XM_2PI * m_pTimer->GetFrameTime(); // Rotate
    NormalizeAngle(m_fRoll); // Normalize to [-pi, pi] for accuracy
    if (m_frameCounter == 50) {
        m_frameCounter = 0;
    }
}

/// Rotate the turret towards a point and file the gun if it is facing
/// sufficiently close to it.
/// \param pos Target point.
/// 
void CZombie::MoveTowards(const Vector2& playerPos) {
    Vector2 direction = playerPos - m_vPos; // Vector from turret to player
    const float distance = direction.Length(); // Distance to player

    // Define a movement speed
    const float movementSpeed = 1.0f;

    if (distance > 0.0f) {
        // Normalize the direction vector using DirectX's Normalize function
        direction.Normalize();

        // Calculate the new position based on movement speed and direction
        Vector2 newPosition = m_vPos + direction * movementSpeed;

        // Update the turret's position
        m_vPos = newPosition;
    }
}



void CZombie::RotateTowards(const Vector2& pos) {
    const Vector2 v = pos - m_vPos; //vector from target to turret
    const float theta = atan2f(v.y, v.x); //orientation of that vector
    float diff = m_fRoll - theta; //difference with turret orientation
    NormalizeAngle(diff); //normalize to [-pi, pi]

    //set rotation speed from diff

    const float fAngleDelta = 0.05f; //allowable angle discrepancy
    const float fTrackingSpeed = 2.0f; //rotation speed when tracking

    if (diff > fAngleDelta)m_fRotSpeed = -fTrackingSpeed; //clockwise
    else if (diff < -fAngleDelta)m_fRotSpeed = fTrackingSpeed; //counterclockwise
    else m_fRotSpeed = 0; //stop rotating

    //fire gun if pointing approximately towards target

    //if(fabsf(diff) < fAngleDelta && m_pGunFireEvent->Triggered())
      //m_pObjectManager->FireGun(this, eSprite::Bullet2);
} //RotateTowards

/// Response to collision. 
/// \param norm Collision normal.
/// \param d Overlap distance.
/// \param pObj Pointer to object being collided with (defaults to `nullptr`,
/// which means collision with a wall).

void CZombie::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
    if (m_bDead)return; //already dead, bail out 

    m_vWanderDirection = Vector2(rand() - RAND_MAX / 2, rand() - RAND_MAX / 2);
    m_vWanderDirection.Normalize();

    //If pObj is a Activity, then do nothing
    if (pObj && pObj->isActivity()) {
        HasBeenInActivity = true;
        return;
    }
    else {
        const Vector2 vOverlap = d * norm; //overlap in direction of this
        m_vPos += vOverlap; //back off this object

        if (pObj && pObj->isBullet()) { //collision with bullet
            // Calculate the pushback velocity based on the collision normal
            const float pushbackSpeed = 25.0f; // Adjust the pushback speed as needed
            m_vKnockbackVelocity = norm * pushbackSpeed; // Store the pushback velocity

            HasBeenShot = true;
            if (--m_nHealth == 0) { //health decrements to zero means death 
                m_pAudio->play(eSound::Boom); //explosion
                m_bDead = true; //flag for deletion from object list
                DeathFX(); //particle effects
            } //if

            else { //not a death blow
                m_pAudio->play(eSound::Clang); //impact sound
                const float f = 0.5f + 0.5f * (float)m_nHealth / m_nMaxHealth; //health fraction
                m_f4Tint = XMFLOAT4(1.0f, f, f, 0); //redden the sprite to indicate damage
            } //else
        } //if
    }
} //CollisionResponse

/// Perform a particle effect to mark the death of the turret.

void CZombie::DeathFX() {
    LParticleDesc2D d; //particle descriptor
    d.m_vPos = m_vPos; //center particle at turret center

    d.m_nSpriteIndex = (UINT)eSprite::Smoke;
    d.m_fLifeSpan = 2.0f;
    d.m_fMaxScale = 4.0f;
    d.m_fScaleInFrac = 0.5f;
    d.m_fFadeOutFrac = 0.8f;
    d.m_fScaleOutFrac = 0;
    m_pParticleEngine->create(d);

    d.m_nSpriteIndex = (UINT)eSprite::Spark;
    d.m_fLifeSpan = 0.5f;
    d.m_fMaxScale = 1.5f;
    d.m_fScaleInFrac = 0.4f;
    d.m_fScaleOutFrac = 0.3f;
    d.m_fFadeOutFrac = 0.5f;
    d.m_f4Tint = XMFLOAT4(Colors::Orange);
    m_pParticleEngine->create(d);
} //DeathFX