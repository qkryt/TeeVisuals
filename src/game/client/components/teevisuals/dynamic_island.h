#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_DYNAMIC_ISLAND_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_DYNAMIC_ISLAND_H

#include <game/client/component.h>
#include <engine/graphics.h>

class CDynamicIsland : public CComponent
{
private:
	IGraphics::CTextureHandle m_LogoTexture;
	float m_CurrentWidth;
	float m_TargetWidth;
	float m_Alpha;
	float m_FpsSmoothed;
	int64_t m_SessionStartTime;

	void RenderPill(float CenterX, float CenterY, float Width, float Height, float Alpha);

public:
	CDynamicIsland();
	int Sizeof() const override { return sizeof(*this); }

	void OnInit() override;
	void OnReset() override;
	void OnRender() override;
	void OnMapLoad() override;
};

#endif
