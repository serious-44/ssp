/*****************************************************************************
 * PokerTH - The open source texas holdem engine                             *
 * Copyright (C) 2006-2012 Felix Hammer, Florian Thauer, Lothar May          *
 *                                                                           *
 * This program is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU Affero General Public License as            *
 * published by the Free Software Foundation, either version 3 of the        *
 * License, or (at your option) any later version.                           *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU Affero General Public License for more details.                       *
 *                                                                           *
 * You should have received a copy of the GNU Affero General Public License  *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                           *
 *                                                                           *
 * Additional permission under GNU AGPL version 3 section 7                  *
 *                                                                           *
 * If you modify this program, or any covered work, by linking or            *
 * combining it with the OpenSSL project's OpenSSL library (or a             *
 * modified version of that library), containing parts covered by the        *
 * terms of the OpenSSL or SSLeay licenses, the authors of PokerTH           *
 * (Felix Hammer, Florian Thauer, Lothar May) grant you additional           *
 * permission to convey the resulting work.                                  *
 * Corresponding Source for a non-source form of such a combination          *
 * shall include the source code for the parts of OpenSSL used as well       *
 * as that of the covered work.                                              *
 *****************************************************************************/

#include "configfile.h"

using namespace std;

ConfigFile::ConfigFile(char *argv0, bool readonly)
{
/*
	configList.push_back(ConfigInfo("ConfigRevision", CONFIG_TYPE_INT, tempIntToString.str()));
	configList.push_back(ConfigInfo("AppDataDir", CONFIG_TYPE_STRING, myQtToolsInterface->getDataPathStdString(myArgv0)));
	configList.push_back(ConfigInfo("Language", CONFIG_TYPE_INT, myQtToolsInterface->getDefaultLanguage()));
	configList.push_back(ConfigInfo("ShowLeftToolBox", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("ShowCountryFlagInAvatar", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("ShowPingStateInAvatar", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("ShowRightToolBox", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("ShowFadeOutCardsAnimation", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("ShowFlipCardsAnimation", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("ShowBlindButtons", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("ShowCardsChanceMonitor", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("DontTranslateInternationalPokerStringsFromStyle", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("DisableSplashScreenOnStartup", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("AccidentallyCallBlocker", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("DontHideAvatarsOfIgnored", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("DisableChatEmoticons", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("AntiPeekMode", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("AlternateFKeysUserActionMode", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("EnableBetInputFocusSwitch", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("FlipsideTux", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("FlipsideOwn", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("FlipsideOwnFile", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("GameTableStylesList", CONFIG_TYPE_STRING_LIST, "GameTableStyles"));
	configList.push_back(ConfigInfo("CurrentGameTableStyle", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("CardDeckStylesList", CONFIG_TYPE_STRING_LIST, "CardDeckStyles"));
	configList.push_back(ConfigInfo("PlayerTooltips", CONFIG_TYPE_STRING_LIST, "PlayerTooltips"));
	configList.push_back(ConfigInfo("CurrentCardDeckStyle", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("LastGameTableStyleDir", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("LastCardDeckStyleDir", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("PlaySoundEffects", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("SoundVolume", CONFIG_TYPE_INT, "8"));
	configList.push_back(ConfigInfo("PlayGameActions", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("PlayLobbyChatNotification", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("PlayNetworkGameNotification", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("PlayBlindRaiseNotification", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("NumberOfPlayers", CONFIG_TYPE_INT, "10"));
	configList.push_back(ConfigInfo("StartCash", CONFIG_TYPE_INT, "5000"));
	configList.push_back(ConfigInfo("FirstSmallBlind", CONFIG_TYPE_INT, "10"));
	configList.push_back(ConfigInfo("RaiseBlindsAtHands", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("RaiseBlindsAtMinutes", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("RaiseSmallBlindEveryHands", CONFIG_TYPE_INT, "8"));
	configList.push_back(ConfigInfo("RaiseSmallBlindEveryMinutes", CONFIG_TYPE_INT, "5"));
	configList.push_back(ConfigInfo("AlwaysDoubleBlinds", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("ManualBlindsOrder", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("ManualBlindsList", CONFIG_TYPE_INT_LIST, "Blind"));
	configList.push_back(ConfigInfo("AfterMBAlwaysDoubleBlinds", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("AfterMBAlwaysRaiseAbout", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("AfterMBAlwaysRaiseValue", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("AfterMBStayAtLastBlind", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("GameSpeed", CONFIG_TYPE_INT, "4"));
	configList.push_back(ConfigInfo("PauseBetweenHands", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("ShowGameSettingsDialogOnNewGame", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("NetNumberOfPlayers", CONFIG_TYPE_INT, "10"));
	configList.push_back(ConfigInfo("NetStartCash", CONFIG_TYPE_INT, "3000"));
	configList.push_back(ConfigInfo("NetFirstSmallBlind", CONFIG_TYPE_INT, "10"));
	configList.push_back(ConfigInfo("NetRaiseBlindsAtHands", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("NetRaiseBlindsAtMinutes", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("NetRaiseSmallBlindEveryHands", CONFIG_TYPE_INT, "8"));
	configList.push_back(ConfigInfo("NetRaiseSmallBlindEveryMinutes", CONFIG_TYPE_INT, "5"));
	configList.push_back(ConfigInfo("NetAlwaysDoubleBlinds", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("NetManualBlindsOrder", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("NetManualBlindsList", CONFIG_TYPE_INT_LIST, "NetBlind"));
	configList.push_back(ConfigInfo("NetAfterMBAlwaysDoubleBlinds", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("NetAfterMBAlwaysRaiseAbout", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("NetAfterMBAlwaysRaiseValue", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("NetAfterMBStayAtLastBlind", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("NetGameSpeed", CONFIG_TYPE_INT, "4"));
	configList.push_back(ConfigInfo("NetDelayBetweenHands", CONFIG_TYPE_INT, "7"));
	configList.push_back(ConfigInfo("NetTimeOutPlayerAction", CONFIG_TYPE_INT, "20"));
	configList.push_back(ConfigInfo("NetAutoLeaveGameAfterFinish", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("ServerPassword", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("ServerUseIpv6", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("ServerUseSctp", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("ServerUseWebSocket", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("ServerPort", CONFIG_TYPE_INT, "7234"));
	configList.push_back(ConfigInfo("ServerWebSocketPort", CONFIG_TYPE_INT, "7233"));
	configList.push_back(ConfigInfo("ServerWebSocketResource", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("ServerWebSocketOrigin", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("ServerUsePutAvatars", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("ServerPutAvatarsAddress", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("ServerPutAvatarsUser", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("ServerPutAvatarsPassword", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("ServerBruteForceProtection", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("InternetServerConfigMode", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("InternetServerListAddress", CONFIG_TYPE_STRING, "pokerth.net/serverlist.xml.z"));
	configList.push_back(ConfigInfo("InternetServerAddress", CONFIG_TYPE_STRING, "pokerth.6dns.org"));
	configList.push_back(ConfigInfo("InternetServerPort", CONFIG_TYPE_INT, "7234"));
	configList.push_back(ConfigInfo("InternetServerUseIpv6", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("InternetServerUseSctp", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("UseAvatarServer", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("AvatarServerAddress", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("UseInternetGamePassword", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("InternetGamePassword", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("InternetGameType", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("InternetGameName", CONFIG_TYPE_STRING, "My Online Game"));
	configList.push_back(ConfigInfo("InternetGameAllowSpectators", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("UseLobbyChat", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("UseAdminIRC", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("AdminIRCServerAddress", CONFIG_TYPE_STRING, "chat.freenode.net"));
	configList.push_back(ConfigInfo("AdminIRCServerPort", CONFIG_TYPE_INT, "6667"));
	configList.push_back(ConfigInfo("AdminIRCChannel", CONFIG_TYPE_STRING, "#test"));
	configList.push_back(ConfigInfo("AdminIRCChannelPassword", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("AdminIRCServerUseIpv6", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("AdminIRCServerNick", CONFIG_TYPE_INT, "PokerTH_Admin"));
	configList.push_back(ConfigInfo("UseLobbyIRC", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("LobbyIRCServerAddress", CONFIG_TYPE_STRING, "chat.freenode.net"));
	configList.push_back(ConfigInfo("LobbyIRCServerPort", CONFIG_TYPE_INT, "6667"));
	configList.push_back(ConfigInfo("LobbyIRCChannel", CONFIG_TYPE_STRING, "#pokerth-lobby"));
	configList.push_back(ConfigInfo("LobbyIRCChannelPassword", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("LobbyIRCServerUseIpv6", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("LobbyIRCServerNick", CONFIG_TYPE_INT, "PokerTH_Lobby"));
	configList.push_back(ConfigInfo("UseChatCleaner", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("ChatCleanerHostAddress", CONFIG_TYPE_STRING, "localhost"));
	configList.push_back(ConfigInfo("ChatCleanerPort", CONFIG_TYPE_INT, "4327"));
	configList.push_back(ConfigInfo("ChatCleanerClientAuth", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("ChatCleanerServerAuth", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("ChatCleanerUseIpv6", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("MyName", CONFIG_TYPE_STRING, "Human Player"));
	configList.push_back(ConfigInfo("MyAvatar", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("MyRememberedNameDuringGuestLogin", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("Opponent1Name", CONFIG_TYPE_STRING, "Player 1"));
	configList.push_back(ConfigInfo("Opponent1Avatar", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("Opponent2Name", CONFIG_TYPE_STRING, "Player 2"));
	configList.push_back(ConfigInfo("Opponent2Avatar", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("Opponent3Name", CONFIG_TYPE_STRING, "Player 3"));
	configList.push_back(ConfigInfo("Opponent3Avatar", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("Opponent4Name", CONFIG_TYPE_STRING, "Player 4"));
	configList.push_back(ConfigInfo("Opponent4Avatar", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("Opponent5Name", CONFIG_TYPE_STRING, "Player 5"));
	configList.push_back(ConfigInfo("Opponent5Avatar", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("Opponent6Name", CONFIG_TYPE_STRING, "Player 6"));
	configList.push_back(ConfigInfo("Opponent6Avatar", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("Opponent7Name", CONFIG_TYPE_STRING, "Player 7"));
	configList.push_back(ConfigInfo("Opponent7Avatar", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("Opponent8Name", CONFIG_TYPE_STRING, "Player 8"));
	configList.push_back(ConfigInfo("Opponent8Avatar", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("Opponent9Name", CONFIG_TYPE_STRING, "Player 9"));
	configList.push_back(ConfigInfo("Opponent9Avatar", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("LogOnOff", CONFIG_TYPE_INT, logOnOffDefault));
	configList.push_back(ConfigInfo("LogDir", CONFIG_TYPE_STRING, logDir));
	configList.push_back(ConfigInfo("LogStoreDuration", CONFIG_TYPE_INT, "2"));
	configList.push_back(ConfigInfo("LogInterval", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("UserDataDir", CONFIG_TYPE_STRING, dataDir));
	configList.push_back(ConfigInfo("CacheDir", CONFIG_TYPE_STRING, cacheDir));
	configList.push_back(ConfigInfo("CLA_NoWriteAccess", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("DisableBackToLobbyWarning", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("DlgGameLobbyGameListSortingSection", CONFIG_TYPE_INT, "2"));
	configList.push_back(ConfigInfo("DlgGameLobbyGameListSortingOrder", CONFIG_TYPE_INT, "1"));
	configList.push_back(ConfigInfo("DlgGameLobbyGameListFilterIndex", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("DlgGameLobbyNickListSortFilterIndex", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("GameTableFullScreenSave", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("GameTableHeightSave", CONFIG_TYPE_INT, "600"));
	configList.push_back(ConfigInfo("GameTableWidthSave", CONFIG_TYPE_INT, "1024"));
	configList.push_back(ConfigInfo("InternetLoginMode", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("InternetLoginPassword", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("InternetSavePassword", CONFIG_TYPE_INT, "0"));
	configList.push_back(ConfigInfo("IfInfoMessageShowList", CONFIG_TYPE_STRING_LIST, "Msg"));
	configList.push_back(ConfigInfo("PlayerIgnoreList", CONFIG_TYPE_STRING_LIST, "Player"));
	configList.push_back(ConfigInfo("DBServerAddress", CONFIG_TYPE_STRING, "127.0.0.1"));
	configList.push_back(ConfigInfo("DBServerUser", CONFIG_TYPE_STRING, "pokerth"));
	configList.push_back(ConfigInfo("DBServerPassword", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("DBServerDatabaseName", CONFIG_TYPE_STRING, "pokerth"));
	configList.push_back(ConfigInfo("DBServerEncryptionKey", CONFIG_TYPE_STRING, ""));
	configList.push_back(ConfigInfo("GameNameBadWordList", CONFIG_TYPE_STRING_LIST, "Regex"));
*/
}

string ConfigFile::readConfigString(string varName) const
{
    string tempString("");
    return tempString;
}

int ConfigFile::readConfigInt(string varName) const
{
    return 0;
}
