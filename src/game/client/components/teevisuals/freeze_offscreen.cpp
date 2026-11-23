#include "freeze_offscreen.h"
#include "teevisuals_common.h"

#include <base/color.h>
#include <engine/client.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>
#include <game/client/animstate.h>
#include <game/client/gameclient.h>
#include <game/client/render.h>

#include <algorithm>
#include <cmath>

static const float PI = 3.14159265358979323846f;

CFreezeOffscreenHud::CFreezeOffscreenHud()
{
}

void CFreezeOffscreenHud::RenderFreezeIndicators()
{
	if(!g_Config.m_ClTvFreezeIndicator)
		return;

	const CCamera *pCam = &GameClient()->m_Camera;
	Graphics()->MapScreenToInterface(pCam->m_Center.x, pCam->m_Center.y);

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(!GameClient()->m_Snap.m_aCharacters[i].m_Active)
			continue;

		const auto &Char = GameClient()->m_Snap.m_aCharacters[i];
		bool IsFrozen = GameClient()->m_aClients[i].m_InsideQuadFreeze;
		float FreezeSecRemaining = -1.0f;

		if(Char.m_HasExtendedData)
		{
			if(Char.m_ExtendedData.m_FreezeEnd > 0)
			{
				IsFrozen = true;
				int TicksLeft = Char.m_ExtendedData.m_FreezeEnd - Client()->GameTick(g_Config.m_ClDummy);
				if(TicksLeft > 0)
					FreezeSecRemaining = (float)TicksLeft / (float)Client()->GameTickSpeed();
			}
			else if(Char.m_ExtendedData.m_FreezeEnd == -1)
			{
				IsFrozen = true; // Deep/infinite freeze
			}
		}

		if(GameClient()->m_aClients[i].m_RegularPredicted.m_IsInFreeze || GameClient()->m_aClients[i].m_RegularPredicted.m_FreezeEnd > 0)
		{
			IsFrozen = true;
			int TicksLeft = GameClient()->m_aClients[i].m_RegularPredicted.m_FreezeEnd - Client()->GameTick(g_Config.m_ClDummy);
			if(TicksLeft > 0)
				FreezeSecRemaining = (float)TicksLeft / (float)Client()->GameTickSpeed();
		}

		if(!IsFrozen)
			continue;

		const vec2 Pos = GameClient()->m_aClients[i].m_RenderPos;

		// Draw frosted freeze badge above player
		const float BadgeW = 46.0f;
		const float BadgeH = 13.0f;
		const float X = Pos.x - BadgeW * 0.5f;
		const float Y = Pos.y - 58.0f;

		// Frosted cyan/ice styling
		ColorRGBA IceBg(0.08f, 0.35f, 0.70f, 0.80f);
		ColorRGBA IceBorder(0.35f, 0.85f, 1.0f, 0.95f);
		Graphics()->DrawRect(X, Y, BadgeW, BadgeH, IceBg, IGraphics::CORNER_ALL, 6.0f);
		Graphics()->DrawRectOutline(X, Y, BadgeW, BadgeH, IceBorder, IGraphics::CORNER_ALL, IGraphics::SIDE_ALL, 6.0f);

		char aFreezeText[32];
		if(FreezeSecRemaining > 0.0f)
			str_format(aFreezeText, sizeof(aFreezeText), "FROZEN %.1fs", FreezeSecRemaining);
		else
			str_copy(aFreezeText, "FROZEN");

		TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
		float TextW = TextRender()->TextWidth(6.5f, aFreezeText, -1, -1.0f);
		TextRender()->Text(Pos.x - TextW * 0.5f, Y + 2.5f, 6.5f, aFreezeText, -1.0f);
		TextRender()->TextColor(TextRender()->DefaultTextColor());
	}
}

void CFreezeOffscreenHud::RenderLocalFreezeHud()
{
	if(!g_Config.m_ClTvFreezeSelf)
		return;

	const int LocalId = GameClient()->m_Snap.m_LocalClientId;
	if(LocalId < 0 || LocalId >= MAX_CLIENTS || !GameClient()->m_Snap.m_aCharacters[LocalId].m_Active)
		return;

	bool IsFrozen = GameClient()->m_aClients[LocalId].m_InsideQuadFreeze;
	const auto &Char = GameClient()->m_Snap.m_aCharacters[LocalId];
	float FreezeSecRemaining = -1.0f;

	if(Char.m_HasExtendedData && (Char.m_ExtendedData.m_FreezeEnd > 0 || Char.m_ExtendedData.m_FreezeEnd == -1))
	{
		IsFrozen = true;
		if(Char.m_ExtendedData.m_FreezeEnd > 0)
		{
			int TicksLeft = Char.m_ExtendedData.m_FreezeEnd - Client()->GameTick(g_Config.m_ClDummy);
			if(TicksLeft > 0)
				FreezeSecRemaining = (float)TicksLeft / (float)Client()->GameTickSpeed();
		}
	}
	if(GameClient()->m_aClients[LocalId].m_RegularPredicted.m_IsInFreeze || GameClient()->m_aClients[LocalId].m_RegularPredicted.m_FreezeEnd > 0)
	{
		IsFrozen = true;
		int TicksLeft = GameClient()->m_aClients[LocalId].m_RegularPredicted.m_FreezeEnd - Client()->GameTick(g_Config.m_ClDummy);
		if(TicksLeft > 0)
			FreezeSecRemaining = (float)TicksLeft / (float)Client()->GameTickSpeed();
	}

	if(!IsFrozen)
		return;

	const float ScreenWidth = 300.0f * Graphics()->ScreenAspect();
	const float ScreenHeight = 300.0f;
	Graphics()->MapScreenToSize(ScreenWidth, ScreenHeight);

	// Compact HUD banner at bottom center
	const float BannerW = 100.0f;
	const float BannerH = 20.0f;
	const float X = (ScreenWidth - BannerW) * 0.5f;
	const float Y = ScreenHeight - 55.0f;

	ColorRGBA FrostBg(0.06f, 0.25f, 0.55f, 0.85f);
	ColorRGBA FrostBorder(0.40f, 0.85f, 1.0f, 1.0f);
	Graphics()->DrawRect(X, Y, BannerW, BannerH, FrostBg, IGraphics::CORNER_ALL, 8.0f);
	Graphics()->DrawRectOutline(X, Y, BannerW, BannerH, FrostBorder, IGraphics::CORNER_ALL, IGraphics::SIDE_ALL, 8.0f);

	char aMsg[64];
	if(FreezeSecRemaining > 0.0f)
		str_format(aMsg, sizeof(aMsg), "YOU ARE FROZEN (%.1fs)", FreezeSecRemaining);
	else
		str_copy(aMsg, "YOU ARE FROZEN");

	const float FontSize = 7.5f;
	const float TextW = TextRender()->TextWidth(FontSize, aMsg, -1, -1.0f);
	TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
	TextRender()->Text(X + (BannerW - TextW) * 0.5f, Y + (BannerH - FontSize) * 0.5f, FontSize, aMsg, -1.0f);
	TextRender()->TextColor(TextRender()->DefaultTextColor());
}

void CFreezeOffscreenHud::RenderOffscreenRadar()
{
	if(!g_Config.m_ClTvOffscreen && !g_Config.m_ClTvOffscreenHud)
		return;

	const float ScreenWidth = 300.0f * Graphics()->ScreenAspect();
	const float ScreenHeight = 300.0f;
	Graphics()->MapScreenToSize(ScreenWidth, ScreenHeight);

	const vec2 ScreenCenter(ScreenWidth * 0.5f, ScreenHeight * 0.5f);
	const vec2 CamCenter = GameClient()->m_Camera.m_Center;
	const float Zoom = std::max(0.05f, GameClient()->m_Camera.m_Zoom);

	const float HalfWorldH = 150.0f * Zoom;
	const float HalfWorldW = HalfWorldH * Graphics()->ScreenAspect();

	const float Margin = (float)g_Config.m_ClTvOffscreenMargin;
	const float MaxRange = (float)g_Config.m_ClTvOffscreenRange;
	const int LocalId = GameClient()->m_Snap.m_LocalClientId;
	const int Filter = g_Config.m_ClTvOffscreenFilter; // 0: all, 1: teammates, 2: team list, 3: war list, 4: helper list
	const int Style = g_Config.m_ClTvOffscreenStyle; // 0: circular arrows, 1: mini-skins

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(i == LocalId || !GameClient()->m_Snap.m_aCharacters[i].m_Active)
			continue;

		const vec2 TargetPos = GameClient()->m_aClients[i].m_RenderPos;
		const vec2 Rel = TargetPos - CamCenter;
		const float Dist = length(Rel);

		if(Dist > MaxRange)
			continue;

		// Check if inside screen viewport
		if(std::abs(Rel.x) <= HalfWorldW && std::abs(Rel.y) <= HalfWorldH)
			continue; // On screen, no off-screen marker needed

		// Social category check
		int ListType = GameClient()->m_TeeVisuals.GetPlayerListType(i); // 1: War, 2: Team, 3: Helper
		bool IsTeammate = !GameClient()->IsOtherTeam(i);

		// Filter check
		if(Filter == 1 && !IsTeammate)
			continue;
		if(Filter == 2 && ListType != 2)
			continue;
		if(Filter == 3 && ListType != 1)
			continue;
		if(Filter == 4 && ListType != 3)
			continue;

		// Fallback legacy filters
		if(!g_Config.m_ClTvOffscreenTeammates && IsTeammate && ListType == 0)
			continue;
		if(!g_Config.m_ClTvOffscreenWar && ListType == 1)
			continue;

		// Determine marker color
		ColorRGBA MarkerCol;
		const char *pPrefix = "";
		if(ListType == 1) // War
		{
			MarkerCol = color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClTvWarColor));
			pPrefix = "[WAR] ";
		}
		else if(ListType == 2) // Team
		{
			MarkerCol = color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClTvTeamColor));
			pPrefix = "[TEAM] ";
		}
		else if(ListType == 3) // Helper
		{
			MarkerCol = color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClTvHelperColor));
			pPrefix = "[HELP] ";
		}
		else // Standard teammate / player
		{
			MarkerCol = ColorRGBA(0.25f, 1.0f, 0.50f, 0.90f);
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
		char aDistBuf[64];
		if(g_Config.m_ClTvOffscreenNames && g_Config.m_ClTvOffscreenDist)
			str_format(aDistBuf, sizeof(aDistBuf), "%s%s (%dm)", pPrefix, GameClient()->m_aClients[i].m_aName, DistMeters);
		else if(g_Config.m_ClTvOffscreenNames)
			str_format(aDistBuf, sizeof(aDistBuf), "%s%s", pPrefix, GameClient()->m_aClients[i].m_aName);
		else if(g_Config.m_ClTvOffscreenDist)
			str_format(aDistBuf, sizeof(aDistBuf), "%dm", DistMeters);
		else
			str_copy(aDistBuf, pPrefix);

		if(Style == 1)
		{
			// Style 1: Mini-skins
			const float CircleRadius = 11.0f;
			ColorRGBA CircleBg(0.08f, 0.08f, 0.10f, 0.85f);
			Graphics()->DrawRect(MarkerPos.x - CircleRadius, MarkerPos.y - CircleRadius, CircleRadius * 2.0f, CircleRadius * 2.0f, CircleBg, IGraphics::CORNER_ALL, CircleRadius);
			Graphics()->DrawRectOutline(MarkerPos.x - CircleRadius, MarkerPos.y - CircleRadius, CircleRadius * 2.0f, CircleRadius * 2.0f, MarkerCol, IGraphics::CORNER_ALL, IGraphics::SIDE_ALL, CircleRadius);

			// Render mini tee
			CTeeRenderInfo TeeInfo = GameClient()->m_aClients[i].m_RenderInfo;
			TeeInfo.m_Size = 13.0f;
			RenderTools()->RenderIdleTeeAtCenter(&TeeInfo, MarkerPos, EMOTE_NORMAL, ScreenDir, 1.0f);

			// Arrow pointing outwards in direction of player
			vec2 ArrowTip = MarkerPos + ScreenDir * (CircleRadius + 4.0f);
			vec2 Perp(-ScreenDir.y, ScreenDir.x);
			vec2 ArrowBaseL = MarkerPos + ScreenDir * CircleRadius + Perp * 3.5f;
			vec2 ArrowBaseR = MarkerPos + ScreenDir * CircleRadius - Perp * 3.5f;

			Graphics()->TextureClear();
			Graphics()->TrianglesBegin();
			Graphics()->SetColor(MarkerCol.r, MarkerCol.g, MarkerCol.b, MarkerCol.a);
			IGraphics::CFreeformItem Tri(ArrowTip.x, ArrowTip.y, ArrowBaseL.x, ArrowBaseL.y, ArrowBaseR.x, ArrowBaseR.y, ArrowBaseR.x, ArrowBaseR.y);
			Graphics()->TrianglesDraw(&Tri, 1);
			Graphics()->TrianglesEnd();
		}
		else
		{
			// Style 0: Circular arrows + text badge
			const float FontSize = 5.0f;
			const float TextW = aDistBuf[0] ? TextRender()->TextWidth(FontSize, aDistBuf, -1, -1.0f) : 0.0f;
			const float PillW = TextW > 0.0f ? TextW + 12.0f : 14.0f;
			const float PillH = 11.0f;

			// Clamp marker within screen bounds
			MarkerPos.x = std::clamp(MarkerPos.x, PillW * 0.5f + 2.0f, ScreenWidth - PillW * 0.5f - 2.0f);
			MarkerPos.y = std::clamp(MarkerPos.y, PillH * 0.5f + 2.0f, ScreenHeight - PillH * 0.5f - 2.0f);

			ColorRGBA DarkBg(0.08f, 0.08f, 0.10f, 0.85f);
			Graphics()->DrawRect(MarkerPos.x - PillW * 0.5f, MarkerPos.y - PillH * 0.5f, PillW, PillH, DarkBg, IGraphics::CORNER_ALL, PillH * 0.5f);
			Graphics()->DrawRectOutline(MarkerPos.x - PillW * 0.5f, MarkerPos.y - PillH * 0.5f, PillW, PillH, MarkerCol, IGraphics::CORNER_ALL, IGraphics::SIDE_ALL, PillH * 0.5f);

			if(TextW > 0.0f)
			{
				TextRender()->TextColor(1.0f, 1.0f, 1.0f, 1.0f);
				TextRender()->Text(MarkerPos.x - TextW * 0.5f, MarkerPos.y - FontSize * 0.5f, FontSize, aDistBuf, -1.0f);
				TextRender()->TextColor(TextRender()->DefaultTextColor());
			}

			// Triangular pointer attached to pill
			vec2 ArrowTip = MarkerPos + ScreenDir * (PillH * 0.5f + 4.0f);
			vec2 Perp(-ScreenDir.y, ScreenDir.x);
			vec2 ArrowBaseL = MarkerPos + ScreenDir * (PillH * 0.5f) + Perp * 3.0f;
			vec2 ArrowBaseR = MarkerPos + ScreenDir * (PillH * 0.5f) - Perp * 3.0f;

			Graphics()->TextureClear();
			Graphics()->TrianglesBegin();
			Graphics()->SetColor(MarkerCol.r, MarkerCol.g, MarkerCol.b, MarkerCol.a);
			IGraphics::CFreeformItem Tri(ArrowTip.x, ArrowTip.y, ArrowBaseL.x, ArrowBaseL.y, ArrowBaseR.x, ArrowBaseR.y, ArrowBaseR.x, ArrowBaseR.y);
			Graphics()->TrianglesDraw(&Tri, 1);
			Graphics()->TrianglesEnd();
		}
	}
}

void CFreezeOffscreenHud::OnRender()
{
	if(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		return;

	RenderFreezeIndicators();
	RenderLocalFreezeHud();
	RenderOffscreenRadar();
}
