#include "cbase.h"
#include "hud.h"
#include "hud_macros.h"
#include "view.h"

#include "iclientmode.h"

#include <KeyValues.h>
#include <vgui_controls/Panel.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/AnimationController.h>

using namespace vgui;

#include "hudelement.h"
#include "hud_numericdisplay.h"

#include "ConVar.h"

//-----------------------------------------------------------------------------
// Purpose: Health panel
//-----------------------------------------------------------------------------
class CHudDev : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudDev, vgui::Panel );

public:
	CHudDev( const char *pElementName );
	virtual void Init( void );
	//virtual void VidInit( void );
	//virtual void Reset( void );
	//virtual void OnThink();
	bool ShouldDraw( void );

	virtual void Paint( void );
	virtual void ApplySchemeSettings( IScheme *scheme );



private:

	CPanelAnimationVar( Color, m_TextColor, "TextColor", "255 255 255 255" );

	CPanelAnimationVarAliasType( float, text_xpos, "text_xpos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, text_ypos, "text_ypos", "0", "proportional_float" );
};	

DECLARE_HUDELEMENT( CHudDev );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudDev::CHudDev( const char *pElementName ) : CHudElement( pElementName ), BaseClass (NULL, "HudDev")
{
#ifndef SWARM_DLL
	vgui::Panel *pParent = g_pClientMode->GetViewport();
#else
	vgui::Panel *pParent = GetClientMode()->GetViewport();
#endif

	SetParent (pParent);

	SetPaintBorderEnabled( false );
	SetPaintBackgroundEnabled( false );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudDev::Init()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudDev::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings(pScheme);
	SetSize( ScreenWidth(), ScreenHeight() );
}

void CHudDev::Paint()
{


	// Get our scheme and font information
	vgui::HScheme scheme = vgui::scheme()->GetScheme( "ClientScheme" );
	vgui::HFont hFont = vgui::scheme()->GetIScheme(scheme)->GetFont( "ClientTitleFont" );
	vgui::surface()->DrawSetTextFont( hFont );
	vgui::surface()->DrawSetTextColor( m_TextColor );


#ifndef DEBUG
 	wchar_t *tempString = g_pVGuiLocalize->Find("#GameUI_DeveloperMode");
#else
	wchar_t *tempString = g_pVGuiLocalize->Find("#GameUI_DebugBuild");
#endif

	if (tempString)
	{
		int iLength = 0;
		for ( wchar_t *wch = tempString; *wch != 0; wch++ )
		{
			iLength += vgui::surface()->GetCharacterWidth( hFont, *wch );
		}
		vgui::surface()->DrawSetTextPos( text_xpos, text_ypos );
		vgui::surface()->DrawPrintText(tempString, wcslen(tempString));
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHudDev::ShouldDraw()
{
#ifndef DEBUG
	ConVarRef pDeveloper( "developer" );

	if (pDeveloper.GetFloat() >= 1)
	{
		return true;
	}

	return false;
#else
	return true;
#endif
}
