#include "freeze_offscreen.h"

#include <base/color.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>
#include <game/client/gameclient.h>

CFreezeOffscreenHud::CFreezeOffscreenHud()
{
}

void CFreezeOffscreenHud::RenderFreezeIndicators()
{
	if(!g_Config.m_ClTvFreezeIndicator)
		return;

	const CCamera *pCam = &m_pClient->m_Camera;
	Graphics()->MapScreenToInterface(pCam->m_Center.x, pCam->m_Center.y);

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(!m_pClient->m_Snap.m_aCharacters[i].m_Active)
			continue;

		const auto &Char = m_pClient->m_Snap.m_aCharacters[i];
		bool IsFrozen = Char.m_Cur.m_IsInFreeze || (Char.m_Cur.m_FreezeEnd > 0) || m_pClient->m_aClients[i].m_InsideQuadFreeze;

		if(!IsFrozen)
			continue;

		const vec2 Pos = m_pClient->m_aClients[i].m_RenderPos;

		// Draw freeze badge above player
		const float BadgeW = 44.0f;
		const float BadgeH = 12.0f;
		const float X = Pos.x - BadgeW * 0.5f;
		const float Y = Pos.y - 56.0f;

		// Frosted cyan/blue background
		ColorRGBA IceBg(0.1f, 0.45f, 0.85f, 0.75f);
		ColorRGBA IceBorder(0.4f, 0.85f, 1.0f, 0.95f);
		Graphics()->DrawRect(X, Y, BadgeW, BadgeH, IceBg, IGraphics::CORNER_ALL, 6.0f);
		Graphics()->DrawRectOutline(X, Y, BadgeW, BadgeH, IceBorder, IGraphics::CORNER_ALL, IGraphics::SIDE_ALL, 6.0f);

		// Freeze Text
		TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
		const char *pText = "FROZEN";
		float TextW = TextRender()->TextWidth(7.0f, pText, -1, -1.0f);
		TextRender()->Text(Pos.x - TextW * 0.5f, Y + 2.5f, 7.0f, pText, -1.0f);
		TextRender()->TextColor(TextRender()->DefaultTextColor());
	}
}

void CFreezeOffscreenHud::RenderOffscreenRadar()
{
	if(!g_Config.m_ClTvOffscreenHud)
		return;

	const float ScreenWidth = 300.0f * Graphics()->ScreenAspect();
	const float ScreenHeight = 300.0f;
	Graphics()->MapScreenToSize(ScreenWidth, ScreenHeight);

	const vec2 ScreenCenter(ScreenWidth * 0.5f, ScreenHeight * 0.5f);
	const vec2 CamCenter = m_pClient->m_Camera.m_Center;
	const float Zoom = std::max(0.05f, m_pClient->m_Camera.m_Zoom);

	const float HalfWorldH = 150.0f * Zoom;
	const float HalfWorldW = HalfWorldH * Graphics()->ScreenAspect();

	const float Margin = 20.0f;
	const float MaxRange = (float)g_Config.m_ClTvOffscreenRange;
	const int LocalId = m_pClient->m_Snap.m_LocalClientId;

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(i == LocalId || !m_pClient->m_Snap.m_aCharacters[i].m_Active)
			continue;

		const vec2 TargetPos = m_pClient->m_aClients[i].m_RenderPos;
		const vec2 Rel = TargetPos - CamCenter;
		const float Dist = length(Rel);

		if(Dist > MaxRange)
			continue;

		// Check if inside screen
		if(std::abs(Rel.x) <= HalfWorldW && std::abs(Rel.y) <= HalfWorldH)
			continue; // on screen, no off-screen marker needed

		// Social category check
		int ListType = m_pClient->m_TeeVisuals.GetPlayerListType(i);
		bool IsTeammate = !m_pClient->IsOtherTeam(i);

		if(!g_Config.m_ClTvOffscreenTeammates && !g_Config.m_ClTvOffscreenWar)
			continue;
		if(!g_Config.m_ClTvOffscreenTeammates && IsTeammate && ListType == 0)
			continue;
		if(!g_Config.m_ClTvOffscreenWar && ListType != 0)
			continue;

		// Determine marker color
		ColorRGBA MarkerCol;
		const char *pPrefix = "";
		if(ListType == 1) // War
		{
			MarkerCol = ColorRGBA(1.0f, 0.25f, 0.25f, 0.9f); // Red
			pPrefix = "[WAR] ";
		}
		else if(ListType == 2) // Team
		{
			MarkerCol = ColorRGBA(0.0f, 0.9f, 1.0f, 0.9f); // Cyan
			pPrefix = "[TEAM] ";
		}
		else if(ListType == 3) // Helper
		{
			MarkerCol = ColorRGBA(1.0f, 0.8f, 0.1f, 0.9f); // Gold
			pPrefix = "[HELP] ";
		}
		else // Normal teammate
		{
			MarkerCol = ColorRGBA(0.2f, 1.0f, 0.45f, 0.85f); // Green
		}

		// Calculate screen intersection point
		const vec2 ScreenHalf(ScreenWidth * 0.5f - Margin, ScreenHeight * 0.5f - Margin);
		const vec2 ScreenDir = normalize(vec2(Rel.x, Rel.y));

		float ScaleX = ScreenDir.x != 0.0f ? ScreenHalf.x / std::abs(ScreenDir.x) : 1e9f;
		float ScaleY = ScreenDir.y != 0.0f ? ScreenHalf.y / std::abs(ScreenDir.y) : 1e9f;
		float MinScale = std::min(ScaleX, ScaleY);

		vec2 MarkerPos = ScreenCenter + ScreenDir * MinScale;

		// Distance in tiles (32 units = 1 tile / meter)
		int DistMeters = (int)(Dist / 32.0f);
		char aDistBuf[32];
		str_format(aDistBuf, sizeof(aDistBuf), "%s%s (%dm)", pPrefix, m_pClient->m_aClients[i].m_aName, DistMeters);

		const float FontSize = 5.0f;
		const float TextW = TextRender()->TextWidth(FontSize, aDistBuf, -1, -1.0f);
		const float PillW = TextW + 12.0f;
		const float PillH = 11.0f;

		// Clamp marker within screen bounds
		MarkerPos.x = std::clamp(MarkerPos.x, PillW * 0.5f + 2.0f, ScreenWidth - PillW * 0.5f - 2.0f);
		MarkerPos.y = std::clamp(MarkerPos.y, PillH * 0.5f + 2.0f, ScreenHeight - PillH * 0.5f - 2.0f);

		// Render Pill Marker
		ColorRGBA DarkBg(0.08f, 0.08f, 0.10f, 0.85f);
		Graphics()->DrawRect(MarkerPos.x - PillW * 0.5f, MarkerPos.y - PillH * 0.5f, PillW, PillH, DarkBg, IGraphics::CORNER_ALL, PillH * 0.5f);
		Graphics()->DrawRectOutline(MarkerPos.x - PillW * 0.5f, MarkerPos.y - PillH * 0.5f, PillW, PillH, MarkerCol, IGraphics::CORNER_ALL, IGraphics::SIDE_ALL, PillH * 0.5f);

		// Render Text
		TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
		TextRender()->Text(MarkerPos.x - TextW * 0.5f, MarkerPos.y - FontSize * 0.5f, FontSize, aDistBuf, -1.0f);
		TextRender()->TextColor(TextRender()->DefaultTextColor());
	}
}

void CFreezeOffscreenHud::OnRender()
{
	if(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		return;

	RenderFreezeIndicators();
	RenderOffscreenRadar();
}
