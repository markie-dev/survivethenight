/// \file ObjectManager.h
/// \brief Interface for the object manager CObjectManager.

#ifndef __L4RC_GAME_OBJECTMANAGER_H__
#define __L4RC_GAME_OBJECTMANAGER_H__

#include "BaseObjectManager.h"
#include "Object.h"
#include "Common.h"

/// \brief The object manager.
///
/// A collection of all of the game objects.

class CObjectManager: 
  public LBaseObjectManager<CObject>,
  public CCommon
{
  private:
    void BroadPhase(); ///< Broad phase collision detection and response.
    void NarrowPhase(CObject*, CObject*); ///< Narrow phase collision detection and response.

  public:
    CObject* create(eSprite, const Vector2&); ///< Create new object.
    
    virtual void draw(); ///< Draw all objects.

    void clearRadios();

    void FireGun(CObject*, eSprite); ///< Fire object's gun.
    const size_t GetNumTurrets() const; ///< Get number of turrets in object list.
}; //CObjectManager

#endif //__L4RC_GAME_OBJECTMANAGER_H__
