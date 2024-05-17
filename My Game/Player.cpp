/// \file Player.cpp
/// \brief Code for the player object class CPlayer.

#include "Player.h"
#include "ComponentIncludes.h"
#include "Helpers.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "ObjectManager.h"
#include <iostream>

/// Create and initialize an player object given its initial position.
/// \param p Initial position of player.

CPlayer::CPlayer(const Vector2& p): CObject(eSprite::Player, p){ 
  m_bIsTarget = true;
  m_bStatic = false;
  m_bIsActivity = false;
} //constructor

/// Move and rotate in response to device input. The amount of motion and
/// rotation speed is proportional to the frame time.

void CPlayer::move() {
    const float t = m_pTimer->GetFrameTime(); // time

    // Move forwards or backwards based on m_fSpeed. 
    // Positive m_fSpeed moves upwards, negative moves downwards
    m_vPos.y += m_fSpeed * t;

    // Apply a fraction of the knockback velocity
    const float knockbackFraction = 0.1f; // Adjust this to control the speed of the knockback
    m_vPos += m_vKnockbackVelocity * knockbackFraction;

    // Reduce the knockback velocity
    m_vKnockbackVelocity *= (1.0f - knockbackFraction);

    // Strafe
    const Vector2 norm = VectorNormalCC(GetViewVector()); // normal to view vector
    float strafeSpeed = 360.0f; // base strafe speed

    if (m_bIsWalking) {
        strafeSpeed *= 0.5f; // reduce strafe speed by half when walking
        
    }

    const float delta = strafeSpeed * t; // change in position for strafing

    if (m_bStrafeRight) m_vPos.x += delta; // strafe right
    else if (m_bStrafeLeft) m_vPos.x -= delta; // strafe left

    m_frameCounter++;
    if ((m_fSpeed != 0.0f || m_bStrafeRight == true || m_bStrafeLeft == true) && m_frameCounter % 25 == 0) {
        if (m_bIsAlternateSprite) {
            m_pPlayer->m_nSpriteIndex = (UINT)eSprite::Player; // walking sprite 1
            m_bIsAlternateSprite = false;
        }
        else {
            m_pPlayer->m_nSpriteIndex = (UINT)eSprite::Player2; // walking sprite 2
            m_bIsAlternateSprite = true;
        }
    }

    m_bStrafeLeft = m_bStrafeRight = false; // reset strafe flags
    m_bIsWalking = false; // reset walking flag
}

// Method to set the walking state, to be called from Game.cpp when Shift is held
void CPlayer::SetWalking(bool isWalking) {
    m_bIsWalking = isWalking;
}

//Get the player's health
UINT CPlayer::GetHealth() const {
    //If nullptr is returned, then the player is dead, so return 0
    if (m_pPlayer == nullptr) {
		return 0;
	}
	return m_nHealth;
}

//Set the player's health
void CPlayer::SetHealth(UINT health) {
	//If the health is set to 0, then the player is dead, so set the common player pointer to nullptr
    if (health == 0) {
		m_pPlayer = nullptr;
	}
    if (health > m_nMaxHealth) {
		m_nHealth = m_nMaxHealth;
	}
	else {m_nHealth = health;}
}

int CPlayer::GetHungerCount() const {
    if (m_pPlayer == nullptr) {
        return 0;
    }
    return m_nHungerCount;
}

void CPlayer::SetHungerCount(int count) {
	//Make sure the hunger count doesn't go below 0 and above three
    if (count < 0) {
		m_nHungerCount = 0;
	}
    else if (count > 3) {
		m_nHungerCount = 3;
	}
    else {
		m_nHungerCount = count;
	}
}

/// Response to collision. If the object being collided with is a bullet, then
/// play a sound, otherwise call `CObject::CollisionResponse` for the default
/// collision response.
/// \param norm Collision normal.
/// \param d Overlap distance.
/// \param pObj Pointer to object being collided with (defaults to `nullptr`,
/// which means collision with a wall).
void CPlayer::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
    if (m_bDead) return; // Already dead, bail out

    if (pObj) {
        if (pObj->isBullet()) {
            // Handle collision with a bullet

        }
        else if (pObj->isActivity()) {
            
        }
        else if (pObj->isTurret()) {
            // Handle collision with a turret
            // Decrease player health by a smaller amount
            const int healthDecreaseAmount = 3; // Adjust as needed

            if (m_nHealth > healthDecreaseAmount) {
                m_nHealth -= healthDecreaseAmount; // Decrease health by the specified amount
                // Play a sound
                m_pAudio->play(eSound::Grunt);
            }
            else {
                m_nHealth = 0; // Ensure health doesn't go negative
            }

            // Update player's color based on remaining health
            const float f = 0.5f + 0.5f * (float)m_nHealth / m_nMaxHealth; // Health fraction
            m_f4Tint = XMFLOAT4(1.0f, f, f, 0); // Redden the health indicator

            // Calculate the pushback velocity based on the collision normal
            const float pushbackSpeed = 50.0f; // Adjust the pushback speed as needed
            m_vKnockbackVelocity = norm * pushbackSpeed; // Store the pushback velocity

            if (m_nHealth == 0) { // Player dies when health reaches zero
                m_pAudio->play(eSound::Boom); // Explosion sound
                m_bDead = true; // Flag for deletion from object list
                DeathFX(); // Particle effects
                m_pPlayer = nullptr; // Clear common player pointer
            }
        }
        
        else if (pObj->isRadio()) {
            //insert code for what you want to happen when the radio piece is picked up
            if (pObj->getSpriteType() == eSprite::Battery) {
                gotBattery = true;
                m_pObjectManager->clearRadios();
            }
            if (pObj->getSpriteType() == eSprite::Antenna) {
                gotAntenna = true;
                m_pObjectManager->clearRadios();
            }
            if (pObj->getSpriteType() == eSprite::LogicBoard) {
                gotLogicBoard = true;
                m_pObjectManager->clearRadios();
            }
        }   
    }


    CObject::CollisionResponse(norm, d, pObj); // Default collision response
}

/// Perform a particle effect to mark the death of the player.

void CPlayer::DeathFX(){
  LParticleDesc2D d; //particle descriptor
  d.m_vPos = m_vPos; //center particle at player center

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
  d.m_f4Tint = XMFLOAT4(Colors::OrangeRed);
  m_pParticleEngine->create(d);
} //DeathFX

/// Set the strafe left flag. This function will be called in response to
/// device inputs.

void CPlayer::StrafeLeft(){
  m_bStrafeLeft = true;
} //StrafeLeft

/// Set the strafe right flag. This function will be called in response to
/// device inputs.

void CPlayer::StrafeRight(){
  m_bStrafeRight = true;
} //StrafeRight

/// Set the strafe back flag. This function will be called in response to
/// device inputs.

void CPlayer::StrafeBack(){
  m_bStrafeBack = true;
} //StrafeBack

/// Set the object's speed, assuming that the object moves in the direction of
/// its view vector. This function will be called in response to device inputs.
/// \param speed Speed.

void CPlayer::SetSpeed(const float speed){
  m_fSpeed = 10*speed;
} //SetSpeed

/// Set the object's rotational speed in revolutions per second. This function
/// will be called in response to device inputs.
/// \param speed Rotational speed in RPS.

void CPlayer::SetRotSpeed(const float speed){
  m_fRotSpeed = 5*speed;
} //SetRotSpeed

float CPlayer::DegToRad(float degrees) {
    return degrees * (M_PI / 180.0f);
}

void CPlayer::SetRotation(float degrees) {
    m_fRoll = DegToRad(degrees);
}

/// Reader function for position.
/// \return Position.

const Vector2& CPlayer::GetPos() const{
  return m_vPos;
} //GetPos

/// Reader function for rotation.
/// \return Rotation in degrees.

float CPlayer::GetRotation() const {
    return m_fRoll;
} // GetRotation

float CPlayer::GetRotSpeed() const {
    return m_fRotSpeed;
} // GetRotation