/// \file TileManager.h
/// \brief Interface for the tile manager CTileManager.

#ifndef __L4RC_GAME_TILEMANAGER_H__
#define __L4RC_GAME_TILEMANAGER_H__

#include <vector>

#include "Common.h"
#include "Settings.h"
#include "Sprite.h"
#include "GameDefines.h"
#include "Game.h"

/// \brief The tile manager.
///
/// The tile manager is responsible for the tile-based background.

class CTileManager: 
  public CCommon, 
  public LSettings
{

  private:
    CGame* m_pGame = nullptr; ///< Pointer to the game object.
    size_t m_nWidth = 0; ///< Number of tiles wide.
    size_t m_nHeight = 0; ///< Number of tiles high.

    float m_fTileSize = 0.0f; ///< Tile width and height.

    char** m_chMap = nullptr; ///< The level map.

    std::vector<BoundingBox> m_vecWalls; ///< AABBs for the walls.
    std::vector<Vector2> m_vecTurrets; ///< Turret positions.
    std::vector<Vector2> m_vecZombies; ///< Turret positions.
    std::vector<Vector2> m_vecTrees; ///< AABBs for the walls.
    Vector2 m_vPlayer; ///< Player location.
    Vector2 m_vActivity;
    Vector2 m_vHouse;
    Vector2 m_vShop;
    Vector2 m_vRadioTower;

    void MakeBoundingBoxes(); ///< Make bounding boxes for walls.

  public:
     CTileManager(size_t n, CGame* pGame) : m_fTileSize((float)n), m_pGame(pGame) { }
    ~CTileManager(); ///< Destructor.

    void LoadMapFromImageFile(char*);
    XMFLOAT4 lerp(XMFLOAT4 a, XMFLOAT4 b, float t);
    void LoadMap(char*); ///< Load a map.
    void Draw(eSprite); ///< Draw the map with a given tile.
    void DrawBoundingBoxes(eSprite); ///< Draw the bounding boxes.
    void GetObjects(std::vector<Vector2>&, Vector2&, Vector2&, Vector2&, std::vector<Vector2>&, std::vector<Vector2>&, Vector2&, Vector2&); ///< Get objects.
    
    const bool Visible(const Vector2&, const Vector2&, float) const; ///< Check visibility.
    const bool CollideWithWall(BoundingSphere, Vector2&, float&) const; ///< Object-wall collision test.
}; //CTileManager

#endif //__L4RC_GAME_TILEMANAGER_H__