#include "teevisuals.h"

#include <engine/client.h>
#include <engine/shared/config.h>
#include <game/client/gameclient.h>

CTeeVisuals::CTeeVisuals()
{
}

void CTeeVisuals::OnInterfacesInit(CGameClient *pClient)
{
	CComponentInterfaces::OnInterfacesInit(pClient);

	m_DynamicIsland.OnInterfacesInit(pClient);
	m_Rainbow.OnInterfacesInit(pClient);
	m_Trails.OnInterfacesInit(pClient);
	m_FreezeOffscreen.OnInterfacesInit(pClient);
	m_PlayerLists.OnInterfacesInit(pClient);
	m_Bindwheel.OnInterfacesInit(pClient);
	m_UiCustom.OnInterfacesInit(pClient);
	m_MobileUx.OnInterfacesInit(pClient);
}

void CTeeVisuals::OnInit()
{
	m_DynamicIsland.OnInit();
	m_Rainbow.OnInit();
	m_Trails.OnInit();
	m_FreezeOffscreen.OnInit();
	m_PlayerLists.OnInit();
	m_Bindwheel.OnInit();
	m_UiCustom.OnInit();
	m_MobileUx.OnInit();
}

void CTeeVisuals::OnReset()
{
	m_DynamicIsland.OnReset();
	m_Rainbow.OnReset();
	m_Trails.OnReset();
	m_FreezeOffscreen.OnReset();
	m_PlayerLists.OnReset();
	m_Bindwheel.OnReset();
	m_UiCustom.OnReset();
	m_MobileUx.OnReset();
}

void CTeeVisuals::OnConsoleInit()
{
	m_DynamicIsland.OnConsoleInit();
	m_Rainbow.OnConsoleInit();
	m_Trails.OnConsoleInit();
	m_FreezeOffscreen.OnConsoleInit();
	m_PlayerLists.OnConsoleInit();
	m_Bindwheel.OnConsoleInit();
	m_UiCustom.OnConsoleInit();
	m_MobileUx.OnConsoleInit();
}

void CTeeVisuals::OnRender()
{
	m_UiCustom.OnRender();
	m_Rainbow.OnRender();
	m_Trails.OnRender();
	m_FreezeOffscreen.OnRender();
	m_DynamicIsland.OnRender();
	m_MobileUx.OnRender();
	m_Bindwheel.OnRender();
}

bool CTeeVisuals::OnChatCommand(const char *pLine)
{
	return m_PlayerLists.OnChatCommand(pLine);
}

int CTeeVisuals::GetPlayerListType(int ClientId) const
{
	if(ClientId < 0 || ClientId >= MAX_CLIENTS)
		return 0;

	const char *pName = GameClient()->m_aClients[ClientId].m_aName;
	const char *pClan = GameClient()->m_aClients[ClientId].m_aClan;

	return (int)m_PlayerLists.GetPlayerCategory(pName, pClan);
}

ColorRGBA CTeeVisuals::GetPlayerListColor(int ClientId, ColorRGBA DefaultColor) const
{
	if(ClientId < 0 || ClientId >= MAX_CLIENTS)
		return DefaultColor;

	const char *pName = GameClient()->m_aClients[ClientId].m_aName;
	const char *pClan = GameClient()->m_aClients[ClientId].m_aClan;

	EPlayerListCategory Cat = m_PlayerLists.GetPlayerCategory(pName, pClan);
	return m_PlayerLists.GetCategoryColor(Cat, DefaultColor);
}

const char *CTeeVisuals::GetPlayerListTag(int ClientId) const
{
	if(ClientId < 0 || ClientId >= MAX_CLIENTS)
		return "";

	const char *pName = GameClient()->m_aClients[ClientId].m_aName;
	const char *pClan = GameClient()->m_aClients[ClientId].m_aClan;

	EPlayerListCategory Cat = m_PlayerLists.GetPlayerCategory(pName, pClan);
	return m_PlayerLists.GetCategoryTag(Cat);
}
