#include "trails.h"
#include "rainbow.h"

#include <base/color.h>
#include <engine/client.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <game/client/gameclient.h>

CPlayerTrails::CPlayerTrails()
{
}

void CPlayerTrails::OnReset()
{
	for(auto &Trail : m_aaTrailPoints)
		Trail.clear();
}

void CPlayerTrails::OnMapLoad()
{
	OnReset();
}

void CPlayerTrails::RenderTrailStrip(int ClientId)
{
	auto &Points = m_aaTrailPoints[ClientId];
	if(Points.size() < 2)
		return;

	const float BaseWidth = (float)g_Config.m_ClTvTrailWidth;
	const int Mode = g_Config.m_ClTvTrailMode;
	const size_t NumPoints = Points.size();

	Graphics()->TextureClear();
	Graphics()->QuadsBegin();

	for(size_t i = 0; i < NumPoints - 1; i++)
	{
		const float T0 = (float)i / (float)NumPoints;
		const float T1 = (float)(i + 1) / (float)NumPoints;

		// Alpha fades towards the older tail points
		const float Alpha0 = (1.0f - T0);
		const float Alpha1 = (1.0f - T1);

		// Width tapers down towards tail
		const float W0 = BaseWidth * (1.0f - T0 * 0.75f);
		const float W1 = BaseWidth * (1.0f - T1 * 0.75f);

		const vec2 P0 = Points[i].m_Pos;
		const vec2 P1 = Points[i + 1].m_Pos;
		vec2 Dir = P1 - P0;
		const float Len = length(Dir);
		if(Len < 0.001f)
			continue;
		Dir = Dir / Len;
		const vec2 Normal(-Dir.y, Dir.x);

		// Compute colors based on mode
		ColorRGBA Col0, Col1;
		if(Mode == 1)
		{
			// Orange-Yellow Gradient: #FF8C00 -> #FFB800 -> #FFE066
			ColorRGBA Orange(1.0f, 0.55f, 0.0f, Alpha0 * 0.8f);
			ColorRGBA Yellow(1.0f, 0.88f, 0.40f, Alpha0 * 0.8f);
			Col0 = mix(Orange, Yellow, T0);

			ColorRGBA Orange1(1.0f, 0.55f, 0.0f, Alpha1 * 0.8f);
			ColorRGBA Yellow1(1.0f, 0.88f, 0.40f, Alpha1 * 0.8f);
			Col1 = mix(Orange1, Yellow1, T1);
		}
		else if(Mode == 2)
		{
			// Rainbow Sync
			ColorRGBA R0 = GameClient()->m_TeeVisuals.m_Rainbow.GetRainbowColor(T0);
			ColorRGBA R1 = GameClient()->m_TeeVisuals.m_Rainbow.GetRainbowColor(T1);
			Col0 = ColorRGBA(R0.r, R0.g, R0.b, Alpha0 * 0.8f);
			Col1 = ColorRGBA(R1.r, R1.g, R1.b, Alpha1 * 0.8f);
		}
		else
		{
			// Static Color
			ColorRGBA Base = color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClTvTrailColor));
			Col0 = ColorRGBA(Base.r, Base.g, Base.b, Alpha0 * 0.8f);
			Col1 = ColorRGBA(Base.r, Base.g, Base.b, Alpha1 * 0.8f);
		}

		const vec2 V0_L = P0 + Normal * (W0 * 0.5f);
		const vec2 V0_R = P0 - Normal * (W0 * 0.5f);
		const vec2 V1_L = P1 + Normal * (W1 * 0.5f);
		const vec2 V1_R = P1 - Normal * (W1 * 0.5f);

		Graphics()->SetColor(Col0.r, Col0.g, Col0.b, Col0.a);
		IGraphics::CFreeformItem Freeform(
			V0_L.x, V0_L.y,
			V1_L.x, V1_L.y,
			V0_R.x, V0_R.y,
			V1_R.x, V1_R.y);
		Graphics()->QuadsDrawFreeform(&Freeform, 1);
	}

	Graphics()->QuadsEnd();
}

void CPlayerTrails::OnRender()
{
	if(!g_Config.m_ClTvTrails)
		return;

	if(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		return;

	const int LocalId = GameClient()->m_Snap.m_LocalClientId;
	const int MaxPoints = std::clamp(g_Config.m_ClTvTrailLength, 5, 100);
	const int64_t Now = time_get();

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		if(!GameClient()->m_Snap.m_aCharacters[i].m_Active)
		{
			m_aaTrailPoints[i].clear();
			continue;
		}

		if(g_Config.m_ClTvTrails == 1 && i != LocalId)
		{
			m_aaTrailPoints[i].clear();
			continue;
		}

		const vec2 CurPos = GameClient()->m_aClients[i].m_RenderPos;
		auto &Points = m_aaTrailPoints[i];

		if(Points.empty() || distance(Points.front().m_Pos, CurPos) > 3.0f)
		{
			Points.push_front({CurPos, Now});
			while(Points.size() > (size_t)MaxPoints)
				Points.pop_back();
		}

		RenderTrailStrip(i);
	}
}
