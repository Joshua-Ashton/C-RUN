//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_custom.h"
#include "NPCevent.h"
#include "basecombatcharacter.h"
#include "AI_BaseNPC.h"
#include "player.h"
#include "in_buttons.h"
#include "triggers.h"
#include "AI_Memory.h"
#include "soundent.h"
#include "gamestats.h"
#include "Filesystem.h"
#include "te_effect_dispatch.h"
#include "particle_parse.h"

#ifdef HL2_DLL
#include "game.h"
#include "weapon_rpg.h"
#include "grenade_ar2.h"
#include "prop_combine_ball.h"
#include "rumble_shared.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern INetworkStringTable *g_pStringTableCustomWeaponsFactory;

acttable_t CWeaponCustom::m_acttable[] =
{
	//player anims
	{ ACT_HL2MP_IDLE, ACT_HL2MP_IDLE_AR2, false },
	{ ACT_HL2MP_RUN, ACT_HL2MP_RUN_AR2, false },
	{ ACT_HL2MP_IDLE_CROUCH, ACT_HL2MP_IDLE_CROUCH_AR2, false },
	{ ACT_HL2MP_WALK_CROUCH, ACT_HL2MP_WALK_CROUCH_AR2, false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK, ACT_HL2MP_GESTURE_RANGE_ATTACK_AR2, false },
	{ ACT_HL2MP_GESTURE_RELOAD, ACT_GESTURE_RELOAD_SMG1, false },
	{ ACT_HL2MP_JUMP, ACT_HL2MP_JUMP_AR2, false },
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_AR2, false },
	//npc anims
};

IMPLEMENT_ACTTABLE( CWeaponCustom );

IMPLEMENT_SERVERCLASS_ST( CWeaponCustom, DT_WeaponCustom )
	SendPropInt( SENDINFO( scopeNum ), 1, SPROP_UNSIGNED ),
END_SEND_TABLE()

BEGIN_DATADESC( CWeaponCustom )
#ifdef HL2_DLL
	DEFINE_FIELD( m_hMissile, FIELD_EHANDLE ),
	DEFINE_FIELD( m_hMissile2, FIELD_EHANDLE ),
#endif
	DEFINE_FIELD( m_bInZoom, FIELD_BOOLEAN ),
END_DATADESC()

//=========================================================
CWeaponCustom::CWeaponCustom()
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCustom::Precache( void )
{
	UTIL_PrecacheOther( "grenade_ar2" );
	UTIL_PrecacheOther( "prop_combine_ball" );
	UTIL_PrecacheOther( "env_entity_dissolver" );

	BaseClass::Precache();

	m_fMinRange1 = GetWpnDataCustom().primary.minRange;
	m_fMaxRange1 = GetWpnDataCustom().primary.maxRange;
	m_fMinRange2 = GetWpnDataCustom().secondary.minRange;
	m_fMaxRange2 = GetWpnDataCustom().secondary.maxRange;

	m_bFiresUnderwater = GetWpnDataCustom().primary.canFireUnderwater;
	m_bAltFiresUnderwater = GetWpnDataCustom().secondary.canFireUnderwater;
	m_bReloadsSingly = GetWpnDataCustom().m_sCanReloadSingly;

	char data[512];
	V_snprintf( data, 512, "%d|%s", entindex(), m_iClassname.ToCStr() );

	g_pStringTableCustomWeaponsFactory->AddString( true, data, V_strlen( data ) );
}

//-----------------------------------------------------------------------------
// Purpose: Give this weapon longer range when wielded by an ally NPC.
//-----------------------------------------------------------------------------
void CWeaponCustom::Equip( CBaseCombatCharacter *pOwner )
{
	/*
	if( pOwner->Classify() == CLASS_PLAYER_ALLY )
	{
		m_fMaxRange1 = 3000;
	}
	else
	{
		m_fMaxRange1 = 1400;
	}
	*/

	//Commented out customization for now untill it is fixed.
	ApplyCustomization();

	BaseClass::Equip( pOwner );
}


void CWeaponCustom::ApplyCustomization(void)
{
	if (GetWpnDataCustom().m_sUsesCustomization)
	{
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

		if ( !pPlayer || !pPlayer->GetViewModel() )
			return;

		pPlayer->GetViewModel()->m_nSkin = GetWpnDataCustom().m_sWeaponSkin;

		if ( GetWpnDataCustom().m_sUsesBodygroupSection1 )
			pPlayer->GetViewModel()->SetBodygroup( GetWpnDataCustom().m_sBodygroup1, GetWpnDataCustom().m_sSubgroup1 );

		if ( GetWpnDataCustom().m_sUsesBodygroupSection2 )
			pPlayer->GetViewModel()->SetBodygroup( GetWpnDataCustom().m_sBodygroup2, GetWpnDataCustom().m_sSubgroup2 );

		if ( GetWpnDataCustom().m_sUsesBodygroupSection3 )
			pPlayer->GetViewModel()->SetBodygroup( GetWpnDataCustom().m_sBodygroup3, GetWpnDataCustom().m_sSubgroup3 );

		if ( GetWpnDataCustom().m_sUsesBodygroupSection4 )
			pPlayer->GetViewModel()->SetBodygroup( GetWpnDataCustom().m_sBodygroup4, GetWpnDataCustom().m_sSubgroup4 );

		if ( GetWpnDataCustom().m_sUsesBodygroupSection5 )
			pPlayer->GetViewModel()->SetBodygroup( GetWpnDataCustom().m_sBodygroup5, GetWpnDataCustom().m_sSubgroup5 );

		if ( GetWpnDataCustom().m_sUsesBodygroupSection6 )
			pPlayer->GetViewModel()->SetBodygroup( GetWpnDataCustom().m_sBodygroup6, GetWpnDataCustom().m_sSubgroup6 );
	}
}

void CWeaponCustom::ItemPostFrame( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( !pOwner )
		return;

	if ( GetWpnDataCustom().m_sUsesZoom )
		CheckZoomToggle();

	//Track the duration of the fire
	//FIXME: Check for IN_ATTACK2 as well?
	//FIXME: What if we're calling ItemBusyFrame?
	m_fFireDuration = pOwner->m_nButtons & IN_ATTACK ? m_fFireDuration + gpGlobals->frametime : 0.0f;

	if ( UsesClipsForAmmo1() )
		CheckReload();

	if ( pOwner->m_nButtons & IN_ATTACK2 && m_flNextPrimaryAttack <= gpGlobals->curtime )
	{
		// Clip empty? Or out of ammo on a no-clip weapon?
		if ( !IsMeleeWeapon() && ( UsesClipsForAmmo1() && m_iClip1 <= 0 || !UsesClipsForAmmo1() && pOwner->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 ) )
		{
			HandleFireOnEmpty();
		}
		else if ( pOwner->GetWaterLevel() == 3 && m_bFiresUnderwater == false )
		{
			// This weapon doesn't fire underwater
			WeaponSound( EMPTY );
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
			return;
		}
		else
		{
			//NOTENOTE: There is a bug with this code with regards to the way machine guns catch the leading edge trigger
			//			on the player hitting the attack key.  It relies on the gun catching that case in the same frame.
			//			However, because the player can also be doing a secondary attack, the edge trigger may be missed.
			//			We really need to hold onto the edge trigger and only clear the condition when the gun has fired its
			//			first shot.  Right now that's too much of an architecture change -- jdw

			// If the firing button was just pressed, or the alt-fire just released, reset the firing time
			if ( pOwner->m_afButtonPressed & IN_ATTACK || pOwner->m_afButtonReleased & IN_ATTACK2 )
			{
				m_flNextPrimaryAttack = gpGlobals->curtime;
			}

			SecondaryAttack();
		}
	}

	if ( pOwner->m_nButtons & IN_ATTACK && m_flNextPrimaryAttack <= gpGlobals->curtime )
	{
		// Clip empty? Or out of ammo on a no-clip weapon?
		if ( !IsMeleeWeapon() &&
			( UsesClipsForAmmo1() && m_iClip1 <= 0 || !UsesClipsForAmmo1() && pOwner->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 ) )
		{
			HandleFireOnEmpty();
		}
		else if ( pOwner->GetWaterLevel() == 3 && m_bFiresUnderwater == false )
		{
			// This weapon doesn't fire underwater
			WeaponSound( EMPTY );
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
			return;
		}
		else
		{
			//NOTENOTE: There is a bug with this code with regards to the way machine guns catch the leading edge trigger
			//			on the player hitting the attack key.  It relies on the gun catching that case in the same frame.
			//			However, because the player can also be doing a secondary attack, the edge trigger may be missed.
			//			We really need to hold onto the edge trigger and only clear the condition when the gun has fired its
			//			first shot.  Right now that's too much of an architecture change -- jdw

			// If the firing button was just pressed, or the alt-fire just released, reset the firing time
			if ( pOwner->m_afButtonPressed & IN_ATTACK || pOwner->m_afButtonReleased & IN_ATTACK2 )
			{
				m_flNextPrimaryAttack = gpGlobals->curtime;
			}

			PrimaryAttack();
		}
	}

	// -----------------------
	//  Reload pressed / Clip Empty
	// -----------------------
	if ( pOwner->m_nButtons & IN_RELOAD && UsesClipsForAmmo1() && !m_bInReload )
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		Reload();
		m_fFireDuration = 0.0f;
	}

	// -----------------------
	//  No buttons down
	// -----------------------
	if ( !( pOwner->m_nButtons & IN_ATTACK || pOwner->m_nButtons & IN_ATTACK2 || pOwner->m_nButtons & IN_RELOAD ) )
	{
		// no fire buttons down or reloading
		if ( !ReloadOrSwitchWeapons() && m_bInReload == false )
		{
			WeaponIdle();
		}
	}
}

void CWeaponCustom::ItemBusyFrame( void )
{
	// Allow zoom toggling even when we're reloading
	if ( GetWpnDataCustom().m_sUsesZoom )
	{
		CheckZoomToggle();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCustom::FireNPCPrimaryAttack( CBaseCombatCharacter *pOperator, Vector &vecShootOrigin, Vector &vecShootDir )
{
	// FIXME: use the returned number of bullets to account for >10hz firerate
	WeaponSoundRealtime( SINGLE_NPC );

	CSoundEnt::InsertSound( SOUND_COMBAT | SOUND_CONTEXT_GUNFIRE, pOperator->GetAbsOrigin(), SOUNDENT_VOLUME_MACHINEGUN, 0.2, pOperator, SOUNDENT_CHANNEL_WEAPON, pOperator->GetEnemy() );
	if ( IsPrimaryBullet() )
	{
		pOperator->FireBullets( 1, vecShootOrigin, vecShootDir, VECTOR_CONE_PRECALCULATED, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 2, entindex(), 0 );
		FireBulletsInfo_t info;
		info.m_iShots = GetWpnDataCustom().primary.shotCount;
		info.m_flDamage = GetWpnDataCustom().primary.damage;
		info.m_flPlayerDamage = GetWpnDataCustom().primary.damage;
		info.m_vecSrc = vecShootOrigin;
		info.m_vecDirShooting = vecShootDir;
		info.m_vecSpread = VECTOR_CONE_PRECALCULATED;
		info.m_flDistance = MAX_TRACE_LENGTH;
		info.m_iAmmoType = m_iPrimaryAmmoType;
		info.m_iTracerFreq = 2;
		FireBullets( info );
		pOperator->DoMuzzleFlash();
	}
	m_iClip1 = m_iClip1 - 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCustom::Operator_ForceNPCFire( CBaseCombatCharacter *pOperator, bool bSecondary )
{
	// Ensure we have enough rounds in the clip
	m_iClip1++;

	Vector vecShootOrigin, vecShootDir;
	QAngle	angShootDir;
	GetAttachment( LookupAttachment( "muzzle" ), vecShootOrigin, angShootDir );
	AngleVectors( angShootDir, &vecShootDir );
	FireNPCPrimaryAttack( pOperator, vecShootOrigin, vecShootDir );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCustom::Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
{
	switch ( pEvent->Event() )
	{
	case EVENT_WEAPON_SMG1:
	{
		Vector vecShootOrigin;
		QAngle angDiscard;

		// Support old style attachment point firing
		if ( pEvent->options == NULL || pEvent->options[0] == '\0' || !pOperator->GetAttachment( pEvent->options, vecShootOrigin, angDiscard ) )
		{
			vecShootOrigin = pOperator->Weapon_ShootPosition();
		}

		CAI_BaseNPC *npc = pOperator->MyNPCPointer();
		ASSERT( npc != NULL );
		Vector vecShootDir = npc->GetActualShootTrajectory( vecShootOrigin );

		FireNPCPrimaryAttack( pOperator, vecShootOrigin, vecShootDir );
	}
	break;

	/*//FIXME: Re-enable/
	case EVENT_WEAPON_AR2_GRENADE:
	{
	CAI_BaseNPC *npc = pOperator->MyNPCPointer();

	Vector vecShootOrigin, vecShootDir;
	vecShootOrigin = pOperator->Weapon_ShootPosition();
	vecShootDir = npc->GetShootEnemyDir( vecShootOrigin );

	Vector vecThrow = m_vecTossVelocity;

	CGrenadeAR2 *pGrenade = (CGrenadeAR2*)Create( "grenade_ar2", vecShootOrigin, vec3_angle, npc );
	pGrenade->SetAbsVelocity( vecThrow );
	pGrenade->SetLocalAngularVelocity( QAngle( 0, 400, 0 ) );
	pGrenade->SetMoveType( MOVETYPE_FLYGRAVITY );
	pGrenade->m_hOwner			= npc;
	pGrenade->m_pMyWeaponAR2	= this;
	pGrenade->SetDamage(sk_npc_dmg_ar2_grenade.GetFloat());

	// FIXME: arrgg ,this is hard coded into the weapon???
	m_flNextGrenadeCheck = gpGlobals->curtime + 6;// wait six seconds before even looking again to see if a grenade can be thrown.

	m_iClip2--;
	}
	break;
	//*/

	default:
		BaseClass::Operator_HandleAnimEvent( pEvent, pOperator );
		break;
	}
}

void CWeaponCustom::ShootBullets( bool isPrimary, bool usePrimaryAmmo )
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if ( !pPlayer )
		return;
	if ( isPrimary || usePrimaryAmmo )
	{
		// Abort here to handle burst and auto fire modes
		if ( UsesClipsForAmmo1() && m_iClip1 == 0 || !UsesClipsForAmmo1() && !pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) )
			return;
	}
	else
	{
		// Abort here to handle burst and auto fire modes
		if ( UsesClipsForAmmo2() && m_iClip2 == 0 || !UsesClipsForAmmo2() && !pPlayer->GetAmmoCount( m_iSecondaryAmmoType ) )
			return;
	}

	m_nShotsFired++;

	pPlayer->DoMuzzleFlash();

	// To make the firing framerate independent, we may have to fire more than one bullet here on low-framerate systems, 
	// especially if the weapon we're firing has a really fast rate of fire.
	int iBulletsToFire = 0;

	float fireRate = isPrimary ? GetPrimaryFireRate() : GetSecondaryFireRate();
	// MUST call sound before removing a round from the clip of a CHLMachineGun
	while ( m_flNextPrimaryAttack <= gpGlobals->curtime )
	{
		WeaponSound( isPrimary ? SINGLE : WPN_DOUBLE, m_flNextPrimaryAttack );
		m_flNextPrimaryAttack = m_flNextPrimaryAttack + fireRate;
		iBulletsToFire++;
	}

	if ( isPrimary )
	{
		m_iPrimaryAttacks++;
		// Make sure we don't fire more than the amount in the clip, if this weapon uses clips
		if ( !GetWpnDataCustom().primary.infiniteAmmoEnabled )
		{
			if ( UsesClipsForAmmo1() )
			{
				if ( iBulletsToFire > m_iClip1 )
					iBulletsToFire = m_iClip1;
				m_iClip1 -= iBulletsToFire;
			}
		}
	}
	else if ( usePrimaryAmmo )
	{
		m_iSecondaryAttacks++;
		// Make sure we don't fire more than the amount in the clip, if this weapon uses clips
		if ( !GetWpnDataCustom().secondary.infiniteAmmoEnabled )
		{
			if ( UsesClipsForAmmo1() )
			{
				if ( iBulletsToFire > m_iClip1 )
					iBulletsToFire = m_iClip1;
				m_iClip1 -= iBulletsToFire;
			}
		}
	}
	else
	{
		m_iSecondaryAttacks++;
		// Make sure we don't fire more than the amount in the clip, if this weapon uses clips
		if ( !GetWpnDataCustom().secondary.infiniteAmmoEnabled )
		{
			if ( UsesClipsForAmmo2() )
			{
				if ( iBulletsToFire > m_iClip2 )
					iBulletsToFire = m_iClip2;
				m_iClip2 -= iBulletsToFire;
			}
		}
	}
	gamestats->Event_WeaponFired( pPlayer, isPrimary, GetClassname() );

	const CustomWeaponInfo::WepDef& wpn = isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary;

	// Fire the bullets
	FireBulletsInfo_t info;
	info.m_iShots = wpn.shotCount;
	info.m_flDamage = wpn.damage;
	info.m_flPlayerDamage = wpn.damage;
	info.m_vecSpread = isPrimary ? GetBulletSpreadPrimary() : GetBulletSpreadSecondary();
	info.m_vecSrc = pPlayer->Weapon_ShootPosition();
	info.m_vecDirShooting = pPlayer->GetAutoaimVector( AUTOAIM_SCALE_DEFAULT );
	info.m_flDistance = MAX_TRACE_LENGTH;
	info.m_iAmmoType = m_iPrimaryAmmoType;
	info.m_iTracerFreq = 2;
	FireBullets( info );

	//Factor in the view kick
	AddViewKick( wpn.recoilEasyDampen, wpn.recoilDegrees, wpn.recoilSeconds );

	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), SOUNDENT_VOLUME_MACHINEGUN, 0.2, pPlayer );

	SendWeaponAnim( GetPrimaryAttackActivity() );
	pPlayer->SetAnimation( PLAYER_ATTACK1 );

	// Register a muzzleflash for the AI
	pPlayer->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );
}

void CWeaponCustom::ShootBulletsLeft( bool isPrimary, bool usePrimaryAmmo )
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if ( !pPlayer )
		return;
	if ( isPrimary || usePrimaryAmmo )
	{
		// Abort here to handle burst and auto fire modes
		if ( UsesClipsForAmmo1() && m_iClip1 == 0 || !UsesClipsForAmmo1() && !pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) )
			return;
	}
	else
	{
		// Abort here to handle burst and auto fire modes
		if ( UsesClipsForAmmo2() && m_iClip2 == 0 || !UsesClipsForAmmo2() && !pPlayer->GetAmmoCount( m_iSecondaryAmmoType ) )
			return;
	}

	m_nShotsFired++;

	pPlayer->DoMuzzleFlash();

	// To make the firing framerate independent, we may have to fire more than one bullet here on low-framerate systems, 
	// especially if the weapon we're firing has a really fast rate of fire.
	int iBulletsToFire = 0;

	float fireRate = isPrimary ? GetPrimaryFireRate() : GetSecondaryFireRate();
	// MUST call sound before removing a round from the clip of a CHLMachineGun
	while ( m_flNextPrimaryAttack <= gpGlobals->curtime )
	{
		WeaponSound( isPrimary ? SINGLE : WPN_DOUBLE, m_flNextPrimaryAttack );
		m_flNextPrimaryAttack = m_flNextPrimaryAttack + fireRate;
		iBulletsToFire++;
	}

	if ( isPrimary )
	{
		m_iPrimaryAttacks++;
		// Make sure we don't fire more than the amount in the clip, if this weapon uses clips
		if ( !GetWpnDataCustom().primary.infiniteAmmoEnabled )
		{
			if ( UsesClipsForAmmo1() )
			{
				if ( iBulletsToFire > m_iClip1 )
					iBulletsToFire = m_iClip1;
				m_iClip1 -= iBulletsToFire;
			}
		}
	}
	else if ( usePrimaryAmmo )
	{
		m_iSecondaryAttacks++;
		// Make sure we don't fire more than the amount in the clip, if this weapon uses clips
		if ( !GetWpnDataCustom().secondary.infiniteAmmoEnabled )
		{
			if ( UsesClipsForAmmo1() )
			{
				if ( iBulletsToFire > m_iClip1 )
					iBulletsToFire = m_iClip1;
				m_iClip1 -= iBulletsToFire;
			}
		}
	}
	else
	{
		m_iSecondaryAttacks++;
		// Make sure we don't fire more than the amount in the clip, if this weapon uses clips
		if ( !GetWpnDataCustom().secondary.infiniteAmmoEnabled )
		{
			if ( UsesClipsForAmmo2() )
			{
				if ( iBulletsToFire > m_iClip2 )
					iBulletsToFire = m_iClip2;
				m_iClip2 -= iBulletsToFire;
			}
		}
	}
	gamestats->Event_WeaponFired( pPlayer, isPrimary, GetClassname() );

	const CustomWeaponInfo::WepDef& wpn = isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary;

	// Fire the bullets
	FireBulletsInfo_t info;
	info.m_iShots = wpn.shotCount;
	info.m_flDamage = wpn.damage;
	info.m_flPlayerDamage = wpn.damage;
	info.m_vecSpread = isPrimary ? GetBulletSpreadPrimary() : GetBulletSpreadSecondary();
	info.m_vecSrc = pPlayer->Weapon_ShootPosition();
	info.m_vecDirShooting = pPlayer->GetAutoaimVector( AUTOAIM_SCALE_DEFAULT );
	info.m_flDistance = MAX_TRACE_LENGTH;
	info.m_iAmmoType = m_iPrimaryAmmoType;
	info.m_iTracerFreq = 2;
	FireBullets( info );

	//Factor in the view kick
	AddViewKick( wpn.recoilEasyDampen, wpn.recoilDegrees, wpn.recoilSeconds );

	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), SOUNDENT_VOLUME_MACHINEGUN, 0.2, pPlayer );

	SendWeaponAnim( GetLeftGunActivity() );
	pPlayer->SetAnimation( PLAYER_ATTACK1 );

	// Register a muzzleflash for the AI
	pPlayer->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );
}

void CWeaponCustom::ShootBulletsRight( bool isPrimary, bool usePrimaryAmmo )
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if ( !pPlayer )
		return;
	if ( isPrimary || usePrimaryAmmo )
	{
		// Abort here to handle burst and auto fire modes
		if ( UsesClipsForAmmo1() && m_iClip1 == 0 || !UsesClipsForAmmo1() && !pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) )
			return;
	}
	else
	{
		// Abort here to handle burst and auto fire modes
		if ( UsesClipsForAmmo2() && m_iClip2 == 0 || !UsesClipsForAmmo2() && !pPlayer->GetAmmoCount( m_iSecondaryAmmoType ) )
			return;
	}

	m_nShotsFired++;

	pPlayer->DoMuzzleFlash();

	// To make the firing framerate independent, we may have to fire more than one bullet here on low-framerate systems, 
	// especially if the weapon we're firing has a really fast rate of fire.
	int iBulletsToFire = 0;

	float fireRate = isPrimary ? GetPrimaryFireRate() : GetSecondaryFireRate();
	// MUST call sound before removing a round from the clip of a CHLMachineGun
	while ( m_flNextPrimaryAttack <= gpGlobals->curtime )
	{
		WeaponSound( isPrimary ? SINGLE : WPN_DOUBLE, m_flNextPrimaryAttack );
		m_flNextPrimaryAttack = m_flNextPrimaryAttack + fireRate;
		iBulletsToFire++;
	}

	if ( isPrimary )
	{
		m_iPrimaryAttacks++;
		// Make sure we don't fire more than the amount in the clip, if this weapon uses clips
		if ( !GetWpnDataCustom().primary.infiniteAmmoEnabled )
		{
			if ( UsesClipsForAmmo1() )
			{
				if ( iBulletsToFire > m_iClip1 )
					iBulletsToFire = m_iClip1;
				m_iClip1 -= iBulletsToFire;
			}
		}
	}
	else if ( usePrimaryAmmo )
	{
		m_iSecondaryAttacks++;
		// Make sure we don't fire more than the amount in the clip, if this weapon uses clips
		if ( !GetWpnDataCustom().secondary.infiniteAmmoEnabled )
		{
			if ( UsesClipsForAmmo1() )
			{
				if ( iBulletsToFire > m_iClip1 )
					iBulletsToFire = m_iClip1;
				m_iClip1 -= iBulletsToFire;
			}
		}
	}
	else
	{
		m_iSecondaryAttacks++;
		// Make sure we don't fire more than the amount in the clip, if this weapon uses clips
		if ( !GetWpnDataCustom().secondary.infiniteAmmoEnabled )
		{
			if ( UsesClipsForAmmo2() )
			{
				if ( iBulletsToFire > m_iClip2 )
					iBulletsToFire = m_iClip2;
				m_iClip2 -= iBulletsToFire;
			}
		}
	}
	gamestats->Event_WeaponFired( pPlayer, isPrimary, GetClassname() );

	const CustomWeaponInfo::WepDef& wpn = isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary;

	// Fire the bullets
	FireBulletsInfo_t info;
	info.m_iShots = wpn.shotCount;
	info.m_flDamage = wpn.damage;
	info.m_flPlayerDamage = wpn.damage;
	info.m_vecSpread = isPrimary ? GetBulletSpreadPrimary() : GetBulletSpreadSecondary();
	info.m_vecSrc = pPlayer->Weapon_ShootPosition();
	info.m_vecDirShooting = pPlayer->GetAutoaimVector( AUTOAIM_SCALE_DEFAULT );
	info.m_flDistance = MAX_TRACE_LENGTH;
	info.m_iAmmoType = m_iPrimaryAmmoType;
	info.m_iTracerFreq = 2;
	FireBullets( info );

	//Factor in the view kick
	AddViewKick( wpn.recoilEasyDampen, wpn.recoilDegrees, wpn.recoilSeconds );

	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), SOUNDENT_VOLUME_MACHINEGUN, 0.2, pPlayer );

	SendWeaponAnim( GetRightGunActivity() );
	pPlayer->SetAnimation( PLAYER_ATTACK1 );

	// Register a muzzleflash for the AI
	pPlayer->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );
}

#ifdef HL2_DLL
extern int g_interactionPlayerLaunchedRPG;

void CWeaponCustom::ShootProjectile( bool isPrimary, bool usePrimaryAmmo )
{
	// Can't have an active missile out
	if ( m_hMissile != NULL )
		return;

	if ( m_hMissile2 != NULL )
		return;
	// Can't be reloading
	if ( GetActivity() == ACT_VM_RELOAD )
		return;

	float fireRate = isPrimary ? GetPrimaryFireRate() : GetSecondaryFireRate();

	if ( isPrimary )
	{
		m_flNextPrimaryAttack = gpGlobals->curtime + fireRate;
		SendWeaponAnim( ACT_VM_PRIMARYATTACK );
		WeaponSound( SINGLE );
	}
	else
	{
		m_flNextSecondaryAttack = gpGlobals->curtime + fireRate;
		SendWeaponAnim( ACT_VM_SECONDARYATTACK );
		WeaponSound( WPN_DOUBLE );
	}

	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	Vector	vForward, vRight, vUp;

	pOwner->EyeVectors( &vForward, &vRight, &vUp );

	Vector	muzzlePoint = pOwner->Weapon_ShootPosition() + vForward * 12.0f + vRight * 6.0f + vUp * -3.0f;

	QAngle vecAngles;
	VectorAngles( vForward, vecAngles );
	m_hMissile = CMissile::Create( muzzlePoint, vecAngles, GetOwner()->edict() );

	//	m_hMissile->m_hOwner = this;

		// If the shot is clear to the player, give the missile a grace period
	trace_t	tr;
	Vector vecEye = pOwner->EyePosition();
	UTIL_TraceLine( vecEye, vecEye + vForward * 128, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );
	if ( tr.fraction == 1.0 )
		m_hMissile->SetGracePeriod( 0.3 );

	// Register a muzzleflash for the AI
	pOwner->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	pOwner->RumbleEffect( RUMBLE_SHOTGUN_SINGLE, 0, RUMBLE_FLAG_RESTART );

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired( pOwner, true, GetClassname() );

	m_nShotsFired++;

	const CustomWeaponInfo::WepDef& wpn = isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary;
	if ( wpn.hasRecoilRPGMissle )
		AddViewKick( wpn.recoilEasyDampen, wpn.recoilDegrees, wpn.recoilSeconds );
	
	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 1000, 0.2, GetOwner(), SOUNDENT_CHANNEL_WEAPON );

	if ( !wpn.infiniteAmmoEnabled )
	{
		if ( isPrimary )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else if ( usePrimaryAmmo )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else
		{
			pOwner->RemoveAmmo( 1, m_iSecondaryAmmoType );
		}
	}

	// Check to see if we should trigger any RPG firing triggers
	//Yes, these now work.
	int iCount = g_hWeaponFireTriggers.Count();
	for ( int i = 0; i < iCount; i++ )
	{
		if ( g_hWeaponFireTriggers[i]->IsTouching( pOwner ) )
		{
			if ( FClassnameIs( g_hWeaponFireTriggers[i], "trigger_rpgfire" ) )
			{
				g_hWeaponFireTriggers[i]->ActivateMultiTrigger( pOwner );
			}
		}
	}

	if ( hl2_episodic.GetBool() )
	{
		CAI_BaseNPC **ppAIs = g_AI_Manager.AccessAIs();
		int nAIs = g_AI_Manager.NumAIs();

		string_t iszStriderClassname = AllocPooledString( "npc_strider" );

		for ( int i = 0; i < nAIs; i++ )
		{
			if ( ppAIs[i]->m_iClassname == iszStriderClassname )
			{
				ppAIs[i]->DispatchInteraction( g_interactionPlayerLaunchedRPG, NULL, m_hMissile );
			}
		}
	}
}

void CWeaponCustom::ShootProjectileRight( bool isPrimary, bool usePrimaryAmmo )
{
	// Can't have an active missile out
	if ( m_hMissile != NULL )
		return;

	if ( m_hMissile2 != NULL )
		return;

	// Can't be reloading
	if ( GetActivity() == ACT_VM_RELOAD )
		return;
	
	float fireRate = isPrimary ? GetPrimaryFireRate() : GetSecondaryFireRate();

	if ( isPrimary )
	{
		m_flNextPrimaryAttack = gpGlobals->curtime + fireRate;
		SendWeaponAnim( ACT_VM_PRIMARYATTACK_R );
		WeaponSound( SINGLE );
	}
	else
	{
		m_flNextSecondaryAttack = gpGlobals->curtime + fireRate;
		SendWeaponAnim( ACT_VM_PRIMARYATTACK_R );
		WeaponSound( WPN_DOUBLE );
	}

	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	Vector	vForward, vRight, vUp;

	pOwner->EyeVectors( &vForward, &vRight, &vUp );

	Vector	muzzlePoint = pOwner->Weapon_ShootPosition() + vForward * 12.0f + vRight * 6.0f + vUp * -3.0f;

	QAngle vecAngles;
	VectorAngles( vForward, vecAngles );
	m_hMissile = CMissile::Create( muzzlePoint, vecAngles, GetOwner()->edict() );

	//	m_hMissile->m_hOwner = this;

	// If the shot is clear to the player, give the missile a grace period
	trace_t	tr;
	Vector vecEye = pOwner->EyePosition();
	UTIL_TraceLine( vecEye, vecEye + vForward * 128, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );
	if ( tr.fraction == 1.0 )
		m_hMissile->SetGracePeriod( 0.3 );

	// Register a muzzleflash for the AI
	pOwner->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	pOwner->RumbleEffect( RUMBLE_SHOTGUN_SINGLE, 0, RUMBLE_FLAG_RESTART );

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired( pOwner, true, GetClassname() );

	m_nShotsFired++;

	const CustomWeaponInfo::WepDef& wpn = isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary;
	if ( wpn.hasRecoilRPGMissle )
		AddViewKick( wpn.recoilEasyDampen, wpn.recoilDegrees, wpn.recoilSeconds );

	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 1000, 0.2, GetOwner(), SOUNDENT_CHANNEL_WEAPON );

	if ( !wpn.infiniteAmmoEnabled )
	{
		if ( isPrimary )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else if ( usePrimaryAmmo )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else
		{
			pOwner->RemoveAmmo( 1, m_iSecondaryAmmoType );
		}
	}

	// Check to see if we should trigger any RPG firing triggers
	//Yes, these now work.
	int iCount = g_hWeaponFireTriggers.Count();
	for ( int i = 0; i < iCount; i++ )
	{
		if ( g_hWeaponFireTriggers[i]->IsTouching( pOwner ) )
		{
			if ( FClassnameIs( g_hWeaponFireTriggers[i], "trigger_rpgfire" ) )
			{
				g_hWeaponFireTriggers[i]->ActivateMultiTrigger( pOwner );
			}
		}
	}

	if ( hl2_episodic.GetBool() )
	{
		CAI_BaseNPC **ppAIs = g_AI_Manager.AccessAIs();
		int nAIs = g_AI_Manager.NumAIs();

		string_t iszStriderClassname = AllocPooledString( "npc_strider" );

		for ( int i = 0; i < nAIs; i++ )
		{
			if ( ppAIs[i]->m_iClassname == iszStriderClassname )
			{
				ppAIs[i]->DispatchInteraction( g_interactionPlayerLaunchedRPG, NULL, m_hMissile );
			}
		}
	}
}

void CWeaponCustom::ShootProjectileLeft( bool isPrimary, bool usePrimaryAmmo )
{
	// Can't have an active missile out
	if ( m_hMissile != NULL )
		return;

	if ( m_hMissile2 != NULL )
		return;

	// Can't be reloading
	if ( GetActivity() == ACT_VM_RELOAD )
		return;

	float fireRate = isPrimary ? GetPrimaryFireRate() : GetSecondaryFireRate();

	if ( isPrimary )
	{
		m_flNextPrimaryAttack = gpGlobals->curtime + fireRate;
		SendWeaponAnim( ACT_VM_PRIMARYATTACK_L );
		WeaponSound( SINGLE );
	}
	else
	{
		m_flNextSecondaryAttack = gpGlobals->curtime + fireRate;
		SendWeaponAnim( ACT_VM_PRIMARYATTACK_L );
		WeaponSound( WPN_DOUBLE );
	}

	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	Vector	vForward, vRight, vUp;

	pOwner->EyeVectors( &vForward, &vRight, &vUp );

	Vector	muzzlePoint = pOwner->Weapon_ShootPosition() + vForward * 12.0f + vRight * -6.0f + vUp * 3.0f;

	QAngle vecAngles;
	VectorAngles( vForward, vecAngles );
	m_hMissile2 = CMissile::Create( muzzlePoint, vecAngles, GetOwner()->edict() );

	//	m_hMissile2->m_hOwner = this;

	// If the shot is clear to the player, give the missile a grace period
	trace_t	tr;
	Vector vecEye = pOwner->EyePosition();
	UTIL_TraceLine( vecEye, vecEye + vForward * 128, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );
	if ( tr.fraction == 1.0 )
		m_hMissile2->SetGracePeriod( 0.3 );
	
	// Register a muzzleflash for the AI
	pOwner->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	pOwner->RumbleEffect( RUMBLE_SHOTGUN_SINGLE, 0, RUMBLE_FLAG_RESTART );

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired( pOwner, true, GetClassname() );

	m_nShotsFired++;

	const CustomWeaponInfo::WepDef& wpn = isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary;
	if ( wpn.hasRecoilRPGMissle )
		AddViewKick( wpn.recoilEasyDampen, wpn.recoilDegrees, wpn.recoilSeconds );

	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 1000, 0.2, GetOwner(), SOUNDENT_CHANNEL_WEAPON );

	if ( !wpn.infiniteAmmoEnabled )
	{
		if ( isPrimary )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else if ( usePrimaryAmmo )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else
		{
			pOwner->RemoveAmmo( 1, m_iSecondaryAmmoType );
		}
	}

	// Check to see if we should trigger any RPG firing triggers
	//Yes, these now work.
	int iCount = g_hWeaponFireTriggers.Count();
	for ( int i = 0; i < iCount; i++ )
	{
		if ( g_hWeaponFireTriggers[i]->IsTouching( pOwner ) )
		{
			if ( FClassnameIs( g_hWeaponFireTriggers[i], "trigger_rpgfire" ) )
			{
				g_hWeaponFireTriggers[i]->ActivateMultiTrigger( pOwner );
			}
		}
	}

	if ( hl2_episodic.GetBool() )
	{
		CAI_BaseNPC **ppAIs = g_AI_Manager.AccessAIs();
		int nAIs = g_AI_Manager.NumAIs();

		string_t iszStriderClassname = AllocPooledString( "npc_strider" );

		for ( int i = 0; i < nAIs; i++ )
		{
			if ( ppAIs[i]->m_iClassname == iszStriderClassname )
			{
				ppAIs[i]->DispatchInteraction( g_interactionPlayerLaunchedRPG, NULL, m_hMissile2 );
			}
		}
	}
}

void CWeaponCustom::ShootSMGGrenade( bool isPrimary, bool usePrimaryAmmo )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	// Deplete the clip completely
	if ( isPrimary )
	{
		SendWeaponAnim( ACT_VM_PRIMARYATTACK );
		m_flNextSecondaryAttack = pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	}
	else
	{
		SendWeaponAnim( ACT_VM_SECONDARYATTACK );
		m_flNextPrimaryAttack = pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	}

	// Register a muzzleflash for the AI
	pOwner->DoMuzzleFlash();
	pOwner->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	WeaponSound( isPrimary ? SINGLE : WPN_DOUBLE );

	pOwner->RumbleEffect( RUMBLE_357, 0, RUMBLE_FLAG_RESTART );

	Vector vecSrc = pOwner->Weapon_ShootPosition();
	Vector	vecThrow;
	// Don't autoaim on grenade tosses
	AngleVectors( pOwner->EyeAngles() + pOwner->GetPunchAngle(), &vecThrow );
	VectorScale( vecThrow, 1000.0f, vecThrow );

	//Create the grenade
	QAngle angles;
	VectorAngles( vecThrow, angles );
	CGrenadeAR2 *pGrenade = ( CGrenadeAR2* )Create( "grenade_ar2", vecSrc, angles, pOwner );
	pGrenade->SetAbsVelocity( vecThrow );

	pGrenade->SetLocalAngularVelocity( RandomAngle( -400, 400 ) );
	pGrenade->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );
	pGrenade->SetThrower( GetOwner() );
	pGrenade->SetDamage( (isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary).SMGGrenadeDamage );

	// player "shoot" animation
	pOwner->SetAnimation( PLAYER_ATTACK1 );
	m_nShotsFired++;

	const CustomWeaponInfo::WepDef& wpn = isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary;
	if ( wpn.hasRecoilSMGGrenade )
		AddViewKick( wpn.recoilEasyDampen, wpn.recoilDegrees, wpn.recoilSeconds );
	
	// Decrease ammo
	if ( !wpn.infiniteAmmoEnabled )
	{
		if ( isPrimary )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else if ( usePrimaryAmmo )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else
		{
			pOwner->RemoveAmmo( 1, m_iSecondaryAmmoType );
		}
	}

	if ( isPrimary )
		m_flNextPrimaryAttack = gpGlobals->curtime + GetPrimaryFireRate();
	else
		m_flNextSecondaryAttack = gpGlobals->curtime + GetSecondaryFireRate();
}

void CWeaponCustom::ShootSMGGrenadeRight( bool isPrimary, bool usePrimaryAmmo )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	// Deplete the clip completely
	if ( isPrimary )
	{
		SendWeaponAnim( ACT_VM_PRIMARYATTACK_R );
		m_flNextSecondaryAttack = pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	}
	else
	{
		SendWeaponAnim( ACT_VM_PRIMARYATTACK_R );
		m_flNextPrimaryAttack = pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	}

	// Register a muzzleflash for the AI
	pOwner->DoMuzzleFlash();
	pOwner->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	WeaponSound( isPrimary ? SINGLE : WPN_DOUBLE );

	pOwner->RumbleEffect( RUMBLE_357, 0, RUMBLE_FLAG_RESTART );

	Vector vecSrc = pOwner->Weapon_ShootPosition();
	Vector	vecThrow;
	// Don't autoaim on grenade tosses
	AngleVectors( pOwner->EyeAngles() + pOwner->GetPunchAngle(), &vecThrow );
	VectorScale( vecThrow, 1000.0f, vecThrow );

	//Create the grenade
	QAngle angles;
	VectorAngles( vecThrow, angles );
	CGrenadeAR2 *pGrenade = ( CGrenadeAR2* )Create( "grenade_ar2", vecSrc, angles, pOwner );
	pGrenade->SetAbsVelocity( vecThrow );

	pGrenade->SetLocalAngularVelocity( RandomAngle( -400, 400 ) );
	pGrenade->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );
	pGrenade->SetThrower( GetOwner() );
	pGrenade->SetDamage( (isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary).SMGGrenadeDamage );

	// player "shoot" animation
	pOwner->SetAnimation( PLAYER_ATTACK1 );

	m_nShotsFired++;

	const CustomWeaponInfo::WepDef& wpn = isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary;
	if ( wpn.hasRecoilSMGGrenade )
		AddViewKick( wpn.recoilEasyDampen, wpn.recoilDegrees, wpn.recoilSeconds );
	
	// Decrease ammo
	if ( !wpn.infiniteAmmoEnabled )
	{
		if ( isPrimary )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else if ( usePrimaryAmmo )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else
		{
			pOwner->RemoveAmmo( 1, m_iSecondaryAmmoType );
		}
	}
	
	if ( isPrimary )
		m_flNextPrimaryAttack = gpGlobals->curtime + GetPrimaryFireRate();
	else
		m_flNextSecondaryAttack = gpGlobals->curtime + GetSecondaryFireRate();
}

void CWeaponCustom::ShootSMGGrenadeLeft( bool isPrimary, bool usePrimaryAmmo )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	// Deplete the clip completely
	if ( isPrimary )
	{
		SendWeaponAnim( ACT_VM_PRIMARYATTACK_L );
		m_flNextSecondaryAttack = pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	}
	else
	{
		SendWeaponAnim( ACT_VM_PRIMARYATTACK_L );
		m_flNextPrimaryAttack = pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	}

	// Register a muzzleflash for the AI
	pOwner->DoMuzzleFlash();
	pOwner->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	WeaponSound( isPrimary ? SINGLE : WPN_DOUBLE );

	pOwner->RumbleEffect( RUMBLE_357, 0, RUMBLE_FLAG_RESTART );

	Vector vecSrc = pOwner->Weapon_ShootPosition();
	Vector	vecThrow;
	// Don't autoaim on grenade tosses
	AngleVectors( pOwner->EyeAngles() + pOwner->GetPunchAngle(), &vecThrow );
	VectorScale( vecThrow, 1000.0f, vecThrow );

	//Create the grenade
	QAngle angles;
	VectorAngles( vecThrow, angles );
	CGrenadeAR2 *pGrenade = ( CGrenadeAR2* )Create( "grenade_ar2", vecSrc, angles, pOwner );
	pGrenade->SetAbsVelocity( vecThrow );

	pGrenade->SetLocalAngularVelocity( RandomAngle( -400, 400 ) );
	pGrenade->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );
	pGrenade->SetThrower( GetOwner() );
	pGrenade->SetDamage( (isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary).SMGGrenadeDamage );

	// player "shoot" animation
	pOwner->SetAnimation( PLAYER_ATTACK1 );
	m_nShotsFired++;


	const CustomWeaponInfo::WepDef& wpn = isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary;
	if ( wpn.hasRecoilSMGGrenade )
		AddViewKick( wpn.recoilEasyDampen, wpn.recoilDegrees, wpn.recoilSeconds );

	// Decrease ammo
	if ( !wpn.infiniteAmmoEnabled )
	{
		if ( isPrimary )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else if ( usePrimaryAmmo )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else
		{
			pOwner->RemoveAmmo( 1, m_iSecondaryAmmoType );
		}
	}

	if ( isPrimary )
		m_flNextPrimaryAttack = gpGlobals->curtime + GetPrimaryFireRate();
	else
		m_flNextSecondaryAttack = gpGlobals->curtime + GetSecondaryFireRate();
}

void CWeaponCustom::ShootAR2EnergyBall( bool isPrimary, bool usePrimaryAmmo )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	// Deplete the clip completely
	if ( isPrimary )
	{
		SendWeaponAnim( ACT_VM_PRIMARYATTACK );
		m_flNextSecondaryAttack = pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	}
	else
	{
		SendWeaponAnim( ACT_VM_SECONDARYATTACK );
		m_flNextPrimaryAttack = pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	}

	// Register a muzzleflash for the AI
	pOwner->DoMuzzleFlash();
	pOwner->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	WeaponSound( isPrimary ? SINGLE : WPN_DOUBLE );

	pOwner->RumbleEffect( RUMBLE_SHOTGUN_DOUBLE, 0, RUMBLE_FLAG_RESTART );

	// Fire the bullets
	Vector vecSrc = pOwner->Weapon_ShootPosition();
	Vector vecAiming = pOwner->GetAutoaimVector( AUTOAIM_SCALE_DEFAULT );

	// Fire the bullets
	Vector vecVelocity = vecAiming * 1000.0f;

	// Fire the combine ball
	const CustomWeaponInfo::WepDef& wpn = isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary;
	CreateCombineBall( vecSrc,
					   vecVelocity,
					   wpn.combineBallRadius,
					   wpn.combineBallMass,
					   wpn.combineBallDuration,
					   pOwner );
	
	// View effects
	color32 white = { 255, 255, 255, 64 };
	UTIL_ScreenFade( pOwner, white, 0.1, 0, FFADE_IN );

	//Disorient the player
	QAngle angles = pOwner->GetLocalAngles();

	angles.x += random->RandomInt( -4, 4 );
	angles.y += random->RandomInt( -4, 4 );
	angles.z = 0;

	pOwner->SnapEyeAngles( angles );

	pOwner->ViewPunch( QAngle( random->RandomInt( -8, -12 ), random->RandomInt( 1, 2 ), 0 ) );

	pOwner->SetAnimation( PLAYER_ATTACK1 );

	m_nShotsFired++;

	// Decrease ammo
	if ( !wpn.infiniteAmmoEnabled )
	{
		if ( isPrimary )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else if ( usePrimaryAmmo )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else
		{
			pOwner->RemoveAmmo( 1, m_iSecondaryAmmoType );
		}
	}
	
	if ( isPrimary )
		m_flNextPrimaryAttack = gpGlobals->curtime + GetPrimaryFireRate();
	else
		m_flNextSecondaryAttack = gpGlobals->curtime + GetSecondaryFireRate();
}

void CWeaponCustom::ShootAR2EnergyBallRight( bool isPrimary, bool usePrimaryAmmo )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	// Deplete the clip completely
	if ( isPrimary )
	{
		SendWeaponAnim( ACT_VM_PRIMARYATTACK );
		m_flNextSecondaryAttack = pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	}
	else
	{
		SendWeaponAnim( ACT_VM_SECONDARYATTACK );
		m_flNextPrimaryAttack = pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	}

	// Register a muzzleflash for the AI
	pOwner->DoMuzzleFlash();
	pOwner->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	WeaponSound( isPrimary ? SINGLE : WPN_DOUBLE );

	pOwner->RumbleEffect( RUMBLE_SHOTGUN_DOUBLE, 0, RUMBLE_FLAG_RESTART );

	// Fire the bullets
	Vector vecSrc = pOwner->Weapon_ShootPosition();
	Vector vecAiming = pOwner->GetAutoaimVector( AUTOAIM_SCALE_DEFAULT );

	// Fire the bullets
	Vector vecVelocity = vecAiming * 1000.0f;

	// Fire the combine ball
	const CustomWeaponInfo::WepDef& wpn = isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary;
	CreateCombineBall( vecSrc,
					   vecVelocity,
					   wpn.combineBallRadius,
					   wpn.combineBallMass,
					   wpn.combineBallDuration,
					   pOwner );

	// View effects
	color32 white = { 255, 255, 255, 64 };
	UTIL_ScreenFade( pOwner, white, 0.1, 0, FFADE_IN );

	//Disorient the player
	QAngle angles = pOwner->GetLocalAngles();

	angles.x += random->RandomInt( -4, 4 );
	angles.y += random->RandomInt( -4, 4 );
	angles.z = 0;

	pOwner->SnapEyeAngles( angles );

	pOwner->ViewPunch( QAngle( random->RandomInt( -8, -12 ), random->RandomInt( 1, 2 ), 0 ) );

	pOwner->SetAnimation( PLAYER_ATTACK1 );

	m_nShotsFired++;

	// Decrease ammo
	if ( !wpn.infiniteAmmoEnabled )
	{
		if ( isPrimary )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else if ( usePrimaryAmmo )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else
		{
			pOwner->RemoveAmmo( 1, m_iSecondaryAmmoType );
		}
	}
	
	if ( isPrimary )
		m_flNextPrimaryAttack = gpGlobals->curtime + GetPrimaryFireRate();
	else
		m_flNextSecondaryAttack = gpGlobals->curtime + GetSecondaryFireRate();
}

void CWeaponCustom::ShootAR2EnergyBallLeft( bool isPrimary, bool usePrimaryAmmo )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner == NULL )
		return;

	// Deplete the clip completely
	if ( isPrimary )
	{
		SendWeaponAnim( ACT_VM_PRIMARYATTACK );
		m_flNextSecondaryAttack = pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	}
	else
	{
		SendWeaponAnim( ACT_VM_SECONDARYATTACK );
		m_flNextPrimaryAttack = pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	}

	// Register a muzzleflash for the AI
	pOwner->DoMuzzleFlash();
	pOwner->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	WeaponSound( isPrimary ? SINGLE : WPN_DOUBLE );

	pOwner->RumbleEffect( RUMBLE_SHOTGUN_DOUBLE, 0, RUMBLE_FLAG_RESTART );

	// Fire the bullets
	Vector vecSrc = pOwner->Weapon_ShootPosition();
	Vector vecAiming = pOwner->GetAutoaimVector( AUTOAIM_SCALE_DEFAULT );
	Vector impactPoint = vecSrc + vecAiming * MAX_TRACE_LENGTH;

	// Fire the bullets
	Vector vecVelocity = vecAiming * 1000.0f;

	// Fire the combine ball
	const CustomWeaponInfo::WepDef& wpn = isPrimary ? GetWpnDataCustom().primary : GetWpnDataCustom().secondary;
	CreateCombineBall( vecSrc,
					   vecVelocity,
					   wpn.combineBallRadius,
					   wpn.combineBallMass,
					   wpn.combineBallDuration,
					   pOwner );

	// View effects
	color32 white = { 255, 255, 255, 64 };
	UTIL_ScreenFade( pOwner, white, 0.1, 0, FFADE_IN );

	//Disorient the player
	QAngle angles = pOwner->GetLocalAngles();

	angles.x += random->RandomInt( -4, 4 );
	angles.y += random->RandomInt( -4, 4 );
	angles.z = 0;

	pOwner->SnapEyeAngles( angles );

	pOwner->ViewPunch( QAngle( random->RandomInt( -8, -12 ), random->RandomInt( 1, 2 ), 0 ) );

	pOwner->SetAnimation( PLAYER_ATTACK1 );

	m_nShotsFired++;

	// Decrease ammo
	if ( !wpn.infiniteAmmoEnabled )
	{
		if ( isPrimary )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else if ( usePrimaryAmmo )
		{
			pOwner->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}
		else
		{
			pOwner->RemoveAmmo( 1, m_iSecondaryAmmoType );
		}
	}

	if ( isPrimary )
		m_flNextPrimaryAttack = gpGlobals->curtime + GetPrimaryFireRate();
	else
		m_flNextSecondaryAttack = gpGlobals->curtime + GetSecondaryFireRate();
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponCustom::GetPrimaryAttackActivity( void )
{
	if ( m_nShotsFired < 2 )
		return ACT_VM_PRIMARYATTACK;

	if ( m_nShotsFired < 3 )
		return ACT_VM_RECOIL1;

	if ( m_nShotsFired < 4 )
		return ACT_VM_RECOIL2;

	return ACT_VM_RECOIL3;
}

Activity CWeaponCustom::GetRightGunActivity( void )
{
	if ( m_nShotsFired < 1 )
		return ACT_VM_PRIMARYATTACK_R;

	if ( m_nShotsFired < 2 )
		return ACT_VM_PRIMARYATTACK_R_RECOIL1;

	if ( m_nShotsFired < 3 )
		return ACT_VM_PRIMARYATTACK_R_RECOIL2;

	return ACT_VM_PRIMARYATTACK_R_RECOIL3;
}

Activity CWeaponCustom::GetLeftGunActivity( void )
{
	if ( m_nShotsFired < 1 )
		return ACT_VM_PRIMARYATTACK_L;

	if ( m_nShotsFired < 2 )
		return ACT_VM_PRIMARYATTACK_L_RECOIL1;

	if ( m_nShotsFired < 3 )
		return ACT_VM_PRIMARYATTACK_L_RECOIL2;

	return ACT_VM_PRIMARYATTACK_L_RECOIL3;
}

void CWeaponCustom::PrimaryAttack( void )
{
	if ( GetWpnDataCustom().primary.hasFire )
	{
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
#ifdef HL2_DLL
		if ( GetWpnDataCustom().primary.missleEnabled )
		{
			if ( GetWpnDataCustom().m_sDualWeapons )
			{
				if ( GetWpnDataCustom().primary.fireBoth )
				{
					ShootProjectileRight( true, true );
					ShootProjectileLeft( true, true );
				}
				else
				{
					if ( !bFlip )
					{
						ShootProjectileRight( true, true );
						bFlip = true;
					}
					else
					{
						ShootProjectileLeft( true, true );
						bFlip = false;
					}
				}
			}
			else
			{
				ShootProjectile( true, true );
			}
		}
		if ( GetWpnDataCustom().primary.SMGGrenadeEnabled )
		{
			if ( GetWpnDataCustom().m_sDualWeapons )
			{
				if ( GetWpnDataCustom().primary.fireBoth )
				{
					ShootSMGGrenadeRight( true, true );
					ShootSMGGrenadeLeft( true, true );
				}
				else
				{
					if ( !bFlip )
					{
						ShootSMGGrenadeRight( true, true );
						bFlip = true;
					}
					else
					{
						ShootSMGGrenadeLeft( true, true );
						bFlip = false;
					}
				}
			}
			else
			{
				ShootSMGGrenade( true, true );
			}
		}
		if ( GetWpnDataCustom().primary.AR2EnergyBallEnabled )
		{
			if ( GetWpnDataCustom().m_sDualWeapons )
			{
				if ( GetWpnDataCustom().primary.fireBoth )
				{
					ShootAR2EnergyBallRight( true, true );
					ShootAR2EnergyBallLeft( true, true );
				}
				else
				{
					if ( !bFlip )
					{
						ShootAR2EnergyBallRight( true, true );
						bFlip = true;
					}
					else
					{
						ShootAR2EnergyBallLeft( true, true );
						bFlip = false;
					}
				}
			}
			else
			{
				ShootAR2EnergyBall( true, true );
			}
		}
#endif
		if ( IsPrimaryBullet() )
		{
			if ( GetWpnDataCustom().m_sDualWeapons )
			{
				if ( GetWpnDataCustom().primary.fireBoth )
				{
					ShootBulletsRight( true, true );
					ShootBulletsLeft( true, true );
					if ( GetWpnDataCustom().m_bUseMuzzleSmoke )
					{
						DispatchParticleEffect( "weapon_muzzle_smoke", PATTACH_POINT_FOLLOW, pPlayer->GetViewModel(), "muzzle", true );
						DispatchParticleEffect( "weapon_muzzle_smoke", PATTACH_POINT_FOLLOW, pPlayer->GetViewModel(), "muzzle2", true );
					}
				}
				else
				{
					if ( !bFlip )
					{
						ShootBulletsRight( true, true );
						if ( GetWpnDataCustom().m_bUseMuzzleSmoke )
						{
							DispatchParticleEffect( "weapon_muzzle_smoke", PATTACH_POINT_FOLLOW, pPlayer->GetViewModel(), "muzzle", true );
						}
						bFlip = true;
					}
					else
					{
						ShootBulletsLeft( true, true );
						if ( GetWpnDataCustom().m_bUseMuzzleSmoke )
						{
							DispatchParticleEffect( "weapon_muzzle_smoke", PATTACH_POINT_FOLLOW, pPlayer->GetViewModel(), "muzzle2", true );
						}
						bFlip = false;
					}
				}
			}
			else
			{
				ShootBullets( true, true );

				if ( GetWpnDataCustom().m_bUseMuzzleSmoke )
				{
					DispatchParticleEffect( "weapon_muzzle_smoke", PATTACH_POINT_FOLLOW, pPlayer->GetViewModel(), "muzzle", true );
				}
			}
		}

		//else if (IsMeleeWeapon())
		//{
			//Do something related to melee weapons.
		//}
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CWeaponCustom::Reload( void )
{
	if ( GetWpnDataCustom().m_sUsesZoom )
	{
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

		color32 zoomColor = { GetWpnDataCustom().m_sZoomColorRed, GetWpnDataCustom().m_sZoomColorGreen, GetWpnDataCustom().m_sZoomColorBlue, GetWpnDataCustom().m_sZoomColorAlpha };

		if ( m_bInZoom )
		{
			if ( pPlayer->SetFOV( this, 0, 0.2f ) )
			{
				if ( GetWpnDataCustom().m_sUsesZoomSound )
				{
					WeaponSound( SPECIAL1 );
				}
				m_bInZoom = false;
				if ( GetWpnDataCustom().m_sUsesZoomColor )
				{
					UTIL_ScreenFade( pPlayer, zoomColor, 0.2f, 0, FFADE_IN | FFADE_PURGE );
				}
			}
		}
	}

	float fCacheTime = m_flNextSecondaryAttack;

	bool fRet = DefaultReload( GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD );
	if ( fRet )
	{
		// Undo whatever the reload process has done to our secondary
		// attack timer. We allow you to interrupt reloading to fire
		// a grenade.
		m_flNextSecondaryAttack = GetOwner()->m_flNextAttack = fCacheTime;

		WeaponSound( RELOAD );
	}

	return fRet;
}

bool CWeaponCustom::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	if ( GetWpnDataCustom().m_sUsesZoom )
	{
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

		color32 zoomColor = { GetWpnDataCustom().m_sZoomColorRed, GetWpnDataCustom().m_sZoomColorGreen, GetWpnDataCustom().m_sZoomColorBlue, GetWpnDataCustom().m_sZoomColorAlpha };

		if ( m_bInZoom )
		{
			if ( pPlayer->SetFOV( this, 0, 0.2f ) )
			{
				if ( GetWpnDataCustom().m_sUsesZoomSound )
				{
					WeaponSound( SPECIAL1 );
				}
				m_bInZoom = false;
				if ( GetWpnDataCustom().m_sUsesZoomColor )
				{
					UTIL_ScreenFade( pPlayer, zoomColor, 0.2f, 0, FFADE_IN | FFADE_PURGE );
				}
			}
		}
	}

	return BaseClass::Holster( pSwitchingTo );
}

void CWeaponCustom::Drop( const Vector &vecVelocity )
{
	if ( GetWpnDataCustom().m_sUsesZoom )
	{
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

		color32 zoomColor = { GetWpnDataCustom().m_sZoomColorRed, GetWpnDataCustom().m_sZoomColorGreen, GetWpnDataCustom().m_sZoomColorBlue, GetWpnDataCustom().m_sZoomColorAlpha };

		if ( m_bInZoom )
		{
			if ( pPlayer->SetFOV( this, 0, 0.2f ) )
			{
				if ( GetWpnDataCustom().m_sUsesZoomSound )
				{
					WeaponSound( SPECIAL1 );
				}
				m_bInZoom = false;
				if ( GetWpnDataCustom().m_sUsesZoomColor )
				{
					UTIL_ScreenFade( pPlayer, zoomColor, 0.2f, 0, FFADE_IN | FFADE_PURGE );
				}
			}
		}
	}

	BaseClass::Drop( vecVelocity );
}

void CWeaponCustom::ToggleZoom( void )
{
	if ( GetWpnDataCustom().m_sUsesZoom )
	{
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

		if ( pPlayer == NULL )
			return;

		color32 zoomColor = { GetWpnDataCustom().m_sZoomColorRed, GetWpnDataCustom().m_sZoomColorGreen, GetWpnDataCustom().m_sZoomColorBlue, GetWpnDataCustom().m_sZoomColorAlpha };

		if ( m_bInZoom )
		{
			if ( pPlayer->SetFOV( this, 0, 0.2f ) )
			{
				if ( GetWpnDataCustom().m_sUsesZoomSound )
				{
					WeaponSound( SPECIAL1 );
				}
				m_bInZoom = false;
				if ( GetWpnDataCustom().m_sUsesZoomColor )
				{
					UTIL_ScreenFade( pPlayer, zoomColor, 0.2f, 0, FFADE_IN | FFADE_PURGE );
				}
			}
		}
		else
		{
			if ( pPlayer->SetFOV( this, 20, 0.05f ) )
			{
				if ( GetWpnDataCustom().m_sUsesZoomSound )
				{
					WeaponSound( SPECIAL1 );
				}
				m_bInZoom = true;
				if ( GetWpnDataCustom().m_sUsesZoomColor )
				{
					UTIL_ScreenFade( pPlayer, zoomColor, 0.2f, 0, FFADE_OUT | FFADE_PURGE | FFADE_STAYOUT );
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCustom::AddViewKick( float easyDampen, float degrees, float seconds )
{
	//#define	EASY_DAMPEN			0.5f
	//#define	MAX_VERTICAL_KICK	1.0f	//Degrees
	//#define	SLIDE_LIMIT			2.0f	//Seconds

	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( pPlayer == NULL )
		return;

	DoMachineGunKick( pPlayer, easyDampen, degrees, m_fFireDuration, seconds );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCustom::SecondaryAttack( void )
{
	if ( GetWpnDataCustom().secondary.hasFire )
	{
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

#ifdef HL2_DLL
		if ( GetWpnDataCustom().secondary.missleEnabled )
		{
			if ( GetWpnDataCustom().m_sDualWeapons )
			{
				if ( GetWpnDataCustom().secondary.fireBoth )
				{
					ShootProjectileRight( false, GetWpnDataCustom().m_sUsePrimaryAmmo );
					ShootProjectileLeft( false, GetWpnDataCustom().m_sUsePrimaryAmmo );
				}
			}
			else
			{
				ShootProjectile( false, GetWpnDataCustom().m_sUsePrimaryAmmo );
			}
		}
		if ( GetWpnDataCustom().secondary.SMGGrenadeEnabled )
		{
			if ( GetWpnDataCustom().m_sDualWeapons )
			{
				if ( GetWpnDataCustom().secondary.fireBoth )
				{
					ShootSMGGrenadeRight( false, GetWpnDataCustom().m_sUsePrimaryAmmo );
					ShootSMGGrenadeLeft( false, GetWpnDataCustom().m_sUsePrimaryAmmo );
				}
			}
			else
			{
				ShootSMGGrenade( false, GetWpnDataCustom().m_sUsePrimaryAmmo );
			}
		}
		if ( GetWpnDataCustom().secondary.AR2EnergyBallEnabled )
		{
			if ( GetWpnDataCustom().m_sDualWeapons )
			{
				if ( GetWpnDataCustom().secondary.fireBoth )
				{
					ShootAR2EnergyBallRight( false, GetWpnDataCustom().m_sUsePrimaryAmmo );
					ShootAR2EnergyBallLeft( false, GetWpnDataCustom().m_sUsePrimaryAmmo );
				}
			}
			else
			{
				ShootAR2EnergyBall( false, GetWpnDataCustom().m_sUsePrimaryAmmo );
			}
		}
#endif
		if ( IsSecondaryBullet() )
		{
			if ( GetWpnDataCustom().m_sDualWeapons )
			{
				if ( GetWpnDataCustom().secondary.fireBoth )
				{
					ShootBulletsRight( false, GetWpnDataCustom().m_sUsePrimaryAmmo );
					ShootBulletsLeft( false, GetWpnDataCustom().m_sUsePrimaryAmmo );
					if ( GetWpnDataCustom().m_bUseMuzzleSmoke )
					{
						DispatchParticleEffect( "weapon_muzzle_smoke", PATTACH_POINT_FOLLOW, pPlayer->GetViewModel(), "muzzle", true );
						DispatchParticleEffect( "weapon_muzzle_smoke", PATTACH_POINT_FOLLOW, pPlayer->GetViewModel(), "muzzle2", true );
					}
				}
			}
			else
			{
				ShootBullets( false, GetWpnDataCustom().m_sUsePrimaryAmmo );

				if ( GetWpnDataCustom().m_bUseMuzzleSmoke )
				{
					DispatchParticleEffect( "weapon_muzzle_smoke", PATTACH_POINT_FOLLOW, pPlayer->GetViewModel(), "muzzle", true );
				}
			}
		}
		//else if (IsMeleeWeapon())
		//{
			//Do Nothing.
		//}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCustom::CheckZoomToggle( void )
{
	if ( GetWpnDataCustom().m_sUsesZoom )
	{
		CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

		//if ( !IsIronsighted() )
		{
			if ( GetWpnDataCustom().m_sUseZoomOnPrimaryFire )
			{
				if ( pPlayer->m_afButtonPressed & IN_ATTACK )
				{
					ToggleZoom();
				}
			}
			else
			{
				if ( pPlayer->m_afButtonPressed & IN_ATTACK2 )
				{
					ToggleZoom();
				}
			}
		}
	}
}

#define	COMBINE_MIN_GRENADE_CLEAR_DIST 256

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : flDot - 
//			flDist - 
// Output : int
//-----------------------------------------------------------------------------
int CWeaponCustom::WeaponRangeAttack2Condition( float flDot, float flDist )
{
	return COND_NONE;
}

//-----------------------------------------------------------------------------
const WeaponProficiencyInfo_t *CWeaponCustom::GetProficiencyValues()
{
	static WeaponProficiencyInfo_t proficiencyTable[] =
	{
		{ 7.0,		0.75	},
		{ 5.00,		0.75	},
		{ 10.0 / 3.0, 0.75	},
		{ 5.0 / 3.0,	0.75	},
		{ 1.00,		1.0		},
	};

	COMPILE_TIME_ASSERT( ARRAYSIZE( proficiencyTable ) == WEAPON_PROFICIENCY_PERFECT + 1 );

	return proficiencyTable;
}
