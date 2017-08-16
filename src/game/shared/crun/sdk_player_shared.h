//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef SDK_PLAYER_SHARED_H
#define SDK_PLAYER_SHARED_H
#ifdef _WIN32
#pragma once
#endif

extern ConVar crun_walkspeed;
extern ConVar crun_runspeed;

// How fast can the player walk?
#define PLAYER_RUN_SPEED crun_runspeed.GetFloat()
#define PLAYER_WALK_SPEED crun_walkspeed.GetFloat()

// Shared header file for players
#if defined( CLIENT_DLL )
#define CSDKPlayer C_SDKPlayer	//FIXME: Lovely naming job between server and client here...
#include "crun/c_sdk_player.h"
#else
#include "crun/sdk_player.h"
#endif

#endif // HL2_PLAYER_SHARED_H