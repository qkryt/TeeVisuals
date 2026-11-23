#include "rainbow.h"

#include <engine/client.h>
#include <engine/shared/config.h>

#include <algorithm>
#include <cmath>

CRainbowEffect::CRainbowEffect()
{
	m_Hue = 0.0f;
}

void CRainbowEffect::OnReset()
{
	m_Hue = 0.0f;
}

void CRainbowEffect::OnRender()
{
	const float DeltaTime = std::clamp(Client()->RenderFrameTime(), 0.001f, 0.1f);
	const float Speed = (float)g_Config.m_ClTvRainbowSpeed * 0.01f;

	m_Hue += DeltaTime * Speed;
	if(m_Hue >= 1.0f)
		m_Hue -= std::floor(m_Hue);
}

ColorHSVA CRainbowEffect::GetRainbowColorHSV(float Offset) const
{
	float CurrentHue = m_Hue + Offset;
	if(CurrentHue >= 1.0f)
		CurrentHue -= std::floor(CurrentHue);
	else if(CurrentHue < 0.0f)
		CurrentHue += std::ceil(-CurrentHue);

	const float Sat = (float)g_Config.m_ClTvRainbowSat / 100.0f;
	const float Val = (float)g_Config.m_ClTvRainbowVal / 100.0f;

	return ColorHSVA(CurrentHue, Sat, Val, 1.0f);
}

ColorRGBA CRainbowEffect::GetRainbowColor(float Offset) const
{
	return color_cast<ColorRGBA>(GetRainbowColorHSV(Offset));
}

bool CRainbowEffect::IsRainbowActive() const
{
	return g_Config.m_ClTvRainbow != 0;
}

bool CRainbowEffect::IsBodyRainbow() const
{
	return g_Config.m_ClTvRainbow != 0 && g_Config.m_ClTvRainbowBody != 0;
}

bool CRainbowEffect::IsFeetRainbow() const
{
	return g_Config.m_ClTvRainbow != 0 && g_Config.m_ClTvRainbowFeet != 0;
}

bool CRainbowEffect::IsWeaponRainbow() const
{
	return g_Config.m_ClTvRainbow != 0 && g_Config.m_ClTvRainbowWeapon != 0;
}

bool CRainbowEffect::IsHookRainbow() const
{
	return g_Config.m_ClTvRainbow != 0 && g_Config.m_ClTvRainbowHook != 0;
}
