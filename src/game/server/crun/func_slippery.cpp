#include "cbase.h"
#include "modelentities.h"

class CFuncSlippery: public CFuncBrush
{
public:
	DECLARE_CLASS( CFuncSlippery, CFuncBrush );
};

LINK_ENTITY_TO_CLASS( func_slippery, CFuncSlippery );