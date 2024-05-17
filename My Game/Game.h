/// \file Game.h
/// \brief Interface for the game class CGame.

#ifndef __L4RC_GAME_GAME_H__
#define __L4RC_GAME_GAME_H__

#include "Component.h"
#include "Common.h"
#include "ObjectManager.h"
#include "Settings.h"
#include "Player.h"
#include "Mouse.h"
#include <iostream>

/// \brief The game class.
///
/// The game class is the object-oriented implementation of the game. This class
/// must contain the following public member functions. `Initialize()` does
/// initialization and will be run exactly once at the start of the game.
/// `ProcessFrame()` will be called once per frame to create and render the
/// next animation frame. `Release()` will be called at game exit but before
/// any destructors are run.

class CGame: 
  public LComponent, 
  public LSettings,
  public CCommon{ 

  private:
    LMouse* m_pMouse;
    void NormalizeAngle(float& angle);
    float RadToDeg(float radians);
    bool m_bDrawFrameRate = false; ///< Draw the frame rate.
    eGameState m_eGameState = eGameState::Title; ///< Game state.
    int m_nNextLevel = 0; ///< Current level number.
    float m_fRotationSpeed = 0.05f;
    float m_fElapsedTime = 0.0f; // Elapsed time in seconds
    float elapsedTime = 0.0f;
    bool isNight = false;
    int m_nGameHours = 0; // Game time in hours
    std::string m_nAmPm = "";
    float m_fKeyStartTime = 0.0f; // Time the key was pressed
    bool farming = false;
    Vector2 playerpos; //player positions
    float messageElapsedTime = 0.0f;
    bool showMessage = false;
    bool showGameOverMessage = false;
    bool holdingDownFKey = false;
    float lastMessageTime = 0.0f; // Last time a message was drawn
    float maxfoodMessageElapsedTime = 0.0f;
    bool showfoodMessage = false;
    float lastfoodMessageTime = 0.0f; // Last time a message was drawn
    float m_fLastShotTime = 0.0f; // Last time a shot was fired
    const float SHOT_COOLDOWN = 0.35f; // Minimum time interval between shots in seconds
    bool isAbleToFarm = false;
    bool isAbleToEat = false;
    bool isAbleToBuild = false;
    bool isAbleToLeave = false;
    bool building = false;
    float m_bKeyStartTime = 0.0f; // Time the key was pressed
    float buildingElapsedTime = 0.0f;
    bool showEscapeMessage = false;
    float lastEscapeMessageTime = 0.0f; // Last time a message was drawn
    float escapeMessageElapsedTime = 0.0f;
    bool spawnedBattery = false;
    bool spawnedLogic = false;
    bool spawnedAntenna = false;

    POINT mousePos, mousePosNew;

    std::vector<Vector2> spawnCoords = {
    {782, 774},
    {242, 1014},
    {2645, 281},
    {2719, 986},
    {1935, 1189},
    {2762, 1202},
    {200, 200}
    };


    
    void LoadImages(); ///< Load images.
    void LoadSounds(); ///< Load sounds.
    void BeginGame(); ///< Begin playing the game.
    void KeyboardHandler(); ///< The keyboard handler.
    void ControllerHandler(); ///< The controller handler.
    void RenderFrame(); ///< Render an animation frame.
    void DrawFrameRateText(); ///< Draw frame rate text to screen.
    void DrawGodModeText(); ///< Draw god mode text if in god mode.
    void CreateObjects(); ///< Create game objects.
    void FollowCamera(); ///< Make camera follow player character.
    void ProcessGameState(); ///< Process game state.
    void MouseHandler();
    void DrawHealthBar();
    void DrawClock();
    void DrawHunger();
    void DrawProgressBar();
    void DrawMessage(std::string message);
    void DrawBackground();
    void DrawRadio();
    Vector3 GetCameraPosition();
    void DrawInstructions();
    void DrawParts();
    void DrawPartsOnTable();
    

  public:
    ~CGame(); ///< Destructor.

    void Initialize(); ///< Initialize the game.
    void ProcessFrame(); ///< Process an animation frame.
    void Release(); ///< Release the renderer.
    int GetGameHours(); ///< Get the game time.
    bool getIsNight();
    std::string getAmPm();
}; //CGame

#endif //__L4RC_GAME_GAME_H__
