#include "cbase.h"

#ifdef CLIENT_DLL
#define CFuncSlippery C_FuncSlippery
#include "c_func_brush.h"
#define CFuncBrush C_FuncBrush
#else
#include "modelentities.h"
#endif

class CFuncSlippery: public CFuncBrush
{
public:
	DECLARE_CLASS( CFuncSlippery, CFuncBrush );
	DECLARE_DATADESC();
	DECLARE_NETWORKCLASS();

	float GetFriction();
	float GetExtraDuckingFriction();
#ifdef GAME_DLL
	void SetFriction(float friction);
	void InputSetFriction(inputdata_t& data);

	void SetExtraDuckingFriction(float friction);
	void InputSetExtraDuckingFriction(inputdata_t& data);
#endif
private:
	CNetworkVar(float, m_fFriction);
	CNetworkVar(float, m_fExtraDuckingFriction);
};