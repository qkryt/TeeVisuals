#include <game/client/components/menus.h>

#include <engine/shared/config.h>
#include <game/client/gameclient.h>
#include <game/client/ui.h>
#include <game/localization.h>

void CMenus::RenderSettingsTeeVisuals(CUIRect MainView)
{
	CUIRect Left, Right, Section, Button, Label;

	// Split into two columns
	MainView.VSplitMid(&Left, &Right, 20.0f);

	// ==================== LEFT COLUMN ====================

	// 1. Dynamic Island HUD Section
	Left.HSplitTop(150.0f, &Section, &Left);
	Section.HSplitTop(22.0f, &Label, &Section);
	Ui()->DoLabel(&Label, "Dynamic Island HUD", 16.0f, TEXTALIGN_ML);

	Section.HSplitTop(18.0f, &Button, &Section);
	if(DoButton_CheckBox(&g_Config.m_ClTvIsland, "Enable Dynamic Island HUD", g_Config.m_ClTvIsland, &Button))
		g_Config.m_ClTvIsland ^= 1;

	Section.HSplitTop(18.0f, &Button, &Section);
	if(DoButton_CheckBox(&g_Config.m_ClTvIslandTimerMode, "Session/Race Timer (Unchecked = Clock)", g_Config.m_ClTvIslandTimerMode, &Button))
		g_Config.m_ClTvIslandTimerMode ^= 1;

	Section.HSplitTop(20.0f, &Button, &Section);
	Ui()->DoScrollbarOption(&g_Config.m_ClTvIslandScale, &g_Config.m_ClTvIslandScale, &Button, "HUD Scale (%)", 50, 200);

	Section.HSplitTop(20.0f, &Button, &Section);
	Ui()->DoScrollbarOption(&g_Config.m_ClTvIslandPosY, &g_Config.m_ClTvIslandPosY, &Button, "HUD Top Offset", 0, 150);

	Section.HSplitTop(20.0f, &Button, &Section);
	Ui()->DoScrollbarOption(&g_Config.m_ClTvIslandBgAlpha, &g_Config.m_ClTvIslandBgAlpha, &Button, "Glass Opacity (%)", 10, 100);

	Left.HSplitTop(10.0f, nullptr, &Left);

	// 2. Rainbow Visuals Section
	Left.HSplitTop(170.0f, &Section, &Left);
	Section.HSplitTop(22.0f, &Label, &Section);
	Ui()->DoLabel(&Label, "Rainbow Effects", 16.0f, TEXTALIGN_ML);

	Section.HSplitTop(18.0f, &Button, &Section);
	if(DoButton_CheckBox(&g_Config.m_ClTvRainbowBody, "Rainbow Tee Body", g_Config.m_ClTvRainbowBody, &Button))
		g_Config.m_ClTvRainbowBody ^= 1;

	Section.HSplitTop(18.0f, &Button, &Section);
	if(DoButton_CheckBox(&g_Config.m_ClTvRainbowFeet, "Rainbow Tee Feet", g_Config.m_ClTvRainbowFeet, &Button))
		g_Config.m_ClTvRainbowFeet ^= 1;

	Section.HSplitTop(18.0f, &Button, &Section);
	if(DoButton_CheckBox(&g_Config.m_ClTvRainbowWeapon, "Rainbow Weapon", g_Config.m_ClTvRainbowWeapon, &Button))
		g_Config.m_ClTvRainbowWeapon ^= 1;

	Section.HSplitTop(18.0f, &Button, &Section);
	if(DoButton_CheckBox(&g_Config.m_ClTvRainbowHook, "Rainbow Hook Line", g_Config.m_ClTvRainbowHook, &Button))
		g_Config.m_ClTvRainbowHook ^= 1;

	Section.HSplitTop(20.0f, &Button, &Section);
	Ui()->DoScrollbarOption(&g_Config.m_ClTvRainbowSpeed, &g_Config.m_ClTvRainbowSpeed, &Button, "Rainbow Speed", 1, 200);

	Section.HSplitTop(20.0f, &Button, &Section);
	Ui()->DoScrollbarOption(&g_Config.m_ClTvRainbowSat, &g_Config.m_ClTvRainbowSat, &Button, "Rainbow Saturation (%)", 0, 100);

	// ==================== RIGHT COLUMN ====================

	// 3. Player Trails Section
	Right.HSplitTop(130.0f, &Section, &Right);
	Section.HSplitTop(22.0f, &Label, &Section);
	Ui()->DoLabel(&Label, "Player Trails", 16.0f, TEXTALIGN_ML);

	Section.HSplitTop(20.0f, &Button, &Section);
	Ui()->DoScrollbarOption(&g_Config.m_ClTvTrails, &g_Config.m_ClTvTrails, &Button, "Trails (0: Off, 1: Self, 2: All)", 0, 2);

	Section.HSplitTop(20.0f, &Button, &Section);
	Ui()->DoScrollbarOption(&g_Config.m_ClTvTrailMode, &g_Config.m_ClTvTrailMode, &Button, "Color Mode (0: Solid, 1: Orange Gradient, 2: Rainbow)", 0, 2);

	Section.HSplitTop(20.0f, &Button, &Section);
	Ui()->DoScrollbarOption(&g_Config.m_ClTvTrailLength, &g_Config.m_ClTvTrailLength, &Button, "Trail Length", 5, 100);

	Section.HSplitTop(20.0f, &Button, &Section);
	Ui()->DoScrollbarOption(&g_Config.m_ClTvTrailWidth, &g_Config.m_ClTvTrailWidth, &Button, "Trail Thickness", 2, 32);

	Right.HSplitTop(10.0f, nullptr, &Right);

	// 4. Freeze & Off-Screen HUD Section
	Right.HSplitTop(130.0f, &Section, &Right);
	Section.HSplitTop(22.0f, &Label, &Section);
	Ui()->DoLabel(&Label, "Freeze & Off-Screen HUD", 16.0f, TEXTALIGN_ML);

	Section.HSplitTop(18.0f, &Button, &Section);
	if(DoButton_CheckBox(&g_Config.m_ClTvFreezeIndicator, "Show Freeze Indicators Above Players", g_Config.m_ClTvFreezeIndicator, &Button))
		g_Config.m_ClTvFreezeIndicator ^= 1;

	Section.HSplitTop(18.0f, &Button, &Section);
	if(DoButton_CheckBox(&g_Config.m_ClTvOffscreenHud, "Show Off-Screen Edge Radar Markers", g_Config.m_ClTvOffscreenHud, &Button))
		g_Config.m_ClTvOffscreenHud ^= 1;

	Section.HSplitTop(18.0f, &Button, &Section);
	if(DoButton_CheckBox(&g_Config.m_ClTvOffscreenTeammates, "Track Off-Screen Teammates", g_Config.m_ClTvOffscreenTeammates, &Button))
		g_Config.m_ClTvOffscreenTeammates ^= 1;

	Section.HSplitTop(18.0f, &Button, &Section);
	if(DoButton_CheckBox(&g_Config.m_ClTvOffscreenWar, "Track Off-Screen War Targets", g_Config.m_ClTvOffscreenWar, &Button))
		g_Config.m_ClTvOffscreenWar ^= 1;

	Right.HSplitTop(10.0f, nullptr, &Right);

	// 5. Social & Smooth Bindwheel Section
	Right.HSplitTop(110.0f, &Section, &Right);
	Section.HSplitTop(22.0f, &Label, &Section);
	Ui()->DoLabel(&Label, "Social System & Bindwheel", 16.0f, TEXTALIGN_ML);

	Section.HSplitTop(18.0f, &Button, &Section);
	if(DoButton_CheckBox(&g_Config.m_ClTvPlayerLists, "Enable War / Team / Helper Lists (.addwar, .addteam)", g_Config.m_ClTvPlayerLists, &Button))
		g_Config.m_ClTvPlayerLists ^= 1;

	Section.HSplitTop(18.0f, &Button, &Section);
	if(DoButton_CheckBox(&g_Config.m_ClTvSmoothBindwheel, "Enable Smooth Bindwheel (+tv_bindwheel)", g_Config.m_ClTvSmoothBindwheel, &Button))
		g_Config.m_ClTvSmoothBindwheel ^= 1;

	Section.HSplitTop(20.0f, &Button, &Section);
	Ui()->DoScrollbarOption(&g_Config.m_ClTvBindwheelRadius, &g_Config.m_ClTvBindwheelRadius, &Button, "Bindwheel Radius", 80, 300);
}
