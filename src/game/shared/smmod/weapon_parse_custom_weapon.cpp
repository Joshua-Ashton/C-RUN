#include "cbase.h"
#include "weapon_parse_custom_weapon.h"

#include "tier0/memdbgon.h"

CustomWeaponInfo::CustomWeaponInfo()
{
	m_bIsCustom = false;
	m_bBuiltRightHanded = true;
	m_bUseMagStyleReloads = false;
	m_bUseMuzzleSmoke = false;
	m_sCanReloadSingly = false;
	m_sDualWeapons = false;
	m_sWeaponOptions = false;
	m_sUsesZoom = false;
	m_sUsesCustomization = false;
	V_memset( &primary, 0, sizeof( WepDef ) );
	V_memset( &secondary, 0, sizeof( WepDef ) );
}

void CustomWeaponInfo::Parse( KeyValues* pKeyValuesData, const char* szWeaponName )
{
	FileWeaponInfo_t::Parse( pKeyValuesData, szWeaponName );

	KeyValues *pWeaponSpec = pKeyValuesData->FindKey( "WeaponSpec" );
	if ( !pWeaponSpec )
		return;

	m_bIsCustom = true;
	m_bUseMagStyleReloads = pKeyValuesData->GetBool( "MagazineStyledReloads" );
	m_bUseMuzzleSmoke = pKeyValuesData->GetBool( "UseMuzzleSmoke" );

	if ( KeyValues *pWeaponOptions = pWeaponSpec->FindKey( "WeaponOptions" ) )
	{
		m_sWeaponOptions = true;
		m_sCanReloadSingly = pWeaponOptions->GetBool( "CanReloadSingly" );
		m_sDualWeapons = pWeaponOptions->GetBool( "DualWeapons" );
		primary.fireBoth = pWeaponOptions->GetBool( "FireBothGuns", true );
		secondary.fireBoth = pWeaponOptions->GetBool( "FireBothGuns" );
	}

	if ( KeyValues *pPrimaryFire = pWeaponSpec->FindKey( "PrimaryFire" ) )
	{
		primary.hasFire = true;
		primary.fireRate = pPrimaryFire->GetFloat( "FireRate", 1.0f );
		primary.ironsightFireRate = pPrimaryFire->GetFloat( "IronsightFireRate", primary.fireRate );
		primary.zoomFireRate = pPrimaryFire->GetFloat( "ZoomFireRate", primary.fireRate );
		primary.infiniteAmmoEnabled = pPrimaryFire->GetBool( "InfiniteAmmo" );
		primary.minRange = pPrimaryFire->GetInt( "MinRange" );
		primary.maxRange = pPrimaryFire->GetInt( "MaxRange" );
		primary.canFireUnderwater = pPrimaryFire->GetBool( "CanFireUnderwater", true );
		if ( KeyValues *pBullet1 = pPrimaryFire->FindKey( "Bullet" ) )
		{
			primary.bulletEnabled = true;
			primary.damage = pBullet1->GetFloat( "Damage" );
			primary.shotCount = pBullet1->GetInt( "ShotCount" );

			if ( KeyValues *pSpread = pBullet1->FindKey( "Spread" ) )
			{
				primary.spread.x = sin( pSpread->GetFloat( "x" ) / 2.0f );
				primary.spread.y = sin( pSpread->GetFloat( "y" ) / 2.0f );
				primary.spread.z = sin( pSpread->GetFloat( "z" ) / 2.0f );
			}

			if ( KeyValues *pIronsightSpread = pBullet1->FindKey( "IronsightSpread" ) )
			{
				primary.ironsightSpread.x = sin( pIronsightSpread->GetFloat( "x" ) / 2.0f );
				primary.ironsightSpread.y = sin( pIronsightSpread->GetFloat( "y" ) / 2.0f );
				primary.ironsightSpread.z = sin( pIronsightSpread->GetFloat( "z" ) / 2.0f );
			}
			else
			{
				primary.ironsightSpread.x = primary.spread.x;
				primary.ironsightSpread.y = primary.spread.y;
				primary.ironsightSpread.z = primary.spread.z;
			}

			if ( KeyValues *pZoomSpread = pBullet1->FindKey( "ZoomSpread" ) )
			{
				primary.zoomSpread.x = sin( pZoomSpread->GetFloat( "x" ) / 2.0f );
				primary.zoomSpread.y = sin( pZoomSpread->GetFloat( "y" ) / 2.0f );
				primary.zoomSpread.z = sin( pZoomSpread->GetFloat( "z" ) / 2.0f );
			}
			else
			{
				primary.zoomSpread.x = primary.spread.x;
				primary.zoomSpread.y = primary.spread.y;
				primary.zoomSpread.z = primary.spread.z;
			}
		}

		if ( KeyValues *pMissle = pPrimaryFire->FindKey( "Missle" ) )
		{
			primary.missleEnabled = true;
			primary.hasRecoilRPGMissle = pMissle->GetBool( "UseRecoil", true );
		}

		if ( KeyValues *pSMGGrenade = pPrimaryFire->FindKey( "SMGGrenade" ) )
		{
			primary.SMGGrenadeEnabled = true;
			primary.SMGGrenadeDamage = pSMGGrenade->GetFloat( "Damage" );
			primary.hasRecoilSMGGrenade = pSMGGrenade->GetBool( "UseRecoil", true );
		}

		if ( KeyValues *pAR2EnergyBall = pPrimaryFire->FindKey( "AR2EnergyBall" ) )
		{
			primary.AR2EnergyBallEnabled = true;
			primary.combineBallRadius = pAR2EnergyBall->GetFloat( "Radius" );
			primary.combineBallMass = pAR2EnergyBall->GetFloat( "Mass" );
			primary.combineBallDuration = pAR2EnergyBall->GetFloat( "Duration" );
		}

		if ( KeyValues *pRecoil = pPrimaryFire->FindKey( "Recoil" ) )
		{
			primary.recoilEasyDampen = pRecoil->GetFloat( "EasyDampen" );
			primary.recoilDegrees = pRecoil->GetFloat( "Degrees" );
			primary.recoilSeconds = pRecoil->GetFloat( "Seconds" );
		}
	}

	KeyValues *pSecondaryFire = pWeaponSpec->FindKey( "SecondaryFire" );
	if ( pSecondaryFire )
	{
		secondary.hasFire = true;
		secondary.fireRate = pSecondaryFire->GetFloat( "FireRate", 1.0f );
		secondary.ironsightFireRate = pSecondaryFire->GetFloat( "IronsightFireRate", secondary.fireRate );
		secondary.zoomFireRate = pSecondaryFire->GetFloat( "ZoomFireRate", secondary.fireRate );
		m_sUsePrimaryAmmo = pSecondaryFire->GetBool( "UsePrimaryAmmo" );
		secondary.infiniteAmmoEnabled = pSecondaryFire->GetBool( "InfiniteAmmo" );
		secondary.minRange = pSecondaryFire->GetInt( "MinRange" );
		secondary.maxRange = pSecondaryFire->GetInt( "MaxRange" );
		secondary.canFireUnderwater = pSecondaryFire->GetBool( "CanFireUnderwater", true );
		if ( KeyValues *pBullet = pSecondaryFire->FindKey( "Bullet" ) )
		{
			secondary.bulletEnabled = true;
			secondary.damage = pBullet->GetFloat( "Damage" );
			secondary.shotCount = pBullet->GetInt( "ShotCount" );

			if ( KeyValues *pSpread = pBullet->FindKey( "Spread" ) )
			{
				secondary.spread.x = sin( pSpread->GetFloat( "x" ) / 2.0f );
				secondary.spread.y = sin( pSpread->GetFloat( "y" ) / 2.0f );
				secondary.spread.z = sin( pSpread->GetFloat( "z" ) / 2.0f );
			}

			if ( KeyValues *pIronsightSpread = pBullet->FindKey( "IronsightSpread" ) )
			{
				secondary.ironsightSpread.x = sin( pIronsightSpread->GetFloat( "x" ) / 2.0f );
				secondary.ironsightSpread.y = sin( pIronsightSpread->GetFloat( "y" ) / 2.0f );
				secondary.ironsightSpread.z = sin( pIronsightSpread->GetFloat( "z" ) / 2.0f );
			}
			else
			{
				secondary.ironsightSpread.x = secondary.spread.x;
				secondary.ironsightSpread.y = secondary.spread.y;
				secondary.ironsightSpread.z = secondary.spread.z;
			}

			if ( KeyValues *pZoomSpread = pBullet->FindKey( "ZoomSpread" ) )
			{
				secondary.zoomSpread.x = sin( pZoomSpread->GetFloat( "x" ) / 2.0f );
				secondary.zoomSpread.y = sin( pZoomSpread->GetFloat( "y" ) / 2.0f );
				secondary.zoomSpread.z = sin( pZoomSpread->GetFloat( "z" ) / 2.0f );
			}
			else
			{
				secondary.zoomSpread.x = secondary.spread.x;
				secondary.zoomSpread.y = secondary.spread.y;
				secondary.zoomSpread.z = secondary.spread.z;
			}
		}

		if ( KeyValues *pMissle = pSecondaryFire->FindKey( "Missle" ) )
		{
			secondary.missleEnabled = true;
			secondary.hasRecoilRPGMissle = pMissle->GetBool( "UseRecoil", true );
		}

		if ( KeyValues *pSMGGrenade = pSecondaryFire->FindKey( "SMGGrenade" ) )
		{
			secondary.SMGGrenadeEnabled = true;
			secondary.SMGGrenadeDamage = pSMGGrenade->GetFloat( "Damage" );
			secondary.hasRecoilSMGGrenade = pSMGGrenade->GetBool( "UseRecoil", true );
		}

		if ( KeyValues *pAR2EnergyBall = pSecondaryFire->FindKey( "AR2EnergyBall" ) )
		{
			secondary.AR2EnergyBallEnabled = true;
			secondary.combineBallRadius = pAR2EnergyBall->GetFloat( "Radius" );
			secondary.combineBallMass = pAR2EnergyBall->GetFloat( "Mass" );
			secondary.combineBallDuration = pAR2EnergyBall->GetFloat( "Duration" );
		}

		if ( KeyValues *pRecoil = pSecondaryFire->FindKey( "Recoil" ) )
		{
			secondary.recoilEasyDampen = pRecoil->GetFloat( "EasyDampen" );
			secondary.recoilDegrees = pRecoil->GetFloat( "Degrees" );
			secondary.recoilSeconds = pRecoil->GetFloat( "Seconds" );
		}
	}

	if ( KeyValues *pZoom = pWeaponSpec->FindKey( "Zoom" ) )
	{
		m_sUsesZoom = true;
		m_sUseZoomOnPrimaryFire = pZoom->GetBool( "UseOnPrimaryFire" );
		m_sUsesZoomSound = pZoom->GetBool( "UsesSound", true );
		m_sUsesZoomColor = pZoom->GetBool( "UsesColor", true );
		KeyValues *pZoomColor = pZoom->FindKey( "ZoomColor" );
		if ( pZoomColor && m_sUsesZoomColor )
		{
			m_sZoomColorRed = pZoomColor->GetInt( "Red" );
			m_sZoomColorGreen = pZoomColor->GetInt( "Green" );
			m_sZoomColorBlue = pZoomColor->GetInt( "Blue" );
			m_sZoomColorAlpha = pZoomColor->GetInt( "Alpha" );
		}
		else
			m_sZoomColorAlpha = m_sZoomColorBlue = m_sZoomColorGreen = m_sZoomColorRed = 0;
	}

	if ( KeyValues *pCustomization = pWeaponSpec->FindKey( "Customization" ) )
	{
		m_sUsesCustomization = true;
		m_sWeaponSkin = pCustomization->GetInt( "Skin" );

		if ( KeyValues *pBodygroup = pWeaponSpec->FindKey( "Bodygroup1" ) )
		{
			m_sBodygroup1 = pBodygroup->GetInt( "Bodygroup" );
			m_sSubgroup1 = pBodygroup->GetInt( "Subgroup" );
		}

		if ( KeyValues *pBodygroup = pWeaponSpec->FindKey( "Bodygroup2" ) )
		{
			m_sBodygroup2 = pBodygroup->GetInt( "Bodygroup" );
			m_sSubgroup2 = pBodygroup->GetInt( "Subgroup" );
		}

		if ( KeyValues *pBodygroup = pWeaponSpec->FindKey( "Bodygroup3" ) )
		{
			m_sBodygroup3 = pBodygroup->GetInt( "Bodygroup" );
			m_sSubgroup3 = pBodygroup->GetInt( "Subgroup" );
		}

		if ( KeyValues *pBodygroup = pWeaponSpec->FindKey( "Bodygroup4" ) )
		{
			m_sBodygroup4 = pBodygroup->GetInt( "Bodygroup" );
			m_sSubgroup4 = pBodygroup->GetInt( "Subgroup" );
		}

		if ( KeyValues *pBodygroup = pWeaponSpec->FindKey( "Bodygroup5" ) )
		{
			m_sBodygroup5 = pBodygroup->GetInt( "Bodygroup" );
			m_sSubgroup5 = pBodygroup->GetInt( "Subgroup" );
		}

		if ( KeyValues *pBodygroup = pWeaponSpec->FindKey( "Bodygroup6" ) )
		{
			m_sBodygroup6 = pBodygroup->GetInt( "Bodygroup" );
			m_sSubgroup6 = pBodygroup->GetInt( "Subgroup" );
		}
	}
}