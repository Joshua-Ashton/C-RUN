#ifndef	C_WEAPONCUSTOMPARSE_H
#define	C_WEAPONCUSTOMPARSE_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_parse.h"

class CustomWeaponInfo : public FileWeaponInfo_t
{
public:
	CustomWeaponInfo();

	virtual void Parse( KeyValues *pKeyValuesData, const char *szWeaponName );


	bool	m_bIsCustom;

	bool	m_bUseMagStyleReloads;
	bool	m_bUseMuzzleSmoke;
	bool	m_sCanReloadSingly;
	bool	m_sDualWeapons;
	bool	m_sWeaponOptions;

	struct WepDef
	{
		bool	bulletEnabled;
		bool	missleEnabled;
		char	ammoType;
		float	damage;
		int		shotCount;
		float	fireRate;
		Vector	spread;
		bool	infiniteAmmoEnabled;
		bool	SMGGrenadeEnabled;
		float	SMGGrenadeDamage;
		bool	AR2EnergyBallEnabled;
		float	combineBallRadius;
		float	combineBallMass;
		float	combineBallDuration;
		float	recoilEasyDampen;
		float	recoilDegrees;
		float	recoilSeconds;
		Vector	ironsightSpread;
		Vector	zoomSpread;
		float	ironsightFireRate;
		float	zoomFireRate;
		bool	hasFire;
		bool	hasRecoilSMGGrenade;
		bool	hasRecoilRPGMissle;
		int		minRange;
		int		maxRange;
		bool	canFireUnderwater;
		bool	fireBoth;
	};

	WepDef primary;
	WepDef secondary;
	bool	 m_sUsePrimaryAmmo;

	bool	m_sUsesZoom;
	bool	m_sUsesZoomSound;
	bool	m_sUsesZoomColor;
	bool	m_sUseZoomOnPrimaryFire;
	int		m_sZoomColorRed;
	int		m_sZoomColorGreen;
	int		m_sZoomColorBlue;
	int		m_sZoomColorAlpha;

	bool	m_sUsesCustomization;
	bool	m_sUsesBodygroupSection1;
	bool	m_sUsesBodygroupSection2;
	bool	m_sUsesBodygroupSection3;
	bool	m_sUsesBodygroupSection4;
	bool	m_sUsesBodygroupSection5;
	bool	m_sUsesBodygroupSection6;
	int		m_sBodygroup1;
	int		m_sBodygroup2;
	int		m_sBodygroup3;
	int		m_sBodygroup4;
	int		m_sBodygroup5;
	int		m_sBodygroup6;
	int		m_sSubgroup1;
	int		m_sSubgroup2;
	int		m_sSubgroup3;
	int		m_sSubgroup4;
	int		m_sSubgroup5;
	int		m_sSubgroup6;
	int		m_sWeaponSkin;
};

#endif	//WEAPONCUSTOM_H