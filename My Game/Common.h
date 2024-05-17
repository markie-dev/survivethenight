/// \file Common.h
/// \brief Interface for the class CCommon.

#ifndef __L4RC_GAME_COMMON_H__
#define __L4RC_GAME_COMMON_H__

#include "Defines.h"

//forward declarations to make the compiler less stroppy

class CObjectManager; 
class LSpriteRenderer;
class LParticleEngine2D;
class CTileManager;
class CPlayer;
class CActivity;
class CHouse;
class CShop;
class CRadioTower;

/// \brief The common variables class.
///
/// CCommon is a singleton class that encapsulates things that are common to
/// different game components, including game state variables. Making it a
/// singleton class means that we can avoid passing its member variables
/// around as parameters, which makes the code minisculely faster, and more
/// importantly, makes the code more readable by reducing function clutter.

class CCommon{
  protected:  
    static LSpriteRenderer* m_pRenderer; ///< Pointer to renderer.
    static CObjectManager* m_pObjectManager; ///< Pointer to object manager.
    static LParticleEngine2D* m_pParticleEngine; ///< Pointer to particle engine.
    static CTileManager* m_pTileManager; ///< Pointer to tile manager. 

    static bool m_bDrawAABBs; ///< Draw AABB flag.
    static bool m_bGodMode; ///< God mode flag.
    
    static bool gotBattery;
    static bool gotAntenna;
    static bool gotLogicBoard;

    static Vector2 m_vWorldSize; ///< World height and width.
    static CPlayer* m_pPlayer; ///< Pointer to player character.
    static CActivity* m_pActivity;
    static CHouse* m_pHouse;
    static CShop* m_pShop;
    static CRadioTower* m_pRadioTower;

}; //CCommon

#endif //__L4RC_GAME_COMMON_H__