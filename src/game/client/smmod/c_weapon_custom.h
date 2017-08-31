#ifndef	C_WEAPONCUSTOM_H
#define	C_WEAPONCUSTOM_H
#ifdef _WIN32
#pragma once
#endif

#ifdef HL2_CLIENT_DLL
#include "hl2/c_basehlcombatweapon.h"
#define CUSTOM_WEAPON_BASE C_HLSelectFireMachineGun
#else
#include "template/weapons/c_basesdkcombatweapon.h"
#define CUSTOM_WEAPON_BASE C_SDKSelectFireMachineGun
#endif

class C_WeaponCustom : public CUSTOM_WEAPON_BASE
{
	DECLARE_CLASS( C_WeaponCustom, CUSTOM_WEAPON_BASE );
public:
	DECLARE_PREDICTABLE();
	DECLARE_CLIENTCLASS();
	C_WeaponCustom() {}
	C_WeaponCustom( const char* className );

	char const				*GetClassname( void ) { return m_szClassName; }

private:
	C_WeaponCustom( const C_WeaponCustom & );
	int scopeNum;
	char m_szClassName[128];
};				

#endif	//WEAPONCUSTOM_H