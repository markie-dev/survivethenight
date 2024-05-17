/// \file Game.cpp
/// \brief Code for the game class CGame.

#include "Game.h"

#include "GameDefines.h"
#include "SpriteRenderer.h"
#include "ComponentIncludes.h"
#include "ParticleEngine.h"
#include "TileManager.h"
#include "Mouse.h"
#include <iostream>
#include "WindowDesc.h"
#include <cmath>
#include <Windows.h>
#include "Activity.h"
#include "House.h"
#include "Tree.h"
#include <random>
#include <algorithm>
#include "Shop.h"
#include "RadioTower.h"
using namespace std;

#include "shellapi.h"
CObject* m_pRadio;
bool radioOn = false;
bool helpCalled = false;



/// Delete the renderer, the object manager, and the tile manager. The renderer
/// needs to be deleted before this destructor runs so it will be done elsewhere.

CGame::~CGame(){
  delete m_pParticleEngine;
  delete m_pObjectManager;
  delete m_pTileManager;
  delete m_pMouse;
} //destructor

/// Initialize the renderer, the tile manager and the object manager, load 
/// images and sounds, and begin the game.

void CGame::Initialize(){
  m_pRenderer = new LSpriteRenderer(eSpriteMode::Batched2D); 
  m_pRenderer->Initialize(eSprite::Size); 
  LoadImages(); //load images from xml file list
  
  m_pTileManager = new CTileManager((size_t)m_pRenderer->GetWidth(eSprite::Tile), this);
  m_pObjectManager = new CObjectManager; //set up the object manager 
  LoadSounds(); //load the sounds for this game

  m_pParticleEngine = new LParticleEngine2D(m_pRenderer);
  m_eGameState = eGameState::Title;
  BeginGame();
} //Initialize

/// Load the specific images needed for this game. This is where `eSprite`
/// values from `GameDefines.h` get tied to the names of sprite tags in
/// `gamesettings.xml`. Those sprite tags contain the name of the corresponding
/// image file. If the image tag or the image file are missing, then the game
/// should abort from deeper in the Engine code leaving you with an error
/// message in a dialog box.

void CGame::LoadImages(){  
  m_pRenderer->BeginResourceUpload();

  m_pRenderer->Load(eSprite::Tile,    "tile"); 
  m_pRenderer->Load(eSprite::Player,  "player");
  m_pRenderer->Load(eSprite::Bullet,  "bullet");
  m_pRenderer->Load(eSprite::Bullet2, "bullet2");
  m_pRenderer->Load(eSprite::Smoke,   "smoke");
  m_pRenderer->Load(eSprite::Spark,   "spark");
  m_pRenderer->Load(eSprite::Turret,  "turret");
  m_pRenderer->Load(eSprite::Line,    "greenline"); 
  m_pRenderer->Load(eSprite::Activity, "activity");
  m_pRenderer->Load(eSprite::House, "house");
  m_pRenderer->Load(eSprite::Tree, "tree");
  m_pRenderer->Load(eSprite::Title, "title");
  m_pRenderer->Load(eSprite::HealthBar, "healthbar");
  m_pRenderer->Load(eSprite::HealthBar80, "healthbar80");
  m_pRenderer->Load(eSprite::HealthBar60, "healthbar60");
  m_pRenderer->Load(eSprite::HealthBar40, "healthbar40");
  m_pRenderer->Load(eSprite::HealthBar20, "healthbar20");
  m_pRenderer->Load(eSprite::HealthBar0, "healthbar0");
  m_pRenderer->Load(eSprite::Player2, "player2");
  m_pRenderer->Load(eSprite::Turret2, "turret2");
  m_pRenderer->Load(eSprite::Frame, "frame");
  m_pRenderer->Load(eSprite::Sun, "sun");
  m_pRenderer->Load(eSprite::Moon, "moon");
  m_pRenderer->Load(eSprite::Corn, "corn");
  m_pRenderer->Load(eSprite::HalfCorn, "halfcorn");
  m_pRenderer->Load(eSprite::Zombie2, "zombie2");
  m_pRenderer->Load(eSprite::Battery, "battery");
  m_pRenderer->Load(eSprite::Shop, "shop");
  m_pRenderer->Load(eSprite::ProgressFrame, "progressframe");
  m_pRenderer->Load(eSprite::Percent, "percent");
  m_pRenderer->Load(eSprite::MessageFrame, "messageframe");
  m_pRenderer->Load(eSprite::Background2, "background");
  m_pRenderer->Load(eSprite::NightFarm, "nightfarm");
  m_pRenderer->Load(eSprite::MaxFood, "maxfood");
  m_pRenderer->Load(eSprite::LogicBoard, "logicBoard");
  m_pRenderer->Load(eSprite::Antenna, "antenna");
  m_pRenderer->Load(eSprite::DeadRadio, "deadRadio");
  m_pRenderer->Load(eSprite::AliveRadio, "aliveRadio");
  m_pRenderer->Load(eSprite::Victory, "victory");
  m_pRenderer->Load(eSprite::PressToFarm, "presstofarm");
  m_pRenderer->Load(eSprite::PressToEat, "presstoeat");
  m_pRenderer->Load(eSprite::RadioParts, "radioparts");
  m_pRenderer->Load(eSprite::PressToBuild, "presstobuild");
  m_pRenderer->Load(eSprite::Radio, "radio");
  m_pRenderer->Load(eSprite::Research, "research");
  m_pRenderer->Load(eSprite::PressToLeave, "presstoleave");
  m_pRenderer->Load(eSprite::Escape, "escape");
  m_pRenderer->Load(eSprite::PlayButton, "playbutton");
  m_pRenderer->Load(eSprite::PlayButton2, "playbutton2");
  m_pRenderer->Load(eSprite::TutorialButton, "tutorialbutton");
  m_pRenderer->Load(eSprite::Title2, "title2");
  m_pRenderer->Load(eSprite::BackButton, "backbutton");
m_pRenderer->EndResourceUpload();
} //LoadImages

/// Initialize the audio player and load game sounds.

void CGame::LoadSounds(){
  m_pAudio->Initialize(eSound::Size);

  m_pAudio->Load(eSound::Grunt, "grunt");
  m_pAudio->Load(eSound::Clang, "clang");
  m_pAudio->Load(eSound::Gun, "gun");
  m_pAudio->Load(eSound::Ricochet, "ricochet");
  m_pAudio->Load(eSound::Start, "start");
  m_pAudio->Load(eSound::Boom, "boom");
} //LoadSounds

/// Release all of the DirectX12 objects by deleting the renderer.

void CGame::Release(){
  delete m_pRenderer;
  m_pRenderer = nullptr; //for safety
} //Release

/// Ask the object manager to create a player object and turrets specified by
/// the tile manager.

void CGame::CreateObjects(){
  std::vector<Vector2> turretpos; //vector of turret positions
  Vector2 acitvitypos;
  Vector2 housepos;
  Vector2 shoppos;
  Vector2 radiotowerpos;
  std::vector<Vector2> treepos;
  std::vector<Vector2> zombiepos;
  acitvitypos = playerpos;
  m_pTileManager->GetObjects(turretpos, playerpos, acitvitypos, housepos, treepos, zombiepos, shoppos, radiotowerpos); //get positions
  m_pPlayer = (CPlayer*)m_pObjectManager->create(eSprite::Player, playerpos);
  m_pActivity = (CActivity*)m_pObjectManager->create(eSprite::Activity, playerpos);
  m_pHouse = (CHouse*)m_pObjectManager->create(eSprite::House, housepos);
  m_pShop = (CShop*)m_pObjectManager->create(eSprite::Shop, shoppos);
  m_pRadioTower = (CRadioTower*)m_pObjectManager->create(eSprite::Research, radiotowerpos);
  
  //m_pObjectManager->create(eSprite::Activity, acitvitypos);

  //for(const Vector2& pos: turretpos)
    //m_pObjectManager->create(eSprite::Turret, pos);

  for (const Vector2& pos : treepos)
      m_pObjectManager->create(eSprite::Tree, pos);
} //CreateObjects

/// Call this function to start a new game. This should be re-entrant so that
/// you can restart a new game without having to shut down and restart the
/// program. Clear the particle engine to get rid of any existing particles,
/// delete any old objects out of the object manager and create some new ones.

void CGame::BeginGame(){  
    m_pMouse = new LMouse();
    m_pParticleEngine->clear(); //clear old particles
    m_pObjectManager->clear(); //clear old objects
    spawnedBattery = false;
    spawnedAntenna = false;
    spawnedLogic = false;

    if (m_eGameState == eGameState::Title) {
        m_pObjectManager->create(eSprite::Title, m_vWinCenter);
        m_pObjectManager->create(eSprite::PlayButton, m_vWinCenter);
        m_pObjectManager->create(eSprite::TutorialButton, Vector2(m_vWinCenter.x, m_vWinCenter.y - 150));

    }
    else if (m_eGameState == eGameState::Tutorial) {
        m_pObjectManager->clear(); //clear old objects
        m_pObjectManager->create(eSprite::Title2, m_vWinCenter);
        m_pObjectManager->create(eSprite::BackButton, Vector2(m_vWinCenter.x - 750, m_vWinCenter.y - 425));
    }
    else if (m_eGameState == eGameState::Level1) {
        m_pTileManager->LoadMap("Media\\Maps\\map1.txt");
        CreateObjects(); //create new objects (must be after map is loaded) 
        m_pAudio->stop(); //stop all  currently playing sounds
        m_pAudio->play(eSound::Start); //play start-of-game sound
        m_fElapsedTime = 0.0f;
    }
    else if (m_eGameState == eGameState::Victory) {
        m_pObjectManager->create(eSprite::Victory, m_vWinCenter);
    }
    else if (m_eGameState == eGameState::Level2) {
        m_pTileManager->LoadMap("Media\\Maps\\map2.txt");
        CreateObjects(); //create new objects (must be after map is loaded) 
        m_pAudio->stop(); //stop all  currently playing sounds
        m_pAudio->play(eSound::Start); //play start-of-game sound
    }
    else if (m_eGameState == eGameState::Level3) {
        m_pTileManager->LoadMap("Media\\Maps\\map3.txt");
        CreateObjects(); //create new objects (must be after map is loaded) 
        m_pAudio->stop(); //stop all  currently playing sounds
        m_pAudio->play(eSound::Start); //play start-of-game sound
    }
    else if (m_eGameState == eGameState::Level4) {
        m_pTileManager->LoadMap("Media\\Maps\\map4.txt");
        CreateObjects(); //create new objects (must be after map is loaded) 
        m_pAudio->stop(); //stop all  currently playing sounds
        m_pAudio->play(eSound::Start); //play start-of-game sound
    }
    else if (m_eGameState == eGameState::Level5) {
        m_pTileManager->LoadMap("Media\\Maps\\map5.txt");
        CreateObjects(); //create new objects (must be after map is loaded) 
        m_pAudio->stop(); //stop all  currently playing sounds
        m_pAudio->play(eSound::Start); //play start-of-game sound
    }
    else if (m_eGameState == eGameState::Level6) {
        m_pTileManager->LoadMap("Media\\Maps\\map6.txt");
        CreateObjects(); //create new objects (must be after map is loaded) 
        m_pAudio->stop(); //stop all  currently playing sounds
        m_pAudio->play(eSound::Start); //play start-of-game sound
    }
    else if (m_eGameState == eGameState::Level7) {
        m_pTileManager->LoadMap("Media\\Maps\\map7.txt");
        CreateObjects(); //create new objects (must be after map is loaded) 
        m_pAudio->stop(); //stop all  currently playing sounds
        m_pAudio->play(eSound::Start); //play start-of-game sound
    }
}

/// Poll the keyboard state and respond to the key presses that happened since
/// the last frame.

void CGame::KeyboardHandler() {
    m_pKeyboard->GetState(); //get current keyboard state
    static float lastEatTime = 0.0f; // Time the player last ate
    static float lastclickTime = 0.0f; // Time the player last ate

    if (m_pKeyboard->TriggerDown(VK_RETURN)) {
        m_nNextLevel = (m_nNextLevel + 1) % 2;
        BeginGame();
    } //if

    if (m_pKeyboard->TriggerDown(VK_F1)) //help
        ShellExecute(0, 0, "https://larc.unt.edu/code/topdown/", 0, 0, SW_SHOW);

    if (m_pKeyboard->TriggerDown(VK_F2)) { //toggle frame rate
        m_bDrawFrameRate = !m_bDrawFrameRate;
    }

    if (m_pKeyboard->TriggerDown(VK_F3)) //toggle AABB drawing
        m_bDrawAABBs = !m_bDrawAABBs;

    if (m_pKeyboard->TriggerDown(VK_BACK)) //start game
        BeginGame();

    if (m_pPlayer) {
        float baseSpeed = 35.0f; // base speed
        float walkSpeedFactor = 0.5f; // walking speed is half of the base speed

        // Check if the shift key is held down for walking
        bool isWalking = m_pKeyboard->Down(VK_SHIFT);

        if (m_pKeyboard->Down('W')) { // move up
            // If walking (shift held down), move at a slower speed
            m_pPlayer->SetSpeed(isWalking ? (baseSpeed * walkSpeedFactor) : baseSpeed);
            m_pActivity->UpdatePos(m_pPlayer->GetPos());
            playerpos = m_pPlayer->GetPos();
        }
        else if (m_pKeyboard->Down('S')) { // move down
            // If walking (shift held down), move at a slower speed
            m_pPlayer->SetSpeed(isWalking ? (-baseSpeed * walkSpeedFactor) : -baseSpeed);
            m_pActivity->UpdatePos(m_pPlayer->GetPos());
            playerpos = m_pPlayer->GetPos();
        }
        else {
            m_pPlayer->SetSpeed(0.0f); // stop
            m_pActivity->HideActivity();
        }
        if (m_pKeyboard->Down('D')) { // move right
            m_pPlayer->SetWalking(m_pKeyboard->Down(VK_SHIFT)); // set walking state before strafing
            m_pPlayer->StrafeRight();
            m_pActivity->UpdatePos(m_pPlayer->GetPos());
            playerpos = m_pPlayer->GetPos();
        }

        if (m_pKeyboard->Down('A')) { // move left
            m_pPlayer->SetWalking(m_pKeyboard->Down(VK_SHIFT)); // set walking state before strafing
            m_pPlayer->StrafeLeft();
            m_pActivity->UpdatePos(m_pPlayer->GetPos());
            playerpos = m_pPlayer->GetPos();
        }
        if (m_pKeyboard->Down('F')) { // Farming
            if (!isNight) {
                Vector2 playerPos = m_pPlayer->GetPos();
                if (playerPos.x >= 2119 && playerPos.x <= 2333 && playerPos.y >= 330 && playerPos.y <= 570) {
                    if (m_pPlayer->GetHungerCount() == 3) { // If player tries to farm with maxfood
                        if (m_pTimer->GetTime() - lastfoodMessageTime > 0.5f) {
                            maxfoodMessageElapsedTime = m_pTimer->GetTime();
                            showfoodMessage = true;
                        }
                    }
                      if (m_fKeyStartTime == 0.0f) { // If 'F' key is just pressed
                          m_fKeyStartTime = m_pTimer->GetTime(); // Record the start time
                      }
                      else {
                          elapsedTime = m_pTimer->GetTime() - m_fKeyStartTime; // Calculate elapsed time
                          farming = true;
                          if (elapsedTime >= 5.0f) { // If 'F' has been held for 5 seconds
                              m_pPlayer->SetHungerCount(m_pPlayer->GetHungerCount() + 1); // Increment HungerCount
                              m_fKeyStartTime = 0.0f; // Reset the start time
                              farming = false;
                          }
                      }
                  }
                  else {
                      elapsedTime = 0.0f; // Reset elapsed time
                      farming = false; // Set farming to false
                      m_fKeyStartTime = 0.0f; // Reset the start time
                  }
              }
              else {
                  //make show message true for 5 seconds
                  Vector2 playerPos = m_pPlayer->GetPos();
                  if (playerPos.x >= 2119 && playerPos.x <= 2333 && playerPos.y >= 330 && playerPos.y <= 570) {
                      if (m_pTimer->GetTime() - lastMessageTime > 0.5f) {
                          messageElapsedTime = m_pTimer->GetTime();
                          showMessage = true;
                      }
                      
                  }
              }
          }
      else {
		  elapsedTime = 0.0f; // Reset elapsed time
		  farming = false; // Set farming to false
		  m_fKeyStartTime = 0.0f; // Reset the start time
          holdingDownFKey = false;
          building = false;
	  }

      if (m_pKeyboard->Down('E')) { // Eating
          float currentTime = m_pTimer->GetTime(); // Get the current time
          if (currentTime - lastEatTime >= 0.5f) { // Check if at least 1 second has passed
              if (m_pPlayer && m_pPlayer->GetHealth() < 12 && m_pPlayer->GetHungerCount() > 0) {
                  m_pPlayer->SetHealth(m_pPlayer->GetHealth() + 3); // Increment health
                  m_pPlayer->SetHungerCount(m_pPlayer->GetHungerCount() - 1); // Decrement HungerCount
                  lastEatTime = currentTime; // Update the last eat time
              }
          }
      }


      if (m_pKeyboard->TriggerDown(VK_LBUTTON)){ //fire gun
          //if (!m_pPlayer) {
          //    cout << "here" << endl;
          //    if (mousePosNew.x >= 662 && mousePosNew.x <= 1257 && mousePosNew.y >= 496 && mousePosNew.y <= 600) {
          //        cout << "Pressed start button" << endl;
          //    }
          //}
          float currentTime = m_pTimer->GetTime(); // Get the current time
          if (currentTime - m_fLastShotTime >= SHOT_COOLDOWN) { // Check if enough time has passed
              m_pObjectManager->FireGun(m_pPlayer, eSprite::Bullet);
              m_fLastShotTime = currentTime; // Update the last shot time
              m_pActivity->UpdatePos(m_pPlayer->GetPos());
          }


      }
      
      if (m_pKeyboard->TriggerDown('G')) { //toggle god mode
          //m_bGodMode = !m_bGodMode;
      }
      if (m_pKeyboard->Down('B')) {
              if (playerpos.x >= 1396 && playerpos.x <= 1496 && playerpos.y >= 359 && playerpos.y <= 504 && isAbleToBuild && !radioOn) {
                  building = true;
                  if (m_bKeyStartTime == 0.0f) { // If 'F' key is just pressed
                      m_bKeyStartTime = m_pTimer->GetTime(); // Record the start time
                  }
                  else {
                      buildingElapsedTime = m_pTimer->GetTime() - m_bKeyStartTime; // Calculate elapsed time

                      if (buildingElapsedTime >= 5.0f) { // If 'F' has been held for 5 seconds
                          m_bKeyStartTime = 0.0f; // Reset the start time
                          radioOn = true;
                          if (m_pTimer->GetTime() - lastEscapeMessageTime > 0.5f) {
                              escapeMessageElapsedTime = m_pTimer->GetTime();
                              showEscapeMessage = true;
                          }
                          gotBattery = false;
                          gotAntenna = false;
                          gotLogicBoard = false;
                          building = false;
                      }
                  }
              }
              else {
                  building = false;
                  m_bKeyStartTime = 0.0f; // Reset the start time
                  buildingElapsedTime = 0.0f;
              }
      }
      else {
          building = false;
          m_bKeyStartTime = 0.0f; // Reset the start time
          buildingElapsedTime = 0.0f;
      }
      if (m_pKeyboard->TriggerDown('R')) {
          if (isAbleToLeave) {
                  helpCalled = true;//you win 
          }
      }

  } //if
} //KeyboardHandle

void CGame::MouseHandler() {
    m_pMouse->GetState();
    mousePos = m_pMouse->GetPosition();

    if (!m_pPlayer) {
        //Make playPos the center of the screen
        Vector2 playerPos = Vector2(m_nWinWidth / 2, m_nWinHeight / 2);

        // Get camera position
        Vector3 cameraPos3D = GetCameraPosition();
        Vector2 cameraPos(cameraPos3D.x, cameraPos3D.y);

        // Convert screen mouse coordinates to world space
        mousePosNew.x = mousePos.x - (m_nWinWidth / 2) + cameraPos.x;
        mousePosNew.y = (m_nWinHeight - mousePos.y) - (m_nWinHeight / 2) + cameraPos.y;

        // For debugging purposes
        //std::cout << "Mouse X: " << mousePosNew.x << " Mouse Y: " << mousePosNew.y << std::endl;
        //std::cout << "Player X: " << playerPos.x << " Player Y: " << playerPos.y << std::endl;
        //std::cout << "Angle: " << angleInDegrees << std::endl;
    }

    if (m_pPlayer) {
        Vector2 playerPos = m_pPlayer->GetPos();

        // Get camera position
        Vector3 cameraPos3D = GetCameraPosition();
        Vector2 cameraPos(cameraPos3D.x, cameraPos3D.y);

        // Convert screen mouse coordinates to world space
        mousePos.x = mousePos.x - ((m_nWinWidth) / 2) + cameraPos.x;
        mousePos.y = (m_nWinHeight - mousePos.y) - (m_nWinHeight / 2) + cameraPos.y;

        // Calculate the angle between the player and the mouse cursor
        float dX = mousePos.x - playerPos.x;
        float dY = mousePos.y - playerPos.y;
        float angleInRadians = atan2(dY, dX);

        // Convert the angle from radians to degrees
        float angleInDegrees = angleInRadians * (180.0f / M_PI);

        // Set the player's rotation
        m_pPlayer->SetRotation(angleInDegrees - 2.0f);

        // For debugging purposes
        //std::cout << "Mouse X: " << mousePos.x << " Mouse Y: " << mousePos.y << std::endl;
        //std::cout << "Player X: " << playerPos.x << " Player Y: " << playerPos.y << std::endl;
        //std::cout << "Angle: " << angleInDegrees << std::endl;
    }
}

float CGame::RadToDeg(float radians) {
    return radians * (180.0f / M_PI);
}


void CGame::NormalizeAngle(float& angle) {
    while (angle > M_PI) angle -= 2.0f * M_PI;
    while (angle < -M_PI) angle += 2.0f * M_PI;
}

/// Poll the XBox controller state and respond to the controls there.

void CGame::ControllerHandler(){
  if(!m_pController->IsConnected())return;

  m_pController->GetState(); //get state of controller's controls 
  
  if(m_pPlayer){ //safety
    m_pPlayer->SetSpeed(100*m_pController->GetRTrigger());
    m_pPlayer->SetRotSpeed(-2.0f*m_pController->GetRThumb().x);

    if(m_pController->GetButtonRSToggle()) //fire gun
      m_pObjectManager->FireGun(m_pPlayer, eSprite::Bullet);

    if(m_pController->GetDPadRight()) //strafe right
      m_pPlayer->StrafeRight();
  
    if(m_pController->GetDPadLeft()) //strafe left
      m_pPlayer->StrafeLeft();

    if(m_pController->GetDPadDown()) //strafe back
      m_pPlayer->StrafeBack();
  } //if
} //ControllerHandler

void CGame::DrawHunger() {
    if (m_pPlayer) {
        int hungerCount = m_pPlayer->GetHungerCount();
        Vector3 cameraPos = m_pRenderer->GetCameraPos();

        if (hungerCount == 1) {
            LSpriteDesc2D desc;
            desc.m_nSpriteIndex = (UINT)eSprite::Corn;
            desc.m_vPos = Vector2(cameraPos.x - 730, cameraPos.y - 470);
            m_pRenderer->Draw(&desc);
        }
        if (hungerCount == 2) {
            LSpriteDesc2D desc;
            desc.m_nSpriteIndex = (UINT)eSprite::Corn;
            desc.m_vPos = Vector2(cameraPos.x - 730, cameraPos.y - 470);
            m_pRenderer->Draw(&desc);

            LSpriteDesc2D desc2;
            desc2.m_nSpriteIndex = (UINT)eSprite::Corn;
            desc2.m_vPos = Vector2(cameraPos.x - 715, cameraPos.y - 470);
            m_pRenderer->Draw(&desc2);
        }
        if (hungerCount == 3) {
            LSpriteDesc2D desc;
            desc.m_nSpriteIndex = (UINT)eSprite::Corn;
            desc.m_vPos = Vector2(cameraPos.x - 730, cameraPos.y - 470);
            m_pRenderer->Draw(&desc);

            LSpriteDesc2D desc2;
            desc2.m_nSpriteIndex = (UINT)eSprite::Corn;
            desc2.m_vPos = Vector2(cameraPos.x - 715, cameraPos.y - 470);
            m_pRenderer->Draw(&desc2);

            LSpriteDesc2D desc3;
            desc3.m_nSpriteIndex = (UINT)eSprite::Corn;
            desc3.m_vPos = Vector2(cameraPos.x - 700, cameraPos.y - 470);
            m_pRenderer->Draw(&desc3);
        }
    }
}

void CGame::DrawProgressBar() {
    if (m_eGameState != eGameState::Title && farming && m_pPlayer->GetHungerCount() < 3 && !isNight) {
        Vector3 cameraPos = m_pRenderer->GetCameraPos(); // Get the camera's position

        int offset = 38;

        // Draw Percentage Bar
        LSpriteDesc2D percent;
        percent.m_nSpriteIndex = (UINT)eSprite::Percent;

        // Get the progress value (between 0.0f and 1.0f)
        float progress = elapsedTime / 5.0f; // Calculate progress based on elapsedTime

        // Scale the percentage bar based on the progress
        percent.m_fXScale = progress * 0.5; // Scale X-axis based on progress and reduce by half
        percent.m_fYScale = 1.0f * 0.5; // Reduce Y-axis scale by half

        // Adjust the offset based on progress
        offset = static_cast<int>(38 * (1 - progress)); // Adjust offset based on progress

        percent.m_vPos = Vector2(playerpos.x - (offset), playerpos.y + 50);

        Vector4 startColor = Vector4(1.0f, 0.0f, 0.0f, 1.0f); // Red color at the start
        Vector4 endColor = Vector4(0.0f, 1.0f, 0.0f, 1.0f); // Green color at the end

        // Calculate the current color based on the progress
        Vector4 currentColor;
        currentColor.x = startColor.x + progress * (endColor.x - startColor.x);
        currentColor.y = startColor.y + progress * (endColor.y - startColor.y);
        currentColor.z = startColor.z + progress * (endColor.z - startColor.z);
        currentColor.w = startColor.w + progress * (endColor.w - startColor.w);

        // Set the tint color of the percentage bar
        percent.m_f4Tint = currentColor;

        m_pRenderer->Draw(&percent);

        // Draw Frame
        LSpriteDesc2D frame;
        frame.m_nSpriteIndex = (UINT)eSprite::ProgressFrame;
        // Add an offset of 10 to raise the frame above the player's head
        frame.m_vPos = Vector2(playerpos.x, playerpos.y + 50);
        frame.m_fXScale = 1.0f * 0.5; // Reduce X-axis scale by half
        frame.m_fYScale = 1.0f * 0.5; // Reduce Y-axis scale by half

        m_pRenderer->Draw(&frame);
    }

    if (m_eGameState != eGameState::Title && building) {
        Vector3 cameraPos = m_pRenderer->GetCameraPos(); // Get the camera's position

        int offset = 38;

        // Draw Percentage Bar
        LSpriteDesc2D percent;
        percent.m_nSpriteIndex = (UINT)eSprite::Percent;

        // Get the progress value (between 0.0f and 1.0f)
        float progress = buildingElapsedTime / 5.0f; // Calculate progress based on elapsedTime

        // Scale the percentage bar based on the progress
        percent.m_fXScale = progress * 0.5; // Scale X-axis based on progress and reduce by half
        percent.m_fYScale = 1.0f * 0.5; // Reduce Y-axis scale by half

        // Adjust the offset based on progress
        offset = static_cast<int>(38 * (1 - progress)); // Adjust offset based on progress

        percent.m_vPos = Vector2(playerpos.x - (offset), playerpos.y + 50);

        Vector4 startColor = Vector4(1.0f, 0.0f, 0.0f, 1.0f); // Red color at the start
        Vector4 endColor = Vector4(0.0f, 1.0f, 0.0f, 1.0f); // Green color at the end

        // Calculate the current color based on the progress
        Vector4 currentColor;
        currentColor.x = startColor.x + progress * (endColor.x - startColor.x);
        currentColor.y = startColor.y + progress * (endColor.y - startColor.y);
        currentColor.z = startColor.z + progress * (endColor.z - startColor.z);
        currentColor.w = startColor.w + progress * (endColor.w - startColor.w);

        // Set the tint color of the percentage bar
        percent.m_f4Tint = currentColor;

        m_pRenderer->Draw(&percent);

        // Draw Frame
        LSpriteDesc2D frame;
        frame.m_nSpriteIndex = (UINT)eSprite::ProgressFrame;
        // Add an offset of 10 to raise the frame above the player's head
        frame.m_vPos = Vector2(playerpos.x, playerpos.y + 50);
        frame.m_fXScale = 1.0f * 0.5; // Reduce X-axis scale by half
        frame.m_fYScale = 1.0f * 0.5; // Reduce Y-axis scale by half

        m_pRenderer->Draw(&frame);
    }
}

void CGame::DrawMessage(std::string message) {
    float currentTime = m_pTimer->GetTime();

    if (m_eGameState != eGameState::Title && (showMessage)) {
        // Draw Frame
        Vector3 cameraPos = m_pRenderer->GetCameraPos(); // Get the camera's position
        LSpriteDesc2D frame, nightfarm;
        frame.m_nSpriteIndex = (UINT)eSprite::MessageFrame;
        frame.m_vPos = Vector2(cameraPos.x + 750, cameraPos.y + 487);
        nightfarm.m_nSpriteIndex = (UINT)eSprite::NightFarm;
        nightfarm.m_vPos = Vector2(cameraPos.x + 745, cameraPos.y + 483);
        frame.m_fXScale = 0.75f; // Reduce X-axis scale by half
        frame.m_fYScale = 0.15f; // Reduce Y-axis scale by half
        nightfarm.m_fXScale = 0.75f; // Reduce X-axis scale by half
        nightfarm.m_fYScale = 0.75f; // Reduce Y-axis scale by half

        // Calculate alpha value based on messageElapsedTime
        float timeSinceMessage = m_pTimer->GetTime() - messageElapsedTime;
        if (timeSinceMessage < 0.5f) {
            // Fade in during the first half second
            frame.m_fAlpha = timeSinceMessage * 2;
            nightfarm.m_fAlpha = timeSinceMessage * 2;
        }
        else if (timeSinceMessage < 1.0f) {
            // Stay fully opaque during the second half second
            frame.m_fAlpha = 1.0f;
            nightfarm.m_fAlpha = 1.0f;
        }
        else {
            // Fade out during the third half second
            frame.m_fAlpha = 1.0f - 2 * (timeSinceMessage - 1.0f);
            nightfarm.m_fAlpha = 1.0f - 2 * (timeSinceMessage - 1.0f);
        }

        if (message == "farmnight") {
            m_pRenderer->Draw(&frame);
            m_pRenderer->Draw(&nightfarm);
        }
        else {
            return;
        }

        lastMessageTime = currentTime; // Update the last message time
    }

    if (m_eGameState != eGameState::Title && (showfoodMessage)) {
        // Draw Frame
        Vector3 cameraPos = m_pRenderer->GetCameraPos(); // Get the camera's position
        LSpriteDesc2D frame, maxfood;
        frame.m_nSpriteIndex = (UINT)eSprite::MessageFrame;
        frame.m_vPos = Vector2(cameraPos.x + 750, cameraPos.y + 487);
        maxfood.m_nSpriteIndex = (UINT)eSprite::MaxFood;
        maxfood.m_vPos = Vector2(cameraPos.x + 747, cameraPos.y + 483);
        frame.m_fXScale = 0.75f; // Reduce X-axis scale by half
        frame.m_fYScale = 0.15f; // Reduce Y-axis scale by half
        maxfood.m_fXScale = 0.75f; // Reduce X-axis scale by half
        maxfood.m_fYScale = 0.75f; // Reduce Y-axis scale by half

        // Calculate alpha value based on messageElapsedTime
        float timeSinceMessage = m_pTimer->GetTime() - maxfoodMessageElapsedTime;
        if (timeSinceMessage < 0.5f) {
            // Fade in during the first half second
            frame.m_fAlpha = timeSinceMessage * 2;
            maxfood.m_fAlpha = timeSinceMessage * 2;
        }
        else if (timeSinceMessage < 1.0f) {
            // Stay fully opaque during the second half second
            frame.m_fAlpha = 1.0f;
            maxfood.m_fAlpha = 1.0f;
        }
        else {
            // Fade out during the third half second
            frame.m_fAlpha = 1.0f - 2 * (timeSinceMessage - 1.0f);
            maxfood.m_fAlpha = 1.0f - 2 * (timeSinceMessage - 1.0f);
        }

        if (message == "maxfood") {
            m_pRenderer->Draw(&frame);
            m_pRenderer->Draw(&maxfood);
        }
        else {
            return;
        }

        lastfoodMessageTime = currentTime; // Update the last message time
    }

    if (m_eGameState != eGameState::Title && (showEscapeMessage)) {
        // Draw Frame
        Vector3 cameraPos = m_pRenderer->GetCameraPos(); // Get the camera's position
        LSpriteDesc2D frame, maxfood;
        frame.m_nSpriteIndex = (UINT)eSprite::MessageFrame;
        frame.m_vPos = Vector2(cameraPos.x + 750, cameraPos.y + 487);
        maxfood.m_nSpriteIndex = (UINT)eSprite::Escape;
        maxfood.m_vPos = Vector2(cameraPos.x + 747, cameraPos.y + 483);
        frame.m_fXScale = 0.75f; // Reduce X-axis scale by half
        frame.m_fYScale = 0.15f; // Reduce Y-axis scale by half
        maxfood.m_fXScale = 0.75f; // Reduce X-axis scale by half
        maxfood.m_fYScale = 0.75f; // Reduce Y-axis scale by half

        // Calculate alpha value based on messageElapsedTime
        float timeSinceMessage = m_pTimer->GetTime() - escapeMessageElapsedTime;
        if (timeSinceMessage < 0.5f) {
            // Fade in during the first half second
            frame.m_fAlpha = timeSinceMessage * 2;
            maxfood.m_fAlpha = timeSinceMessage * 2;
        }
        else if (timeSinceMessage < 1.0f) {
            // Stay fully opaque during the second half second
            frame.m_fAlpha = 1.0f;
            maxfood.m_fAlpha = 1.0f;
        }
        else {
            // Fade out during the third half second
            frame.m_fAlpha = 1.0f - 2 * (timeSinceMessage - 2.0f);
            maxfood.m_fAlpha = 1.0f - 2 * (timeSinceMessage - 2.0f);
        }

        if (message == "escapemessage") {
            m_pRenderer->Draw(&frame);
            m_pRenderer->Draw(&maxfood);
        }
        else {
            return;
        }

        lastEscapeMessageTime = currentTime; // Update the last message time
    }
}

void CGame::DrawBackground() {
    if (m_eGameState != eGameState::Title) {
        LSpriteDesc2D background;
        background.m_nSpriteIndex = (UINT)eSprite::Background2;
        // Add an offset of 10 to raise the frame above the player's head
        background.m_vPos = Vector2(2600, 700);

        m_pRenderer->Draw(&background);
    }
}

void CGame::DrawClock() {
    static int currentDayIndex = 0; 
    static bool hasIncrementedDay = false; // Add a flag to check if we've incremented the day for the current 12:00 AM minute
    static bool hasSpawnedZombies = false;
    int zombieCount = 6;
    if (m_eGameState != eGameState::Title && m_eGameState != eGameState::Victory && m_eGameState != eGameState::Tutorial) {
        int gameMinutes = (static_cast<int>(m_fElapsedTime * 30) + 12 * 60) % (24 * 60); // Modulo to loop back to 0 at midnight, add 23 hours to start at 11 PM

        // Calculate game hours and minutes
        int gameHours = gameMinutes / 60;
        int gameMins = gameMinutes % 60;

        // Calculate game day (1 real day = 1 game week)
        std::string daysOfWeek[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };

        // Convert to 12-hour format and determine AM/PM
        std::string am_pm = (gameHours >= 12 && gameHours < 24) ? "PM" : "AM";
        gameHours = gameHours % 12;
        gameHours = (gameHours == 0) ? 12 : gameHours; // Convert 0 hours to 12

        // If it's 12:00 AM and we haven't incremented the day yet, increment the day
        if (gameHours == 12 && gameMins == 0 && am_pm == "AM" && !hasIncrementedDay) {
            currentDayIndex = (currentDayIndex + 1) % 7;
            hasIncrementedDay = true; // Set the flag to true so we don't increment the day again during this minute
            // Increase the number of zombies by 10 each day until it reaches 70
            spawnedBattery = false;
            spawnedAntenna = false;
            spawnedLogic = false;
            zombieCount = std::min(zombieCount + 10, 70);
        }

        // If it's past 12:00 AM, reset the flag so we can increment the day again the next time it hits 12:00 AM
        if (gameHours == 12 && gameMins != 0 && am_pm == "AM") {
            hasIncrementedDay = false;
        }

        std::string gameDay = daysOfWeek[currentDayIndex];

        // Format game time string
        std::string gameTime = gameDay + " - " + std::to_string(gameHours) + ":" + (gameMins < 10 ? "0" : "") + std::to_string(gameMins) + " " + am_pm;

       // Draw Frame
        LSpriteDesc2D desc;
        desc.m_nSpriteIndex = (UINT)eSprite::Frame;
        Vector3 cameraPos = m_pRenderer->GetCameraPos(); // Get the camera's position
        desc.m_vPos = Vector2(cameraPos.x - 750, cameraPos.y + 487); 
        desc.m_fXScale = 1.0f;
        desc.m_fYScale = 1.0f; 
        desc.m_fAlpha = 0.75f;
        m_pRenderer->Draw(&desc);

        const Vector2 pos(70.0f, 38.0f); //hard-coded position
        if (gameDay == "Wednesday") {
            //adjust position for Wednesday
            m_pRenderer->DrawScreenText(gameTime.c_str(), { pos.x - 27 , pos.y });
        }else if (gameDay == "Friday") {
			//adjust position for Friday
			m_pRenderer->DrawScreenText(gameTime.c_str(), { pos.x + 3, pos.y });
		}else if (gameDay == "Tuesday") {
            //adjust position for Tuesday
            m_pRenderer->DrawScreenText(gameTime.c_str(), { pos.x - 10, pos.y });
        }else if (gameDay == "Thursday") {
            //adjust position for Thursday
            m_pRenderer->DrawScreenText(gameTime.c_str(), { pos.x - 17, pos.y });
        }else if (gameDay == "Saturday") {
            //adjust position for Saturday
            m_pRenderer->DrawScreenText(gameTime.c_str(), { pos.x - 20, pos.y });
        }else if (gameDay == "Sunday") {
            //adjust position for Sunday
            m_pRenderer->DrawScreenText(gameTime.c_str(), { pos.x - 3, pos.y });
        }
        else {
            m_pRenderer->DrawScreenText(gameTime.c_str(), pos);
        }
        desc.m_vPos = Vector2(cameraPos.x - 530, cameraPos.y + 487);
        desc.m_fXScale = 1.0f;
        desc.m_fYScale = 1.0f;

        //Update Game Hours
        m_nGameHours = gameHours;
        m_nAmPm = am_pm;

        std::random_device rd;
        std::mt19937 g(rd());
        std::uniform_int_distribution<> distr(0, spawnCoords.size() - 1);

        if (gameHours == 12 && gameMins == 0 && am_pm == "AM") {
            if (!gotBattery && !gotAntenna && !gotLogicBoard && !spawnedBattery) {
                Vector2 batteryPos = spawnCoords[distr(g)];
                m_pObjectManager->create(eSprite::Battery, batteryPos);
                spawnedBattery = true;
                cout << "Spawned Battery at: " << batteryPos.x << ", " << batteryPos.y << endl;
            }
            else if (gotBattery && !gotAntenna && !gotLogicBoard && !spawnedAntenna) {
                Vector2 antennaPos = spawnCoords[distr(g)];
                m_pObjectManager->create(eSprite::Antenna, antennaPos);
                spawnedAntenna = true;
                cout << "Spawned Antenna at: " << antennaPos.x << ", " << antennaPos.y << endl;
            }
            else if (gotBattery && gotAntenna && !gotLogicBoard && !spawnedLogic) {
                Vector2 logicPos = spawnCoords[distr(g)];
                m_pObjectManager->create(eSprite::LogicBoard, logicPos);
                spawnedLogic = true;
                cout << "Spawned LogicBoard at: " << logicPos.x << ", " << logicPos.y << endl;
            }
            else {}
        }
        if (gameHours == 5 && gameMins == 0 && am_pm == "AM") {
            m_pObjectManager->clearRadios();
        }

        // If it's between 6:00 AM and 5:59 PM, draw the sun else draw the moon
        if ((gameHours > 6 && gameHours < 12 && am_pm == "AM") ||
            (gameHours < 6 && am_pm == "PM") ||
            (gameHours == 6 && gameMins > 0 && am_pm == "AM") ||
            (gameHours == 12 && am_pm == "PM")) {
            desc.m_nSpriteIndex = (UINT)eSprite::Sun;
            isNight = false;
            hasSpawnedZombies = false; // Reset the flag so we can spawn the zombies again
        }
        else {
            desc.m_nSpriteIndex = (UINT)eSprite::Moon;
            isNight = true;

            if (!hasSpawnedZombies) {
                std::vector<Vector2> turretpos; //vector of turret positions
                std::vector<Vector2> zombiepos;
                Vector2 playerpos; //player positions
                Vector2 acitvitypos;
                Vector2 housepos;
                Vector2 shoppos;
                Vector2 radiotowerpos;
                std::vector<Vector2> treepos;
                acitvitypos = playerpos;
                m_pTileManager->GetObjects(turretpos, playerpos, acitvitypos, housepos, treepos, zombiepos, shoppos, radiotowerpos); //get positions
                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(turretpos.begin(), turretpos.end(), g);
                std::shuffle(zombiepos.begin(), zombiepos.end(), g);

                for (int i = 0; i < zombieCount; i++) {
                    m_pObjectManager->create(eSprite::Zombie2, zombiepos[i]);
                    //m_pObjectManager->create(eSprite::Zombie2, zombiepos);
                }
                for (int i = 0; i < zombieCount; i++) {
                    m_pObjectManager->create(eSprite::Turret, turretpos[i]);
                }
                hasSpawnedZombies = true; // Set the flag to true after spawning the zombies
            }
        }
        m_pRenderer->Draw(&desc);  
    }
}

bool CGame::getIsNight() {
    return isNight;

}
void CGame::DrawHealthBar() {
    if (m_eGameState != eGameState::Title && m_eGameState != eGameState::Victory && m_eGameState != eGameState::Tutorial) { // Only draw the health bar if the game is not in the title state
        // Draw Frame
        LSpriteDesc2D frame;
        frame.m_nSpriteIndex = (UINT)eSprite::Frame;
        Vector3 cameraPos = m_pRenderer->GetCameraPos(); // Get the camera's position
        frame.m_vPos = Vector2(cameraPos.x - 845, cameraPos.y - 470);
        frame.m_fXScale = 0.5f;
        frame.m_fYScale = 1.0f;
        frame.m_fAlpha = 0.75f;
        m_pRenderer->Draw(&frame);
        
        LSpriteDesc2D desc;
        // Get the player's health
        int health = m_pPlayer->GetHealth();

        // Set the sprite index based on the player's health
        if (health == 12) {
            desc.m_nSpriteIndex = (UINT)eSprite::HealthBar;
        }
        else if (health == 9) {
            desc.m_nSpriteIndex = (UINT)eSprite::HealthBar60;
        }
        else if (health == 6) {
            desc.m_nSpriteIndex = (UINT)eSprite::HealthBar40;
        }
        else if (health == 3) {
            desc.m_nSpriteIndex = (UINT)eSprite::HealthBar20;
        }
        else {
            desc.m_nSpriteIndex = (UINT)eSprite::HealthBar0;
        }
        desc.m_vPos = Vector2(cameraPos.x - 850, cameraPos.y - 470); // Adjust the health bar's position based on the camera's position

        desc.m_fXScale = 1.0f;
        desc.m_fYScale = 1.0f; 
        m_pRenderer->Draw(&desc);
    }
}

void CGame::DrawInstructions() {
    Vector3 cameraPos = m_pRenderer->GetCameraPos(); // Get the camera's position
    if (isAbleToFarm && !isAbleToEat) {
        LSpriteDesc2D instruct;
        instruct.m_nSpriteIndex = (UINT)eSprite::PressToFarm;
        // Add an offset of 10 to raise the frame above the player's head
        instruct.m_vPos = Vector2(cameraPos.x + 770, cameraPos.y - 460);

        m_pRenderer->Draw(&instruct);
    }
    if (isAbleToEat && !isAbleToFarm) {
		LSpriteDesc2D instruct;
		instruct.m_nSpriteIndex = (UINT)eSprite::PressToEat;
		// Add an offset of 10 to raise the frame above the player's head
		instruct.m_vPos = Vector2(cameraPos.x + 770, cameraPos.y - 440);

		m_pRenderer->Draw(&instruct);
	}
    if (isAbleToEat && isAbleToFarm) {
		LSpriteDesc2D instruct, instruct2;
		instruct.m_nSpriteIndex = (UINT)eSprite::PressToFarm;
        instruct2.m_nSpriteIndex = (UINT)eSprite::PressToEat;
		// Add an offset of 10 to raise the frame above the player's head
		instruct2.m_vPos = Vector2(cameraPos.x + 770, cameraPos.y - 440);
        instruct.m_vPos = Vector2(cameraPos.x + 770, cameraPos.y - 460);

		m_pRenderer->Draw(&instruct);
		m_pRenderer->Draw(&instruct2);
	}
    if (isAbleToBuild) {
        LSpriteDesc2D instruct;
        instruct.m_nSpriteIndex = (UINT)eSprite::PressToBuild;
        // Add an offset of 10 to raise the frame above the player's head
        instruct.m_vPos = Vector2(cameraPos.x + 773, cameraPos.y - 460);

        m_pRenderer->Draw(&instruct);
    }
    if (isAbleToLeave && m_eGameState != eGameState::Victory) {
        LSpriteDesc2D instruct;
        instruct.m_nSpriteIndex = (UINT)eSprite::PressToLeave;
        // Add an offset of 10 to raise the frame above the player's head
        instruct.m_vPos = Vector2(cameraPos.x + 773, cameraPos.y - 460);

        m_pRenderer->Draw(&instruct);
    }
}

void CGame::DrawPartsOnTable() {

}

void CGame::DrawParts() {
    Vector3 cameraPos = m_pRenderer->GetCameraPos(); // Get the camera's position
    if (m_eGameState != eGameState::Title && m_eGameState != eGameState::Victory && m_eGameState != eGameState::Tutorial) {
		LSpriteDesc2D battery;
        LSpriteDesc2D antenna;
        LSpriteDesc2D logicboard;
        LSpriteDesc2D radio;
		battery.m_nSpriteIndex = (UINT)eSprite::Battery;
        battery.m_vPos = Vector2(cameraPos.x - 830, cameraPos.y - 420);
        antenna.m_nSpriteIndex = (UINT)eSprite::Antenna;
        antenna.m_vPos = Vector2(cameraPos.x - 800, cameraPos.y - 420);
        logicboard.m_nSpriteIndex = (UINT)eSprite::LogicBoard;
        logicboard.m_vPos = Vector2(cameraPos.x - 770, cameraPos.y - 420);
        radio.m_nSpriteIndex = (UINT)eSprite::Radio;
        radio.m_vPos = Vector2(cameraPos.x - 920, cameraPos.y - 420);

        //Tint the battery greyscale
        if (!gotBattery)
            battery.m_f4Tint = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
        if (!gotAntenna)
            antenna.m_f4Tint = Vector4(0.5f, 0.5f, 0.5f, 1.0f);
        if (!gotLogicBoard)
            logicboard.m_f4Tint = Vector4(0.5f, 0.5f, 0.5f, 1.0f);

        //Add transparency
        if(!gotBattery)
			battery.m_fAlpha = 0.50f;
        else
            battery.m_fAlpha = 1.0f;

        if (!gotAntenna)
            antenna.m_fAlpha = 0.50f;
        else
            antenna.m_fAlpha = 1.0f;

        if (!gotLogicBoard)
			logicboard.m_fAlpha = 0.50f;
		else
			logicboard.m_fAlpha = 1.0f;


        if(radioOn)
            m_pRenderer->Draw(&radio);
		m_pRenderer->Draw(&battery);
		m_pRenderer->Draw(&antenna);
        m_pRenderer->Draw(&logicboard);
	}

}

void CGame::DrawFrameRateText(){
  const std::string s = std::to_string(m_pTimer->GetFPS()) + " fps"; //frame rate
  const Vector2 pos(m_nWinWidth - 128.0f, 30.0f); //hard-coded position
  m_pRenderer->DrawScreenText(s.c_str(), pos); //draw to screen
} //DrawFrameRateText

/// Draw the god mode text to a hard-coded position in the window using the
/// font specified in `gamesettings.xml`.

void CGame::DrawGodModeText(){
    Vector2 playerPos = m_pPlayer->GetPos();
    if (playerPos.x >= 2119 && playerPos.x <= 2333 && playerPos.y >= 330 && playerPos.y <= 570) {
        std::cout << "Player is in the farm" << std::endl;
        const Vector2 pos(64.0f, 30.0f); //hard-coded position
        m_pRenderer->DrawScreenText("Farming..", pos); //draw to screen
    }
    else {
        const Vector2 pos(64.0f, 30.0f); //hard-coded position
        m_pRenderer->DrawScreenText("Not Farming..", pos); //draw to screen
    }
  //const Vector2 pos(64.0f, 30.0f); //hard-coded position
  //m_pRenderer->DrawScreenText("God Mode", pos); //draw to screen
} //DrawGodModeText

void CGame::DrawRadio() {
    //LSpriteDesc2D radio;
    //radio.m_nSpriteIndex = (UINT)eSprite::DeadRadio;
    //radio.m_vPos = { 1600, 950 };
    //if (radioOn)
    //    radio.m_nSpriteIndex = (UINT)eSprite::AliveRadio;
    //if (m_eGameState != eGameState::Victory && m_eGameState != eGameState::Title)
    //    m_pRenderer->Draw(&radio);
}

/// Ask the object manager to draw the game objects. The renderer is notified of
/// the start and end of the frame so that it can let Direct3D do its
/// pipelining jiggery-pokery.

void CGame::RenderFrame(){
  m_pRenderer->BeginFrame(); //required before rendering

  DrawBackground();
  m_pObjectManager->draw(); //draw objects
  m_pParticleEngine->Draw(); //draw particles
  DrawHealthBar();
  DrawClock();
  DrawHunger();
  DrawProgressBar();
  DrawMessage("farmnight");
  DrawMessage("maxfood");
  DrawMessage("escapemessage");
  DrawInstructions();
  DrawRadio();
  DrawParts();

  if (playerpos.x >= 2119 && playerpos.x <= 2333 && playerpos.y >= 330 && playerpos.y <= 570 && !isNight && m_pPlayer->GetHungerCount() < 3) {
      isAbleToFarm = true;
  }
  else {
      isAbleToFarm = false;
  }

  if (m_pPlayer->GetHungerCount() > 0 && m_pPlayer->GetHungerCount() <= 3 && m_pPlayer->GetHealth() < 12) {
      isAbleToEat = true;
  }
  else {
      isAbleToEat = false;
  }
  //&& gotBattery && gotAntenna && gotLogicBoard
  if (playerpos.x >= 1396 && playerpos.x <= 1496 && playerpos.y >= 359 && playerpos.y <= 504 && !radioOn && gotBattery && gotAntenna && gotLogicBoard) {
      isAbleToBuild = true;
  }
  else {
	  isAbleToBuild = false;
  }
  //
  if (playerpos.x >= 962 && playerpos.x <= 1047 && playerpos.y >= 1172 && playerpos.y <= 1191 && radioOn) {
      isAbleToLeave = true;
  }
  else {
      isAbleToLeave = false;
  }

  //cout << playerpos.x << ", " << playerpos.y << endl;
  //cout << isAbleToLeave << endl;

  if (m_pTimer->GetTime() - messageElapsedTime > 3.0f) {
      showMessage = false;
  }

  if (m_pTimer->GetTime() - maxfoodMessageElapsedTime > 3.0f) {
      showfoodMessage = false;
  }

  if (m_pTimer->GetTime() - escapeMessageElapsedTime > 5.0f) {
      showEscapeMessage = false;
  }
  
  if(m_bDrawFrameRate)DrawFrameRateText(); //draw frame rate, if required
  if(m_bGodMode)DrawGodModeText(); //draw god mode text, if required

  m_pRenderer->EndFrame(); //required after rendering
} //RenderFrame

/// Make the camera follow the player, but don't let it get too close to the
/// edge unless the world is smaller than the window, in which case we just
/// center everything.

void CGame::FollowCamera() {
    if (m_pPlayer == nullptr) return; //safety

    Vector3 vCameraPos(m_pPlayer->GetPos()); //player position

    if (m_vWorldSize.x > m_nWinWidth) { //world wider than screen
        vCameraPos.x = std::max(vCameraPos.x, m_nWinWidth / 2.0f); //stay away from the left edge
        vCameraPos.x = std::min(vCameraPos.x, m_vWorldSize.x - m_nWinWidth / 2.0f);  //stay away from the right edge
    }
    else {
        vCameraPos.x = m_vWorldSize.x / 2.0f; //center horizontally.
    }

    if (m_vWorldSize.y > m_nWinHeight) { //world higher than screen
        vCameraPos.y = std::max(vCameraPos.y, m_nWinHeight / 2.0f);  //stay away from the bottom edge
        vCameraPos.y = std::min(vCameraPos.y, m_vWorldSize.y - m_nWinHeight / 2.0f); //stay away from the top edge
    }
    else {
        vCameraPos.y = m_vWorldSize.y / 2.0f; //center vertically
    }

    m_pRenderer->SetCameraPos(vCameraPos); //camera to player
}

Vector3 CGame::GetCameraPosition() {
    if (m_pRenderer != nullptr) {
        return m_pRenderer->GetCameraPos();
    }
    return Vector3(0, 0, 0); // Return a default value if renderer is null
}

/// This function will be called regularly to process and render a frame
/// of animation, which involves the following. Handle keyboard input.
/// Notify the audio player at the start of each frame so that it can prevent
/// multiple copies of a sound from starting on the same frame.  
/// Move the game objects. Render a frame of animation. 

void CGame::ProcessFrame(){
  KeyboardHandler(); //handle keyboard input
  MouseHandler();
  ControllerHandler(); //handle controller input
  m_pAudio->BeginFrame(); //notify audio player that frame has begun
  
  m_pTimer->Tick([&](){ //all time-dependent function calls should go here
    m_fElapsedTime += m_pTimer->GetFrameTime(); //elapsed time in seconds
    m_pObjectManager->move(); //move all objects
    FollowCamera(); //make camera follow player
    m_pParticleEngine->step(); //advance particle animation
  });
  RenderFrame(); //render a frame of animation
  ProcessGameState(); //check for end of game
} //ProcessFrame

int CGame::GetGameHours(){
    return m_nGameHours;
}

std::string CGame::getAmPm() {
    	return m_nAmPm;
}

/// Take action appropriate to the current game state. If the game is currently
/// playing, then if the player has been killed or all turrets have been
/// killed, then enter the wait state. If the game has been in the wait
/// state for longer than 3 seconds, then restart the game.

void CGame::ProcessGameState() {
    static float t = 0; // time at start of game
    switch (m_eGameState) {
    case eGameState::Title:
        if (mousePosNew.x >= 662 && mousePosNew.x <= 1257 && mousePosNew.y >= 496 && mousePosNew.y <= 600 && m_pKeyboard->TriggerDown(VK_LBUTTON)) {
            m_eGameState = eGameState::Level1;
            BeginGame();
        }
        if (mousePosNew.x >= 662 && mousePosNew.x <= 1257 && mousePosNew.y >= 346 && mousePosNew.y <= 450 && m_pKeyboard->TriggerDown(VK_LBUTTON)) {
            m_eGameState = eGameState::Tutorial;
            BeginGame();
        }
        break;
    case eGameState::Tutorial:
        if (mousePosNew.x >= 75 && mousePosNew.x <= 347 && mousePosNew.y >= 64 && mousePosNew.y <= 166 && m_pKeyboard->TriggerDown(VK_LBUTTON)) {
            m_eGameState = eGameState::Title;
            BeginGame();
        }
        break;
    case eGameState::Level1:
        //std::cout << "Zombie Count: " << m_pObjectManager->GetNumTurrets() << std::endl;
        
        if (m_pPlayer == nullptr) {
            gotBattery = false; gotAntenna = false; gotLogicBoard = false;
            radioOn = false;
            m_eGameState = eGameState::Waiting1; // now waiting
            t = m_pTimer->GetTime(); 
        }else if (helpCalled == true) {
            m_eGameState = eGameState::Victory;
            m_pPlayer == nullptr;
            m_pObjectManager->clear();
            m_pParticleEngine->clear();
            //Stop sounds
            m_pAudio->stop(); //stop all  currently playing sounds

            BeginGame();
            
        }                                    // if
        //else if (m_pObjectManager->GetNumTurrets() == 0) {
        //    m_eGameState = eGameState::Waiting2; // now waiting
        //    t = m_pTimer->GetTime();             // start wait timer
        //}
        break;
    case eGameState::Victory:
        t = m_pTimer->GetTime();
        if (m_pKeyboard->TriggerDown(VK_LBUTTON) || m_pKeyboard->TriggerDown(VK_SPACE)) {
            exit(0);
        }
        if (m_pTimer->GetTime() - t > 3.0f) { // 3 seconds has elapsed since level end
            m_eGameState = eGameState::Title;
            BeginGame();
        }              // if
        break;
    /*case eGameState::Level2:
        if (m_pPlayer == nullptr) {
            m_eGameState = eGameState::Waiting2; // now waiting
            t = m_pTimer->GetTime();             // start wait timer
        }                                      // if
        else if (m_pObjectManager->GetNumTurrets() == 0) {
            m_eGameState = eGameState::Waiting3; //now waiting
            t = m_pTimer->GetTime(); //start wait timer
        }
        break;
    case eGameState::Level3:
        if (m_pPlayer == nullptr) {
            m_eGameState = eGameState::Waiting3; // now waiting
            t = m_pTimer->GetTime();             // start wait timer
        }                                      // if
        else if (m_pObjectManager->GetNumTurrets() == 0) {
            m_eGameState = eGameState::Waiting4; //now waiting
            t = m_pTimer->GetTime(); //start wait timer
        }
        break;
    case eGameState::Level4:
        if (m_pPlayer == nullptr) {
            m_eGameState = eGameState::Waiting4; // now waiting
            t = m_pTimer->GetTime();             // start wait timer
        }                                      // if
        else if (m_pObjectManager->GetNumTurrets() == 0) {
            m_eGameState = eGameState::Waiting5; //now waiting
            t = m_pTimer->GetTime(); //start wait timer
        }
        break;
    case eGameState::Level5:
        if (m_pPlayer == nullptr) {
            m_eGameState = eGameState::Waiting5; // now waiting
            t = m_pTimer->GetTime();             // start wait timer
        }                                      // if
        else if (m_pObjectManager->GetNumTurrets() == 0) {
            m_eGameState = eGameState::Waiting6; //now waiting
            t = m_pTimer->GetTime(); //start wait timer
        }
        break;
    case eGameState::Level6:
        if (m_pPlayer == nullptr) {
            m_eGameState = eGameState::Waiting6; // now waiting
            t = m_pTimer->GetTime();             // start wait timer
        }                                      // if
        else if (m_pObjectManager->GetNumTurrets() == 0) {
            m_eGameState = eGameState::Waiting7; //now waiting
            t = m_pTimer->GetTime(); //start wait timer
        }
        break;
    case eGameState::Level7:
        if (m_pPlayer == nullptr) {
            m_eGameState = eGameState::Waiting7; // now waiting
            t = m_pTimer->GetTime();             // start wait timer
        }                                      // if
        /*else if (m_pObjectManager->GetNumTurrets() == 0) {
            m_eGameState = eGameState::Waiting2; //now waiting
            t = m_pTimer->GetTime(); //start wait timer
        }*/
        //break;
    case eGameState::Waiting1:
        //m_pRenderer->DrawScreenText("Game Over", {0, 0});
        if (m_pTimer->GetTime() - t >
            3.0f) { // 3 seconds has elapsed since level end
            // if(m_pObjectManager->GetNumTurrets() == 0) //player won
            // m_nNextLevel = (m_nNextLevel + 1) % 4; //note: 4 instead of 3
            m_eGameState = eGameState::Level1;
            BeginGame(); // restart game
        }              // if
        break;
    case eGameState::Waiting2:
        break;
    case eGameState::Waiting3:
        if (m_pTimer->GetTime() - t >
            3.0f) { // 3 seconds has elapsed since level end
            // if(m_pObjectManager->GetNumTurrets() == 0) //player won
            // m_n NextLevel = (m_nNextLevel + 1) % 4; //note: 4 instead of 3
            m_eGameState = eGameState::Level3;
            BeginGame(); // restart game
        }              // if
        break;
    case eGameState::Waiting4:
        if (m_pTimer->GetTime() - t >
            3.0f) { // 3 seconds has elapsed since level end
            // if(m_pObjectManager->GetNumTurrets() == 0) //player won
            // m_n NextLevel = (m_nNextLevel + 1) % 4; //note: 4 instead of 3
            m_eGameState = eGameState::Level4;
            BeginGame(); // restart game
        }              // if
        break;
    case eGameState::Waiting5:
        if (m_pTimer->GetTime() - t >
            3.0f) { // 3 seconds has elapsed since level end
            // if(m_pObjectManager->GetNumTurrets() == 0) //player won
            // m_n NextLevel = (m_nNextLevel + 1) % 4; //note: 4 instead of 3
            m_eGameState = eGameState::Level5;
            BeginGame(); // restart game
        }              // if
        break;
    case eGameState::Waiting6:
        if (m_pTimer->GetTime() - t >
            3.0f) { // 3 seconds has elapsed since level end
            // if(m_pObjectManager->GetNumTurrets() == 0) //player won
            // m_n NextLevel = (m_nNextLevel + 1) % 4; //note: 4 instead of 3
            m_eGameState = eGameState::Level6;
            BeginGame(); // restart game
        }              // if
        break;
    case eGameState::Waiting7:
        if (m_pTimer->GetTime() - t >
            3.0f) { // 3 seconds has elapsed since level end
            // if(m_pObjectManager->GetNumTurrets() == 0) //player won
            // m_n NextLevel = (m_nNextLevel + 1) % 4; //note: 4 instead of 3
            m_eGameState = eGameState::Level7;
            BeginGame(); // restart game
        }              // if
        break;
    } // switch
} // CheckForEndOfGame 