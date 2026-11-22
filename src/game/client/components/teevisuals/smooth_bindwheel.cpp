#include "smooth_bindwheel.h"

#include <base/color.h>
#include <engine/client.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/textrender.h>
#include <game/client/gameclient.h>

#include <cmath>

static const float PI = 3.14159265358979323846f;

CSmoothBindwheel::CSmoothBindwheel()
{
	m_Active = false;
	m_WasActive = false;
	m_OpenProgress = 0.0f;
	m_CursorPos = vec2(0.0f, 0.0f);
	m_HoveredIndex = -1;

	for(int i = 0; i < 16; i++)
		m_aSliceHoverAnim[i] = 0.0f;

	// Populate with 8 default versatile slots
	m_vSlots.push_back({"Good Game!", "say \"Good game!\""});
	m_vSlots.push_back({"Heart <3", "emote 1"});
	m_vSlots.push_back({"Help Me!", "say_team \"Help!\""});
	m_vSlots.push_back({"Happy :)", "emote 3"});
	m_vSlots.push_back({"Nice Hook!", "say \"Nice hook!\""});
	m_vSlots.push_back({"Surprise :O", "emote 2"});
	m_vSlots.push_back({"Restart /r", "say \"/r\""});
	m_vSlots.push_back({"Toggle Ready", "ready_change"});
}

void CSmoothBindwheel::OnInit()
{
	OnReset();
}

void CSmoothBindwheel::OnReset()
{
	m_Active = false;
	m_WasActive = false;
	m_OpenProgress = 0.0f;
	m_CursorPos = vec2(0.0f, 0.0f);
	m_HoveredIndex = -1;
}

void CSmoothBindwheel::OnConsoleInit()
{
	Console()->Register("+tv_bindwheel", "", CFGFLAG_CLIENT, ConKeyBindwheel, this, "Open smooth bindwheel");
}

void CSmoothBindwheel::ConKeyBindwheel(IConsole::IResult *pResult, void *pUserData)
{
	CSmoothBindwheel *pSelf = (CSmoothBindwheel *)pUserData;
	pSelf->SetActive(pResult->GetInteger(0) != 0);
}

void CSmoothBindwheel::SetActive(bool Active)
{
	if(m_Active == Active)
		return;

	m_Active = Active;
	if(!m_Active)
	{
		ExecuteSelected();
		m_CursorPos = vec2(0.0f, 0.0f);
	}
	else
	{
		m_CursorPos = vec2(0.0f, 0.0f);
		m_HoveredIndex = -1;
	}
}

void CSmoothBindwheel::ExecuteSelected()
{
	if(m_HoveredIndex >= 0 && m_HoveredIndex < (int)m_vSlots.size())
	{
		const char *pCmd = m_vSlots[m_HoveredIndex].m_aCommand;
		if(pCmd && pCmd[0])
		{
			Console()->ExecuteLine(pCmd);
		}
	}
}

float CSmoothBindwheel::EaseOutBack(float t) const
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	float tm1 = t - 1.0f;
	return 1.0f + c3 * tm1 * tm1 * tm1 + c1 * tm1 * tm1;
}

bool CSmoothBindwheel::OnCursorMove(float x, float y, IInput::ECursorType CursorType)
{
	if(!m_Active)
		return false;

	m_CursorPos += vec2(x, y);
	return true;
}

bool CSmoothBindwheel::OnInput(const IInput::CEvent &Event)
{
	if(!m_Active)
		return false;

	// Mouse button release or touch release executes
	if(Event.m_Flags & IInput::FLAG_RELEASE)
	{
		if(Event.m_Key == KEY_MOUSE_1 || Event.m_Key >= KEY_CUSTOM_FIRST)
		{
			SetActive(false);
			return true;
		}
	}

	return false;
}

void CSmoothBindwheel::SetSlot(size_t Index, const char *pTitle, const char *pCmd)
{
	if(Index >= m_vSlots.size())
		m_vSlots.resize(Index + 1);

	str_copy(m_vSlots[Index].m_aTitle, pTitle ? pTitle : "");
	str_copy(m_vSlots[Index].m_aCommand, pCmd ? pCmd : "");
}

void CSmoothBindwheel::OnRender()
{
	const float DeltaTime = std::clamp(Client()->RenderFrameTime(), 0.001f, 0.1f);

	if(m_Active)
		m_OpenProgress += (1.0f - m_OpenProgress) * std::clamp(DeltaTime * 14.0f, 0.0f, 1.0f);
	else
		m_OpenProgress += (0.0f - m_OpenProgress) * std::clamp(DeltaTime * 14.0f, 0.0f, 1.0f);

	if(m_OpenProgress < 0.005f)
		return;

	const float ScreenWidth = 300.0f * Graphics()->ScreenAspect();
	const float ScreenHeight = 300.0f;
	Graphics()->MapScreen(0.0f, 0.0f, ScreenWidth, ScreenHeight);

	const vec2 Center(ScreenWidth * 0.5f, ScreenHeight * 0.5f);
	const int NumSlots = (int)m_vSlots.size();
	if(NumSlots == 0)
		return;

	// Determine hovered slot from cursor position
	const float CursorDist = length(m_CursorPos);
	m_HoveredIndex = -1;

	if(CursorDist > 15.0f)
	{
		float Angle = std::atan2(m_CursorPos.y, m_CursorPos.x);
		if(Angle < 0.0f)
			Angle += 2.0f * PI;

		const float SlotAngle = (2.0f * PI) / (float)NumSlots;
		// Shift so first slot is centered at top (-PI/2)
		float Shifted = Angle + (PI * 0.5f) + (SlotAngle * 0.5f);
		while(Shifted >= 2.0f * PI)
			Shifted -= 2.0f * PI;

		m_HoveredIndex = (int)(Shifted / SlotAngle) % NumSlots;
	}

	// Update slice hover animations
	for(int i = 0; i < NumSlots; i++)
	{
		float Target = (i == m_HoveredIndex) ? 1.0f : 0.0f;
		m_aSliceHoverAnim[i] += (Target - m_aSliceHoverAnim[i]) * std::clamp(DeltaTime * 16.0f, 0.0f, 1.0f);
	}

	const float Scale = EaseOutBack(std::clamp(m_OpenProgress, 0.0f, 1.0f));
	const float BaseRadius = (float)g_Config.m_ClTvBindwheelRadius * 0.45f * Scale;
	const float SlotAngle = (2.0f * PI) / (float)NumSlots;

	// Render center circle with TeeVisuals accent
	ColorRGBA CenterBg(0.08f, 0.08f, 0.10f, 0.85f * m_OpenProgress);
	ColorRGBA CenterOutline(1.0f, 0.55f, 0.0f, 0.8f * m_OpenProgress); // Orange glow
	Graphics()->DrawRect(Center.x - 14.0f * Scale, Center.y - 14.0f * Scale, 28.0f * Scale, 28.0f * Scale, CenterBg, IGraphics::CORNER_ALL, 14.0f * Scale);
	Graphics()->DrawRectOutline(Center.x - 14.0f * Scale, Center.y - 14.0f * Scale, 28.0f * Scale, 28.0f * Scale, CenterOutline, IGraphics::CORNER_ALL, IGraphics::SIDE_ALL, 14.0f * Scale);

	// Render wheel buttons
	for(int i = 0; i < NumSlots; i++)
	{
		const float Angle = -PI * 0.5f + (float)i * SlotAngle;
		const float Hover = m_aSliceHoverAnim[i];
		const float ItemRadius = BaseRadius + Hover * 6.0f;

		const vec2 ItemCenter = Center + vec2(std::cos(Angle), std::sin(Angle)) * ItemRadius;

		const float BoxW = 46.0f * Scale + Hover * 4.0f;
		const float BoxH = 16.0f * Scale + Hover * 2.0f;
		const float BoxX = ItemCenter.x - BoxW * 0.5f;
		const float BoxY = ItemCenter.y - BoxH * 0.5f;
		const float Radius = BoxH * 0.5f;

		// Colors
		ColorRGBA BgCol = (i == m_HoveredIndex) ? ColorRGBA(0.18f, 0.14f, 0.08f, 0.95f * m_OpenProgress) : ColorRGBA(0.06f, 0.06f, 0.08f, 0.80f * m_OpenProgress);
		ColorRGBA BorderCol = (i == m_HoveredIndex) ? ColorRGBA(1.0f, 0.72f, 0.0f, 1.0f * m_OpenProgress) : ColorRGBA(1.0f, 0.55f, 0.0f, 0.45f * m_OpenProgress);

		Graphics()->DrawRect(BoxX, BoxY, BoxW, BoxH, BgCol, IGraphics::CORNER_ALL, Radius);
		Graphics()->DrawRectOutline(BoxX, BoxY, BoxW, BoxH, BorderCol, IGraphics::CORNER_ALL, IGraphics::SIDE_ALL, Radius);

		// Title
		const char *pTitle = m_vSlots[i].m_aTitle;
		const float FontSize = (5.5f + Hover * 0.6f) * Scale;
		const float TextW = TextRender()->TextWidth(FontSize, pTitle, -1, -1.0f);

		if(i == m_HoveredIndex)
			TextRender()->TextColor(1.0f, 0.88f, 0.40f, m_OpenProgress);
		else
			TextRender()->TextColor(0.95f, 0.95f, 0.95f, m_OpenProgress);

		TextRender()->Text(ItemCenter.x - TextW * 0.5f, ItemCenter.y - FontSize * 0.5f, FontSize, pTitle, -1.0f);
	}

	TextRender()->TextColor(TextRender()->DefaultTextColor());
}
