#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_MOBILE_UX_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_MOBILE_UX_H

#include "teevisuals_common.h"

#include <base/vmath.h>
#include <game/client/component.h>

class CMobileUx : public CComponent
{
private:
	bool m_TouchActive;
	vec2 m_BindwheelButtonPos;
	float m_BindwheelButtonRadius;

public:
	CMobileUx();
	int Sizeof() const override { return sizeof(*this); }

	void OnInit() override;
	void OnReset() override;
	void OnRender() override;

	bool IsMobilePlatform() const;
	TeeVisuals::SSafeArea GetCurrentSafeArea() const;

	float GetTouchScale() const;
	float GetJoystickOpacity() const;

	bool OnTouchDown(int TouchId, vec2 Pos);
	bool OnTouchMove(int TouchId, vec2 Pos);
	bool OnTouchUp(int TouchId);
};

#endif
