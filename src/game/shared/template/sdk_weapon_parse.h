//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef SDK_WEAPON_PARSE_H
#define SDK_WEAPON_PARSE_H
#ifdef _WIN32
#pragma once
#endif

#include "networkvar.h"

#include "smmod/weapon_parse_custom_weapon.h"

//--------------------------------------------------------------------------------------------------------
class CSDKWeaponInfo : public CustomWeaponInfo
{
public:
	DECLARE_CLASS_GAMEROOT( CSDKWeaponInfo, CustomWeaponInfo );
	
	CSDKWeaponInfo();
	
	virtual void Parse( ::KeyValues *pKeyValuesData, const char *szWeaponName );


public:

	float		m_flPlayerDamage;
};


#endif // HL2MP_WEAPON_PARSE_H
