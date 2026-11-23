#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_BINDWHEEL_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_BINDWHEEL_H

#include <base/color.h>
#include <base/vmath.h>
#include <engine/console.h>
#include <game/client/component.h>

#include <string>
#include <vector>

enum EWheelSlotType
{
	SLOT_EMOTE = 0,
	SLOT_CHAT,
	SLOT_COMMAND
};

struct SWheelSlot
{
	char m_aTitle[32];
	char m_aCommand[128];
	int m_Type; // EWheelSlotType
	ColorRGBA m_Color;
};

class CBindwheel : public CComponent
{
private:
	bool m_Active;
	bool m_WasActive;
	float m_OpenProgress;
	vec2 m_CursorPos;
	int m_HoveredIndex;
	float m_aSliceHoverAnim[16];

	std::vector<SWheelSlot> m_vSlots;

	static void ConKeyBindwheel(IConsole::IResult *pResult, void *pUserData);

	void ExecuteSelected();
	void LoadConfig();
	void SaveConfig();

public:
	CBindwheel();
	int Sizeof() const override { return sizeof(*this); }

	void OnInit() override;
	void OnReset() override;
	void OnConsoleInit() override;
	void OnRender() override;

	bool OnCursorMove(float x, float y, IInput::ECursorType CursorType) override;
	bool OnInput(const IInput::CEvent &Event) override;

	void SetActive(bool Active);
	bool IsActive() const { return m_Active || m_OpenProgress > 0.01f; }

	std::vector<SWheelSlot> &GetSlots() { return m_vSlots; }
	const std::vector<SWheelSlot> &GetSlots() const { return m_vSlots; }
	void SetSlot(size_t Index, const char *pTitle, const char *pCmd, int Type = SLOT_COMMAND);

	// Touch support
	void TouchDown(vec2 Pos);
	void TouchMove(vec2 Pos);
	void TouchUp();
};

#endif
