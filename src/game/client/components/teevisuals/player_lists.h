#ifndef GAME_CLIENT_COMPONENTS_TEEVISUALS_PLAYER_LISTS_H
#define GAME_CLIENT_COMPONENTS_TEEVISUALS_PLAYER_LISTS_H

#include <base/color.h>
#include <game/client/component.h>

#include <string>
#include <vector>

enum EPlayerListCategory
{
	CATEGORY_NONE = 0,
	CATEGORY_WAR = 1,
	CATEGORY_TEAM = 2,
	CATEGORY_HELPER = 3
};

struct SPlayerEntry
{
	char m_aName[64];
	char m_aReason[128];
	int64_t m_AddedAt;
	EPlayerListCategory m_Category;
};

struct SClanEntry
{
	char m_aClan[64];
	EPlayerListCategory m_Category;
};

class CPlayerLists : public CComponent
{
private:
	std::vector<SPlayerEntry> m_vPlayers;
	std::vector<SClanEntry> m_vClans;

	void Save();
	void Load();
	void PrintList(EPlayerListCategory Category);
	void PrintAllLists();
	void PrintHelp();

	bool ParseNickAndReason(const char *pInput, char *pNick, size_t NickSize, char *pReason, size_t ReasonSize);

public:
	CPlayerLists();
	int Sizeof() const override { return sizeof(*this); }

	void OnInit() override;
	void OnReset() override;

	bool AddPlayer(const char *pName, const char *pReason, EPlayerListCategory Category);
	bool RemovePlayer(const char *pName, EPlayerListCategory Category = CATEGORY_NONE);

	bool AddClan(const char *pClan, EPlayerListCategory Category);
	bool RemoveClan(const char *pClan, EPlayerListCategory Category = CATEGORY_NONE);

	EPlayerListCategory GetPlayerCategory(const char *pName, const char *pClan) const;
	ColorRGBA GetCategoryColor(EPlayerListCategory Cat, ColorRGBA DefaultColor) const;
	const char *GetCategoryTag(EPlayerListCategory Cat) const;

	bool OnChatCommand(const char *pLine);

	const std::vector<SPlayerEntry> &GetPlayers() const { return m_vPlayers; }
	const std::vector<SClanEntry> &GetClans() const { return m_vClans; }

	void ClearAll();
};

#endif
