#include "mobile_ux.h"
#include "bindwheel.h"

#include <base/color.h>
#include <engine/client.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>
#include <game/client/gameclient.h>

CMobileUx::CMobileUx()
{
	m_TouchActive = false;
	m_BindwheelButtonPos = vec2(0.0f, 0.0f);
	m_BindwheelButtonRadius = 14.0f;
}

void CMobileUx::OnInit()
{
	OnReset();
}

void CMobileUx::OnReset()
{
	m_TouchActive = false;
}

bool CMobileUx::IsMobilePlatform() const
{
#if defined(CONF_PLATFORM_ANDROID) || defined(CONF_PLATFORM_IOS) || defined(ANDROID) || defined(__ANDROID__) || defined(__IPHONEOS__)
	return true;
#else
	return g_Config.m_ClTvMobileSafeArea != 0 && g_Config.m_ClTouchControls != 0;
#endif
}

TeeVisuals::SSafeArea CMobileUx::GetCurrentSafeArea() const
{
	const bool Active = IsMobilePlatform() || (g_Config.m_ClTvMobileSafeArea != 0);
	return TeeVisuals::GetSafeArea(Active);
}

float CMobileUx::GetTouchScale() const
{
	return (float)g_Config.m_ClTvMobileTouchScale * 0.01f;
}

float CMobileUx::GetJoystickOpacity() const
{
	return (float)g_Config.m_ClTvMobileJoystickAlpha * 0.01f;
}

void CMobileUx::OnRender()
{
	// Only render mobile touch helper button when online and on mobile/touch controls
	if(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		return;

	if(!IsMobilePlatform() && !g_Config.m_ClTouchControls)
		return;

	const float ScreenWidth = 300.0f * Graphics()->ScreenAspect();
	const float ScreenHeight = 300.0f;
	Graphics()->MapScreenToSize(ScreenWidth, ScreenHeight);

	TeeVisuals::SSafeArea Safe = GetCurrentSafeArea();

	// Position dedicated Bindwheel trigger button at top right
	m_BindwheelButtonRadius = 13.0f * GetTouchScale();
	m_BindwheelButtonPos = vec2(ScreenWidth - Safe.m_Right - m_BindwheelButtonRadius - 8.0f, Safe.m_Top + m_BindwheelButtonRadius + 8.0f);

	// Draw dedicated circular touch button for Smooth Bindwheel
	ColorRGBA BgCol(0.12f, 0.10f, 0.08f, 0.75f * GetJoystickOpacity());
	ColorRGBA BorderCol = TeeVisuals::COLOR_ORANGE.WithAlpha(0.9f);

	Graphics()->DrawRect(
		m_BindwheelButtonPos.x - m_BindwheelButtonRadius,
		m_BindwheelButtonPos.y - m_BindwheelButtonRadius,
		m_BindwheelButtonRadius * 2.0f,
		m_BindwheelButtonRadius * 2.0f,
		BgCol,
		IGraphics::CORNER_ALL,
		m_BindwheelButtonRadius);

	Graphics()->DrawRectOutline(
		m_BindwheelButtonPos.x - m_BindwheelButtonRadius,
		m_BindwheelButtonPos.y - m_BindwheelButtonRadius,
		m_BindwheelButtonRadius * 2.0f,
		m_BindwheelButtonRadius * 2.0f,
		BorderCol,
		IGraphics::CORNER_ALL,
		IGraphics::SIDE_ALL,
		m_BindwheelButtonRadius);

	const char *pLabel = "TV";
	const float FontSize = 6.5f * GetTouchScale();
	const float TextW = TextRender()->TextWidth(FontSize, pLabel, -1, -1.0f);

	TextRender()->TextColor(TeeVisuals::COLOR_YELLOW.r, TeeVisuals::COLOR_YELLOW.g, TeeVisuals::COLOR_YELLOW.b, 1.0f);
	TextRender()->Text(m_BindwheelButtonPos.x - TextW * 0.5f, m_BindwheelButtonPos.y - FontSize * 0.5f, FontSize, pLabel, -1.0f);
	TextRender()->TextColor(TextRender()->DefaultTextColor());
}

bool CMobileUx::OnTouchDown(int TouchId, vec2 Pos)
{
	if(distance(Pos, m_BindwheelButtonPos) <= m_BindwheelButtonRadius * 1.5f)
	{
		m_TouchActive = true;
		GameClient()->m_TeeVisuals.m_Bindwheel.TouchDown(vec2(0.0f, 0.0f));
		return true;
	}
	return false;
}

bool CMobileUx::OnTouchMove(int TouchId, vec2 Pos)
{
	if(m_TouchActive)
	{
		vec2 Delta = Pos - m_BindwheelButtonPos;
		GameClient()->m_TeeVisuals.m_Bindwheel.TouchMove(Delta);
		return true;
	}
	return false;
}

bool CMobileUx::OnTouchUp(int TouchId)
{
	if(m_TouchActive)
	{
		m_TouchActive = false;
		GameClient()->m_TeeVisuals.m_Bindwheel.TouchUp();
		return true;
	}
	return false;
}
