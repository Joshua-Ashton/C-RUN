//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include <KeyValues.h>
#include "sdk_weapon_parse.h"
#include "ammodef.h"

FileWeaponInfo_t* CreateWeaponInfo()
{
	return new CSDKWeaponInfo;
}

CSDKWeaponInfo::CSDKWeaponInfo()
{
	m_flPlayerDamage = 0;
}


void CSDKWeaponInfo::Parse( KeyValues *pKeyValuesData, const char *szWeaponName )
{
	BaseClass::Parse( pKeyValuesData, szWeaponName );

	m_flPlayerDamage = pKeyValuesData->GetInt( "damage", 0 );
}


