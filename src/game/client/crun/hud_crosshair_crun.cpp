//=========== Copyright © 2014, rHetorical, All rights reserved. =============
//
// Purpose: The basic crosshair refuses to draw, so here is a crosshair to replace it.
//
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "iclientmode.h"
#include "engine/IEngineSound.h"
#include "vgui_controls/AnimationController.h"
#include "vgui_controls/Controls.h"
#include <vgui_controls/Panel.h>
#include <vgui/isurface.h>
#include "clientmode.h"
#include "c_baseplayer.h"
#include "materialsystem/IMaterial.h"
#include "materialsystem/IMesh.h"
#include "materialsystem/imaterialvar.h"
#include "mathlib/mathlib.h"
#include "../hud_crosshair.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

namespace vgui
{
	class IScheme;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CHudTemplateCrosshair : public CHudElement, public vgui::Panel
{
public:
	DECLARE_CLASS_SIMPLE( CHudTemplateCrosshair, vgui::Panel );

	CHudTemplateCrosshair( const char *name );

	//virtual void OnThink();
	virtual void Paint();
	virtual void Init();
	virtual bool ShouldDraw();
	virtual void ApplySchemeSettings( vgui::IScheme *scheme );

	virtual void LevelShutdown( void );

	//stub
	void SetCrosshair( CHudTexture *texture, Color& clr ) {}
	void ResetCrosshair() {}

private:

	void	UpdateEventTime( void );
	//bool	m_bDraw;
	int					m_iCrosshairTextureID;
	IVguiMatInfo		*m_pCrosshair;	

	CHudTexture		*m_icon01;
};

using namespace vgui;

DECLARE_HUDELEMENT( CHudTemplateCrosshair );

CHudTemplateCrosshair::CHudTemplateCrosshair( const char *pName ) :
	vgui::Panel( NULL, "HudCrosshair" ), CHudElement( pName )
{
	vgui::Panel *pParent = GetClientMode()->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_PLAYERDEAD );
}

void CHudTemplateCrosshair::ApplySchemeSettings( IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );

	m_icon01 = HudIcons().GetIcon( "crosshair_default" );

	SetPaintBackgroundEnabled( false );

	SetSize( ScreenWidth(), ScreenHeight() );
}

void CHudTemplateCrosshair::LevelShutdown( void )
{
}

void CHudTemplateCrosshair::Init()
{
}

bool CHudTemplateCrosshair::ShouldDraw()
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return false;

	if ( !crosshair.GetBool() && !IsX360() )
		return false;

	return true;
}

void CHudTemplateCrosshair::Paint()
{
	int		xCenter	= ( ScreenWidth() - m_icon01->Width() ) / 2;
	int		yCenter = ( ScreenHeight() - m_icon01->Height() ) / 2;

	Color clr(255,255,255,255);

	m_icon01->DrawSelf( xCenter, yCenter, clr );
}
