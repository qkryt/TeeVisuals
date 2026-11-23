#include "player_lists.h"

#include <base/color.h>
#include <base/system.h>
#include <engine/shared/config.h>
#include <engine/shared/json.h>
#include <engine/storage.h>
#include <game/client/gameclient.h>

#include <algorithm>
#include <cstring>

static const char *LIST_FILE_JSON = "teevisuals_lists.json";
static const char *LIST_FILE_LEGACY = "teevisuals_players.json";

CPlayerLists::CPlayerLists()
{
}

void CPlayerLists::OnInit()
{
	Load();
}

void CPlayerLists::OnReset()
{
}

void CPlayerLists::ClearAll()
{
	m_vPlayers.clear();
	m_vClans.clear();
	Save();
}

void CPlayerLists::Load()
{
	m_vPlayers.clear();
	m_vClans.clear();

	void *pFileData = nullptr;
	unsigned FileSize = 0;

	// Try reading primary JSON file, then fallback to legacy
	if(!Storage()->ReadFile(LIST_FILE_JSON, IStorage::TYPE_ALL, &pFileData, &FileSize))
	{
		if(!Storage()->ReadFile(LIST_FILE_LEGACY, IStorage::TYPE_ALL, &pFileData, &FileSize))
			return;
	}

	json_settings Settings{};
	char aError[256];
	json_value *pJson = JsonParseEx(&Settings, static_cast<const json_char *>(pFileData), FileSize, aError);
	free(pFileData);

	if(!pJson)
		return;

	const json_value &PlayersObj = (*pJson)["players"];
	if(PlayersObj.type == json_object)
	{
		const char *apCats[3] = {"war", "team", "helper"};
		EPlayerListCategory aCatEnum[3] = {CATEGORY_WAR, CATEGORY_TEAM, CATEGORY_HELPER};

		for(int c = 0; c < 3; c++)
		{
			const json_value &CatArr = PlayersObj[apCats[c]];
			if(CatArr.type == json_array)
			{
				for(unsigned i = 0; i < CatArr.u.array.length; i++)
				{
					const json_value &Entry = CatArr[i];
					if(Entry.type != json_object)
						continue;

					const char *pName = (const char *)Entry["name"];
					const char *pReason = (const char *)Entry["reason"];
					int64_t AddedAt = (int64_t)(json_int_t)Entry["added_at"];

					if(pName && pName[0])
					{
						SPlayerEntry Pe{};
						str_copy(Pe.m_aName, pName);
						str_copy(Pe.m_aReason, pReason ? pReason : "");
						Pe.m_AddedAt = AddedAt ? AddedAt : time_timestamp();
						Pe.m_Category = aCatEnum[c];
						m_vPlayers.push_back(Pe);
					}
				}
			}
		}
	}
	else if(PlayersObj.type == json_array)
	{
		// Fallback array format parser
		for(unsigned i = 0; i < PlayersObj.u.array.length; i++)
		{
			const json_value &Entry = PlayersObj[i];
			if(Entry.type != json_object)
				continue;

			const char *pName = (const char *)Entry["name"];
			const char *pReason = (const char *)Entry["reason"];
			int Cat = (int)(json_int_t)Entry["category"];

			if(pName && pName[0])
			{
				SPlayerEntry Pe{};
				str_copy(Pe.m_aName, pName);
				str_copy(Pe.m_aReason, pReason ? pReason : "");
				Pe.m_AddedAt = time_timestamp();
				Pe.m_Category = (EPlayerListCategory)std::clamp(Cat, (int)CATEGORY_WAR, (int)CATEGORY_HELPER);
				m_vPlayers.push_back(Pe);
			}
		}
	}

	const json_value &ClansObj = (*pJson)["clans"];
	if(ClansObj.type == json_object)
	{
		const char *apCats[2] = {"war", "team"};
		EPlayerListCategory aCatEnum[2] = {CATEGORY_WAR, CATEGORY_TEAM};

		for(int c = 0; c < 2; c++)
		{
			const json_value &CatArr = ClansObj[apCats[c]];
			if(CatArr.type == json_array)
			{
				for(unsigned i = 0; i < CatArr.u.array.length; i++)
				{
					const char *pClan = (const char *)CatArr[i];
					if(pClan && pClan[0])
					{
						SClanEntry Ce{};
						str_copy(Ce.m_aClan, pClan);
						Ce.m_Category = aCatEnum[c];
						m_vClans.push_back(Ce);
					}
				}
			}
		}
	}

	json_value_free(pJson);
}

void CPlayerLists::Save()
{
	IOHANDLE File = Storage()->OpenFile(LIST_FILE_JSON, IOFLAG_WRITE, IStorage::TYPE_SAVE);
	if(!File)
		return;

	char aBuf[512];
	io_write(File, "{\n  \"players\": {\n", 17);

	const char *apCats[3] = {"war", "team", "helper"};
	EPlayerListCategory aCatEnum[3] = {CATEGORY_WAR, CATEGORY_TEAM, CATEGORY_HELPER};

	for(int c = 0; c < 3; c++)
	{
		str_format(aBuf, sizeof(aBuf), "    \"%s\": [\n", apCats[c]);
		io_write(File, aBuf, str_length(aBuf));

		std::vector<SPlayerEntry> vFiltered;
		for(const auto &P : m_vPlayers)
		{
			if(P.m_Category == aCatEnum[c])
				vFiltered.push_back(P);
		}

		for(size_t i = 0; i < vFiltered.size(); i++)
		{
			const auto &P = vFiltered[i];
			str_format(aBuf, sizeof(aBuf), "      {\"name\": \"%s\", \"reason\": \"%s\", \"added_at\": %lld}%s\n",
				P.m_aName, P.m_aReason, (long long)P.m_AddedAt, (i + 1 < vFiltered.size()) ? "," : "");
			io_write(File, aBuf, str_length(aBuf));
		}

		str_format(aBuf, sizeof(aBuf), "    ]%s\n", (c < 2) ? "," : "");
		io_write(File, aBuf, str_length(aBuf));
	}

	io_write(File, "  },\n  \"clans\": {\n", 16);
	for(int c = 0; c < 2; c++)
	{
		str_format(aBuf, sizeof(aBuf), "    \"%s\": [\n", apCats[c]);
		io_write(File, aBuf, str_length(aBuf));

		std::vector<SClanEntry> vFiltered;
		for(const auto &Cl : m_vClans)
		{
			if(Cl.m_Category == aCatEnum[c])
				vFiltered.push_back(Cl);
		}

		for(size_t i = 0; i < vFiltered.size(); i++)
		{
			const auto &Cl = vFiltered[i];
			str_format(aBuf, sizeof(aBuf), "      \"%s\"%s\n", Cl.m_aClan, (i + 1 < vFiltered.size()) ? "," : "");
			io_write(File, aBuf, str_length(aBuf));
		}

		str_format(aBuf, sizeof(aBuf), "    ]%s\n", (c < 1) ? "," : "");
		io_write(File, aBuf, str_length(aBuf));
	}

	io_write(File, "  }\n}\n", 6);
	io_close(File);
}

bool CPlayerLists::AddPlayer(const char *pName, const char *pReason, EPlayerListCategory Category)
{
	if(!pName || !pName[0])
		return false;

	for(auto &P : m_vPlayers)
	{
		if(str_comp(P.m_aName, pName) == 0)
		{
			P.m_Category = Category;
			if(pReason && pReason[0])
				str_copy(P.m_aReason, pReason);
			P.m_AddedAt = time_timestamp();
			Save();
			return true;
		}
	}

	SPlayerEntry Entry{};
	str_copy(Entry.m_aName, pName);
	str_copy(Entry.m_aReason, pReason ? pReason : "");
	Entry.m_AddedAt = time_timestamp();
	Entry.m_Category = Category;
	m_vPlayers.push_back(Entry);
	Save();
	return true;
}

bool CPlayerLists::RemovePlayer(const char *pName, EPlayerListCategory Category)
{
	for(auto it = m_vPlayers.begin(); it != m_vPlayers.end(); ++it)
	{
		if(str_comp(it->m_aName, pName) == 0 && (Category == CATEGORY_NONE || it->m_Category == Category))
		{
			m_vPlayers.erase(it);
			Save();
			return true;
		}
	}
	return false;
}

bool CPlayerLists::AddClan(const char *pClan, EPlayerListCategory Category)
{
	if(!pClan || !pClan[0])
		return false;

	for(auto &C : m_vClans)
	{
		if(str_comp(C.m_aClan, pClan) == 0)
		{
			C.m_Category = Category;
			Save();
			return true;
		}
	}

	SClanEntry Entry{};
	str_copy(Entry.m_aClan, pClan);
	Entry.m_Category = Category;
	m_vClans.push_back(Entry);
	Save();
	return true;
}

bool CPlayerLists::RemoveClan(const char *pClan, EPlayerListCategory Category)
{
	for(auto it = m_vClans.begin(); it != m_vClans.end(); ++it)
	{
		if(str_comp(it->m_aClan, pClan) == 0 && (Category == CATEGORY_NONE || it->m_Category == Category))
		{
			m_vClans.erase(it);
			Save();
			return true;
		}
	}
	return false;
}

EPlayerListCategory CPlayerLists::GetPlayerCategory(const char *pName, const char *pClan) const
{
	if(!g_Config.m_ClTvPlayerLists)
		return CATEGORY_NONE;

	if(pName && pName[0])
	{
		for(const auto &P : m_vPlayers)
		{
			if(str_comp(P.m_aName, pName) == 0)
				return P.m_Category;
		}
	}

	if(pClan && pClan[0])
	{
		for(const auto &C : m_vClans)
		{
			if(str_comp(C.m_aClan, pClan) == 0)
				return C.m_Category;
		}
	}

	return CATEGORY_NONE;
}

ColorRGBA CPlayerLists::GetCategoryColor(EPlayerListCategory Cat, ColorRGBA DefaultColor) const
{
	switch(Cat)
	{
	case CATEGORY_WAR:
		return color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClTvWarColor));
	case CATEGORY_TEAM:
		return color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClTvTeamColor));
	case CATEGORY_HELPER:
		return color_cast<ColorRGBA>(ColorHSLA(g_Config.m_ClTvHelperColor));
	default:
		return DefaultColor;
	}
}

const char *CPlayerLists::GetCategoryTag(EPlayerListCategory Cat) const
{
	if(!g_Config.m_ClTvListsTag)
		return "";

	switch(Cat)
	{
	case CATEGORY_WAR:
		return "[WAR]";
	case CATEGORY_TEAM:
		return "[TEAM]";
	case CATEGORY_HELPER:
		return "[HELPER]";
	default:
		return "";
	}
}

void CPlayerLists::PrintList(EPlayerListCategory Category)
{
	const char *pCatName = Category == CATEGORY_WAR ? "War" : (Category == CATEGORY_TEAM ? "Team" : "Helper");
	char aBuf[256];
	str_format(aBuf, sizeof(aBuf), "--- %s List ---", pCatName);
	GameClient()->ClientMessage(aBuf);

	int Count = 0;
	for(const auto &P : m_vPlayers)
	{
		if(P.m_Category == Category)
		{
			Count++;
			if(P.m_aReason[0])
				str_format(aBuf, sizeof(aBuf), "  #%d: %s (Reason: %s)", Count, P.m_aName, P.m_aReason);
			else
				str_format(aBuf, sizeof(aBuf), "  #%d: %s", Count, P.m_aName);
			GameClient()->ClientMessage(aBuf);
		}
	}

	for(const auto &C : m_vClans)
	{
		if(C.m_Category == Category)
		{
			Count++;
			str_format(aBuf, sizeof(aBuf), "  [Clan]: %s", C.m_aClan);
			GameClient()->ClientMessage(aBuf);
		}
	}

	if(Count == 0)
	{
		str_copy(aBuf, "  (Empty)");
		GameClient()->ClientMessage(aBuf);
	}
}

void CPlayerLists::PrintAllLists()
{
	GameClient()->ClientMessage("=== TeeVisuals Social Lists ===");
	PrintList(CATEGORY_WAR);
	PrintList(CATEGORY_TEAM);
	PrintList(CATEGORY_HELPER);
}

void CPlayerLists::PrintHelp()
{
	GameClient()->ClientMessage("=== TeeVisuals Social Commands (. / !) ===");
	GameClient()->ClientMessage("  .addwar <nick> [reason] / .delwar <nick>");
	GameClient()->ClientMessage("  .addteam <nick> [reason] / .delteam <nick>");
	GameClient()->ClientMessage("  .addhelper <nick> [reason] / .delhelper <nick>");
	GameClient()->ClientMessage("  .addclanwar <clan> / .delclanwar <clan>");
	GameClient()->ClientMessage("  .addclanteam <clan> / .delclanteam <clan>");
	GameClient()->ClientMessage("  .warlist | .teamlist | .helperlist | .lists");
	GameClient()->ClientMessage("Tip: prefix . executes locally; prefix ! broadcasts result to chat.");
}

bool CPlayerLists::ParseNickAndReason(const char *pInput, char *pNick, size_t NickSize, char *pReason, size_t ReasonSize)
{
	pNick[0] = '\0';
	pReason[0] = '\0';

	while(*pInput == ' ')
		pInput++;

	if(!*pInput)
		return false;

	if(*pInput == '"')
	{
		// Quoted nickname
		pInput++;
		size_t i = 0;
		while(*pInput && *pInput != '"' && i < NickSize - 1)
		{
			pNick[i++] = *pInput++;
		}
		pNick[i] = '\0';
		if(*pInput == '"')
			pInput++;
	}
	else
	{
		size_t i = 0;
		while(*pInput && *pInput != ' ' && i < NickSize - 1)
		{
			pNick[i++] = *pInput++;
		}
		pNick[i] = '\0';
	}

	while(*pInput == ' ')
		pInput++;

	str_copy(pReason, pInput, ReasonSize);
	return pNick[0] != '\0';
}

bool CPlayerLists::OnChatCommand(const char *pLine)
{
	if(!pLine || (pLine[0] != '.' && pLine[0] != '!'))
		return false;

	const bool Broadcast = (pLine[0] == '!');
	const char *pCmdLine = pLine + 1;

	char aCmd[64];
	int i = 0;
	while(pCmdLine[i] && pCmdLine[i] != ' ' && i < (int)sizeof(aCmd) - 1)
	{
		aCmd[i] = pCmdLine[i];
		i++;
	}
	aCmd[i] = '\0';

	const char *pArgs = &pCmdLine[i];

	char aNick[64] = "";
	char aReason[128] = "";
	char aFeedback[256] = "";

	if(str_comp_nocase(aCmd, "addwar") == 0)
	{
		if(!ParseNickAndReason(pArgs, aNick, sizeof(aNick), aReason, sizeof(aReason)))
			str_format(aFeedback, sizeof(aFeedback), "Usage: %caddwar <nick> [reason]", pLine[0]);
		else
		{
			AddPlayer(aNick, aReason, CATEGORY_WAR);
			if(aReason[0])
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added '%s' to War list (Reason: %s)", aNick, aReason);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added '%s' to War list", aNick);
		}
	}
	else if(str_comp_nocase(aCmd, "delwar") == 0)
	{
		if(!ParseNickAndReason(pArgs, aNick, sizeof(aNick), aReason, sizeof(aReason)))
			str_format(aFeedback, sizeof(aFeedback), "Usage: %cdelwar <nick>", pLine[0]);
		else
		{
			if(RemovePlayer(aNick, CATEGORY_WAR))
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Removed '%s' from War list", aNick);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] '%s' was not in War list", aNick);
		}
	}
	else if(str_comp_nocase(aCmd, "addteam") == 0)
	{
		if(!ParseNickAndReason(pArgs, aNick, sizeof(aNick), aReason, sizeof(aReason)))
			str_format(aFeedback, sizeof(aFeedback), "Usage: %caddteam <nick> [reason]", pLine[0]);
		else
		{
			AddPlayer(aNick, aReason, CATEGORY_TEAM);
			if(aReason[0])
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added '%s' to Team list (Reason: %s)", aNick, aReason);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added '%s' to Team list", aNick);
		}
	}
	else if(str_comp_nocase(aCmd, "delteam") == 0)
	{
		if(!ParseNickAndReason(pArgs, aNick, sizeof(aNick), aReason, sizeof(aReason)))
			str_format(aFeedback, sizeof(aFeedback), "Usage: %cdelteam <nick>", pLine[0]);
		else
		{
			if(RemovePlayer(aNick, CATEGORY_TEAM))
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Removed '%s' from Team list", aNick);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] '%s' was not in Team list", aNick);
		}
	}
	else if(str_comp_nocase(aCmd, "addhelper") == 0)
	{
		if(!ParseNickAndReason(pArgs, aNick, sizeof(aNick), aReason, sizeof(aReason)))
			str_format(aFeedback, sizeof(aFeedback), "Usage: %caddhelper <nick> [reason]", pLine[0]);
		else
		{
			AddPlayer(aNick, aReason, CATEGORY_HELPER);
			if(aReason[0])
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added '%s' to Helper list (Reason: %s)", aNick, aReason);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added '%s' to Helper list", aNick);
		}
	}
	else if(str_comp_nocase(aCmd, "delhelper") == 0)
	{
		if(!ParseNickAndReason(pArgs, aNick, sizeof(aNick), aReason, sizeof(aReason)))
			str_format(aFeedback, sizeof(aFeedback), "Usage: %cdelhelper <nick>", pLine[0]);
		else
		{
			if(RemovePlayer(aNick, CATEGORY_HELPER))
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Removed '%s' from Helper list", aNick);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] '%s' was not in Helper list", aNick);
		}
	}
	else if(str_comp_nocase(aCmd, "addclanwar") == 0)
	{
		if(!ParseNickAndReason(pArgs, aNick, sizeof(aNick), aReason, sizeof(aReason)))
			str_format(aFeedback, sizeof(aFeedback), "Usage: %caddclanwar <clan>", pLine[0]);
		else
		{
			AddClan(aNick, CATEGORY_WAR);
			str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added Clan '%s' to War list", aNick);
		}
	}
	else if(str_comp_nocase(aCmd, "delclanwar") == 0)
	{
		if(!ParseNickAndReason(pArgs, aNick, sizeof(aNick), aReason, sizeof(aReason)))
			str_format(aFeedback, sizeof(aFeedback), "Usage: %cdelclanwar <clan>", pLine[0]);
		else
		{
			if(RemoveClan(aNick, CATEGORY_WAR))
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Removed Clan '%s' from War list", aNick);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Clan '%s' was not in War list", aNick);
		}
	}
	else if(str_comp_nocase(aCmd, "addclanteam") == 0)
	{
		if(!ParseNickAndReason(pArgs, aNick, sizeof(aNick), aReason, sizeof(aReason)))
			str_format(aFeedback, sizeof(aFeedback), "Usage: %caddclanteam <clan>", pLine[0]);
		else
		{
			AddClan(aNick, CATEGORY_TEAM);
			str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added Clan '%s' to Team list", aNick);
		}
	}
	else if(str_comp_nocase(aCmd, "delclanteam") == 0)
	{
		if(!ParseNickAndReason(pArgs, aNick, sizeof(aNick), aReason, sizeof(aReason)))
			str_format(aFeedback, sizeof(aFeedback), "Usage: %cdelclanteam <clan>", pLine[0]);
		else
		{
			if(RemoveClan(aNick, CATEGORY_TEAM))
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Removed Clan '%s' from Team list", aNick);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Clan '%s' was not in Team list", aNick);
		}
	}
	else if(str_comp_nocase(aCmd, "warlist") == 0)
	{
		PrintList(CATEGORY_WAR);
		return true;
	}
	else if(str_comp_nocase(aCmd, "teamlist") == 0)
	{
		PrintList(CATEGORY_TEAM);
		return true;
	}
	else if(str_comp_nocase(aCmd, "helperlist") == 0)
	{
		PrintList(CATEGORY_HELPER);
		return true;
	}
	else if(str_comp_nocase(aCmd, "lists") == 0)
	{
		PrintAllLists();
		return true;
	}
	else if(str_comp_nocase(aCmd, "tvhelp") == 0)
	{
		PrintHelp();
		return true;
	}
	else
	{
		// Unrecognized command - pass through to server if on '!' prefix
		return false;
	}

	if(aFeedback[0])
	{
		GameClient()->ClientMessage(aFeedback);
		if(Broadcast)
		{
			// Broadcast formatted output to server chat
			GameClient()->m_Chat.SendChat(0, aFeedback);
		}
	}

	return true;
}
