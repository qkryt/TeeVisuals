#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_TEEVISUALS_COMMON_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_TEEVISUALS_COMMON_H

#include <base/color.h>
#include <base/vmath.h>

#include <cmath>

namespace TeeVisuals
{
	// Brand Color Palette: Orange-Yellow Gradient (#FF8C00 -> #FFB800 -> #FFE066)
	inline const ColorRGBA COLOR_ORANGE(1.0f, 0.55f, 0.0f, 1.0f);   // #FF8C00
	inline const ColorRGBA COLOR_AMBER(1.0f, 0.72f, 0.0f, 1.0f);    // #FFB800
	inline const ColorRGBA COLOR_YELLOW(1.0f, 0.88f, 0.40f, 1.0f);  // #FFE066

	// Social List Palette
	inline const ColorRGBA COLOR_WAR(1.0f, 0.30f, 0.30f, 1.0f);     // #FF4D4D
	inline const ColorRGBA COLOR_TEAM(0.30f, 1.0f, 0.53f, 1.0f);    // #4DFF88
	inline const ColorRGBA COLOR_HELPER(0.30f, 0.76f, 1.0f, 1.0f);  // #4DC3FF

	// Easing Algorithms
	inline float EaseOutCubic(float t)
	{
		float inv = 1.0f - std::clamp(t, 0.0f, 1.0f);
		return 1.0f - inv * inv * inv;
	}

	inline float EaseOutBack(float t)
	{
		const float c1 = 1.70158f;
		const float c3 = c1 + 1.0f;
		float tm1 = std::clamp(t, 0.0f, 1.0f) - 1.0f;
		return 1.0f + c3 * tm1 * tm1 * tm1 + c1 * tm1 * tm1;
	}

	inline float Lerp(float a, float b, float t)
	{
		return a + (b - a) * std::clamp(t, 0.0f, 1.0f);
	}

	// Safe Area Helper
	struct SSafeArea
	{
		float m_Top = 0.0f;
		float m_Bottom = 0.0f;
		float m_Left = 0.0f;
		float m_Right = 0.0f;
	};

	inline SSafeArea GetSafeArea(bool IsMobile = false)
	{
		SSafeArea Area;
		if(IsMobile)
		{
			// Safe margins on modern smartphones with notches or Dynamic Island
			Area.m_Top = 14.0f;
			Area.m_Bottom = 8.0f;
			Area.m_Left = 16.0f;
			Area.m_Right = 16.0f;
		}
		return Area;
	}
}

#endif
