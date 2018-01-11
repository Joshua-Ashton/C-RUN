#include "cbase.h"
#include "func_slippery.h"

BEGIN_DATADESC(CFuncSlippery)
	DEFINE_FIELD_NAME(m_fFriction, "friction", FIELD_FLOAT),
	DEFINE_FIELD_NAME(m_fExtraDuckingFriction, "extraduckingfriction", FIELD_FLOAT),
#ifdef GAME_DLL
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetFriction", InputSetFriction),
	DEFINE_INPUTFUNC(FIELD_FLOAT, "SetExtraDuckingFriction", InputSetExtraDuckingFriction)
#endif
END_DATADESC()

IMPLEMENT_NETWORKCLASS_ALIASED(FuncSlippery, DT_FuncSlippery)

BEGIN_NETWORK_TABLE(CFuncSlippery, DT_FuncSlippery)
#ifdef GAME_DLL
	SendPropFloat(SENDINFO(m_fFriction)),
	SendPropFloat(SENDINFO(m_fExtraDuckingFriction))
#else
	RecvPropFloat(RECVINFO(m_fFriction)),
	RecvPropFloat(RECVINFO(m_fExtraDuckingFriction))
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

LINK_ENTITY_TO_CLASS( func_slippery, CFuncSlippery );