#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_FREEZE_OFFSCREEN_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_FREEZE_OFFSCREEN_H

#include <base/vmath.h>
#include <game/client/component.h>

class CFreezeOffscreenHud : public CComponent
{
private:
	void RenderFreezeIndicators();
	void RenderLocalFreezeHud();
	void RenderOffscreenRadar();

public:
	CFreezeOffscreenHud();
	int Sizeof() const override { return sizeof(*this); }

	void OnRender() override;
};

#endif
