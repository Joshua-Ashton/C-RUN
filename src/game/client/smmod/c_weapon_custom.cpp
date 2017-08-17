#include "cbase.h"
#include "c_weapon_custom.h"
#include "igamemovement.h"
#include "networkstringtabledefs.h"

#include <string>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

struct customWeapons
{
	int entindex;
	std::string cName;

	customWeapons( int entindex, const std::string& name )
		: entindex( entindex ), cName( name ) {}
};

CUtlVector<customWeapons> factories;

static class Cleaner : public CAutoGameSystem
{
public:
	void LevelShutdownPreEntity() override
	{
		factories.Purge();
	}
} cleaner;

void OnCustomWeaponsFactoryTableChanged( void*, INetworkStringTable*, int, const char *newString, void const* )
{
	CUtlVector<char*> data;
	V_SplitString( newString, "|", data );

	int entindex = V_atoi( data[0] );
	std::string className = data[1];
	data.PurgeAndDeleteElements();

	factories.AddToTail( customWeapons( entindex, className ) );
}

static IClientNetworkable* C_WeaponCustom_CreateObject( int entnum, int serialNum )
{
	std::string cName;
	FOR_EACH_VEC( factories, i )
		if ( factories[i].entindex == entnum )
		{
			cName = factories[i].cName;
			break;
		}
	
	if ( cName.empty() )
		Warning("C_WeaponCustom_CreateObject did not recieve classname from server\n");

	C_WeaponCustom *pRet = cName.empty() ? new C_WeaponCustom : new C_WeaponCustom( cName.c_str() );
	if ( !pRet ) return NULL;
	pRet->Init( entnum, serialNum );
	return pRet;
}

IMPLEMENT_CLIENTCLASS_FACTORY( C_WeaponCustom, DT_WeaponCustom, CWeaponCustom, C_WeaponCustom_CreateObject )
BEGIN_RECV_TABLE( C_WeaponCustom, DT_WeaponCustom )
	RecvPropInt( RECVINFO( scopeNum ) ),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_WeaponCustom )
END_PREDICTION_DATA()

C_WeaponCustom::C_WeaponCustom( const char* className ) : scopeNum( 0 )
{
	V_strncpy( m_szClassName, className, 128 );
}