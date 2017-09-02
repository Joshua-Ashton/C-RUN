#include "cbase.h"
#include "singleplay_gamerules.h"
#include "sdk_gamerules_sp.h"
#include "ammodef.h"

#ifdef GAME_DLL
	#include "voice_gamemgr.h"
#endif


// =====Convars=============

// ===Ammo/Damage===

REGISTER_GAMERULES_CLASS( CSDKGameRules );

void InitBodyQue() { }

bool CSDKGameRules::ShouldCollide( int collisionGroup0, int collisionGroup1 )
{
	// The smaller number is always first
	if ( collisionGroup0 > collisionGroup1 )
	{
		// swap so that lowest is always first
		int tmp = collisionGroup0;
		collisionGroup0 = collisionGroup1;
		collisionGroup1 = tmp;
	}

	if ( collisionGroup0 == COLLISION_GROUP_PLAYER_MOVEMENT )
	{
		collisionGroup0 = COLLISION_GROUP_PLAYER;
	}

	if( collisionGroup1 == COLLISION_GROUP_PLAYER_MOVEMENT )
	{
		collisionGroup1 = COLLISION_GROUP_PLAYER;
	}

	//If collisionGroup0 is not a player then NPC_ACTOR behaves just like an NPC.
	if ( collisionGroup1 == COLLISION_GROUP_NPC_ACTOR && collisionGroup0 != COLLISION_GROUP_PLAYER )
	{
		collisionGroup1 = COLLISION_GROUP_NPC;
	}

	//players don't collide against NPC Actors.
	//I could've done this up where I check if collisionGroup0 is NOT a player but I decided to just
	//do what the other checks are doing in this function for consistency sake.
	if ( collisionGroup1 == COLLISION_GROUP_NPC_ACTOR && collisionGroup0 == COLLISION_GROUP_PLAYER )
		return false;
		
	// In cases where NPCs are playing a script which causes them to interpenetrate while riding on another entity,
	// such as a train or elevator, you need to disable collisions between the actors so the mover can move them.
	if ( collisionGroup0 == COLLISION_GROUP_NPC_SCRIPTED && collisionGroup1 == COLLISION_GROUP_NPC_SCRIPTED )
		return false;

	return true;
}

// shared ammo definition
// JAY: Trying to make a more physical bullet response
#define BULLET_MASS_GRAINS_TO_LB(grains)	(0.002285*(grains)/16.0f)
#define BULLET_MASS_GRAINS_TO_KG(grains)	lbs2kg(BULLET_MASS_GRAINS_TO_LB(grains))

// exaggerate all of the forces, but use real numbers to keep them consistent
#define BULLET_IMPULSE_EXAGGERATION			3.5
// convert a velocity in ft/sec and a mass in grains to an impulse in kg in/s
#define BULLET_IMPULSE(grains, ftpersec)	((ftpersec)*12*BULLET_MASS_GRAINS_TO_KG(grains)*BULLET_IMPULSE_EXAGGERATION)

CAmmoDef* GetAmmoDef()
{
	static CAmmoDef def;

	return &def;
}

//=========================================================
//=========================================================
bool CSDKGameRules::IsMultiplayer( void )
{
	return false;
}

void CSDKGameRules::PlayerThink( CBasePlayer *pPlayer )
{
}

#ifdef GAME_DLL

// This being required here is a bug. It should be in shared\BaseGrenade_shared.cpp

class CVoiceGameMgrHelper : public IVoiceGameMgrHelper
{
public:
	virtual bool	CanPlayerHearPlayer( CBasePlayer* pListener, CBasePlayer* pTalker, bool &bProximity ) { return true; }
};

CVoiceGameMgrHelper g_VoiceGameMgrHelper;
IVoiceGameMgrHelper* g_pVoiceGameMgrHelper = &g_VoiceGameMgrHelper;

void CSDKGameRules::InitDefaultAIRelationships( void )
{
	int i, j;

	//  Allocate memory for default relationships
	CBaseCombatCharacter::AllocateDefaultRelationships();

	// --------------------------------------------------------------
	// First initialize table so we can report missing relationships
	// --------------------------------------------------------------
	for (i=0;i<NUM_AI_CLASSES;i++)
	{
		for (j=0;j<NUM_AI_CLASSES;j++)
		{
			// By default all relationships are neutral of priority zero
			CBaseCombatCharacter::SetDefaultRelationship( (Class_T)i, (Class_T)j, D_NU, 0 );
		}
	}

		// ------------------------------------------------------------
		//	> CLASS_PLAYER
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER,			CLASS_NONE,				D_NU, 0);			
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER,			CLASS_PLAYER,			D_NU, 0);			
}

	const char* CSDKGameRules::AIClassText(int classType)
	{
		switch (classType)
		{
			case CLASS_NONE:			return "CLASS_NONE";
			case CLASS_PLAYER:			return "CLASS_PLAYER";
			default:					return "MISSING CLASS in ClassifyText()";
		}
	}
#endif // #ifdef GAME_DLL
