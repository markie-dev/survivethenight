/// \file TileManager.cpp
/// \brief Code for the tile manager CTileManager.

#include "TileManager.h"
#include "SpriteRenderer.h"
#include "Abort.h"
#define STBI_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include "Game.h"

/// Construct a tile manager using square tiles, given the width and height

/// Delete the memory used for storing the map.
CTileManager::~CTileManager(){
  for(size_t i=0; i<m_nHeight; i++)
    delete [] m_chMap[i];

  delete [] m_chMap;
} //destructor

/// Make the AABBs for the walls. Care is taken to use the longest horizontal
/// and vertical AABBs possible so that there aren't so many of them.

void CTileManager::LoadMapFromImageFile(char* filename) {
    m_vecTurrets.clear(); //clear turrets from previous level
    m_vecTrees.clear(); //clear trees from previous level


    if (m_chMap != nullptr) { //unload any previous maps
        for (int i = 0; i < m_nHeight; i++)
            delete[] m_chMap[i];
        delete[] m_chMap;
    } //if

    //read map file into a byte buffer 

    int channels = 0, w = 0, h = 0;
    unsigned char* buffer = stbi_load(filename, &w, &h, &channels, 0);
    m_nWidth = (size_t)w;
    m_nHeight = (size_t)h;

    //allocate space for the map 

    m_chMap = new char* [m_nHeight];

    for (int i = 0; i < m_nHeight; i++)
        m_chMap[i] = new char[m_nWidth];

    //load the map information from the buffer to the map

    int index = 0;

    for (int i = 0; i < m_nHeight; i++)
        for (int j = 0; j < m_nWidth; j++) {
            m_chMap[i][j] =
                (buffer[index] == 0 && buffer[index + 1] == 0 && buffer[index + 2] == 0) ? 'W' : 'F'; //load character into map
            if (buffer[index] == 0 && buffer[index + 1] == 255 && buffer[index + 2] == 0)
                m_vecTurrets.push_back(Vector2((float)j, m_nHeight - (float)i) * m_fTileSize);
            index += channels;
        } //for

      //finish up

    m_vWorldSize = Vector2((float)m_nWidth, (float)m_nHeight) * m_fTileSize;
    MakeBoundingBoxes();

    stbi_image_free(buffer);
} //LoadMapFromImageFile

void CTileManager::MakeBoundingBoxes(){
  m_vecWalls.clear(); //no walls yet

  BoundingBox aabb; //current bounding box
  const float t = m_fTileSize; //shorthand for tile width and height
  const Vector3 vTileExtents = 0.5f*t*Vector3::One; //tile extents extended to 3D
  BoundingBox b; //single-tile bounding box
  b.Extents = vTileExtents; //bounding box extents cover a single tile

  //horizontal walls with more than one tile

  const Vector2 vstart(t/2, t*(m_nHeight - 0.5f)); //start position
  Vector2 pos = vstart; //set current position to start position
  
  for(size_t i=0; i<m_nHeight; i++){ //for each row
    size_t j = 0; //column index
    pos.x = vstart.x; //set start position x coordinate

    while(j < m_nWidth){ //for each column
      while(j < m_nWidth && m_chMap[i][j] != ('W')){ //skip over non-wall entries
        j++; //next column
        pos.x += t; //move right by tile width
      } //while

      if(j < m_nWidth){ //found leftmost tile in a wall
        aabb.Center = Vector3(pos.x, pos.y, 0); //bounding box center
        aabb.Extents = vTileExtents; //bounding box extents
        j++; //next column
        pos.x += t; //move right by tile width
      } //if

      bool bSingleTile = true; //as far as we know, this is a single-tile wall

      while(j < m_nWidth && m_chMap[i][j] == 'W'){ //for each adjacent wall tile
        b.Center = Vector3(pos.x, pos.y, 0); //bounding box center
        BoundingBox::CreateMerged(aabb, aabb, b); //merge b into aabb
        bSingleTile = false; //the wall now has at least 2 tiles in it
        j++; //next column
        pos.x += t; //move right by tile width
      } //while

      if(!bSingleTile) //skip this wall if it is a single tile
        m_vecWalls.push_back(aabb); //add horizontal wall to the list
    } //while

    pos.y -= t; //next row
  } //for

  //vertical walls, the single tiles get caught here

  pos = vstart; //reset current position to start position
  
  for(size_t j=0; j<m_nWidth; j++){ //for each column
    size_t i = 0; //row index
    pos.y = vstart.y; //set start position y coordinate

    while(i < m_nHeight){ //for each row
      while(i < m_nHeight && m_chMap[i][j] != 'W'){ //skip over non-wall entries
        i++; //next row
        pos.y -= t; //move down by tile height
      } //while

      if(i < m_nHeight){ //found topmost tile in a wall
        aabb.Center = Vector3(pos.x, pos.y, 0); //bounding box center
        aabb.Extents = vTileExtents; //bounding box extents
        i++; //next row
        pos.y -= t; //move down by tile height
      } //if
      
      bool bSingleTile = true; //as far as we know, this is a single-tile wall

      while(i < m_nHeight && m_chMap[i][j] == 'W'){ //for each adjacent wall tile
        b.Center = Vector3(pos.x, pos.y, 0); //bounding box center
        BoundingBox::CreateMerged(aabb, aabb, b); //merge b into aabb
        bSingleTile = false; //the wall now has at least 2 tiles in it
        i++; //next row
        pos.y -= t; //move down by tile height
      } //while
      
      if(!bSingleTile) //skip this wall if it is a single tile
        m_vecWalls.push_back(aabb); //add horizontal wall to the list
    } //while

    pos.x += t; //next column
  } //for

  //orphaned single tiles

  pos = vstart; //reset current position to start position
  
  for(size_t i=0; i<m_nHeight; i++){ //for each row
    for(size_t j=0; j<m_nWidth; j++){ //for each column
      if(m_chMap[i][j] == 'W' && //is a wall tile and
        ((i == 0 || m_chMap[i - 1][j] != 'W') && //has non-wall tile below or is on edge
         (i == m_nHeight - 1 || m_chMap[i + 1][j] != 'W') && //has non-wall tile above or is on edge
         (j == 0 || m_chMap[i][j - 1] != 'W') && //has non-wall tile at left or is on edge
         (j == m_nWidth - 1 || m_chMap[i][j + 1] != 'W') //has non-wall tile at right or is on edge
        )
      ){    
        b.Center = Vector3(pos.x, pos.y, 0); //bounding box center
        m_vecWalls.push_back(b); //add single-tile wall to the list
      } //if

      pos.x += t; //next column
    } //for
    
    pos.x = vstart.x; //first column
    pos.y -= t; //next row
  } //for
} //MakeBoundingBoxes

/// Delete the old map (if any), allocate the right sized chunk of memory for
/// the new map, and read it from a text file.
/// \param filename Name of the map file.

void CTileManager::LoadMap(char* filename){
  if(m_chMap != nullptr){ //unload any previous maps
    for(size_t i=0; i<m_nHeight; i++)
      delete [] m_chMap[i];

    delete [] m_chMap;
  } //if

  m_vecTurrets.clear(); //clear out the turret list
  m_vecTrees.clear(); //clear out the tree list

  FILE *input; //input file handle

  fopen_s(&input, filename, "rb"); //open the map file
  if(input == nullptr) //abort if it's missing
    ABORT("Map &s not found.", filename); //panic

  //read map file into a character buffer 

  fseek(input, 0, SEEK_END); //seek to end of map file
  const size_t n = ftell(input); //get file size in bytes
  rewind(input); //seek to start of file

  char *buffer = new char[n + 1]; //temporary character buffer
  fread(buffer, n, 1, input); //read the whole thing in a chunk
  fclose(input); //close the map file, we're done with it

  //get map width and height into m_nWidth and m_nHeight

  m_nWidth = 0; 
  m_nHeight = 0;
  size_t w = 0; //width of current row
  bool bFirstLine = true;

  for(size_t i=0; i<n; i++){
    if(buffer[i] != '\n')
      w++; //skip characters until the end of line
    else{
      if(w == 0)ABORT("Panic!"); //should never happen

      if(w != m_nWidth && !bFirstLine && w != 0) //not the same length as the previous one
        ABORT("Line %d of map is not the same length as the previous one.", m_nHeight);

      m_nWidth = w; w = 0; m_nHeight++; //next line
      bFirstLine = false; //the next line is not the first
    } //else
  } //for

  //m_nWidth--;

  //allocate space for the map 
  
  m_chMap = new char*[m_nHeight];

  for(size_t i=0; i<m_nHeight; i++)
    m_chMap[i] = new char[m_nWidth];

  //load the map information from the buffer to the map

  size_t index = 0; //index into character buffer
  
  for(size_t i=0; i<m_nHeight; i++){
    for(size_t j=0; j<m_nWidth; j++){
      const char c = buffer[index];

      if(c == 'T'){ //Turret
        m_chMap[i][j] = 'F'; //floor tile
        const Vector2 pos = m_fTileSize*Vector2(j + 0.5f, m_nHeight - i - 0.5f);
        m_vecTurrets.push_back(pos);
      } //if
      if (c == 'Z') { //Zombie
          m_chMap[i][j] = 'F'; //floor tile
          const Vector2 pos = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          m_vecZombies.push_back(pos);
      } //if
      if (c == 'E') {   //Tree
          m_chMap[i][j] = 'F'; //floor tile
          const Vector2 pos = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
          m_vecTrees.push_back(pos);
      } //if

      if (c == 'H') {   //House
          m_chMap[i][j] = 'F'; //floor tile
          m_vHouse = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
      } //if
      if (c == 'S') {   //House
          m_chMap[i][j] = 'F'; //floor tile
          m_vShop = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
      } //if

      if (c == 'D') {   //House
          m_chMap[i][j] = 'F'; //floor tile
          m_vRadioTower = m_fTileSize * Vector2(j + 0.5f, m_nHeight - i - 0.5f);
      } //if

      else if(c == 'P'){    //Player
        m_chMap[i][j] = 'F'; //floor tile
        m_vPlayer = m_fTileSize*Vector2(j + 0.5f, m_nHeight - i - 0.5f);
        m_vActivity = m_vPlayer;
      } //else if

      else m_chMap[i][j] = c; //load character into map

      index++; //next index
    } //for

    index++; //skip end of line character
  } //for

  m_vWorldSize = Vector2((float)m_nWidth + 1, (float)m_nHeight)*m_fTileSize;
  MakeBoundingBoxes();
  delete [] buffer; //clean up
} //LoadMap

/// Get positions of objects listed on map.
/// \param turrets [out] Vector of turret positions
/// \param player [out] Player position.

void CTileManager::GetObjects(std::vector<Vector2>& turrets, Vector2& player, Vector2& activity, Vector2& house, std::vector<Vector2>& trees, std::vector<Vector2>& zombies, Vector2& shop, Vector2& radiotower){
  turrets = m_vecTurrets;
  player = m_vPlayer;
  activity = m_vActivity;
  house = m_vHouse;
  trees = m_vecTrees;
  zombies = m_vecZombies;
  shop = m_vShop;
  radiotower = m_vRadioTower;
} //GetObjects

/// This is for debug purposes so that you can verify that
/// the collision shapes are in the right places.
/// \param t Line sprite to be stretched to draw the line.

void CTileManager::DrawBoundingBoxes(eSprite t){
  for(auto& p: m_vecWalls)
    m_pRenderer->DrawBoundingBox(t, p);
} //DrawBoundingBoxes

/// Draw order is top-down, left-to-right so that the image
/// agrees with the map text file viewed in NotePad.
/// \param t Sprite type for a 3-frame sprite: 0 is floor, 1 is wall, 2 is an error tile.
 
XMFLOAT4 CTileManager::lerp(XMFLOAT4 start, XMFLOAT4 end, float factor) {
    return XMFLOAT4(
        start.x + factor * (end.x - start.x),
        start.y + factor * (end.y - start.y),
        start.z + factor * (end.z - start.z),
        start.w + factor * (end.w - start.w)
    );
}

void CTileManager::Draw(eSprite t) {
    LSpriteDesc2D desc; //sprite descriptor for tile
    desc.m_nSpriteIndex = (UINT)t; //sprite index for tile

    XMFLOAT4 dayColor = XMFLOAT4(Colors::White);
    XMFLOAT4 nightColor = XMFLOAT4(Colors::DarkCyan);

    // Get the current game time
    int gameHours = m_pGame->GetGameHours();
    std::string am_pm = m_pGame->getAmPm();

    float lerpFactor = 0.0f;
    if (gameHours == 12 && am_pm == "AM") {
        // Transition from day to night from 5PM to 6PM
        lerpFactor = 1.0f;
    }
    else if ((am_pm == "PM" && gameHours < 5) || (am_pm == "AM" && gameHours >= 6)) {
        // Full day from 6AM to 5PM
        lerpFactor = 0.0f;
    }
    else if (gameHours == 12 && am_pm == "PM") {
		// Transition from day to night from 5PM to 6PM
		lerpFactor = 0.0f;
	}
    else if (am_pm == "PM" && gameHours == 5) {
        // Transition from day to night from 5PM to 6PM
        lerpFactor = 0.5f;
    }
    else if ((am_pm == "PM" && gameHours > 5) || (am_pm == "AM" && gameHours < 5)) {
        // Full night from 6PM to 5AM
        lerpFactor = 1.0f;
    }
    else if (am_pm == "AM" && gameHours == 5) {
        // Transition from night to day from 5AM to 6AM
        lerpFactor = 0.5f;
    }

    // Interpolate the tint color using the lerpFactor
    desc.m_f4Tint = lerp(dayColor, nightColor, lerpFactor);

    const Vector2 campos = m_pRenderer->GetCameraPos(); //camera position

    // Calculate camera's corresponding tile indices
    const int camTileX = (int)(campos.x / m_fTileSize);
    const int camTileY = (int)(m_nHeight - (campos.y / m_fTileSize));

    // Calculate the number of tiles across half the screen width/height
    const int tilesAcrossHalfWidth = (m_nWinWidth / 2) / m_fTileSize + 1; // +1 to ensure we capture edge tiles
    const int tilesAcrossHalfHeight = (m_nWinHeight / 2) / m_fTileSize + 1; // +1 to ensure we capture edge tiles

    // Determine visible tile range based on camera position
    const int left = std::max(0, camTileX - tilesAcrossHalfWidth);
    const int right = std::min(static_cast<int>(m_nWidth) - 2, camTileX + tilesAcrossHalfWidth);
    const int topTileIndex = std::max(0, camTileY - tilesAcrossHalfHeight);
    const int bottomTileIndex = std::min(static_cast<int>(m_nHeight) - 1, camTileY + tilesAcrossHalfHeight);

    // Print out the values for verification
    //std::cout << "camTileX: " << camTileX << std::endl;
    //std::cout << "tilesAcrossHalfWidth: " << tilesAcrossHalfWidth << std::endl;
    //std::cout << "camTileX + tilesAcrossHalfWidth: " << camTileX + tilesAcrossHalfWidth << std::endl;

    for (int i = topTileIndex; i <= bottomTileIndex; i++) //for each column
        for (int j = left; j <= right; j++) { //for each row
            desc.m_vPos.x = (j + 0.5f) * m_fTileSize; //horizontal component of tile position
            desc.m_vPos.y = (m_nHeight - 1 - i + 0.5f) * m_fTileSize; //vertical component of tile position

            switch (m_chMap[i][j]) { //select which frame of the tile sprite is to be drawn
            case 'F': desc.m_nCurrentFrame = 0; break; //green floor
            case 'T': desc.m_nCurrentFrame = 3; break; //green floor
            case 'W': desc.m_nCurrentFrame = 1; break; //wall
            case 'G': desc.m_nCurrentFrame = 3; break; //grass
            case 'R': desc.m_nCurrentFrame = 4; break; //road
            case 'E': desc.m_nCurrentFrame = 3; break; //road
            case 'A': desc.m_nCurrentFrame = 5; break; //road
            case 'Z': desc.m_nCurrentFrame = 3; break; //Zombie2
            case 'S': desc.m_nCurrentFrame = 3; break; //Shop
            case 'H': desc.m_nCurrentFrame = 3; break; //Shop
            default:
                std::cout << "Unexpected tile character: " << m_chMap[i][j] << " at [" << i << "][" << j << "]" << std::endl;
                desc.m_nCurrentFrame = 2;
                break;
            } //switch


            m_pRenderer->Draw(&desc); //finally we can draw a tile
        } //for
} //Draw

/// Check whether a circle is visible from a point, that is, either the left
/// or the right side of the object (from the perspective of the point)
/// has no walls between it and the point. This gives some weird behavior
/// when the circle is partially hidden by a block, but it doesn't seem
/// particularly unnatural in practice. It'll do.
/// \param p0 A point.
/// \param p1 Center of circle.
/// \param r Radius of circle.
/// \return true If the circle is visible from the point.

const bool CTileManager::Visible(const Vector2& p0, const Vector2& p1, float r) const{
  bool visible = true;

  for(auto i=m_vecWalls.begin(); i!=m_vecWalls.end() && visible; i++){
    Vector2 direction = p0 - p1;
    direction.Normalize();
    const Vector2 norm = Vector2(-direction.y, direction.x);

    const float delta = std::min(r, 16.0f);

    //left-hand triangle
    const Vector3 v0(p0);
    const Vector3 v1(p1 + r*norm);
    const Vector3 v2(p1 + (r - delta)*norm);
    
    //right-hand triangle
    const Vector3 v3(p1 - r*norm);
    const Vector3 v4(p1 - (r - delta)*norm);

    visible = !(*i).Intersects(v0, v1, v2) || !(*i).Intersects(v0, v3, v4);
  } //for

  return visible;
} //Visible


/// Check whether a bounding sphere collides with one of the wall bounding boxes.
/// If so, compute the collision normal and the overlap distance. 
/// \param s Bounding sphere of object.
/// \param norm [out] Collision normal.
/// \param d [out] Overlap distance.
/// \return true if the bounding sphere overlaps a wall.

const bool CTileManager::CollideWithWall(
  BoundingSphere s, Vector2& norm, float& d) const
{
  bool hit = false; //return result, true if there is a collision with a wall

  for(auto i=m_vecWalls.begin(); i!=m_vecWalls.end() && !hit; i++){
    const BoundingBox& aabb = *i; //shorthand

    Vector3 corner[8]; //for corners of aabb
    aabb.GetCorners(corner);  //get corners of aabb
    s.Center.z = corner[0].z; //make sure they are at the same depth

    //the first 4 corners of aabb are the same as the last 4 but with different z

    hit = s.Intersects(aabb); //includes when they are touching

    if(hit){ //collision with either a point or an edge
      bool bPointCollide = false; //true if colliding with corner of bounding box

      for(UINT i=0; i<4 && !bPointCollide; i++) //check first 4 corners
        if(s.Contains(corner[i])){ //collision of bounding sphere with corner
          bPointCollide = true;
          Vector3 norm3 = s.Center - corner[i]; //vector from corner to sphere center
          norm = (Vector2)norm3; //cast to 2D
          d = s.Radius - norm.Length(); //overlap distance
          norm.Normalize(); //norm needs to be a unit vector
        } //if

      if(!bPointCollide){ //edge collide
        const float fLeft   = corner[0].x; //left of wall
        const float fRight  = corner[1].x; //right of wall
        const float fBottom = corner[1].y; //bottom of wall
        const float fTop    = corner[2].y; //top of wall

        const float epsilon = 0.01f; //small amount of separation

        if(s.Center.x <= fLeft){ //collide with left edge
          norm = -Vector2::UnitX; //normal
          d = s.Center.x - fLeft + s.Radius + epsilon; //overlap
        } //if

        else if(fRight <= s.Center.x){ //collide with right edge
          norm = Vector2::UnitX; //normal
          d = fRight - s.Center.x + s.Radius + epsilon; //overlap
        } //if
   
        else if(s.Center.y <= fBottom){ //collide with bottom edge
          norm = -Vector2::UnitY; //normal
          d = s.Center.y - fBottom + s.Radius + epsilon; //overlap
        } //if

        else if(fTop <= s.Center.y){ //collide with top edge
          norm = Vector2::UnitY; //normal
          d =  fTop - s.Center.y + s.Radius + epsilon; //overlap
        } //if 
      } //if
    } //if
  } //for
  return hit;
} //CollideWithWall