#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_TRAILS_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_TRAILS_H

#include <base/vmath.h>
#include <game/client/component.h>

#include <deque>

class CPlayerTrails : public CComponent
{
public:
	struct STrailPoint
	{
		vec2 m_Pos;
		int64_t m_Timestamp;
	};

private:
	std::deque<STrailPoint> m_aaTrailPoints[MAX_CLIENTS];

	void RenderTrailStrip(int ClientId);

public:
	CPlayerTrails();
	int Sizeof() const override { return sizeof(*this); }

	void OnRender() override;
	void OnReset() override;
	void OnMapLoad() override;
};

#endif
