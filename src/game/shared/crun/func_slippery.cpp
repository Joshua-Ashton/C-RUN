#include "cbase.h"
#include "func_slippery.h"

BEGIN_DATADESC(CFuncSlippery)
	DEFINE_FIELD_NAME(m_fFriction, "friction", FIELD_FLOAT),
	DEFINE_FIELD_NAME(m_fExtraDuckingFriction, "extraduckingfriction", FIELD_FLOAT),
	DEFINE_FIELD_NAME(m_bDisablePlayerMovement, "disableplayermovement", FIELD_BOOLEAN),
	DEFINE_FIELD_NAME(m_bDisableFrictionChanges, "disablefrictionchanges", FIELD_BOOLEAN),
#ifdef GAME_DLL
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetFriction", InputSetFriction),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetExtraDuckingFriction", InputSetExtraDuckingFriction),
	DEFINE_INPUTFUNC(FIELD_INTEGER, "SetDisablePlayerMovement", InputSetDisablePlayerMovement),
	DEFINE_INPUTFUNC(FIELD_INTEGER, "SetDisableFrictionChanges", InputSetDisableFrictionChanges)
#endif
END_DATADESC()

IMPLEMENT_NETWORKCLASS_ALIASED(FuncSlippery, DT_FuncSlippery)

BEGIN_NETWORK_TABLE(CFuncSlippery, DT_FuncSlippery)
#ifdef GAME_DLL
	SendPropFloat(SENDINFO(m_fFriction)),
	SendPropFloat(SENDINFO(m_fExtraDuckingFriction)),
	SendPropBool(SENDINFO(m_bDisableFrictionChanges)),
	SendPropBool(SENDINFO(m_bDisablePlayerMovement))
#else
	RecvPropFloat(RECVINFO(m_fFriction)),
	RecvPropFloat(RECVINFO(m_fExtraDuckingFriction)),
	RecvPropBool(RECVINFO(m_bDisableFrictionChanges)),
	RecvPropBool(RECVINFO(m_bDisablePlayerMovement))
#endif
END_NETWORK_TABLE()

float CFuncSlippery::GetFriction()
{
	return m_fFriction;
}

#ifdef GAME_DLL
void CFuncSlippery::SetFriction(float friction)
{
	m_fFriction = friction;
}
void CFuncSlippery::InputSetFriction(inputdata_t& data)
{
	SetFriction(data.value.Float());
}
#endif

float CFuncSlippery::GetExtraDuckingFriction()
{
	return m_fExtraDuckingFriction;
}

#ifdef GAME_DLL
void CFuncSlippery::SetExtraDuckingFriction(float friction)
{
	m_fExtraDuckingFriction = friction;
}
void CFuncSlippery::InputSetExtraDuckingFriction(inputdata_t& data)
{
	SetExtraDuckingFriction(data.value.Float());
}
#endif

bool CFuncSlippery::GetDisableFrictionChanges()
{
	return m_bDisableFrictionChanges;
}
#ifdef GAME_DLL
void CFuncSlippery::SetDisableFrictionChanges(bool disabled)
{
	m_bDisableFrictionChanges = disabled;
}
void CFuncSlippery::InputSetDisableFrictionChanges(inputdata_t& data)
{
	SetDisableFrictionChanges(data.value.Int() != 0);
}
#endif

bool CFuncSlippery::GetDisablePlayerMovement()
{
	return m_bDisablePlayerMovement;
}
#ifdef GAME_DLL
void CFuncSlippery::SetDisablePlayerMovement(bool disabled)
{
	m_bDisablePlayerMovement = disabled;
}
void CFuncSlippery::InputSetDisablePlayerMovement(inputdata_t& data)
{
	SetDisablePlayerMovement(data.value.Int() != 0);
}
#endif

LINK_ENTITY_TO_CLASS( func_slippery, CFuncSlippery );