#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_UI_CUSTOM_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_UI_CUSTOM_H

#include <engine/graphics.h>
#include <game/client/component.h>

#include <string>
#include <vector>

class CUiCustom : public CComponent
{
private:
	IGraphics::CTextureHandle m_MenuBgTexture;
	char m_aLastLoadedBg[256];
	char m_aLastLoadedFont[256];

	std::vector<std::string> m_vAvailableFonts;
	std::vector<std::string> m_vAvailableBackgrounds;

	static int FontScanCallback(const char *pName, int IsDir, int StorageType, void *pUser);
	static int BgScanCallback(const char *pName, int IsDir, int StorageType, void *pUser);

public:
	CUiCustom();
	int Sizeof() const override { return sizeof(*this); }

	void OnInit() override;
	void OnReset() override;
	void OnRender() override;

	void ScanFonts();
	void ScanBackgrounds();

	void LoadCustomBackground();
	void LoadCustomFont();
	void ResetCustomFont();
	void ResetCustomBackground();

	bool RenderMenuBackground(float Width, float Height);

	const std::vector<std::string> &GetAvailableFonts() const { return m_vAvailableFonts; }
	const std::vector<std::string> &GetAvailableBackgrounds() const { return m_vAvailableBackgrounds; }
};

#endif
