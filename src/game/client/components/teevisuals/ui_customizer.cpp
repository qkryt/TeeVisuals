#include "ui_customizer.h"

#include <base/color.h>
#include <engine/client.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/storage.h>
#include <game/client/gameclient.h>

CUiCustomizer::CUiCustomizer()
{
	m_aLastLoadedBg[0] = '\0';
	m_aLastLoadedFont[0] = '\0';
}

void CUiCustomizer::OnInit()
{
	LoadCustomBackground();
	LoadCustomFont();
}

void CUiCustomizer::OnReset()
{
}

void CUiCustomizer::LoadCustomBackground()
{
	if(g_Config.m_ClTvMenuBg[0] == '\0')
	{
		m_MenuBgTexture.Invalidate();
		m_aLastLoadedBg[0] = '\0';
		return;
	}

	if(str_comp(m_aLastLoadedBg, g_Config.m_ClTvMenuBg) == 0 && m_MenuBgTexture.IsValid())
		return;

	m_MenuBgTexture = Graphics()->LoadTexture(g_Config.m_ClTvMenuBg, IStorage::TYPE_ALL);
	if(m_MenuBgTexture.IsValid())
	{
		str_copy(m_aLastLoadedBg, g_Config.m_ClTvMenuBg);
	}
}

void CUiCustomizer::LoadCustomFont()
{
	if(g_Config.m_ClTvCustomFont[0] == '\0')
		return;

	if(str_comp(m_aLastLoadedFont, g_Config.m_ClTvCustomFont) == 0)
		return;

	str_copy(m_aLastLoadedFont, g_Config.m_ClTvCustomFont);
}

void CUiCustomizer::OnRender()
{
	// Reload if config changed
	if(str_comp(m_aLastLoadedBg, g_Config.m_ClTvMenuBg) != 0)
		LoadCustomBackground();
	if(str_comp(m_aLastLoadedFont, g_Config.m_ClTvCustomFont) != 0)
		LoadCustomFont();
}

bool CUiCustomizer::RenderMenuBackground(float Width, float Height)
{
	if(!m_MenuBgTexture.IsValid())
		return false;

	Graphics()->TextureSet(m_MenuBgTexture);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Aspect-Fill calculation
	const float ScreenAspect = Width / Height;
	const float ImgAspect = 16.0f / 9.0f; // Default assumption for loaded background wallpapers

	float DrawW = Width;
	float DrawH = Height;
	float DrawX = 0.0f;
	float DrawY = 0.0f;

	if(ScreenAspect > ImgAspect)
	{
		DrawH = Width / ImgAspect;
		DrawY = (Height - DrawH) * 0.5f;
	}
	else
	{
		DrawW = Height * ImgAspect;
		DrawX = (Width - DrawW) * 0.5f;
	}

	IGraphics::CQuadItem Quad(DrawX, DrawY, DrawW, DrawH);
	Graphics()->QuadsDrawTL(&Quad, 1);
	Graphics()->QuadsEnd();

	// Optional darkening dim overlay
	if(g_Config.m_ClTvMenuBgDim > 0)
	{
		float DimAlpha = (float)g_Config.m_ClTvMenuBgDim / 100.0f;
		Graphics()->TextureClear();
		Graphics()->DrawRect(0.0f, 0.0f, Width, Height, ColorRGBA(0.0f, 0.0f, 0.0f, DimAlpha), 0, 0.0f);
	}

	return true;
}
