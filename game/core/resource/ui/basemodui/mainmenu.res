"Resource/UI/MainMenu.res"
{
	"MainMenu"
	{
		"ControlName"			"Frame"
		"fieldName"				"MainMenu"
		"xpos"					"0"
		"ypos"					"0"
		"wide"					"f0"
		"tall"					"f0"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"			"0"
		"PaintBackgroundType"	"0"
	}
	

	"BtnContinueGame"
	{	
		"ControlName"				"BaseModHybridButton"
		"fieldName"				"BtnContinueGame"
		"xpos"					"0"
		"ypos"					"250" //"250" 30
		"wide"					"280"
		"tall"					"16"
		"autoResize"				"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"				"1"
		"navUp"					"BtnQuit"
		"navDown"				"BtnSingleplayer"
		"labelText"				"#Template_ContinueGame"
		"tooltiptext"				"#Template_ContinueGame"	[$X360]
		"tooltiptext"				"#Template_ContinueGame"	[$WIN32]
		"style"					"AlienSwarmMenuButton"
		"command"				"LoadLastSave_NoConfirm"
		"ActivationType"			"1"
		"EnableCondition"			"Never" [$DEMO]
	}	
	

	"BtnSingleplayer"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnSingleplayer"
		"xpos"					"0"
		"ypos"					"270" // "265" 45
		"wide"					"280"
		"tall"					"16"
		"autoResize"				"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"				"0"
		"navUp"					"BtnContinueGame"
		"navDown"				"BtnChangeGamers"
		"labelText"				"#Template_NewGame"
		"tooltiptext"				"#L4D360UI_MainMenu_PlaySolo_Tip"
		"style"					"AlienSwarmMenuButton"
		"command"				"SoloPlay"	
		"ActivationType"			"1"
	}

	
	"BtnOptions"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnOptions"
		"xpos"					"0"
		"ypos"					"290"
		"wide"					"280"
		"tall"					"16"
		"autoResize"				"1"
		"pinCorner"				"0"
		"visible"				"1"
		"enabled"				"1"
		"tabPosition"				"0"
		"navUp"					"BtnChangeGamers"
		"navDown"				"BtnExtras"
		"labelText"				"#L4D360UI_MainMenu_Options"
		"tooltiptext"				"#L4D360UI_MainMenu_Options_Tip"
		"style"					"AlienSwarmMenuButton"
		//"command"				"FlmOptionsFlyout"			[!$X360GUEST]
		//"command"				"FlmOptionsGuestFlyout"		[$X360GUEST]	
		"command"				"FlmOptionsFlyout"
		"ActivationType"			"1"
	}

//	"BtnStatsAndAchievements"
//	{
//		"ControlName"				"BaseModHybridButton"
//		"fieldName"				"BtnStatsAndAchievements"
//		"xpos"					"0"
//		"ypos"					"265"
//		"wide"					"210"
//		"tall"					"16"
//		"autoResize"				"1"
//		"pinCorner"				"0"
//		"visible"				"0"		[$X360GUEST]
//		"enabled"				"0"		[$X360GUEST]
//		"visible"				"0"		[!$X360GUEST]		// jms: TEMP disabled
//		"enabled"				"1"		[!$X360GUEST]
//		"tabPosition"				"0"
//		"navUp"					"BtnOptions"
//		"navDown"				"BtnExtras"
//		"labelText"				"#L4D360UI_MainMenu_StatsAndAchievements"
//		"tooltiptext"				"#L4D360UI_MainMenu_StatsAndAchievements_Tip"	[$X360]
//		"tooltiptext"				"#L4D360UI_MainMenu_PCStatsAndAchievements_Tip"	[$WIN32]
//		"style"					"AlienSwarmMenuButton"
//		"command"				"StatsAndAchievements"
//		"ActivationType"			"1"
//		"EnableCondition"			"Never" [$DEMO]
//	}

	"BtnExtras"
	{
		"ControlName"				"BaseModHybridButton"
		"fieldName"				"BtnExtras"
		"xpos"					"0"
		"ypos"					"310" 
		"wide"					"280"
		"tall"					"16"
		"autoResize"				"1"
		"pinCorner"				"0"
		"visible"				"1" //No show unless we atleast have dev commentary
		"enabled"				"1"
		"tabPosition"				"0"
		"navUp"					"BtnOptions"
		"navDown"				"BtnQuit"
		"labelText"				"#L4D360UI_MainMenu_Extras"
		"tooltiptext"				"#L4D360UI_MainMenu_Extras_Tip"
		"style"					"AlienSwarmMenuButton"	
		"command"				"FlmExtrasFlyoutCheck"
		"ActivationType"			"1"
	}

	"BtnQuit"
	{
		"ControlName"			"BaseModHybridButton"
		"fieldName"				"BtnQuit"
		"xpos"					"0"
		"ypos"					"330"
		"wide"					"280"
		"tall"					"16"
		"autoResize"			"1"
		"pinCorner"				"0"
		"visible"				"0"		[$X360]
		"visible"				"1"		[$WIN32]
		"enabled"				"1"
		"tabPosition"			"0"
		"navUp"					"BtnExtras"
		"navDown"				"BtnContinueGame"
		"style"					"AlienSwarmMenuButton"
		"ActivationType"		"1"
		"labelText"				"#GameUI_GameMenu_Quit"			[$WIN32]
		"tooltiptext"			"#L4D360UI_MainMenu_Quit_Tip"		[$WIN32]
		"labelText"				"#L4D360UI_MainMenu_QuitDemo"		[$X360]
		"tooltiptext"			"L4D360UI_MainMenu_QuitDemo_Tip"	[$X360]
		"command"				"QuitGame"
	}

	"FlmCampaignFlyout"
	{
		"ControlName"			"FlyoutMenu"
		"fieldName"				"FlmCampaignFlyout"
		"visible"				"0"
		"wide"					"0"
		"tall"					"0"
		"zpos"					"3"
		"InitialFocus"			"BtnQuickMatch"
		"ResourceFile"			"resource/UI/basemodui/CampaignFlyout.res"
	}
	
	"FlmOptionsFlyout"
	{
		"ControlName"			"FlyoutMenu"
		"fieldName"				"FlmOptionsFlyout"
		"visible"				"0"
		"wide"					"0"
		"tall"					"0"
		"zpos"					"3"
		"InitialFocus"			"BtnAudioVideo"	[$X360]
		"InitialFocus"			"BtnVideo"	[$WIN32]
		"ResourceFile"			"resource/UI/basemodui/OptionsFlyout.res"
	}
	
	"FlmOptionsGuestFlyout"
	{
		"ControlName"			"FlyoutMenu"
		"fieldName"				"FlmOptionsGuestFlyout"
		"visible"				"0"
		"wide"					"0"
		"tall"					"0"
		"zpos"					"3"
		"InitialFocus"			"BtnAudioVideo"
		"ResourceFile"			"resource/UI/basemodui/OptionsGuestFlyout.res"
	}

	"FlmExtrasFlyout_Simple"
	{
		"ControlName"			"FlyoutMenu"
		"fieldName"				"FlmExtrasFlyout_Simple"
		"visible"				"0"
		"wide"					"0"
		"tall"					"0"
		"zpos"					"3"
		"InitialFocus"			"BtnCommentary"
		"ResourceFile"			"resource/UI/basemodui/ExtrasFlyout.res"
	}

	"FlmExtrasFlyout_Live"
	{
		"ControlName"			"FlyoutMenu"
		"fieldName"				"FlmExtrasFlyout_Live"
		"visible"				"0"
		"wide"					"0"
		"tall"					"0"
		"zpos"					"3"
		"InitialFocus"			"BtnCommunity"
		"ResourceFile"			"resource/UI/basemodui/ExtrasFlyoutLive.res"
	}
			
	"PnlQuickJoin"
	{
		"ControlName"			"QuickJoinPanel"
		"fieldName"				"PnlQuickJoin"
		"ResourceFile"			"resource/UI/basemodui/QuickJoin.res"
		"visible"				"0"
		"wide"					"500"	[$X360]
		"wide"					"240"	[$WIN32]
		"tall"					"300"
		"xpos"					"r260"	[$X360]
		"xpos"					"80"	[$WIN32]
		"ypos"					"r120"	[$X360]
		"ypos"					"r75"	[$WIN32]
		"navUp"					"BtnQuit"
		"navDown"				"BtnSingleplayer"
	}
	
	"PnlQuickJoinGroups"	[$WIN32]
	{
		"ControlName"			"QuickJoinGroupsPanel"
		"fieldName"				"PnlQuickJoinGroups"
		"ResourceFile"			"resource/UI/basemodui/QuickJoinGroups.res"
		"visible"				"0"
		"wide"					"500"
		"tall"					"300"
		"xpos"					"c0"
		"ypos"					"r75"
		"navUp"					""
		"navDown"				""
	}

	"LblPlayer1GamerTag"	[$X360]
	{
		"ControlName"			"Label"
		"fieldName"				"LblPlayer1GamerTag"
		"xpos"					"0"
		"ypos"					"r120"
		"wide"					"300"
		"tall"					"16"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"DefaultMedium"
		"fgcolor_override"		"125 125 125 255"
		"noshortcutsyntax"		"1"
	}
	
	// either this or the enable
	"LblPlayer2GamerTag"	[$X360]
	{
		"ControlName"			"Label"
		"fieldName"				"LblPlayer2GamerTag"
		"xpos"					"0"
		"ypos"					"r100"
		"wide"					"300"
		"tall"					"16"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"DefaultMedium"
		"fgcolor_override"		"125 125 125 255"
		"noshortcutsyntax"		"1"
	}
	
	"LblPlayer2Enable"	[$X360]
	{
		"ControlName"			"Label"
		"fieldName"				"LblPlayer2Enable"
		"xpos"					"0"
		"ypos"					"r100"
		"wide"					"300"
		"tall"					"16"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"labelText"				"#L4D360UI_MainMenu_SignInMessageSsStart2"
		"Font"					"DefaultMedium"
		"fgcolor_override"		"125 125 125 255"
	}
	
	"LblPlayer2DisableIcon"		[$X360]
	{
		"ControlName"			"Label"
		"fieldName"				"LblPlayer2DisableIcon"
		"xpos"					"0"
		"ypos"					"r140"
		"wide"					"30"
		"tall"					"16"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"GameUIButtonsMini"
		"labelText"				"#GameUI_Icons_BACK"
	}
	
	"LblPlayer2Disable"		[$X360]
	{
		"ControlName"			"Label"
		"fieldName"				"LblPlayer2Disable"
		"xpos"					"125"
		"ypos"					"r140"
		"wide"					"300"
		"tall"					"16"
		"autoResize"			"0"
		"pinCorner"				"0"
		"visible"				"0"
		"enabled"				"1"
		"tabPosition"			"0"
		"Font"					"DefaultMedium"
		"labelText"				"#L4D360UI_MainMenu_SignInMessageSsDisable"
		"fgcolor_override"		"125 125 125 255"
	}
}
