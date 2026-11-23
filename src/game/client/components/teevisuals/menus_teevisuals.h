#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_MENUS_TEEVISUALS_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_MENUS_TEEVISUALS_H

#include <game/client/ui.h>

class CMenus;

class CMenusTeeVisuals
{
public:
	enum ESubTab
	{
		SUBTAB_HUD_ISLAND = 0,
		SUBTAB_VISUALS,
		SUBTAB_SOCIAL_LISTS,
		SUBTAB_BINDWHEEL_MOBILE,
		SUBTAB_PORTED_FEATURES,
		NUM_SUBTABS
	};

	static void RenderSettings(CMenus *pMenus, CUIRect MainView);

private:
	static void RenderSubTabHudIsland(CMenus *pMenus, CUIRect View);
	static void RenderSubTabVisuals(CMenus *pMenus, CUIRect View);
	static void RenderSubTabSocialLists(CMenus *pMenus, CUIRect View);
	static void RenderSubTabBindwheelMobile(CMenus *pMenus, CUIRect View);
	static void RenderSubTabPortedFeatures(CMenus *pMenus, CUIRect View);
};

#endif
