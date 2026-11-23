#include "bindwheel.h"
#include "teevisuals_common.h"

#include <base/color.h>
#include <engine/client.h>
#include <engine/console.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/shared/json.h>
#include <engine/storage.h>
#include <engine/textrender.h>
#include <game/client/gameclient.h>

#include <cmath>

static const float PI = 3.14159265358979323846f;
static const char *BINDWHEEL_CONFIG_FILE = "teevisuals_bindwheel.json";

CBindwheel::CBindwheel()
{
	m_Active = false;
	m_WasActive = false;
	m_OpenProgress = 0.0f;
	m_CursorPos = vec2(0.0f, 0.0f);
	m_HoveredIndex = -1;

	for(int i = 0; i < 16; i++)
		m_aSliceHoverAnim[i] = 0.0f;

	// Populate with 8 default versatile slots
	m_vSlots.push_back({"Good Game!", "say \"Good game!\"", SLOT_CHAT, TeeVisuals::COLOR_ORANGE});
	m_vSlots.push_back({"Heart <3", "emote 1", SLOT_EMOTE, TeeVisuals::COLOR_YELLOW});
	m_vSlots.push_back({"Help Me!", "say_team \"Help!\"", SLOT_CHAT, TeeVisuals::COLOR_WAR});
	m_vSlots.push_back({"Happy :)", "emote 3", SLOT_EMOTE, TeeVisuals::COLOR_TEAM});
	m_vSlots.push_back({"Nice Hook!", "say \"Nice hook!\"", SLOT_CHAT, TeeVisuals::COLOR_AMBER});
	m_vSlots.push_back({"Surprise :O", "emote 2", SLOT_EMOTE, TeeVisuals::COLOR_HELPER});
	m_vSlots.push_back({"Restart /r", "say \"/r\"", SLOT_COMMAND, TeeVisuals::COLOR_ORANGE});
	m_vSlots.push_back({"Toggle Ready", "ready_change", SLOT_COMMAND, TeeVisuals::COLOR_YELLOW});
}

void CBindwheel::OnInit()
{
	LoadConfig();
	OnReset();
}

void CBindwheel::OnReset()
{
	m_Active = false;
	m_WasActive = false;
	m_OpenProgress = 0.0f;
	m_CursorPos = vec2(0.0f, 0.0f);
	m_HoveredIndex = -1;
}

void CBindwheel::OnConsoleInit()
{
	Console()->Register("+tv_bindwheel", "", CFGFLAG_CLIENT, ConKeyBindwheel, this, "Open Smooth Bindwheel radial menu");
}

void CBindwheel::ConKeyBindwheel(IConsole::IResult *pResult, void *pUserData)
{
	CBindwheel *pSelf = (CBindwheel *)pUserData;
	pSelf->SetActive(pResult->GetInteger(0) != 0);
}

void CBindwheel::SetActive(bool Active)
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

void CBindwheel::ExecuteSelected()
{
	if(m_HoveredIndex >= 0 && m_HoveredIndex < (int)m_vSlots.size())
	{
		const char *pCmd = m_vSlots[m_HoveredIndex].m_aCommand;
		if(pCmd && pCmd[0])
		{
			Console()->ExecuteLine(pCmd, IConsole::CLIENT_ID_UNSPECIFIED);
		}
	}
}

bool CBindwheel::OnCursorMove(float x, float y, IInput::ECursorType CursorType)
{
	if(!m_Active)
		return false;

	m_CursorPos += vec2(x, y);
	return true;
}

bool CBindwheel::OnInput(const IInput::CEvent &Event)
{
	if(!m_Active)
		return false;

	// Mouse button 1 release executes selection
	if(Event.m_Flags & IInput::FLAG_RELEASE)
	{
		if(Event.m_Key == KEY_MOUSE_1)
		{
			SetActive(false);
			return true;
		}
	}

	return false;
}

void CBindwheel::TouchDown(vec2 Pos)
{
	m_Active = true;
	m_CursorPos = Pos;
}

void CBindwheel::TouchMove(vec2 Pos)
{
	if(m_Active)
		m_CursorPos = Pos;
}

void CBindwheel::TouchUp()
{
	if(m_Active)
		SetActive(false);
}

void CBindwheel::SetSlot(size_t Index, const char *pTitle, const char *pCmd, int Type)
{
	if(Index >= m_vSlots.size())
		m_vSlots.resize(Index + 1);

	str_copy(m_vSlots[Index].m_aTitle, pTitle ? pTitle : "");
	str_copy(m_vSlots[Index].m_aCommand, pCmd ? pCmd : "");
	m_vSlots[Index].m_Type = Type;
	SaveConfig();
}

void CBindwheel::LoadConfig()
{
	void *pFileData = nullptr;
	unsigned FileSize = 0;
	if(!Storage()->ReadFile(BINDWHEEL_CONFIG_FILE, IStorage::TYPE_ALL, &pFileData, &FileSize))
		return;

	json_settings Settings{};
	char aError[256];
	json_value *pJson = JsonParseEx(&Settings, static_cast<const json_char *>(pFileData), FileSize, aError);
	free(pFileData);

	if(!pJson)
		return;

	const json_value &Slots = (*pJson)["slots"];
	if(Slots.type == json_array && Slots.u.array.length > 0)
	{
		m_vSlots.clear();
		for(unsigned i = 0; i < Slots.u.array.length; i++)
		{
			const json_value &Entry = Slots[i];
			if(Entry.type != json_object)
				continue;

			const char *pTitle = (const char *)Entry["title"];
			const char *pCmd = (const char *)Entry["command"];
			int Type = (int)(json_int_t)Entry["type"];

			if(pTitle && pTitle[0])
			{
				SWheelSlot Slot{};
				str_copy(Slot.m_aTitle, pTitle);
				str_copy(Slot.m_aCommand, pCmd ? pCmd : "");
				Slot.m_Type = Type;
				Slot.m_Color = (i % 2 == 0) ? TeeVisuals::COLOR_ORANGE : TeeVisuals::COLOR_AMBER;
				m_vSlots.push_back(Slot);
			}
		}
	}

	json_value_free(pJson);
}

void CBindwheel::SaveConfig()
{
	IOHANDLE File = Storage()->OpenFile(BINDWHEEL_CONFIG_FILE, IOFLAG_WRITE, IStorage::TYPE_SAVE);
	if(!File)
		return;

	char aBuf[512];
	io_write(File, "{\n  \"slots\": [\n", 14);
	for(size_t i = 0; i < m_vSlots.size(); i++)
	{
		const auto &Slot = m_vSlots[i];
		str_format(aBuf, sizeof(aBuf), "    {\"title\": \"%s\", \"command\": \"%s\", \"type\": %d}%s\n",
			Slot.m_aTitle, Slot.m_aCommand, Slot.m_Type, (i + 1 < m_vSlots.size()) ? "," : "");
		io_write(File, aBuf, str_length(aBuf));
	}
	io_write(File, "  ]\n}\n", 5);
	io_close(File);
}

void CBindwheel::OnRender()
{
	if(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		return;

	const float DeltaTime = std::clamp(Client()->RenderFrameTime(), 0.001f, 0.1f);
	const float AnimSpeed = (float)g_Config.m_ClTvBindwheelSpeed * 0.12f;

	if(m_Active)
		m_OpenProgress += (1.0f - m_OpenProgress) * std::clamp(DeltaTime * AnimSpeed, 0.0f, 1.0f);
	else
		m_OpenProgress += (0.0f - m_OpenProgress) * std::clamp(DeltaTime * AnimSpeed, 0.0f, 1.0f);

	if(m_OpenProgress < 0.005f)
		return;

	const float ScreenWidth = 300.0f * Graphics()->ScreenAspect();
	const float ScreenHeight = 300.0f;
	Graphics()->MapScreenToSize(ScreenWidth, ScreenHeight);

	const vec2 Center(ScreenWidth * 0.5f, ScreenHeight * 0.5f);
	const int AllowedSlots = std::clamp(g_Config.m_ClTvBindwheelSlots, 4, 12);
	const int NumSlots = std::min((int)m_vSlots.size(), AllowedSlots);
	if(NumSlots <= 0)
		return;

	// Determine hovered slot from cursor position
	const float CursorDist = length(m_CursorPos);
	m_HoveredIndex = -1;

	// Deadzone in the center (radius 18 units) = Cancel
	if(CursorDist > 18.0f)
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
		m_aSliceHoverAnim[i] += (Target - m_aSliceHoverAnim[i]) * std::clamp(DeltaTime * 18.0f, 0.0f, 1.0f);
	}

	// Cubic-out easing
	const float Eased = TeeVisuals::EaseOutCubic(std::clamp(m_OpenProgress, 0.0f, 1.0f));
	const float Scale = 0.6f + 0.4f * Eased;
	const float Alpha = std::clamp(m_OpenProgress, 0.0f, 1.0f);
	const float BaseRadius = (float)g_Config.m_ClTvBindwheelRadius * 0.45f * Scale;
	const float SlotAngle = (2.0f * PI) / (float)NumSlots;

	// Render center circle with "Cancel" in center
	ColorRGBA CenterBg(0.07f, 0.06f, 0.05f, 0.88f * Alpha);
	ColorRGBA CenterOutline = (m_HoveredIndex == -1) ? TeeVisuals::COLOR_ORANGE.WithAlpha(0.9f * Alpha) : TeeVisuals::COLOR_AMBER.WithAlpha(0.4f * Alpha);
	Graphics()->DrawRect(Center.x - 16.0f * Scale, Center.y - 16.0f * Scale, 32.0f * Scale, 32.0f * Scale, CenterBg, IGraphics::CORNER_ALL, 16.0f * Scale);
	Graphics()->DrawRectOutline(Center.x - 16.0f * Scale, Center.y - 16.0f * Scale, 32.0f * Scale, 32.0f * Scale, CenterOutline, IGraphics::CORNER_ALL, IGraphics::SIDE_ALL, 16.0f * Scale);

	const char *pCenterLabel = "X";
	float CenterTextW = TextRender()->TextWidth(7.0f * Scale, pCenterLabel, -1, -1.0f);
	TextRender()->TextColor(1.0f, 0.7f, 0.2f, Alpha);
	TextRender()->Text(Center.x - CenterTextW * 0.5f, Center.y - 3.5f * Scale, 7.0f * Scale, pCenterLabel, -1.0f);

	// Render wheel buttons
	for(int i = 0; i < NumSlots; i++)
	{
		const float Angle = -PI * 0.5f + (float)i * SlotAngle;
		const float Hover = m_aSliceHoverAnim[i];
		const float ItemRadius = BaseRadius + Hover * 8.0f;

		const vec2 ItemCenter = Center + vec2(std::cos(Angle), std::sin(Angle)) * ItemRadius;

		const float BoxW = (48.0f + Hover * 6.0f) * Scale;
		const float BoxH = (16.0f + Hover * 3.0f) * Scale;
		const float BoxX = ItemCenter.x - BoxW * 0.5f;
		const float BoxY = ItemCenter.y - BoxH * 0.5f;
		const float Radius = BoxH * 0.5f;

		// Colors
		ColorRGBA BgCol = (i == m_HoveredIndex) ? ColorRGBA(0.20f, 0.15f, 0.06f, 0.95f * Alpha) : ColorRGBA(0.08f, 0.08f, 0.10f, 0.82f * Alpha);
		ColorRGBA BorderCol = (i == m_HoveredIndex) ? TeeVisuals::COLOR_YELLOW.WithAlpha(1.0f * Alpha) : TeeVisuals::COLOR_ORANGE.WithAlpha(0.6f * Alpha);

		Graphics()->DrawRect(BoxX, BoxY, BoxW, BoxH, BgCol, IGraphics::CORNER_ALL, Radius);
		Graphics()->DrawRectOutline(BoxX, BoxY, BoxW, BoxH, BorderCol, IGraphics::CORNER_ALL, IGraphics::SIDE_ALL, Radius);

		// Title
		const char *pTitle = m_vSlots[i].m_aTitle;
		const float FontSize = (5.5f + Hover * 0.8f) * Scale;
		const float TextW = TextRender()->TextWidth(FontSize, pTitle, -1, -1.0f);

		if(i == m_HoveredIndex)
			TextRender()->TextColor(TeeVisuals::COLOR_YELLOW.r, TeeVisuals::COLOR_YELLOW.g, TeeVisuals::COLOR_YELLOW.b, Alpha);
		else
			TextRender()->TextColor(0.95f, 0.95f, 0.95f, Alpha);

		TextRender()->Text(ItemCenter.x - TextW * 0.5f, ItemCenter.y - FontSize * 0.5f, FontSize, pTitle, -1.0f);
	}

	TextRender()->TextColor(TextRender()->DefaultTextColor());
}
