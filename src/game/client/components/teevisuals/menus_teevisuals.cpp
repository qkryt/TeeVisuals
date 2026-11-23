#include "menus_teevisuals.h"
#include "teevisuals_common.h"

#include <base/color.h>
#include <engine/client.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>
#include <game/client/components/menus.h>
#include <game/client/gameclient.h>
#include <game/client/lineinput.h>
#include <game/client/ui.h>
#include <game/client/ui_scrollregion.h>
#include <game/localization.h>

#include <vector>

void CMenusTeeVisuals::RenderSettings(CMenus *pMenus, CUIRect MainView)
{
	static int s_CurSubTab = SUBTAB_HUD_ISLAND;

	CUIRect SubTabBar, Content;
	MainView.HSplitTop(26.0f, &SubTabBar, &Content);
	Content.HSplitTop(8.0f, nullptr, &Content);

	const char *apSubTabs[NUM_SUBTABS] = {
		Localize("HUD & Island"),
		Localize("Visuals"),
		Localize("Social Lists"),
		Localize("Bindwheel & Mobile"),
		Localize("Ported Features")};

	static CButtonContainer s_aSubTabButtons[NUM_SUBTABS];
	const float TabWidth = SubTabBar.w / (float)NUM_SUBTABS;

	for(int i = 0; i < NUM_SUBTABS; i++)
	{
		CUIRect TabRect;
		SubTabBar.VSplitLeft(TabWidth, &TabRect, &SubTabBar);

		const bool Active = (s_CurSubTab == i);
		if(Active)
		{
			// Brand orange-yellow gradient underline/glow
			pMenus->Graphics()->DrawRect(TabRect.x, TabRect.y, TabRect.w, TabRect.h, ColorRGBA(0.20f, 0.14f, 0.05f, 0.90f), IGraphics::CORNER_ALL, 4.0f);
			pMenus->Graphics()->DrawRectOutline(TabRect.x, TabRect.y, TabRect.w, TabRect.h, TeeVisuals::COLOR_ORANGE, IGraphics::CORNER_ALL, IGraphics::SIDE_ALL, 4.0f);
		}

		if(pMenus->DoButton_Menu(&s_aSubTabButtons[i], apSubTabs[i], Active, &TabRect))
		{
			s_CurSubTab = i;
		}
	}

	switch(s_CurSubTab)
	{
	case SUBTAB_HUD_ISLAND:
		RenderSubTabHudIsland(pMenus, Content);
		break;
	case SUBTAB_VISUALS:
		RenderSubTabVisuals(pMenus, Content);
		break;
	case SUBTAB_SOCIAL_LISTS:
		RenderSubTabSocialLists(pMenus, Content);
		break;
	case SUBTAB_BINDWHEEL_MOBILE:
		RenderSubTabBindwheelMobile(pMenus, Content);
		break;
	case SUBTAB_PORTED_FEATURES:
		RenderSubTabPortedFeatures(pMenus, Content);
		break;
	default:
		break;
	}
}

void CMenusTeeVisuals::RenderSubTabHudIsland(CMenus *pMenus, CUIRect View)
{
	CUIRect Left, Right, Section, Item, Label;
	View.VSplitMid(&Left, &Right, 16.0f);

	// ==================== LEFT: Dynamic Island HUD ====================
	Left.HSplitTop(24.0f, &Label, &Left);
	pMenus->TextRender()->TextColor(TeeVisuals::COLOR_ORANGE.r, TeeVisuals::COLOR_ORANGE.g, TeeVisuals::COLOR_ORANGE.b, 1.0f);
	pMenus->Ui()->DoLabel(&Label, "Dynamic Island HUD", 15.0f, TEXTALIGN_ML);
	pMenus->TextRender()->TextColor(pMenus->TextRender()->DefaultTextColor());

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvIsland, "Enable Dynamic Island HUD", g_Config.m_ClTvIsland, &Item))
		g_Config.m_ClTvIsland ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvIslandGlow, "Enable Orange-Yellow Border Glow", g_Config.m_ClTvIslandGlow, &Item))
		g_Config.m_ClTvIslandGlow ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvIslandScale, &g_Config.m_ClTvIslandScale, &Item, "HUD Scale (%)", 50, 200);

	Left.HSplitTop(20.0f, &Item, &Left);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvIslandX, &g_Config.m_ClTvIslandX, &Item, "Horizontal X Offset", -500, 500);

	Left.HSplitTop(20.0f, &Item, &Left);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvIslandY, &g_Config.m_ClTvIslandY, &Item, "Vertical Y Offset", 0, 300);

	Left.HSplitTop(20.0f, &Item, &Left);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvIslandAlpha, &g_Config.m_ClTvIslandAlpha, &Item, "Glassmorphism Opacity (%)", 0, 100);

	Left.HSplitTop(20.0f, &Item, &Left);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvIslandTimeMode, &g_Config.m_ClTvIslandTimeMode, &Item, "Time Display (0: Clock, 1: Session, 2: Race)", 0, 2);

	// ==================== RIGHT: Freeze & Off-Screen HUD ====================
	Right.HSplitTop(24.0f, &Label, &Right);
	pMenus->TextRender()->TextColor(TeeVisuals::COLOR_YELLOW.r, TeeVisuals::COLOR_YELLOW.g, TeeVisuals::COLOR_YELLOW.b, 1.0f);
	pMenus->Ui()->DoLabel(&Label, "Freeze & Off-Screen HUD", 15.0f, TEXTALIGN_ML);
	pMenus->TextRender()->TextColor(pMenus->TextRender()->DefaultTextColor());

	Right.HSplitTop(20.0f, &Item, &Right);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvFreezeIndicator, "Show Freeze Indicators Above Players", g_Config.m_ClTvFreezeIndicator, &Item))
		g_Config.m_ClTvFreezeIndicator ^= 1;

	Right.HSplitTop(20.0f, &Item, &Right);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvFreezeSelf, "Show Local Player Frozen Banner", g_Config.m_ClTvFreezeSelf, &Item))
		g_Config.m_ClTvFreezeSelf ^= 1;

	Right.HSplitTop(8.0f, nullptr, &Right);

	Right.HSplitTop(20.0f, &Item, &Right);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvOffscreen, "Enable Off-Screen Edge Radar", g_Config.m_ClTvOffscreen, &Item))
		g_Config.m_ClTvOffscreen ^= 1;

	Right.HSplitTop(20.0f, &Item, &Right);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvOffscreenStyle, &g_Config.m_ClTvOffscreenStyle, &Item, "Radar Style (0: Circular Arrows, 1: Mini-Skins)", 0, 1);

	Right.HSplitTop(20.0f, &Item, &Right);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvOffscreenFilter, &g_Config.m_ClTvOffscreenFilter, &Item, "Filter (0: All, 1: Teammates, 2: Team, 3: War, 4: Help)", 0, 4);

	Right.HSplitTop(20.0f, &Item, &Right);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvOffscreenMargin, &g_Config.m_ClTvOffscreenMargin, &Item, "Screen Edge Margin", 5, 60);

	Right.HSplitTop(20.0f, &Item, &Right);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvOffscreenNames, "Show Player Names on Radar Marker", g_Config.m_ClTvOffscreenNames, &Item))
		g_Config.m_ClTvOffscreenNames ^= 1;

	Right.HSplitTop(20.0f, &Item, &Right);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvOffscreenDist, "Show Distance on Radar Marker", g_Config.m_ClTvOffscreenDist, &Item))
		g_Config.m_ClTvOffscreenDist ^= 1;

	Right.HSplitTop(20.0f, &Item, &Right);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvOffscreenRange, &g_Config.m_ClTvOffscreenRange, &Item, "Max Radar Tracking Range", 500, 10000);
}

void CMenusTeeVisuals::RenderSubTabVisuals(CMenus *pMenus, CUIRect View)
{
	CUIRect Left, Right, Item, Label;
	View.VSplitMid(&Left, &Right, 16.0f);

	// ==================== LEFT: Rainbow Mode ====================
	Left.HSplitTop(24.0f, &Label, &Left);
	pMenus->TextRender()->TextColor(TeeVisuals::COLOR_ORANGE.r, TeeVisuals::COLOR_ORANGE.g, TeeVisuals::COLOR_ORANGE.b, 1.0f);
	pMenus->Ui()->DoLabel(&Label, "HSV Rainbow Engine", 15.0f, TEXTALIGN_ML);
	pMenus->TextRender()->TextColor(pMenus->TextRender()->DefaultTextColor());

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvRainbow, "Master Rainbow Switch", g_Config.m_ClTvRainbow, &Item))
		g_Config.m_ClTvRainbow ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvRainbowBody, "Rainbow Tee Body", g_Config.m_ClTvRainbowBody, &Item))
		g_Config.m_ClTvRainbowBody ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvRainbowFeet, "Rainbow Tee Feet", g_Config.m_ClTvRainbowFeet, &Item))
		g_Config.m_ClTvRainbowFeet ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvRainbowWeapon, "Rainbow Active Weapon", g_Config.m_ClTvRainbowWeapon, &Item))
		g_Config.m_ClTvRainbowWeapon ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvRainbowHook, "Rainbow Hook Line & Hand", g_Config.m_ClTvRainbowHook, &Item))
		g_Config.m_ClTvRainbowHook ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvRainbowSpeed, &g_Config.m_ClTvRainbowSpeed, &Item, "Rainbow Cycling Speed", 1, 200);

	Left.HSplitTop(20.0f, &Item, &Left);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvRainbowSat, &g_Config.m_ClTvRainbowSat, &Item, "Rainbow Saturation (%)", 0, 100);

	Left.HSplitTop(20.0f, &Item, &Left);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvRainbowVal, &g_Config.m_ClTvRainbowVal, &Item, "Rainbow Brightness (%)", 0, 100);

	// ==================== RIGHT: Player Trails & Custom UI ====================
	Right.HSplitTop(24.0f, &Label, &Right);
	pMenus->TextRender()->TextColor(TeeVisuals::COLOR_YELLOW.r, TeeVisuals::COLOR_YELLOW.g, TeeVisuals::COLOR_YELLOW.b, 1.0f);
	pMenus->Ui()->DoLabel(&Label, "Player Trails & UI Customization", 15.0f, TEXTALIGN_ML);
	pMenus->TextRender()->TextColor(pMenus->TextRender()->DefaultTextColor());

	Right.HSplitTop(20.0f, &Item, &Right);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvTrails, &g_Config.m_ClTvTrails, &Item, "Player Trails (0: Off, 1: Self, 2: Team, 3: All)", 0, 3);

	Right.HSplitTop(20.0f, &Item, &Right);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvTrailMode, &g_Config.m_ClTvTrailMode, &Item, "Color Mode (0: Solid, 1: Orange Gradient, 2: Rainbow)", 0, 2);

	Right.HSplitTop(20.0f, &Item, &Right);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvTrailLength, &g_Config.m_ClTvTrailLength, &Item, "Trail Length (Points)", 5, 100);

	Right.HSplitTop(20.0f, &Item, &Right);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvTrailWidth, &g_Config.m_ClTvTrailWidth, &Item, "Trail Thickness", 2, 32);

	Right.HSplitTop(10.0f, nullptr, &Right);

	// Custom Fonts controls
	Right.HSplitTop(20.0f, &Item, &Right);
	char aFontLabel[128];
	str_format(aFontLabel, sizeof(aFontLabel), "Custom Font: %s", g_Config.m_ClTvCustomFont[0] ? g_Config.m_ClTvCustomFont : "(Default)");
	pMenus->Ui()->DoLabel(&Item, aFontLabel, 11.0f, TEXTALIGN_ML);

	Right.HSplitTop(22.0f, &Item, &Right);
	CUIRect BtnFontReset, BtnFontCycle;
	Item.VSplitMid(&BtnFontReset, &BtnFontCycle, 6.0f);

	static CButtonContainer s_BtnFontReset;
	if(pMenus->DoButton_Menu(&s_BtnFontReset, "Reset Font", 0, &BtnFontReset))
	{
		pMenus->GameClient()->m_TeeVisuals.m_UiCustom.ResetCustomFont();
	}

	static CButtonContainer s_BtnFontCycle;
	if(pMenus->DoButton_Menu(&s_BtnFontCycle, "Next Font", 0, &BtnFontCycle))
	{
		const auto &vFonts = pMenus->GameClient()->m_TeeVisuals.m_UiCustom.GetAvailableFonts();
		if(!vFonts.empty())
		{
			static size_t s_FontIdx = 0;
			s_FontIdx = (s_FontIdx + 1) % vFonts.size();
			str_copy(g_Config.m_ClTvCustomFont, vFonts[s_FontIdx].c_str());
			pMenus->GameClient()->m_TeeVisuals.m_UiCustom.LoadCustomFont();
		}
	}

	Right.HSplitTop(10.0f, nullptr, &Right);

	// Custom Background controls
	Right.HSplitTop(20.0f, &Item, &Right);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvMenuBgDim, &g_Config.m_ClTvMenuBgDim, &Item, "Menu Wallpaper Dimming (%)", 0, 100);

	Right.HSplitTop(22.0f, &Item, &Right);
	CUIRect BtnBgReset, BtnBgCycle;
	Item.VSplitMid(&BtnBgReset, &BtnBgCycle, 6.0f);

	static CButtonContainer s_BtnBgReset;
	if(pMenus->DoButton_Menu(&s_BtnBgReset, "Reset Wallpaper", 0, &BtnBgReset))
	{
		pMenus->GameClient()->m_TeeVisuals.m_UiCustom.ResetCustomBackground();
	}

	static CButtonContainer s_BtnBgCycle;
	if(pMenus->DoButton_Menu(&s_BtnBgCycle, "Next Wallpaper", 0, &BtnBgCycle))
	{
		const auto &vBgs = pMenus->GameClient()->m_TeeVisuals.m_UiCustom.GetAvailableBackgrounds();
		if(!vBgs.empty())
		{
			static size_t s_BgIdx = 0;
			s_BgIdx = (s_BgIdx + 1) % vBgs.size();
			str_copy(g_Config.m_ClTvMenuBg, vBgs[s_BgIdx].c_str());
			pMenus->GameClient()->m_TeeVisuals.m_UiCustom.LoadCustomBackground();
		}
	}
}

void CMenusTeeVisuals::RenderSubTabSocialLists(CMenus *pMenus, CUIRect View)
{
	CUIRect Left, Right, Item, Label;
	View.VSplitMid(&Left, &Right, 16.0f);

	// ==================== LEFT: Quick Add & Config ====================
	Left.HSplitTop(24.0f, &Label, &Left);
	pMenus->TextRender()->TextColor(TeeVisuals::COLOR_ORANGE.r, TeeVisuals::COLOR_ORANGE.g, TeeVisuals::COLOR_ORANGE.b, 1.0f);
	pMenus->Ui()->DoLabel(&Label, "Social Lists Configuration", 15.0f, TEXTALIGN_ML);
	pMenus->TextRender()->TextColor(pMenus->TextRender()->DefaultTextColor());

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvPlayerLists, "Enable War / Team / Helper Lists", g_Config.m_ClTvPlayerLists, &Item))
		g_Config.m_ClTvPlayerLists ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvListsWorld, "Highlight in In-Game World Nameplates", g_Config.m_ClTvListsWorld, &Item))
		g_Config.m_ClTvListsWorld ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvListsScoreboard, "Highlight in Scoreboard", g_Config.m_ClTvListsScoreboard, &Item))
		g_Config.m_ClTvListsScoreboard ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvListsTag, "Show Category Tag [WAR] / [TEAM] / [HELPER]", g_Config.m_ClTvListsTag, &Item))
		g_Config.m_ClTvListsTag ^= 1;

	Left.HSplitTop(10.0f, nullptr, &Left);

	// Quick Add Form
	static CLineInputBuffered<64> s_InputName;
	static CLineInputBuffered<128> s_InputReason;
	static int s_SelectedCategory = CATEGORY_WAR;

	Left.HSplitTop(16.0f, &Item, &Left);
	pMenus->Ui()->DoLabel(&Item, "Nickname or Clan Name:", 10.5f, TEXTALIGN_ML);
	Left.HSplitTop(18.0f, &Item, &Left);
	pMenus->Ui()->DoEditBox(&s_InputName, &Item, 10.0f);

	Left.HSplitTop(16.0f, &Item, &Left);
	pMenus->Ui()->DoLabel(&Item, "Reason / Note:", 10.5f, TEXTALIGN_ML);
	Left.HSplitTop(18.0f, &Item, &Left);
	pMenus->Ui()->DoEditBox(&s_InputReason, &Item, 10.0f);

	Left.HSplitTop(6.0f, nullptr, &Left);

	// Category selector
	Left.HSplitTop(20.0f, &Item, &Left);
	CUIRect BtnWar, BtnTeam, BtnHelp;
	Item.VSplit3(&BtnWar, &BtnTeam, &BtnHelp);

	static CButtonContainer s_BtnCatWar, s_BtnCatTeam, s_BtnCatHelp;
	if(pMenus->DoButton_Menu(&s_BtnCatWar, "War", s_SelectedCategory == CATEGORY_WAR, &BtnWar))
		s_SelectedCategory = CATEGORY_WAR;
	if(pMenus->DoButton_Menu(&s_BtnCatTeam, "Team", s_SelectedCategory == CATEGORY_TEAM, &BtnTeam))
		s_SelectedCategory = CATEGORY_TEAM;
	if(pMenus->DoButton_Menu(&s_BtnCatHelp, "Helper", s_SelectedCategory == CATEGORY_HELPER, &BtnHelp))
		s_SelectedCategory = CATEGORY_HELPER;

	Left.HSplitTop(8.0f, nullptr, &Left);

	Left.HSplitTop(22.0f, &Item, &Left);
	CUIRect BtnAddPlayer, BtnAddClan;
	Item.VSplitMid(&BtnAddPlayer, &BtnAddClan, 6.0f);

	static CButtonContainer s_BtnAddPlayer, s_BtnAddClan;
	if(pMenus->DoButton_Menu(&s_BtnAddPlayer, "Add Player", 0, &BtnAddPlayer))
	{
		if(s_InputName.GetString()[0])
		{
			pMenus->GameClient()->m_TeeVisuals.m_PlayerLists.AddPlayer(s_InputName.GetString(), s_InputReason.GetString(), (EPlayerListCategory)s_SelectedCategory);
			s_InputName.Clear();
			s_InputReason.Clear();
		}
	}

	if(pMenus->DoButton_Menu(&s_BtnAddClan, "Add Clan", 0, &BtnAddClan))
	{
		if(s_InputName.GetString()[0])
		{
			pMenus->GameClient()->m_TeeVisuals.m_PlayerLists.AddClan(s_InputName.GetString(), (EPlayerListCategory)s_SelectedCategory);
			s_InputName.Clear();
			s_InputReason.Clear();
		}
	}

	// ==================== RIGHT: Active Entries List ====================
	Right.HSplitTop(24.0f, &Label, &Right);
	pMenus->TextRender()->TextColor(TeeVisuals::COLOR_YELLOW.r, TeeVisuals::COLOR_YELLOW.g, TeeVisuals::COLOR_YELLOW.b, 1.0f);
	pMenus->Ui()->DoLabel(&Label, "Active Entries (teevisuals_lists.json)", 15.0f, TEXTALIGN_ML);
	pMenus->TextRender()->TextColor(pMenus->TextRender()->DefaultTextColor());

	const auto &vPlayers = pMenus->GameClient()->m_TeeVisuals.m_PlayerLists.GetPlayers();
	const auto &vClans = pMenus->GameClient()->m_TeeVisuals.m_PlayerLists.GetClans();

	static CScrollRegion s_ScrollRegion;
	s_ScrollRegion.Begin(&Right);

	char aRowBuf[256];
	int EntryIndex = 0;

	for(const auto &P : vPlayers)
	{
		CUIRect Row, DelBtn;
		Right.HSplitTop(18.0f, &Row, &Right);
		if(!s_ScrollRegion.AddRect(Row))
			continue;

		Row.VSplitRight(24.0f, &Row, &DelBtn);

		const char *pCatTag = P.m_Category == CATEGORY_WAR ? "[WAR]" : (P.m_Category == CATEGORY_TEAM ? "[TEAM]" : "[HELP]");
		if(P.m_aReason[0])
			str_format(aRowBuf, sizeof(aRowBuf), "%s %s (%s)", pCatTag, P.m_aName, P.m_aReason);
		else
			str_format(aRowBuf, sizeof(aRowBuf), "%s %s", pCatTag, P.m_aName);

		ColorRGBA CatCol = (P.m_Category == CATEGORY_WAR) ? TeeVisuals::COLOR_WAR : ((P.m_Category == CATEGORY_TEAM) ? TeeVisuals::COLOR_TEAM : TeeVisuals::COLOR_HELPER);
		pMenus->TextRender()->TextColor(CatCol.r, CatCol.g, CatCol.b, 1.0f);
		pMenus->Ui()->DoLabel(&Row, aRowBuf, 10.0f, TEXTALIGN_ML);
		pMenus->TextRender()->TextColor(pMenus->TextRender()->DefaultTextColor());

		static CButtonContainer s_aDelButtons[128];
		if(EntryIndex < 128 && pMenus->DoButton_Menu(&s_aDelButtons[EntryIndex], "X", 0, &DelBtn))
		{
			pMenus->GameClient()->m_TeeVisuals.m_PlayerLists.RemovePlayer(P.m_aName, P.m_Category);
			break;
		}
		EntryIndex++;
	}

	for(const auto &C : vClans)
	{
		CUIRect Row, DelBtn;
		Right.HSplitTop(18.0f, &Row, &Right);
		if(!s_ScrollRegion.AddRect(Row))
			continue;

		Row.VSplitRight(24.0f, &Row, &DelBtn);

		const char *pCatTag = C.m_Category == CATEGORY_WAR ? "[WAR CLAN]" : "[TEAM CLAN]";
		str_format(aRowBuf, sizeof(aRowBuf), "%s %s", pCatTag, C.m_aClan);

		ColorRGBA CatCol = (C.m_Category == CATEGORY_WAR) ? TeeVisuals::COLOR_WAR : TeeVisuals::COLOR_TEAM;
		pMenus->TextRender()->TextColor(CatCol.r, CatCol.g, CatCol.b, 1.0f);
		pMenus->Ui()->DoLabel(&Row, aRowBuf, 10.0f, TEXTALIGN_ML);
		pMenus->TextRender()->TextColor(pMenus->TextRender()->DefaultTextColor());

		static CButtonContainer s_aDelClanButtons[64];
		if(EntryIndex < 64 && pMenus->DoButton_Menu(&s_aDelClanButtons[EntryIndex], "X", 0, &DelBtn))
		{
			pMenus->GameClient()->m_TeeVisuals.m_PlayerLists.RemoveClan(C.m_aClan, C.m_Category);
			break;
		}
		EntryIndex++;
	}

	s_ScrollRegion.End();
}

void CMenusTeeVisuals::RenderSubTabBindwheelMobile(CMenus *pMenus, CUIRect View)
{
	CUIRect Left, Right, Item, Label;
	View.VSplitMid(&Left, &Right, 16.0f);

	// ==================== LEFT: Smooth Bindwheel ====================
	Left.HSplitTop(24.0f, &Label, &Left);
	pMenus->TextRender()->TextColor(TeeVisuals::COLOR_ORANGE.r, TeeVisuals::COLOR_ORANGE.g, TeeVisuals::COLOR_ORANGE.b, 1.0f);
	pMenus->Ui()->DoLabel(&Label, "Smooth Bindwheel (+tv_bindwheel)", 15.0f, TEXTALIGN_ML);
	pMenus->TextRender()->TextColor(pMenus->TextRender()->DefaultTextColor());

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvSmoothBindwheel, "Enable Smooth Bindwheel", g_Config.m_ClTvSmoothBindwheel, &Item))
		g_Config.m_ClTvSmoothBindwheel ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvBindwheelSlots, &g_Config.m_ClTvBindwheelSlots, &Item, "Number of Slots (4-12)", 4, 12);

	Left.HSplitTop(20.0f, &Item, &Left);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvBindwheelRadius, &g_Config.m_ClTvBindwheelRadius, &Item, "Radial Menu Radius", 60, 300);

	Left.HSplitTop(20.0f, &Item, &Left);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvBindwheelSpeed, &g_Config.m_ClTvBindwheelSpeed, &Item, "Animation Speed", 30, 250);

	Left.HSplitTop(10.0f, nullptr, &Left);

	// Slots list overview
	auto &vSlots = pMenus->GameClient()->m_TeeVisuals.m_Bindwheel.GetSlots();
	const int AllowedSlots = std::clamp(g_Config.m_ClTvBindwheelSlots, 4, 12);
	for(int s = 0; s < std::min((int)vSlots.size(), AllowedSlots); s++)
	{
		Left.HSplitTop(17.0f, &Item, &Left);
		char aSlotBuf[128];
		str_format(aSlotBuf, sizeof(aSlotBuf), "Slot #%d: '%s' -> %s", s + 1, vSlots[s].m_aTitle, vSlots[s].m_aCommand);
		pMenus->Ui()->DoLabel(&Item, aSlotBuf, 10.0f, TEXTALIGN_ML);
	}

	// ==================== RIGHT: Mobile & Touch Controls ====================
	Right.HSplitTop(24.0f, &Label, &Right);
	pMenus->TextRender()->TextColor(TeeVisuals::COLOR_YELLOW.r, TeeVisuals::COLOR_YELLOW.g, TeeVisuals::COLOR_YELLOW.b, 1.0f);
	pMenus->Ui()->DoLabel(&Label, "Mobile & Touch Controls", 15.0f, TEXTALIGN_ML);
	pMenus->TextRender()->TextColor(pMenus->TextRender()->DefaultTextColor());

	Right.HSplitTop(20.0f, &Item, &Right);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTouchControls, "Enable On-Screen Touch Controls", g_Config.m_ClTouchControls, &Item))
		g_Config.m_ClTouchControls ^= 1;

	Right.HSplitTop(20.0f, &Item, &Right);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClTvMobileSafeArea, "Force Safe Area Margins on Mobile", g_Config.m_ClTvMobileSafeArea, &Item))
		g_Config.m_ClTvMobileSafeArea ^= 1;

	Right.HSplitTop(20.0f, &Item, &Right);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvMobileTouchScale, &g_Config.m_ClTvMobileTouchScale, &Item, "Touch Button Scale (%)", 50, 200);

	Right.HSplitTop(20.0f, &Item, &Right);
	pMenus->Ui()->DoScrollbarOption(&g_Config.m_ClTvMobileJoystickAlpha, &g_Config.m_ClTvMobileJoystickAlpha, &Item, "Joystick Opacity (%)", 10, 100);
}

void CMenusTeeVisuals::RenderSubTabPortedFeatures(CMenus *pMenus, CUIRect View)
{
	CUIRect Left, Right, Item, Label;
	View.VSplitMid(&Left, &Right, 16.0f);

	// ==================== LEFT: Ported QoL Features ====================
	Left.HSplitTop(24.0f, &Label, &Left);
	pMenus->TextRender()->TextColor(TeeVisuals::COLOR_ORANGE.r, TeeVisuals::COLOR_ORANGE.g, TeeVisuals::COLOR_ORANGE.b, 1.0f);
	pMenus->Ui()->DoLabel(&Label, "Ported Community & QoL Tools", 15.0f, TEXTALIGN_ML);
	pMenus->TextRender()->TextColor(pMenus->TextRender()->DefaultTextColor());

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClAutoDummyConnect, "Auto Dummy Connect on Launch", g_Config.m_ClAutoDummyConnect, &Item))
		g_Config.m_ClAutoDummyConnect ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClClientUsersScoreboard, "Show Client Users in Scoreboard", g_Config.m_ClClientUsersScoreboard, &Item))
		g_Config.m_ClClientUsersScoreboard ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClSpecPauseRadio, "Spectator Pause Radio Menu", g_Config.m_ClSpecPauseRadio, &Item))
		g_Config.m_ClSpecPauseRadio ^= 1;

	Left.HSplitTop(20.0f, &Item, &Left);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClLocalPractice, "Local Solo Practice World", g_Config.m_ClLocalPractice, &Item))
		g_Config.m_ClLocalPractice ^= 1;

	// ==================== RIGHT: Visual & Server Enhancements ====================
	Right.HSplitTop(24.0f, &Label, &Right);
	pMenus->TextRender()->TextColor(TeeVisuals::COLOR_YELLOW.r, TeeVisuals::COLOR_YELLOW.g, TeeVisuals::COLOR_YELLOW.b, 1.0f);
	pMenus->Ui()->DoLabel(&Label, "Visual & Server Enhancements", 15.0f, TEXTALIGN_ML);
	pMenus->TextRender()->TextColor(pMenus->TextRender()->DefaultTextColor());

	Right.HSplitTop(20.0f, &Item, &Right);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClPhysicBalls, "Physical Ball Simulations (Media)", g_Config.m_ClPhysicBalls, &Item))
		g_Config.m_ClPhysicBalls ^= 1;

	Right.HSplitTop(20.0f, &Item, &Right);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClMovingTiles, "Animated Moving Tiles Background", g_Config.m_ClMovingTiles, &Item))
		g_Config.m_ClMovingTiles ^= 1;

	Right.HSplitTop(20.0f, &Item, &Right);
	if(pMenus->DoButton_CheckBox(&g_Config.m_ClChatBubbles, "Player Chat Bubbles Above Tees", g_Config.m_ClChatBubbles, &Item))
		g_Config.m_ClChatBubbles ^= 1;
}

void CMenus::RenderSettingsTeeVisuals(CUIRect MainView)
{
	CMenusTeeVisuals::RenderSettings(this, MainView);
}
