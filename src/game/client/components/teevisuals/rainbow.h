#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_RAINBOW_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_RAINBOW_H

#include <base/color.h>
#include <game/client/component.h>

class CRainbowEffect : public CComponent
{
private:
	float m_Hue;

public:
	CRainbowEffect();
	int Sizeof() const override { return sizeof(*this); }

	void OnRender() override;
	void OnReset() override;

	ColorRGBA GetRainbowColor(float Offset = 0.0f) const;
	ColorHSVA GetRainbowColorHSV(float Offset = 0.0f) const;

	bool IsBodyRainbow() const;
	bool IsFeetRainbow() const;
	bool IsWeaponRainbow() const;
	bool IsHookRainbow() const;
};

#endif
