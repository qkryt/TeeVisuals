#include "dynamic_island.h"

#include <base/color.h>
#include <base/time.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>
#include <game/client/gameclient.h>
#include <game/client/render.h>

CDynamicIsland::CDynamicIsland()
{
	m_CurrentWidth = 240.0f;
	m_TargetWidth = 240.0f;
	m_Alpha = 0.0f;
	m_FpsSmoothed = 60.0f;
	m_SessionStartTime = time_get();
}

void CDynamicIsland::OnInit()
{
	m_LogoTexture = Graphics()->LoadTexture("ui/teevisuals_logo.png", IStorage::TYPE_ALL);
	m_SessionStartTime = time_get();
}

void CDynamicIsland::OnReset()
{
	m_SessionStartTime = time_get();
}

void CDynamicIsland::OnMapLoad()
{
	m_SessionStartTime = time_get();
}

void CDynamicIsland::RenderPill(float CenterX, float CenterY, float Width, float Height, float Alpha)
{
	const float HalfW = Width * 0.5f;
	const float HalfH = Height * 0.5f;
	const float X = CenterX - HalfW;
	const float Y = CenterY - HalfH;
	const float Radius = HalfH;

	const float BgAlpha = Alpha * (g_Config.m_ClTvIslandBgAlpha / 100.0f);

	// 1. Soft outer glow / border (Orange-Yellow gradient accent)
	ColorRGBA BorderTop(1.0f, 0.55f, 0.0f, Alpha * 0.85f);    // #FF8C00
	ColorRGBA BorderBottom(1.0f, 0.88f, 0.40f, Alpha * 0.85f); // #FFE066
	Graphics()->DrawRectOutline(X - 1.0f, Y - 1.0f, Width + 2.0f, Height + 2.0f, BorderTop, IGraphics::CORNER_ALL, IGraphics::SIDE_ALL, Radius + 1.0f);

	// 2. Glassmorphic dark pill body
	ColorRGBA BgColor(0.06f, 0.06f, 0.08f, BgAlpha);
	Graphics()->DrawRect(X, Y, Width, Height, BgColor, IGraphics::CORNER_ALL, Radius);

	// 3. Inner subtle top highlight (glass reflection)
	ColorRGBA HighlightColor(1.0f, 0.85f, 0.4f, Alpha * 0.12f);
	Graphics()->DrawRect(X + Radius * 0.3f, Y + 1.0f, Width - Radius * 0.6f, Height * 0.35f, HighlightColor, IGraphics::CORNER_T, Radius * 0.5f);
}

void CDynamicIsland::OnRender()
{
	if(!g_Config.m_ClTvIsland)
		return;

	// Do not render during loading or if UI is hidden
	if(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		return;

	const float DeltaTime = std::clamp(Client()->RenderFrameTime(), 0.001f, 0.1f);

	// Smooth FPS calculation
	const float CurrentFps = 1.0f / DeltaTime;
	m_FpsSmoothed += (CurrentFps - m_FpsSmoothed) * std::clamp(DeltaTime * 4.0f, 0.0f, 1.0f);

	// Ping lookup
	int Ping = 0;
	if(GameClient()->m_Snap.m_pLocalInfo)
		Ping = GameClient()->m_Snap.m_pLocalInfo->m_Latency;
	else if(GameClient()->m_Snap.m_LocalClientId >= 0 && GameClient()->m_Snap.m_apPlayerInfos[GameClient()->m_Snap.m_LocalClientId])
		Ping = GameClient()->m_Snap.m_apPlayerInfos[GameClient()->m_Snap.m_LocalClientId]->m_Latency;
	Ping = std::clamp(Ping, 0, 9999);

	// Clock / Timer
	char aTimeBuf[32];
	if(g_Config.m_ClTvIslandTimerMode == 1)
	{
		int64_t Elapsed = (time_get() - m_SessionStartTime) / time_freq();
		int Hours = (int)(Elapsed / 3600);
		int Minutes = (int)((Elapsed % 3600) / 60);
		int Seconds = (int)(Elapsed % 60);
		str_format(aTimeBuf, sizeof(aTimeBuf), "%02d:%02d:%02d", Hours, Minutes, Seconds);
	}
	else
	{
		str_timestamp_format(aTimeBuf, sizeof(aTimeBuf), "%H:%M:%S");
	}

	char aFpsBuf[32];
	str_format(aFpsBuf, sizeof(aFpsBuf), "%d FPS", (int)(m_FpsSmoothed + 0.5f));

	char aPingBuf[32];
	str_format(aPingBuf, sizeof(aPingBuf), "%d ms", Ping);

	// Virtual coordinate space (300 height)
	const float ScreenWidth = 300.0f * Graphics()->ScreenAspect();
	const float ScreenHeight = 300.0f;
	Graphics()->MapScreenToSize(ScreenWidth, ScreenHeight);

	const float Scale = (float)g_Config.m_ClTvIslandScale / 100.0f;
	const float Height = 17.0f * Scale;
	const float FontSize = 6.5f * Scale;
	const float LogoSize = 11.0f * Scale;
	const float Spacing = 5.0f * Scale;

	const float TitleWidth = TextRender()->TextWidth(FontSize, "TeeVisuals", -1, -1.0f);
	const float PingWidth = TextRender()->TextWidth(FontSize, aPingBuf, -1, -1.0f);
	const float TimeWidth = TextRender()->TextWidth(FontSize, aTimeBuf, -1, -1.0f);
	const float FpsWidth = TextRender()->TextWidth(FontSize, aFpsBuf, -1, -1.0f);
	const float SepWidth = TextRender()->TextWidth(FontSize, "|", -1, -1.0f);

	// Desired width calculation with padding
	m_TargetWidth = (LogoSize + Spacing * 2.0f + TitleWidth + Spacing * 2.0f +
			 SepWidth + Spacing * 2.0f + PingWidth + Spacing * 2.0f +
			 SepWidth + Spacing * 2.0f + TimeWidth + Spacing * 2.0f +
			 SepWidth + Spacing * 2.0f + FpsWidth + Spacing * 3.0f);

	// Smooth width interpolation
	m_CurrentWidth += (m_TargetWidth - m_CurrentWidth) * std::clamp(DeltaTime * 12.0f, 0.0f, 1.0f);

	// Fade in
	m_Alpha += (1.0f - m_Alpha) * std::clamp(DeltaTime * 8.0f, 0.0f, 1.0f);

	const float CenterX = ScreenWidth * 0.5f;
	const float CenterY = (float)g_Config.m_ClTvIslandPosY + Height * 0.5f;

	// Render Capsule / Pill
	RenderPill(CenterX, CenterY, m_CurrentWidth, Height, m_Alpha);

	// Render contents
	float CurX = CenterX - (m_CurrentWidth * 0.5f) + Spacing * 2.0f;
	const float TextY = CenterY - FontSize * 0.5f;

	// 1. Mini Logo Texture
	if(m_LogoTexture.IsValid())
	{
		Graphics()->TextureSet(m_LogoTexture);
		Graphics()->QuadsBegin();
		Graphics()->SetColor(1.0f, 1.0f, 1.0f, m_Alpha);
		const float LogoY = CenterY - LogoSize * 0.5f;
		IGraphics::CQuadItem Quad(CurX, LogoY, LogoSize, LogoSize);
		Graphics()->QuadsDrawTL(&Quad, 1);
		Graphics()->QuadsEnd();
	}
	CurX += LogoSize + Spacing;

	// 2. "TeeVisuals" title in warm gradient gold/orange
	TextRender()->TextColor(1.0f, 0.65f, 0.1f, m_Alpha); // #FFA61A
	TextRender()->Text(CurX, TextY, FontSize, "TeeVisuals", -1.0f);
	CurX += TitleWidth + Spacing;

	// Separator 1
	TextRender()->TextColor(1.0f, 0.72f, 0.0f, m_Alpha * 0.7f); // #FFB800
	TextRender()->Text(CurX, TextY, FontSize, "|", -1.0f);
	CurX += SepWidth + Spacing;

	// 3. Ping
	ColorRGBA PingColor = Ping < 50 ? ColorRGBA(0.3f, 1.0f, 0.4f, m_Alpha) : (Ping < 120 ? ColorRGBA(1.0f, 0.85f, 0.2f, m_Alpha) : ColorRGBA(1.0f, 0.35f, 0.35f, m_Alpha));
	TextRender()->TextColor(PingColor);
	TextRender()->Text(CurX, TextY, FontSize, aPingBuf, -1.0f);
	CurX += PingWidth + Spacing;

	// Separator 2
	TextRender()->TextColor(1.0f, 0.72f, 0.0f, m_Alpha * 0.7f);
	TextRender()->Text(CurX, TextY, FontSize, "|", -1.0f);
	CurX += SepWidth + Spacing;

	// 4. Time
	TextRender()->TextColor(0.95f, 0.95f, 0.98f, m_Alpha);
	TextRender()->Text(CurX, TextY, FontSize, aTimeBuf, -1.0f);
	CurX += TimeWidth + Spacing;

	// Separator 3
	TextRender()->TextColor(1.0f, 0.72f, 0.0f, m_Alpha * 0.7f);
	TextRender()->Text(CurX, TextY, FontSize, "|", -1.0f);
	CurX += SepWidth + Spacing;

	// 5. FPS
	TextRender()->TextColor(0.4f, 0.85f, 1.0f, m_Alpha);
	TextRender()->Text(CurX, TextY, FontSize, aFpsBuf, -1.0f);

	// Reset text color
	TextRender()->TextColor(TextRender()->DefaultTextColor());
}
