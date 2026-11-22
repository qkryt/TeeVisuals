#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_SMOOTH_BINDWHEEL_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_SMOOTH_BINDWHEEL_H

#include <base/vmath.h>
#include <engine/console.h>
#include <game/client/component.h>

#include <vector>

struct SWheelSlot
{
	char m_aTitle[32];
	char m_aCommand[128];
};

class CSmoothBindwheel : public CComponent
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
	float EaseOutBack(float t) const;

public:
	CSmoothBindwheel();
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
	void SetSlot(size_t Index, const char *pTitle, const char *pCmd);
};

#endif
