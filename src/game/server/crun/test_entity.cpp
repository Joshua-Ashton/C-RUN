//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: 
//
//=============================================================================

// If you don't have this, nothing will work
#include "cbase.h"

// AI
#include "ai_basenpc.h"

// Since most of of our entites are models, we need this.
#include "baseanimating.h"
#include "baseglowanimating.h"

//Sprite Attachment
#include "sprite.h"

// Particle testing. 
#include "particle_parse.h"
#include "particle_system.h"

// Player Pickup
#include "player_pickup.h"

// Name of our entity's model. We use this for cbaseanimating.
#define	ENTITY_MODEL	"models/props_shapes/cube.mdl"

//----------------------------------------------------------
// Basic pickup example. 
//----------------------------------------------------------
class CrHetoricalTestPickup: public CBaseGlowAnimating, public CDefaultPlayerPickupVPhysics
{
public:
	DECLARE_CLASS( CrHetoricalTestPickup, CBaseGlowAnimating );
	DECLARE_DATADESC();
 
	CrHetoricalTestPickup()
	{
	}

	bool CreateVPhysics()
	{
		VPhysicsInitNormal( SOLID_VPHYSICS, 0, false );
		return true;
	}

	void Spawn( void );
	void Precache( void );
	void OnUse();

	//Use
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	int ObjectCaps();

	//Pickup
	void Pickup( void );
	void OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason );
	void OnPhysGunDrop( CBasePlayer *pPhysGunUser, PhysGunDrop_t reason );

	void InputTestEntity ( inputdata_t &inputData );

private:

	CHandle<CBasePlayer>	m_hPhysicsAttacker;

	COutputEvent m_OnPlayerPickup;
	COutputEvent m_OnPhysGunDrop;
	COutputEvent m_OnExplode;

	COutputEvent m_OnTestInput;
};

//----------------------------------------------------------
// Basic pickup example. 
//----------------------------------------------------------
LINK_ENTITY_TO_CLASS( rhetorical_test_pickup, CrHetoricalTestPickup );

BEGIN_DATADESC(  CrHetoricalTestPickup )

	// Save/load
	DEFINE_USEFUNC( Use ),

	// Output
	DEFINE_INPUTFUNC( FIELD_VOID, "TestEntity", InputTestEntity ),
	DEFINE_OUTPUT( m_OnPlayerPickup, "OnPlayerPickup" ),
	DEFINE_OUTPUT( m_OnPhysGunDrop, "OnPhysGunDrop" ),

	DEFINE_OUTPUT( m_OnTestInput, "OnTestInput" ),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Precache assets used by the entity
//-----------------------------------------------------------------------------
void CrHetoricalTestPickup::Precache( void )
{
	PrecacheModel( ENTITY_MODEL );
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CrHetoricalTestPickup::Spawn( void )
{
	Precache();
	SetModel( ENTITY_MODEL );
	SetSolid( SOLID_VPHYSICS );

	// In order to pick it up, needs to be physics.
	CreateVPhysics();

	SetUse( &CrHetoricalTestPickup::Use );
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CrHetoricalTestPickup::ObjectCaps()
{ 
	int caps = BaseClass::ObjectCaps();

	caps |= FCAP_IMPULSE_USE;

	return caps;
}

//-----------------------------------------------------------------------------
// Purpose: On +use
//-----------------------------------------------------------------------------
void CrHetoricalTestPickup::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBasePlayer *pPlayer = ToBasePlayer( pActivator );
	if ( pPlayer )
	{
		pPlayer->PickupObject( this );
		OnUse();
	}
}

//-----------------------------------------------------------------------------
// Purpose: On +use funtion
//-----------------------------------------------------------------------------
void CrHetoricalTestPickup::OnUse()
{
	SetGlowVector( 0.3f, 0.1f, 0.6f );
	AddGlowEffect();
	m_OnTestInput.FireOutput( this, this );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CrHetoricalTestPickup::OnPhysGunPickup( CBasePlayer *pPhysGunUser, PhysGunPickup_t reason )
{
	m_hPhysicsAttacker = pPhysGunUser;

	if ( reason == PICKED_UP_BY_CANNON || reason == PICKED_UP_BY_PLAYER )
	{
		m_OnPlayerPickup.FireOutput( pPhysGunUser, this );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CrHetoricalTestPickup::OnPhysGunDrop( CBasePlayer *pPhysGunUser, PhysGunDrop_t reason )
{
	RemoveGlowEffect();
	m_OnPhysGunDrop.FireOutput( pPhysGunUser, this );
}

void CrHetoricalTestPickup::InputTestEntity (inputdata_t &inputData)
{
	//Fire output
	m_OnTestInput.FireOutput( this, this );
}