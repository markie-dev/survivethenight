#include "House.h"
#include "ComponentIncludes.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Helpers.h"

/// Create and initialize a bullet object given its initial position.
/// \param t Sprite type of bullet.
/// \param p Initial position of bullet.

CHouse::CHouse(eSprite t, const Vector2& p) : CObject(t, p) {
    m_bIsBullet = false;
    m_bStatic = true;
    m_bIsTarget = false;
    m_bIsActivity = false;
    m_bIsHouse = true;
} //constructor

void CHouse::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
} //CollisionResponse

/// Create a smoke particle effect to mark the death of the bullet.