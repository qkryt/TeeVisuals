#include "ui_custom.h"

#include <base/color.h>
#include <base/system.h>
#include <engine/client.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/storage.h>
#include <engine/textrender.h>
#include <game/client/gameclient.h>

#include <algorithm>
#include <cstring>

CUiCustom::CUiCustom()
{
	m_aLastLoadedBg[0] = '\0';
	m_aLastLoadedFont[0] = '\0';
}

void CUiCustom::OnInit()
{
	ScanFonts();
	ScanBackgrounds();
	LoadCustomBackground();
	LoadCustomFont();
}

void CUiCustom::OnReset()
{
}

int CUiCustom::FontScanCallback(const char *pName, int IsDir, int StorageType, void *pUser)
{
	CUiCustom *pSelf = (CUiCustom *)pUser;
	if(IsDir || !pName || !pName[0])
		return 0;

	const char *pExt = str_endswith(pName, ".ttf");
	if(!pExt)
		pExt = str_endswith(pName, ".otf");
	if(!pExt)
		pExt = str_endswith(pName, ".ttc");

	if(pExt)
	{
		std::string Name = pName;
		if(std::find(pSelf->m_vAvailableFonts.begin(), pSelf->m_vAvailableFonts.end(), Name) == pSelf->m_vAvailableFonts.end())
		{
			pSelf->m_vAvailableFonts.push_back(Name);
		}
	}

	return 0;
}

int CUiCustom::BgScanCallback(const char *pName, int IsDir, int StorageType, void *pUser)
{
	CUiCustom *pSelf = (CUiCustom *)pUser;
	if(IsDir || !pName || !pName[0])
		return 0;

	const char *pExt = str_endswith(pName, ".png");
	if(!pExt)
		pExt = str_endswith(pName, ".jpg");
	if(!pExt)
		pExt = str_endswith(pName, ".jpeg");

	if(pExt)
	{
		std::string Name = pName;
		if(std::find(pSelf->m_vAvailableBackgrounds.begin(), pSelf->m_vAvailableBackgrounds.end(), Name) == pSelf->m_vAvailableBackgrounds.end())
		{
			pSelf->m_vAvailableBackgrounds.push_back(Name);
		}
	}

	return 0;
}

void CUiCustom::ScanFonts()
{
	m_vAvailableFonts.clear();
	Storage()->ListDirectory(IStorage::TYPE_ALL, "fonts", FontScanCallback, this);
	Storage()->ListDirectory(IStorage::TYPE_ALL, "entity/fonts", FontScanCallback, this);
	std::sort(m_vAvailableFonts.begin(), m_vAvailableFonts.end());
}

void CUiCustom::ScanBackgrounds()
{
	m_vAvailableBackgrounds.clear();
	Storage()->ListDirectory(IStorage::TYPE_ALL, "backgrounds", BgScanCallback, this);
	Storage()->ListDirectory(IStorage::TYPE_ALL, "themes", BgScanCallback, this);
	std::sort(m_vAvailableBackgrounds.begin(), m_vAvailableBackgrounds.end());
}

void CUiCustom::LoadCustomBackground()
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

void CUiCustom::ResetCustomBackground()
{
	g_Config.m_ClTvMenuBg[0] = '\0';
	m_MenuBgTexture.Invalidate();
	m_aLastLoadedBg[0] = '\0';
}

void CUiCustom::LoadCustomFont()
{
	if(g_Config.m_ClTvCustomFont[0] == '\0')
		return;

	if(str_comp(m_aLastLoadedFont, g_Config.m_ClTvCustomFont) == 0)
		return;

	str_copy(m_aLastLoadedFont, g_Config.m_ClTvCustomFont);
	TextRender()->SetCustomFace(g_Config.m_ClTvCustomFont);
}

void CUiCustom::ResetCustomFont()
{
	g_Config.m_ClTvCustomFont[0] = '\0';
	m_aLastLoadedFont[0] = '\0';
	TextRender()->SetCustomFace("");
}

void CUiCustom::OnRender()
{
	// Reload if config changed
	if(str_comp(m_aLastLoadedBg, g_Config.m_ClTvMenuBg) != 0)
		LoadCustomBackground();
	if(str_comp(m_aLastLoadedFont, g_Config.m_ClTvCustomFont) != 0)
		LoadCustomFont();
}

bool CUiCustom::RenderMenuBackground(float Width, float Height)
{
	if(!m_MenuBgTexture.IsValid())
		return false;

	Graphics()->TextureSet(m_MenuBgTexture);
	Graphics()->QuadsBegin();
	Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Aspect-Fill calculation without stretching or letterboxing
	const float ScreenAspect = Width / Height;
	const float ImgAspect = 16.0f / 9.0f; // Typical standard wallpaper aspect ratio

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
		const float Alpha = (float)g_Config.m_ClTvMenuBgDim * 0.01f;
		Graphics()->DrawRect(0.0f, 0.0f, Width, Height, ColorRGBA(0.0f, 0.0f, 0.0f, Alpha), 0, 0.0f);
	}

	return true;
}
