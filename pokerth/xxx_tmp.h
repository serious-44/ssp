#ifndef XXX_TMP_H
#define XXX_TMP_H

#include <memory>
#include <string>
#include "game_defs.h"
#include "gamedata.h"
//#include "playerdata.h"
//#include "game.h"

class Game;

class MD5Buf {

};

typedef unsigned DB_id;
#define DB_ID_INVALID 0

struct ServerStats {
};

class ClientCallback
{
public:
        virtual ~ClientCallback();

        virtual void SignalNetClientConnect(int actionID) = 0;
        virtual void SignalNetClientGameInfo(int actionID) = 0;
        virtual void SignalNetClientError(int errorID, int osErrorID) = 0;
        virtual void SignalNetClientNotification(int notificationId) = 0;
        virtual void SignalNetClientStatsUpdate(const ServerStats &stats) = 0;
        virtual void SignalNetClientPingUpdate(unsigned minPing, unsigned avgPing, unsigned maxPing) = 0;
        virtual void SignalNetClientShowTimeoutDialog(NetTimeoutReason reason, unsigned remainingSec) = 0;
        virtual void SignalNetClientRemovedFromGame(int notificationId) = 0;

        virtual void SignalNetClientGameListNew(unsigned gameId) = 0;
        virtual void SignalNetClientGameListRemove(unsigned gameId) = 0;
        virtual void SignalNetClientGameListUpdateMode(unsigned gameId, GameMode mode) = 0;
        virtual void SignalNetClientGameListUpdateAdmin(unsigned gameId, unsigned adminPlayerId) = 0;
        virtual void SignalNetClientGameListPlayerJoined(unsigned gameId, unsigned playerId) = 0;
        virtual void SignalNetClientGameListPlayerLeft(unsigned gameId, unsigned playerId) = 0;
        virtual void SignalNetClientGameListSpectatorJoined(unsigned gameId, unsigned playerId) = 0;
        virtual void SignalNetClientGameListSpectatorLeft(unsigned gameId, unsigned playerId) = 0;

        virtual void SignalNetClientGameStart(std::shared_ptr<Game> game) = 0;
        virtual void SignalNetClientSelfJoined(unsigned playerId, const std::string &playerName, bool isGameAdmin) = 0;
        virtual void SignalNetClientPlayerJoined(unsigned playerId, const std::string &playerName, bool isGameAdmin) = 0;
        virtual void SignalNetClientPlayerChanged(unsigned playerId, const std::string &newPlayerName) = 0;
        virtual void SignalNetClientPlayerLeft(unsigned playerId, const std::string &playerName, int removeReason) = 0;
        virtual void SignalNetClientSpectatorJoined(unsigned playerId, const std::string &playerName) = 0;
        virtual void SignalNetClientSpectatorLeft(unsigned playerId, const std::string &playerName, int removeReason) = 0;
        virtual void SignalNetClientNewGameAdmin(unsigned playerId, const std::string &playerName) = 0;

        virtual void SignalNetClientGameChatMsg(const std::string &playerName, const std::string &msg) = 0;
        virtual void SignalNetClientLobbyChatMsg(const std::string &playerName, const std::string &msg) = 0;
        virtual void SignalNetClientPrivateChatMsg(const std::string &playerName, const std::string &msg) = 0;
        virtual void SignalNetClientMsgBox(const std::string &msg) = 0;
        virtual void SignalNetClientMsgBox(unsigned msgId) = 0;
        virtual void SignalNetClientWaitDialog() = 0;

        virtual void SignalNetClientServerListAdd(unsigned serverId) = 0;
        virtual void SignalNetClientServerListClear() = 0;
        virtual void SignalNetClientServerListShow() = 0;

        virtual void SignalNetClientLoginShow() = 0;
        virtual void SignalNetClientRejoinPossible(unsigned gameId) = 0;
        virtual void SignalNetClientPostRiverShowCards(unsigned playerId) = 0;

        virtual void SignalLobbyPlayerJoined(unsigned playerId, const std::string &nickName) = 0;
        virtual void SignalLobbyPlayerKicked(const std::string &nickName, const std::string &byWhom, const std::string &reason) = 0;
        virtual void SignalLobbyPlayerLeft(unsigned playerId) = 0;

        virtual void SignalSelfGameInvitation(unsigned gameId, unsigned playerIdFrom) = 0;
        virtual void SignalPlayerGameInvitation(unsigned gameId, unsigned playerIdWho, unsigned playerIdFrom) = 0;
        virtual void SignalRejectedGameInvitation(unsigned gameId, unsigned playerIdWho, DenyGameInvitationReason reason) = 0;
};

class ServerCallback
{
public:
        virtual ~ServerCallback();

        virtual void SignalNetServerSuccess(int actionID) = 0;
        virtual void SignalNetServerError(int errorID, int osErrorID) = 0;
};

class IrcCallback
{
public:
        virtual ~IrcCallback();

        virtual void SignalIrcConnect(const std::string &server) = 0;
        virtual void SignalIrcSelfJoined(const std::string &nickName, const std::string &channel) = 0;
        virtual void SignalIrcPlayerJoined(const std::string &nickName) = 0;
        virtual void SignalIrcPlayerChanged(const std::string &oldNick, const std::string &newNick) = 0;
        virtual void SignalIrcPlayerKicked(const std::string &nickName, const std::string &byWhom, const std::string &reason) = 0;
        virtual void SignalIrcPlayerLeft(const std::string &nickName) = 0;
        virtual void SignalIrcChatMsg(const std::string &nickName, const std::string &msg) = 0;
        virtual void SignalIrcError(int errorCode) = 0;
        virtual void SignalIrcServerError(int errorCode) = 0;
};



#endif
