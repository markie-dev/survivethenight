/// \file GameDefines.h
/// \brief Game specific defines.

#ifndef __L4RC_GAME_GAMEDEFINES_H__
#define __L4RC_GAME_GAMEDEFINES_H__

#include "Defines.h"

/// \brief Sprite enumerated type.
///
/// An enumerated type for the sprites, which will be cast to an unsigned
/// integer and used for the index of the corresponding texture in graphics
/// memory. `Size` must be last.

enum class eSprite: UINT{
  Background, Battery, Bullet, Bullet2, Player, Turret, Smoke, Spark, Tile, Line, Activity, House, Tree, Farm, Title, HealthBar, HealthBar80, HealthBar60, HealthBar40, HealthBar20, HealthBar0, Player2, Turret2, Frame, Sun, Moon, Corn, HalfCorn, Zombie2, Shop, ProgressFrame, Percent, MessageFrame, Background2, NightFarm, MaxFood, LogicBoard, Antenna, DeadRadio, AliveRadio, Victory, PressToFarm, PressToEat, RadioParts, PressToBuild, Radio, Research, PressToLeave, Escape, PlayButton, PlayButton2, TutorialButton, Title2, BackButton, 
  Size  //MUST BE LAST
}; //eSprite

/// \brief Sound enumerated type.
///
/// An enumerated type for the sounds, which will be cast to an unsigned
/// integer and used for the index of the corresponding sample. `Size` must 
/// be last.

enum class eSound: UINT{
  Start, Boom, Clang, Grunt, Gun, Ricochet,
  Size  //MUST BE LAST
}; //eSound

/// \brief Game state enumerated type.
///
/// An enumerated type for the game state, which can be either playing or
/// waiting a short interval for the level to end after winning or losing to
/// let sounds and particle effects play to the end without being cut short.

enum class eGameState{
  Level1, Level2, Level3, Level4, Level5, Level6, Level7, Waiting1, Waiting2, Waiting3, Waiting4, Waiting5, Waiting6, Waiting7, Title, Victory, Tutorial,
}; //eGameState

#endif //__L4RC_GAME_GAMEDEFINES_H__
