#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_TEEVISUALS_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_TEEVISUALS_H

#include <base/color.h>
#include <game/client/component.h>

#include "bindwheel.h"
#include "dynamic_island.h"
#include "freeze_offscreen.h"
#include "mobile_ux.h"
#include "player_lists.h"
#include "rainbow.h"
#include "trails.h"
#include "ui_custom.h"

class CTeeVisuals : public CComponent
{
public:
	CDynamicIsland m_DynamicIsland;
	CRainbowEffect m_Rainbow;
	CPlayerTrails m_Trails;
	CFreezeOffscreenHud m_FreezeOffscreen;
	CPlayerLists m_PlayerLists;
	CBindwheel m_Bindwheel;
	CUiCustom m_UiCustom;
	CMobileUx m_MobileUx;

	CTeeVisuals();
	int Sizeof() const override { return sizeof(*this); }

	void OnInterfacesInit(CGameClient *pClient) override;
	void OnInit() override;
	void OnReset() override;
	void OnRender() override;
	void OnConsoleInit() override;

	bool OnChatCommand(const char *pLine);

	int GetPlayerListType(int ClientId) const;
	ColorRGBA GetPlayerListColor(int ClientId, ColorRGBA DefaultColor) const;
	const char *GetPlayerListTag(int ClientId) const;
};

#endif
