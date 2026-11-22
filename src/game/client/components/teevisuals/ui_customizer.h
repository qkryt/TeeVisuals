#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_UI_CUSTOMIZER_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_UI_CUSTOMIZER_H

#include <engine/graphics.h>
#include <game/client/component.h>

class CUiCustomizer : public CComponent
{
private:
	IGraphics::CTextureHandle m_MenuBgTexture;
	char m_aLastLoadedBg[256];
	char m_aLastLoadedFont[256];

	void LoadCustomBackground();
	void LoadCustomFont();

public:
	CUiCustomizer();
	int Sizeof() const override { return sizeof(*this); }

	void OnInit() override;
	void OnReset() override;
	void OnRender() override;

	bool RenderMenuBackground(float Width, float Height);
};

#endif
