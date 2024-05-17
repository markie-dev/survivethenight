#include "Activity.h"
#include "ComponentIncludes.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Helpers.h"

/// Create and initialize a bullet object given its initial position.
/// \param t Sprite type of bullet.
/// \param p Initial position of bullet.

CActivity::CActivity(eSprite t, const Vector2& p) : CObject(t, p) {
    m_bIsBullet = false;
    m_bStatic = false;
    m_bIsTarget = false;
    m_bIsActivity = true;
} //constructor

void CActivity::UpdatePos(const Vector2& playerPos) {
    // Follow the player make sure the activity is always on top of the player
    m_vPos = playerPos;
}

void CActivity::HideActivity() {
    //Move the activity off screen
    m_vPos = Vector2(-1000.0f, -1000.0f);
}


/// Response to collision, which for a bullet means playing a sound and a
/// particle effect, and then dying. 
/// \param norm Collision normal.
/// \param d Overlap distance.
/// \param pObj Pointer to object being collided with (defaults to nullptr).

void CActivity::CollisionResponse(const Vector2& norm, float d, CObject* pObj) {
} //CollisionResponse

/// Create a smoke particle effect to mark the death of the bullet.