/// \file ObjectManager.cpp
/// \brief Code for the the object manager class CObjectManager.

#include "ObjectManager.h"
#include "ComponentIncludes.h"

#include "Player.h"
#include "Turret.h"
#include "Bullet.h"
#include "Zombie.h"
#include "ParticleEngine.h"
#include "Helpers.h"
#include "GameDefines.h"
#include "TileManager.h"
#include "Activity.h"

/// Create an object and put a pointer to it at the back of the object list
/// `m_stdObjectList`, which it inherits from `LBaseObjectManager`.
/// \param t Sprite type.
/// \param pos Initial position.
/// \return Pointer to the object created.

CObject* CObjectManager::create(eSprite t, const Vector2& pos){
  CObject* pObj = nullptr;

  switch(t){ //create object of type t
    case eSprite::Player:  pObj = new CPlayer(pos); break;
    case eSprite::Turret:  pObj = new CTurret(pos); break;
    case eSprite::Zombie2:  pObj = new CZombie(pos); break;
    case eSprite::Bullet:  pObj = new CBullet(eSprite::Bullet,  pos); break;
    case eSprite::Bullet2: pObj = new CBullet(eSprite::Bullet2, pos); break;
    case eSprite::Activity:pObj = new CActivity(eSprite::Activity, pos); break;
    case eSprite::Battery: pObj = new CObject(eSprite::Battery, pos); pObj->setSpriteType(eSprite::Battery); break;
    case eSprite::Antenna: pObj = new CObject(eSprite::Antenna, pos); pObj->setSpriteType(eSprite::Antenna); break;
    case eSprite::LogicBoard: pObj = new CObject(eSprite::LogicBoard, pos); pObj->setSpriteType(eSprite::LogicBoard); break;
    //case eSprite::Radio1: pObj = new CObject(eSprite::Radio1, pos); pObj->setSpriteType(eSprite::Radio1); break;
    default: pObj = new CObject(t, pos);
  } //switch
  
  m_stdObjectList.push_back(pObj); //push pointer onto object list
  return pObj; //return pointer to created object
} //create

void CObjectManager::clearRadios() {
    for (CObject* pObj : m_stdObjectList) //for each object
        if (pObj->m_bIsRadio) { //if the object is an elixir
            pObj->m_bDead = true;
        }
}

/// Draw the tiled background and the objects in the object list.

void CObjectManager::draw(){
  m_pTileManager->Draw(eSprite::Tile); //draw tiled background

  if(m_bDrawAABBs)
    m_pTileManager->DrawBoundingBoxes(eSprite::Line); //draw AABBs

  LBaseObjectManager::draw();
} //draw

/// Perform collision detection and response for each object with the world
/// edges and for all objects with another object, making sure that each pair
/// of objects is processed only once.

void CObjectManager::BroadPhase(){
  LBaseObjectManager::BroadPhase(); //collide with other objects

  //collide with walls

  for(CObject* pObj: m_stdObjectList) //for each object
    if(!pObj->m_bDead){ //for each non-dead object, that is
      for(int i=0; i<2; i++){ //can collide with 2 edges simultaneously
        Vector2 norm; //collision normal
        float d = 0; //overlap distance
        BoundingSphere s(Vector3(pObj->m_vPos), pObj->m_fRadius);
        
        if(m_pTileManager->CollideWithWall(s, norm, d)) //collide with wall
          pObj->CollisionResponse(norm, d); //respond 
      } //for
  } //for
} //BroadPhase

/// Perform collision detection and response for a pair of objects. Makes
/// use of the helper function Identify() because this function may be called
/// with the objects in an arbitrary order.
/// \param p0 Pointer to the first object.
/// \param p1 Pointer to the second object.

void CObjectManager::NarrowPhase(CObject* p0, CObject* p1){
  Vector2 vSep = p0->m_vPos - p1->m_vPos; //vector from *p1 to *p0
  const float d = p0->m_fRadius + p1->m_fRadius - vSep.Length(); //overlap

  if(d > 0.0f){ //bounding circles overlap
    vSep.Normalize(); //vSep is now the collision normal

    p0->CollisionResponse( vSep, d, p1); //this changes separation of objects
    p1->CollisionResponse(-vSep, d, p0); //same separation and opposite normal
  } //if
} //NarrowPhase

/// Create a bullet object and a flash particle effect. It is assumed that the
/// object is round and that the bullet appears at the edge of the object in
/// the direction that it is facing and continues moving in that direction.
/// \param pObj Pointer to an object.
/// \param bullet Sprite type of bullet.

void CObjectManager::FireGun(CObject* pObj, eSprite bullet){
  m_pAudio->play(eSound::Gun);

  const Vector2 view = pObj->GetViewVector(); //firing object view vector
  const float w0 = 0.5f*m_pRenderer->GetWidth(pObj->m_nSpriteIndex); //firing object width
  const float w1 = m_pRenderer->GetWidth(bullet); //bullet width
  const Vector2 pos = pObj->m_vPos + (w0 + w1)*view; //bullet initial position

  //create bullet object

  CObject* pBullet = create(bullet, pos); //create bullet
  
  const Vector2 norm = VectorNormalCC(view); //normal to view direction
  const float m = 2.0f*m_pRandom->randf() - 1.0f; //random deflection magnitude
  const Vector2 deflection = 0.01f*m*norm; //random deflection

  pBullet->m_vVelocity = pObj->m_vVelocity + 450.0f*(view + deflection);
  pBullet->m_fRoll = pObj->m_fRoll; 

  //particle effect for gun fire
  
  LParticleDesc2D d;

  d.m_nSpriteIndex = (UINT)eSprite::Spark;
  d.m_vPos = pos;
  d.m_vVel = pObj->m_fSpeed*view;
  d.m_fLifeSpan = 0.25f;
  d.m_fScaleInFrac = 0.4f;
  d.m_fFadeOutFrac = 0.5f;
  d.m_fMaxScale = 0.5f;
  d.m_f4Tint = XMFLOAT4(Colors::Yellow);
  
  m_pParticleEngine->create(d);
} //FireGun

/// Reader function for the number of turrets. 
/// \return Number of turrets in the object list.

const size_t CObjectManager::GetNumTurrets() const{
  size_t n = 0; //number of turrets
  
  for(CObject* pObj: m_stdObjectList) //for each object
    if(pObj->isTurret())
      n++;

  return n;
} //GetNumTurrets