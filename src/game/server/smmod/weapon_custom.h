#ifndef	WEAPONCUSTOM_H
#define	WEAPONCUSTOM_H

#include "smmod/weapon_parse_custom_weapon.h"

#ifdef HL2_DLL
#include "weapon_rpg.h"
#include "basehlcombatweapon.h"
#define CUSTOM_WEAPON_BASE CHLSelectFireMachineGun
#else
#include "template/weapons/basesdkcombatweapon.h"
#define CUSTOM_WEAPON_BASE CSDKSelectFireMachineGun
#endif

class CWeaponCustom : public CUSTOM_WEAPON_BASE
{
	DECLARE_DATADESC();
public:
	DECLARE_CLASS( CWeaponCustom, CUSTOM_WEAPON_BASE );

	CWeaponCustom();

	DECLARE_SERVERCLASS();

	DECLARE_ACTTABLE();
	
	void	Precache( void );
	void	AddViewKick(float easyDampen, float degrees, float seconds);
	void	ShootBullets( bool isPrimary = true, bool usePrimaryAmmo = true );
	void	ShootBulletsRight(bool isPrimary = true, bool usePrimaryAmmo = true);
	void	ShootBulletsLeft(bool isPrimary = true, bool usePrimaryAmmo = true);

#ifdef HL2_DLL
	void	ShootProjectile( bool isPrimary, bool usePrimaryAmmo );
	void	ShootProjectileRight(bool isPrimary, bool usePrimaryAmmo);
	void	ShootProjectileLeft(bool isPrimary, bool usePrimaryAmmo);

	void	ShootAR2EnergyBall(bool isPrimary, bool usePrimaryAmmo);
	void	ShootAR2EnergyBallRight(bool isPrimary, bool usePrimaryAmmo);
	void	ShootAR2EnergyBallLeft(bool isPrimary, bool usePrimaryAmmo);

	void	ShootSMGGrenade(bool isPrimary, bool usePrimaryAmmo);
	void	ShootSMGGrenadeRight(bool isPrimary, bool usePrimaryAmmo);
	void	ShootSMGGrenadeLeft(bool isPrimary, bool usePrimaryAmmo);
#endif
	//void	ShootFragGrenadeThrow(bool isPrimary, bool usePrimaryAmmo);
	//void	ShootFragGrenadeRoll(bool isPrimary, bool usePrimaryAmmo);

	void	ItemPostFrame( void );
	void	ItemBusyFrame(void);
	void	PrimaryAttack( void );
	void	SecondaryAttack( void );

	int		GetMinBurst() { return 2; }
	int		GetMaxBurst() { return 5; }

	virtual void Equip( CBaseCombatCharacter *pOwner );
	bool	Reload( void );
	void	Drop(const Vector &vecVelocity);
	bool	Holster(CBaseCombatWeapon *pSwitchingTo = NULL);

	void	ApplyCustomization(void);

	float	GetPrimaryFireRate( void ) 
	{ 
		/*if (IsIronsighted())
		{
			return GetWpnDataCustom().primary.ironsightFireRate;
		}
		else*/ if (m_bInZoom)
		{
			return GetWpnDataCustom().primary.zoomFireRate;
		}
		else
		{
			return GetWpnDataCustom().primary.fireRate;
		} 
	}

	float	GetSecondaryFireRate(void)
	{
		/*if (IsIronsighted())
		{
			return GetWpnDataCustom().secondary.ironsightFireRate;
		}
		else*/ if (m_bInZoom)
		{
			return GetWpnDataCustom().secondary.zoomFireRate;
		}
		else
		{
			return GetWpnDataCustom().secondary.fireRate;
		}
	}

	int		CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }
	int		WeaponRangeAttack2Condition( float flDot, float flDist );
	Activity	GetPrimaryAttackActivity( void );
	Activity	GetLeftGunActivity(void);
	Activity	GetRightGunActivity(void);

	virtual const Vector& GetBulletSpreadPrimary( void )
	{
		/*if (IsIronsighted())
		{
			return GetWpnDataCustom().primary.ironsightSpread;
		}
		else*/ if (m_bInZoom)
		{
			return GetWpnDataCustom().primary.zoomSpread;
		}
		else
		{
			return GetWpnDataCustom().primary.spread;
		}
	}

	virtual const Vector& GetBulletSpreadSecondary( void )
	{
		/*if (IsIronsighted())
		{
			return GetWpnDataCustom().secondary.ironsightSpread;
		}
		else*/ if (m_bInZoom)
		{
			return GetWpnDataCustom().secondary.zoomSpread;
		}
		else
		{
			return GetWpnDataCustom().secondary.spread;
		}
	}
	bool IsPrimaryBullet(void) { return GetWpnDataCustom().primary.bulletEnabled; }
	bool IsSecondaryBullet(void) { return GetWpnDataCustom().secondary.bulletEnabled; }

	const WeaponProficiencyInfo_t *GetProficiencyValues();

	void FireNPCPrimaryAttack( CBaseCombatCharacter *pOperator, Vector &vecShootOrigin, Vector &vecShootDir );
	void Operator_ForceNPCFire( CBaseCombatCharacter  *pOperator, bool bSecondary );
	void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );

	const CustomWeaponInfo &GetWpnDataCustom( void ) const
	{
		return static_cast< const CustomWeaponInfo& >( GetWpnData() );
	}

private:
	void	CheckZoomToggle(void);
	void	ToggleZoom(void);

private:
	bool				m_bInZoom;
	bool				bFlip;
#ifdef HL2_DLL
	CHandle<CMissile>	m_hMissile;
	CHandle<CMissile>	m_hMissile2;
#endif
	CNetworkVar( int, scopeNum );
};

#endif	//WEAPONCUSTOM_H