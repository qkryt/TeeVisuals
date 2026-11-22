#include "player_lists.h"

#include <base/color.h>
#include <base/system.h>
#include <engine/shared/config.h>
#include <engine/shared/json.h>
#include <engine/storage.h>
#include <game/client/gameclient.h>

#include <cstring>

static const char *LIST_FILE = "teevisuals_players.json";

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

void CPlayerLists::Load()
{
	m_vPlayers.clear();
	m_vClans.clear();

	void *pFileData = nullptr;
	unsigned FileSize = 0;
	if(!Storage()->ReadFile(LIST_FILE, IStorage::TYPE_ALL, &pFileData, &FileSize))
		return;

	json_settings Settings{};
	char aError[256];
	json_value *pJson = JsonParseEx(&Settings, static_cast<const json_char *>(pFileData), FileSize, aError);
	free(pFileData);

	if(!pJson)
		return;

	const json_value &Players = (*pJson)["players"];
	if(Players.type == json_array)
	{
		for(unsigned i = 0; i < Players.u.array.length; i++)
		{
			const json_value &Entry = Players[i];
			if(Entry.type != json_object)
				continue;

			const char *pName = (const char *)Entry["name"];
			const char *pReason = (const char *)Entry["reason"];
			int Cat = (int)(json_int_t)Entry["category"];

			if(pName && pName[0])
			{
				SPlayerEntry Pe;
				str_copy(Pe.m_aName, pName);
				str_copy(Pe.m_aReason, pReason ? pReason : "");
				Pe.m_Category = (EPlayerListCategory)std::clamp(Cat, (int)CATEGORY_WAR, (int)CATEGORY_HELPER);
				m_vPlayers.push_back(Pe);
			}
		}
	}

	const json_value &Clans = (*pJson)["clans"];
	if(Clans.type == json_array)
	{
		for(unsigned i = 0; i < Clans.u.array.length; i++)
		{
			const json_value &Entry = Clans[i];
			if(Entry.type != json_object)
				continue;

			const char *pClan = (const char *)Entry["clan"];
			int Cat = (int)(json_int_t)Entry["category"];

			if(pClan && pClan[0])
			{
				SClanEntry Ce;
				str_copy(Ce.m_aClan, pClan);
				Ce.m_Category = (EPlayerListCategory)std::clamp(Cat, (int)CATEGORY_WAR, (int)CATEGORY_HELPER);
				m_vClans.push_back(Ce);
			}
		}
	}

	json_value_free(pJson);
}

void CPlayerLists::Save()
{
	IOHANDLE File = Storage()->OpenFile(LIST_FILE, IOFLAG_WRITE, IStorage::TYPE_SAVE);
	if(!File)
		return;

	char aBuf[2048];
	io_write(File, "{\n  \"players\": [\n", 16);
	for(size_t i = 0; i < m_vPlayers.size(); i++)
	{
		const auto &P = m_vPlayers[i];
		str_format(aBuf, sizeof(aBuf), "    {\"name\": \"%s\", \"reason\": \"%s\", \"category\": %d}%s\n",
			P.m_aName, P.m_aReason, (int)P.m_Category, (i + 1 < m_vPlayers.size()) ? "," : "");
		io_write(File, aBuf, str_length(aBuf));
	}
	io_write(File, "  ],\n  \"clans\": [\n", 15);
	for(size_t i = 0; i < m_vClans.size(); i++)
	{
		const auto &C = m_vClans[i];
		str_format(aBuf, sizeof(aBuf), "    {\"clan\": \"%s\", \"category\": %d}%s\n",
			C.m_aClan, (int)C.m_Category, (i + 1 < m_vClans.size()) ? "," : "");
		io_write(File, aBuf, str_length(aBuf));
	}
	io_write(File, "  ]\n}\n", 5);
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
			Save();
			return true;
		}
	}

	SPlayerEntry Entry;
	str_copy(Entry.m_aName, pName);
	str_copy(Entry.m_aReason, pReason ? pReason : "");
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

	SClanEntry Entry;
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
		return ColorRGBA(1.0f, 0.25f, 0.25f, 1.0f); // Crimson Red
	case CATEGORY_TEAM:
		return ColorRGBA(0.0f, 0.9f, 1.0f, 1.0f); // Cyan
	case CATEGORY_HELPER:
		return ColorRGBA(1.0f, 0.8f, 0.1f, 1.0f); // Warm Gold
	default:
		return DefaultColor;
	}
}

const char *CPlayerLists::GetCategoryTag(EPlayerListCategory Cat) const
{
	switch(Cat)
	{
	case CATEGORY_WAR:
		return "[WAR]";
	case CATEGORY_TEAM:
		return "[TEAM]";
	case CATEGORY_HELPER:
		return "[HELP]";
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

bool CPlayerLists::OnChatCommand(const char *pLine)
{
	if(!pLine || (pLine[0] != '.' && pLine[0] != '!'))
		return false;

	const bool Broadcast = (pLine[0] == '!');
	const char *pCmdLine = pLine + 1;

	char aCmd[64];
	char aArg1[64] = "";
	char aArg2[128] = "";

	// Split command and arguments
	int i = 0;
	while(pCmdLine[i] && pCmdLine[i] != ' ' && i < (int)sizeof(aCmd) - 1)
	{
		aCmd[i] = pCmdLine[i];
		i++;
	}
	aCmd[i] = '\0';

	while(pCmdLine[i] == ' ')
		i++;

	int j = 0;
	while(pCmdLine[i] && pCmdLine[i] != ' ' && j < (int)sizeof(aArg1) - 1)
	{
		aArg1[j] = pCmdLine[i];
		i++;
		j++;
	}
	aArg1[j] = '\0';

	while(pCmdLine[i] == ' ')
		i++;

	str_copy(aArg2, &pCmdLine[i]);

	char aFeedback[256] = "";

	if(str_comp_nocase(aCmd, "addwar") == 0)
	{
		if(!aArg1[0])
			str_format(aFeedback, sizeof(aFeedback), "Usage: %caddwar <nick> [reason]", pLine[0]);
		else
		{
			AddPlayer(aArg1, aArg2, CATEGORY_WAR);
			if(aArg2[0])
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added '%s' to War list (Reason: %s)", aArg1, aArg2);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added '%s' to War list", aArg1);
		}
	}
	else if(str_comp_nocase(aCmd, "delwar") == 0)
	{
		if(!aArg1[0])
			str_format(aFeedback, sizeof(aFeedback), "Usage: %cdelwar <nick>", pLine[0]);
		else
		{
			if(RemovePlayer(aArg1, CATEGORY_WAR))
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Removed '%s' from War list", aArg1);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] '%s' was not in War list", aArg1);
		}
	}
	else if(str_comp_nocase(aCmd, "addteam") == 0)
	{
		if(!aArg1[0])
			str_format(aFeedback, sizeof(aFeedback), "Usage: %caddteam <nick> [reason]", pLine[0]);
		else
		{
			AddPlayer(aArg1, aArg2, CATEGORY_TEAM);
			if(aArg2[0])
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added '%s' to Team list (Reason: %s)", aArg1, aArg2);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added '%s' to Team list", aArg1);
		}
	}
	else if(str_comp_nocase(aCmd, "delteam") == 0)
	{
		if(!aArg1[0])
			str_format(aFeedback, sizeof(aFeedback), "Usage: %cdelteam <nick>", pLine[0]);
		else
		{
			if(RemovePlayer(aArg1, CATEGORY_TEAM))
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Removed '%s' from Team list", aArg1);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] '%s' was not in Team list", aArg1);
		}
	}
	else if(str_comp_nocase(aCmd, "addhelper") == 0)
	{
		if(!aArg1[0])
			str_format(aFeedback, sizeof(aFeedback), "Usage: %caddhelper <nick> [reason]", pLine[0]);
		else
		{
			AddPlayer(aArg1, aArg2, CATEGORY_HELPER);
			if(aArg2[0])
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added '%s' to Helper list (Reason: %s)", aArg1, aArg2);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added '%s' to Helper list", aArg1);
		}
	}
	else if(str_comp_nocase(aCmd, "delhelper") == 0)
	{
		if(!aArg1[0])
			str_format(aFeedback, sizeof(aFeedback), "Usage: %cdelhelper <nick>", pLine[0]);
		else
		{
			if(RemovePlayer(aArg1, CATEGORY_HELPER))
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Removed '%s' from Helper list", aArg1);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] '%s' was not in Helper list", aArg1);
		}
	}
	else if(str_comp_nocase(aCmd, "addclanwar") == 0)
	{
		if(!aArg1[0])
			str_format(aFeedback, sizeof(aFeedback), "Usage: %caddclanwar <clan>", pLine[0]);
		else
		{
			AddClan(aArg1, CATEGORY_WAR);
			str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added Clan '%s' to War list", aArg1);
		}
	}
	else if(str_comp_nocase(aCmd, "delclanwar") == 0)
	{
		if(!aArg1[0])
			str_format(aFeedback, sizeof(aFeedback), "Usage: %cdelclanwar <clan>", pLine[0]);
		else
		{
			if(RemoveClan(aArg1, CATEGORY_WAR))
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Removed Clan '%s' from War list", aArg1);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Clan '%s' was not in War list", aArg1);
		}
	}
	else if(str_comp_nocase(aCmd, "addclanteam") == 0)
	{
		if(!aArg1[0])
			str_format(aFeedback, sizeof(aFeedback), "Usage: %caddclanteam <clan>", pLine[0]);
		else
		{
			AddClan(aArg1, CATEGORY_TEAM);
			str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Added Clan '%s' to Team list", aArg1);
		}
	}
	else if(str_comp_nocase(aCmd, "delclanteam") == 0)
	{
		if(!aArg1[0])
			str_format(aFeedback, sizeof(aFeedback), "Usage: %cdelclanteam <clan>", pLine[0]);
		else
		{
			if(RemoveClan(aArg1, CATEGORY_TEAM))
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Removed Clan '%s' from Team list", aArg1);
			else
				str_format(aFeedback, sizeof(aFeedback), "[TeeVisuals] Clan '%s' was not in Team list", aArg1);
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
	else
	{
		// Not one of our commands
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
