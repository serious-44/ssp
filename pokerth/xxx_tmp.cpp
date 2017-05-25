#include "xxx_tmp.h"

#include <QDebug>

ClientCallback::~ClientCallback() {}

void ClientCallback::SignalNetClientConnect(int actionID) {}
void ClientCallback::SignalNetClientGameInfo(int actionID) {}
void ClientCallback::SignalNetClientError(int errorID, int osErrorID) {}
void ClientCallback::SignalNetClientNotification(int notificationId) {}
void ClientCallback::SignalNetClientStatsUpdate(const ServerStats &stats) {}
void ClientCallback::SignalNetClientPingUpdate(unsigned minPing, unsigned avgPing, unsigned maxPing) {}
void ClientCallback::SignalNetClientShowTimeoutDialog(NetTimeoutReason reason, unsigned remainingSec) {}
void ClientCallback::SignalNetClientRemovedFromGame(int notificationId) {}

void ClientCallback::SignalNetClientGameListNew(unsigned gameId) {}
void ClientCallback::SignalNetClientGameListRemove(unsigned gameId) {}
void ClientCallback::SignalNetClientGameListUpdateMode(unsigned gameId, GameMode mode) {}
void ClientCallback::SignalNetClientGameListUpdateAdmin(unsigned gameId, unsigned adminPlayerId) {}
void ClientCallback::SignalNetClientGameListPlayerJoined(unsigned gameId, unsigned playerId) {}
void ClientCallback::SignalNetClientGameListPlayerLeft(unsigned gameId, unsigned playerId) {}
void ClientCallback::SignalNetClientGameListSpectatorJoined(unsigned gameId, unsigned playerId) {}
void ClientCallback::SignalNetClientGameListSpectatorLeft(unsigned gameId, unsigned playerId) {}

void ClientCallback::SignalNetClientGameStart(std::shared_ptr<Game> game) {}
void ClientCallback::SignalNetClientSelfJoined(unsigned playerId, const std::string &playerName, bool isGameAdmin) {}
void ClientCallback::SignalNetClientPlayerJoined(unsigned playerId, const std::string &playerName, bool isGameAdmin) {}
void ClientCallback::SignalNetClientPlayerChanged(unsigned playerId, const std::string &newPlayerName) {}
void ClientCallback::SignalNetClientPlayerLeft(unsigned playerId, const std::string &playerName, int removeReason) {}
void ClientCallback::SignalNetClientSpectatorJoined(unsigned playerId, const std::string &playerName) {}
void ClientCallback::SignalNetClientSpectatorLeft(unsigned playerId, const std::string &playerName, int removeReason) {}
void ClientCallback::SignalNetClientNewGameAdmin(unsigned playerId, const std::string &playerName) {}

void ClientCallback::SignalNetClientGameChatMsg(const std::string &playerName, const std::string &msg) {}
void ClientCallback::SignalNetClientLobbyChatMsg(const std::string &playerName, const std::string &msg) {}
void ClientCallback::SignalNetClientPrivateChatMsg(const std::string &playerName, const std::string &msg) {}
void ClientCallback::SignalNetClientMsgBox(const std::string &msg) {}
void ClientCallback::SignalNetClientMsgBox(unsigned msgId) {}
void ClientCallback::SignalNetClientWaitDialog() {}

void ClientCallback::SignalNetClientServerListAdd(unsigned serverId) {}
void ClientCallback::SignalNetClientServerListClear() {}
void ClientCallback::SignalNetClientServerListShow() {}

void ClientCallback::SignalNetClientLoginShow() {}
void ClientCallback::SignalNetClientRejoinPossible(unsigned gameId) {}
void ClientCallback::SignalNetClientPostRiverShowCards(unsigned playerId) {}

void ClientCallback::SignalLobbyPlayerJoined(unsigned playerId, const std::string &nickName) {}
void ClientCallback::SignalLobbyPlayerKicked(const std::string &nickName, const std::string &byWhom, const std::string &reason) {}
void ClientCallback::SignalLobbyPlayerLeft(unsigned playerId) {}

void ClientCallback::SignalSelfGameInvitation(unsigned gameId, unsigned playerIdFrom) {}
void ClientCallback::SignalPlayerGameInvitation(unsigned gameId, unsigned playerIdWho, unsigned playerIdFrom) {}
void ClientCallback::SignalRejectedGameInvitation(unsigned gameId, unsigned playerIdWho, DenyGameInvitationReason reason) {}


ServerCallback::~ServerCallback() {}

void ServerCallback::SignalNetServerSuccess(int actionID) {}
void ServerCallback::SignalNetServerError(int errorID, int osErrorID) {}

IrcCallback::~IrcCallback() {}

void IrcCallback::SignalIrcConnect(const std::string &server) {}
void IrcCallback::SignalIrcSelfJoined(const std::string &nickName, const std::string &channel) {}
void IrcCallback::SignalIrcPlayerJoined(const std::string &nickName) {}
void IrcCallback::SignalIrcPlayerChanged(const std::string &oldNick, const std::string &newNick) {}
void IrcCallback::SignalIrcPlayerKicked(const std::string &nickName, const std::string &byWhom, const std::string &reason) {}
void IrcCallback::SignalIrcPlayerLeft(const std::string &nickName) {}
void IrcCallback::SignalIrcChatMsg(const std::string &nickName, const std::string &msg) {}
void IrcCallback::SignalIrcError(int errorCode) {}
void IrcCallback::SignalIrcServerError(int errorCode) {}

void internal_log_err(const std::string &msg) {
    qDebug() << "Log err:" << QString::fromStdString(msg);
}

void internal_log_msg(const std::string &msg) {
    qDebug() << "Log msg:" << QString::fromStdString(msg);
}

void internal_log_level(const std::string &msg, int logLevel) {
    qDebug() << "Log level:" << QString::fromStdString(msg);
}
