//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#include "cbase.h"
#include "sdk_player.h"
#include "gamerules.h"
#include "entitylist.h"
#include "physics.h"
#include "game.h"
#include "player_resource.h"
#include "engine/IEngineSound.h"
#include "sdk_shareddefs.h"
#include "tier0/vprof.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void Host_Say( edict_t *pEdict, bool teamonly );

extern CBaseEntity*	FindPickerEntityClass( CBasePlayer *pPlayer, char *classname );
extern bool			g_fGameOver;

/*
===========
ClientPutInServer

called each time a player is spawned into the game
============
*/
void ClientPutInServer( edict_t *pEdict, const char *playername )
{
	// Allocate a CBasePlayer for pev, and call spawn
	CSDKPlayer *pPlayer = CSDKPlayer::CreatePlayer( "player", pEdict );
	pPlayer->SetPlayerName( playername );
}


void ClientActive( edict_t *pEdict, bool bLoadGame )
{
	CSDKPlayer *pPlayer = dynamic_cast< CSDKPlayer* >( CBaseEntity::Instance( pEdict ) );
	Assert( pPlayer );

	if ( !pPlayer )
	{
		return;
	}

	pPlayer->InitialSpawn();

	if ( !bLoadGame )
	{
		pPlayer->Spawn();
	}
}

/*
===============
const char *GetGameDescription()

Returns the descriptive name of this .dll.  E.g., Half-Life, or Team Fortress 2
===============
*/
const char *GetGameDescription()
{
	return GAMENAME;
}

/*
//-----------------------------------------------------------------------------
// Purpose: Given a player and optional name returns the entity of that 
//			classname that the player is nearest facing
//			
// Input  :
// Output :
//-----------------------------------------------------------------------------
CBaseEntity* FindEntity( edict_t *pEdict, char *classname)
{
	// If no name was given set bits based on the picked
	if (FStrEq(classname,"")) 
	{
		return (FindPickerEntityClass( static_cast<CBasePlayer*>(GetContainingEntity(pEdict)), classname ));
	}
	return NULL;
}
*/

//-----------------------------------------------------------------------------
// Purpose: Precache game-specific models & sounds
//-----------------------------------------------------------------------------
PRECACHE_REGISTER_BEGIN( GLOBAL, ClientGamePrecache )

	// Player model
	PRECACHE( MODEL, "models/player.mdl");

	// Gib 
	PRECACHE( MODEL, "models/gibs/agibs.mdl" );
	PRECACHE( MODEL, "models/gibs/hgibs.mdl" );
	PRECACHE( MODEL, "models/gibs/hgibs_spine.mdl" );
	PRECACHE( MODEL, "models/gibs/hgibs_scapula.mdl" );
	
	PRECACHE( GAMESOUND, "Player.Death"  );
	PRECACHE( GAMESOUND, "Player.BurnPain"  );

	PRECACHE( GAMESOUND, "Bullets.DefaultNearmiss" );
	PRECACHE( GAMESOUND, "FX_RicochetSound.Ricochet" );

	PRECACHE( GAMESOUND, "Geiger.BeepHigh" );
	PRECACHE( GAMESOUND, "Geiger.BeepLow" );

	PRECACHE( KV_DEP_FILE, "resource/ParticleEmitters.txt" )
PRECACHE_REGISTER_END()

void ClientGamePrecache( void )
{
}

// called by ClientKill and DeadThink
void respawn( CBaseEntity *pEdict, bool fCopyCorpse )
{
	if (gpGlobals->coop || gpGlobals->deathmatch)
	{
		if ( fCopyCorpse )
		{
			// make a copy of the dead body for appearances sake
			((CSDKPlayer *)pEdict)->CreateCorpse();
		}

		// respawn player
		pEdict->Spawn();
	}
	else
	{   // restart the entire server
		#ifdef SWARM_DLL
			engine->ServerCommand( "reload\n" );
			//engine->ServerCommand( "exec reload.cfg\n" );
		#else 
			engine->ServerCommand( "reload\n" );
		#endif
	}
}

void GameStartFrame( void )
{
	VPROF("GameStartFrame()");
	if ( g_fGameOver )
		return;

	gpGlobals->teamplay = (teamplay.GetInt() != 0);
}

void FinishClientPutInServer( CBasePlayer* pPlayer )
{
}

void ClientFullyConnect( edict_t *pEntity )
{
}

//=========================================================
// instantiate the proper game rules object
//=========================================================
void InstallGameRules()
{
	CreateGameRulesObject( "CSDKGameRules" );
}