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
#include "guiwrapper.h"
//#include "session.h"
#include "game.h"
#include "playerinterface.h"
#include "guilog.h"
#include "log.h"
//#include "gametableimpl.h"
//#include "startwindowimpl.h"
#include "configfile.h"
//#include "soundevents.h"
//#include <net/socket_msg.h>

#include <QDebug>

using namespace std;


GuiWrapper::GuiWrapper(ConfigFile *c, GameTableImpl *s, GuiLog *g) :
    myGuiLog(g),
    myW(s),
    myConfig(c)
{
    ////qDebug() << "";
    //myW = new gameTableImpl(myConfig);
    //myGuiLog = new guiLog(myW, myConfig);

    //myStartWindow->setGuiLog(myGuiLog);
    //myW->setStartWindow(myStartWindow);
}


GuiWrapper::~GuiWrapper()
{
    //qDebug() << "";
    //delete myGuiLog;

}

void GuiWrapper::initGui(int speed)
{
    //qDebug() << "initGui";
    myW->signalInitGui(speed);
}

std::shared_ptr<Session> GuiWrapper::getSession()
{
    //qDebug() << "getSession";
    return std::shared_ptr<Session>();// myStartWindow->getSession();
}
void GuiWrapper::setSession(std::shared_ptr<Session> /*session*/)
{
    //qDebug() << "setSession";
    /*myStartWindow->setSession(session);*/
}

void GuiWrapper::refreshSet() const
{
    //qDebug() << "refreshSet";
    myW->signalRefreshSet();
}
void GuiWrapper::refreshCash() const
{
    //qDebug() << "refreshCash";
    myW->signalRefreshCash();
}
void GuiWrapper::refreshAction(int playerID, int playerAction) const
{
    //qDebug() << "refreshAction";
    myW->signalRefreshAction(playerID, playerAction);
}
void GuiWrapper::refreshChangePlayer() const
{
    //qDebug() << "refreshChangePlayer";
    myW->signalRefreshChangePlayer();
}
void GuiWrapper::refreshAll() const
{
    //qDebug() << "refreshAll";
    myW->signalRefreshAll();
}
void GuiWrapper::refreshPot() const
{
    //qDebug() << "refreshPot";
    myW->signalRefreshPot();
}
void GuiWrapper::refreshGroupbox(int playerID, int status) const
{
    //qDebug() << "refreshGroupbox";
    myW->signalRefreshGroupbox(playerID, status);
}
void GuiWrapper::refreshPlayerName() const
{
    //qDebug() << "refreshPlayerName";
    myW->signalRefreshPlayerName();
}
void GuiWrapper::refreshButton() const
{
    //qDebug() << "refreshButton";
    myW->signalRefreshButton();
}
void GuiWrapper::refreshGameLabels(GameState state) const
{
    //qDebug() << "refreshGameLabels";
    myW->signalRefreshGameLabels(state);
}

void GuiWrapper::setPlayerAvatar(int myUniqueID, const std::string &myAvatar) const
{
    //qDebug() << "setPlayerAvatar";
    myW->signalSetPlayerAvatar(myUniqueID, QString::fromUtf8(myAvatar.c_str()));
}


void GuiWrapper::waitForGuiUpdateDone() const
{
    //qDebug() << "waitForGuiUpdateDone";
    myW->signalGuiUpdateDone();
    myW->waitForGuiUpdateDone();
}

void GuiWrapper::dealBeRoCards(int myBeRoID)
{
    //qDebug() << "dealBeRoCards";
    myW->signalDealBeRoCards(myBeRoID);
}

void GuiWrapper::dealHoleCards()
{
    //qDebug() << "dealHoleCards";
    myW->signalDealHoleCards();
}
void GuiWrapper::dealFlopCards()
{
    //qDebug() << "dealFlopCards";
    myW->signalDealFlopCards0();
}
void GuiWrapper::dealTurnCard()
{
    //qDebug() << "dealTurnCard";
    myW->signalDealTurnCards0();
}
void GuiWrapper::dealRiverCard()
{
    //qDebug() << "dealRiverCard";
    myW->signalDealRiverCards0();
}

void GuiWrapper::nextPlayerAnimation()
{
    //qDebug() << "nextPlayerAnimation";
    myW->signalNextPlayerAnimation();
}

void GuiWrapper::beRoAnimation2(int myBeRoID)
{
    //qDebug() << "beRoAnimation2" << myBeRoID;
    myW->signalBeRoAnimation2(myBeRoID);
}

void GuiWrapper::preflopAnimation1()
{
    //qDebug() << "preflopAnimation1";
    myW->signalPreflopAnimation1();
}
void GuiWrapper::preflopAnimation2()
{
    //qDebug() << "preflopAnimation2";
    myW->signalPreflopAnimation2();
}

void GuiWrapper::flopAnimation1()
{
    //qDebug() << "flopAnimation1";
    myW->signalFlopAnimation1();
}
void GuiWrapper::flopAnimation2()
{
    //qDebug() << "flopAnimation2";
    myW->signalFlopAnimation2();
}

void GuiWrapper::turnAnimation1()
{
    //qDebug() << "turnAnimation1";
    myW->signalTurnAnimation1();
}
void GuiWrapper::turnAnimation2()
{
    //qDebug() << "turnAnimation2";
    myW->signalTurnAnimation2();
}

void GuiWrapper::riverAnimation1()
{
    //qDebug() << "riverAnimation1";
    myW->signalRiverAnimation1();
}
void GuiWrapper::riverAnimation2()
{
    //qDebug() << "riverAnimation2";
    myW->signalRiverAnimation2();
}

void GuiWrapper::postRiverAnimation1()
{
    //qDebug() << "postRiverAnimation1";
    myW->signalPostRiverAnimation1();
}
void GuiWrapper::postRiverRunAnimation1()
{
    //qDebug() << "postRiverRunAnimation1";
    myW->signalPostRiverRunAnimation1();
}

void GuiWrapper::flipHolecardsAllIn()
{
    //qDebug() << "flipHolecardsAllIn";
    myW->signalFlipHolecardsAllIn();
}

void GuiWrapper::nextRoundCleanGui()
{
    //qDebug() << "nextRoundCleanGui";
    myW->signalNextRoundCleanGui();
}

void GuiWrapper::meInAction()
{
    //qDebug() << "meInAction";
    myW->signalMeInAction();
}
void GuiWrapper::updateMyButtonsState()
{
    //qDebug() << "updateMyButtonsState";
    myW->signalUpdateMyButtonsState();
}
void GuiWrapper::disableMyButtons()
{
    //qDebug() << "disableMyButtons";
    myW->signalDisableMyButtons();
}
void GuiWrapper::startTimeoutAnimation(int playerNum, int timeoutSec)
{
    //qDebug() << "startTimeoutAnimation";
    //myW->signalStartTimeoutAnimation(playerNum, timeoutSec);
}
void GuiWrapper::stopTimeoutAnimation(int playerNum)
{
    //qDebug() << "stopTimeoutAnimation";
    //myW->signalStopTimeoutAnimation(playerNum);
}

void GuiWrapper::startVoteOnKick(unsigned playerId, unsigned voteStarterPlayerId, int timeoutSec, int numVotesNeededToKick)
{
    //myW->signalStartVoteOnKick(playerId, voteStarterPlayerId, timeoutSec, numVotesNeededToKick);
}
void GuiWrapper::changeVoteOnKickButtonsState(bool showHide)
{
    //myW->signalChangeVoteOnKickButtonsState(showHide);
}
void GuiWrapper::refreshVotesMonitor(int currentVotes, int numVotesNeededToKick)
{
    //myW->refreshVotesMonitor(currentVotes, numVotesNeededToKick);
}
void GuiWrapper::endVoteOnKick()
{
    //myW->signalEndVoteOnKick();
}

void GuiWrapper::logPlayerActionMsg(string playerName, int action, int setValue)
{
    //qDebug() << "logPlayerActionMsg" << QString::fromStdString(playerName) << action << setValue;
    myGuiLog->signalLogPlayerActionMsg(QString::fromUtf8(playerName.c_str()), action, setValue);
}
void GuiWrapper::logNewGameHandMsg(int gameID, int handID)
{
    //qDebug() << "logNewGameHandMsg";
    myGuiLog->signalLogNewGameHandMsg(gameID, handID);
}
void GuiWrapper::logNewBlindsSetsMsg(int sbSet, int bbSet, std::string sbName, std::string bbName)
{
    //qDebug() << "logNewBlindsSetsMsg";
    //myW->soundEvent_blindsWereSet(sbSet);
    myGuiLog->signalLogNewBlindsSetsMsg(sbSet, bbSet, QString::fromUtf8(sbName.c_str()), QString::fromUtf8(bbName.c_str()));

}
void GuiWrapper::logPlayerWinsMsg(std::string playerName, int pot, bool main)
{
    //qDebug() << "logPlayerWinsMsg";
    myGuiLog->signalLogPlayerWinsMsg(QString::fromUtf8(playerName.c_str()), pot, main);
}
void GuiWrapper::logPlayerSitsOut(std::string playerName)
{
    //qDebug() << "logPlayerSitsOut";
    myGuiLog->signalLogPlayerSitsOut(QString::fromUtf8(playerName.c_str()));
}
void GuiWrapper::logDealBoardCardsMsg(int roundID, int card1, int card2, int card3, int card4, int card5)
{
    //qDebug() << "logDealBoardCardsMsg";
    myGuiLog->signalLogDealBoardCardsMsg(roundID, card1, card2, card3, card4, card5);
}
void GuiWrapper::logFlipHoleCardsMsg(string playerName, int card1, int card2, int cardsValueInt, string showHas)
{
    //qDebug() << "logFlipHoleCardsMsg";
    myGuiLog->signalLogFlipHoleCardsMsg(QString::fromUtf8(playerName.c_str()), card1, card2, cardsValueInt, QString::fromUtf8(showHas.c_str()));
}
void GuiWrapper::logPlayerWinGame(std::string playerName, int gameID)
{
    //qDebug() << "logPlayerWinGame";
    myGuiLog->signalLogPlayerWinGame(QString::fromUtf8(playerName.c_str()), gameID);
}
void GuiWrapper::flushLogAtGame(int gameID)
{
    //qDebug() << "flushLogAtGame";
    //myGuiLog->signalFlushLogAtGame(gameID);
}
void GuiWrapper::flushLogAtHand()
{
    //qDebug() << "flushLogAtHand";
    //myGuiLog->signalFlushLogAtHand();
}


void GuiWrapper::SignalNetClientConnect(int actionID)
{
    //myStartWindow->signalNetClientConnect(actionID);
}
void GuiWrapper::SignalNetClientServerListAdd(unsigned serverId)
{
    //myStartWindow->signalNetClientServerListAdd(serverId);
}
void GuiWrapper::SignalNetClientServerListShow()
{
    //myStartWindow->signalNetClientServerListShow();
}
void GuiWrapper::SignalNetClientLoginShow()
{
    //myStartWindow->signalNetClientLoginShow();
}
void GuiWrapper::SignalNetClientRejoinPossible(unsigned gameId)
{
    //myStartWindow->signalNetClientRejoinPossible(gameId);
}
void GuiWrapper::SignalNetClientPostRiverShowCards(unsigned playerId)
{
    //myW->signalPostRiverShowCards(playerId);
}
void GuiWrapper::SignalNetClientServerListClear()
{
    //myStartWindow->signalNetClientServerListClear();
}
void GuiWrapper::SignalNetClientGameInfo(int actionID)
{
    //myStartWindow->signalNetClientGameInfo(actionID);
}
void GuiWrapper::SignalNetClientError(int errorID, int osErrorID)
{
    //myStartWindow->signalNetClientError(errorID, osErrorID);
}
void GuiWrapper::SignalNetClientNotification(int notificationId)
{
    //myStartWindow->signalNetClientNotification(notificationId);
}
void GuiWrapper::SignalNetClientStatsUpdate(const ServerStats &stats)
{
    //myStartWindow->signalNetClientStatsUpdate(stats);
}
void GuiWrapper::SignalNetClientPingUpdate(unsigned minPing, unsigned avgPing, unsigned maxPing)
{
    //myW->signalNetClientPingUpdate(minPing, avgPing, maxPing);
}
void GuiWrapper::SignalNetClientShowTimeoutDialog(NetTimeoutReason reason, unsigned remainingSec)
{
    //myStartWindow->signalNetClientShowTimeoutDialog(reason, remainingSec);
}
void GuiWrapper::SignalNetClientRemovedFromGame(int notificationId)
{
    //myStartWindow->signalNetClientRemovedFromGame(notificationId);
}
void GuiWrapper::SignalNetClientSelfJoined(unsigned playerId, const string &playerName, bool isGameAdmin)
{
    //myStartWindow->signalNetClientSelfJoined(playerId, QString::fromUtf8(playerName.c_str()), isGameAdmin);
}
void GuiWrapper::SignalNetClientPlayerJoined(unsigned playerId, const string &playerName, bool isGameAdmin)
{
    //myStartWindow->signalNetClientPlayerJoined(playerId, QString::fromUtf8(playerName.c_str()), isGameAdmin);
    //if (!playerName.empty() && playerName[0] != '#' && myW->isVisible()) {
    //	QString tmpName(QString::fromUtf8(playerName.c_str()));
    //	myGuiLog->signalLogPlayerJoinedMsg(tmpName);
    //}
}
void GuiWrapper::SignalNetClientPlayerChanged(unsigned playerId, const string &newPlayerName)
{
    //myStartWindow->signalNetClientPlayerChanged(playerId, QString::fromUtf8(newPlayerName.c_str()));
    //myW->signalRefreshSpectatorsDisplay();
}
void GuiWrapper::SignalNetClientPlayerLeft(unsigned playerId, const string &playerName, int removeReason)
{
    //QString tmpName(QString::fromUtf8(playerName.c_str()));
    //myStartWindow->signalNetClientPlayerLeft(playerId, tmpName);
    //myW->signalNetClientPlayerLeft(playerId);
    //if (!playerName.empty() && playerName[0] != '#' && myW->isVisible())
    //	myGuiLog->signalLogPlayerLeftMsg(tmpName, removeReason == NTF_NET_REMOVED_KICKED);
}
void GuiWrapper::SignalNetClientSpectatorJoined(unsigned playerId, const string &playerName)
{
    //myStartWindow->signalNetClientSpectatorJoined(playerId, QString::fromUtf8(playerName.c_str()));
    //myW->signalNetClientSpectatorJoined(playerId);
    //if (!playerName.empty() && playerName[0] != '#' && myW->isVisible()) {
    //	QString tmpName(QString::fromUtf8(playerName.c_str()));
    //	myGuiLog->signalLogSpectatorJoinedMsg(tmpName);
    //}
}
void GuiWrapper::SignalNetClientSpectatorLeft(unsigned playerId, const string &playerName, int removeReason)
{
    //QString tmpName(QString::fromUtf8(playerName.c_str()));
    //myStartWindow->signalNetClientSpectatorLeft(playerId, tmpName);
    //myW->signalNetClientSpectatorLeft(playerId);
    //if (!playerName.empty() && playerName[0] != '#' && myW->isVisible())
    //	myGuiLog->signalLogSpectatorLeftMsg(tmpName, removeReason == NTF_NET_REMOVED_KICKED);
}
void GuiWrapper::SignalNetClientNewGameAdmin(unsigned playerId, const string &playerName)
{
    //myStartWindow->signalNetClientNewGameAdmin(playerId, QString::fromUtf8(playerName.c_str()));
    //if (!playerName.empty() && playerName[0] != '#' && myW->isVisible())
    //	myGuiLog->signalLogNewGameAdminMsg(QString::fromUtf8(playerName.c_str()));
}

void GuiWrapper::SignalNetClientGameListNew(unsigned gameId)
{
    //myStartWindow->signalNetClientGameListNew(gameId);
}
void GuiWrapper::SignalNetClientGameListRemove(unsigned gameId)
{
    //myStartWindow->signalNetClientGameListRemove(gameId);
}
void GuiWrapper::SignalNetClientGameListUpdateMode(unsigned gameId, GameMode mode)
{
    //myStartWindow->signalNetClientGameListUpdateMode(gameId, mode);
}
void GuiWrapper::SignalNetClientGameListUpdateAdmin(unsigned gameId, unsigned adminPlayerId)
{
    //myStartWindow->signalNetClientGameListUpdateAdmin(gameId, adminPlayerId);
}
void GuiWrapper::SignalNetClientGameListPlayerJoined(unsigned gameId, unsigned playerId)
{
    //myStartWindow->signalNetClientGameListPlayerJoined(gameId, playerId);
}
void GuiWrapper::SignalNetClientGameListPlayerLeft(unsigned gameId, unsigned playerId)
{
    //myStartWindow->signalNetClientGameListPlayerLeft(gameId, playerId);
}
void GuiWrapper::SignalNetClientGameListSpectatorJoined(unsigned gameId, unsigned playerId)
{
    //myStartWindow->signalNetClientGameListSpectatorJoined(gameId, playerId);
}
void GuiWrapper::SignalNetClientGameListSpectatorLeft(unsigned gameId, unsigned playerId)
{
    //myStartWindow->signalNetClientGameListSpectatorLeft(gameId, playerId);
}
void GuiWrapper::SignalNetClientGameStart(std::shared_ptr<Game> game)
{
    //myStartWindow->signalNetClientGameStart(game);
}

void GuiWrapper::SignalNetClientWaitDialog()
{
    //myStartWindow->signalShowClientDialog();
}
void GuiWrapper::SignalNetClientGameChatMsg(const string &playerName, const string &msg)
{
    //myStartWindow->signalNetClientGameChatMsg(QString::fromUtf8(playerName.c_str()), QString::fromUtf8(msg.c_str()));
}
void GuiWrapper::SignalNetClientLobbyChatMsg(const string &playerName, const string &msg)
{
    //myStartWindow->signalNetClientLobbyChatMsg(QString::fromUtf8(playerName.c_str()), QString::fromUtf8(msg.c_str()));
}
void GuiWrapper::SignalNetClientPrivateChatMsg(const std::string &playerName, const std::string &msg)
{
    //myStartWindow->signalNetClientPrivateChatMsg(QString::fromUtf8(playerName.c_str()), QString::fromUtf8(msg.c_str()));
}
void GuiWrapper::SignalNetClientMsgBox(const string &msg)
{
    //myStartWindow->signalNetClientMsgBox(QString::fromUtf8(msg.c_str()));
}
void GuiWrapper::SignalNetClientMsgBox(unsigned msgId)
{
    //myStartWindow->signalNetClientMsgBox(msgId);
}

void GuiWrapper::SignalNetServerSuccess(int /*actionID*/) { }
void GuiWrapper::SignalNetServerError(int errorID, int osErrorID)
{
    //myStartWindow->signalNetServerError(errorID, osErrorID);
}

void GuiWrapper::SignalIrcConnect(const string &/*server*/)
{
    /*myStartWindow->signalIrcConnect(QString::fromUtf8(server.c_str()));*/
}
void GuiWrapper::SignalIrcSelfJoined(const string &/*nickName*/, const string &/*channel*/)
{
    /*myStartWindow->signalIrcSelfJoined(QString::fromUtf8(nickName.c_str()), QString::fromUtf8(channel.c_str()));*/
}
void GuiWrapper::SignalIrcPlayerJoined(const string &/*nickName*/)
{
    /*myStartWindow->signalIrcPlayerJoined(QString::fromUtf8(nickName.c_str()));*/
}
void GuiWrapper::SignalIrcPlayerChanged(const string &/*oldNick*/, const string &/*newNick*/)
{
    /* myStartWindow->signalIrcPlayerChanged(QString::fromUtf8(oldNick.c_str()), QString::fromUtf8(newNick.c_str()));*/
}
void GuiWrapper::SignalIrcPlayerKicked(const std::string &/*nickName*/, const std::string &/*byWhom*/, const std::string &/*reason*/)
{
    /*myStartWindow->signalIrcPlayerKicked(QString::fromUtf8(nickName.c_str()), QString::fromUtf8(byWhom.c_str()), QString::fromUtf8(reason.c_str()));*/
}
void GuiWrapper::SignalIrcPlayerLeft(const std::string &/*nickName*/)
{
    /*myStartWindow->signalIrcPlayerLeft(QString::fromUtf8(nickName.c_str()));*/
}
void GuiWrapper::SignalIrcChatMsg(const std::string &/*nickName*/, const std::string &/*msg*/)
{
    /*myStartWindow->signalIrcChatMessage(QString::fromUtf8(nickName.c_str()), QString::fromUtf8(msg.c_str()));*/
}
void GuiWrapper::SignalIrcError(int /*errorCode*/)
{
    /*myStartWindow->signalIrcError(errorCode);*/
}
void GuiWrapper::SignalIrcServerError(int /*errorCode*/)
{
    /*myStartWindow->signalIrcServerError(errorCode);*/
}

void GuiWrapper::SignalLobbyPlayerJoined(unsigned playerId, const string &nickName)
{
    //myStartWindow->signalLobbyPlayerJoined(playerId, QString::fromUtf8(nickName.c_str()));
}
void GuiWrapper::SignalLobbyPlayerKicked(const std::string &nickName, const std::string &byWhom, const std::string &reason)
{
    //myStartWindow->signalLobbyPlayerKicked(QString::fromUtf8(nickName.c_str()), QString::fromUtf8(byWhom.c_str()), QString::fromUtf8(reason.c_str()));
}
void GuiWrapper::SignalLobbyPlayerLeft(unsigned playerId)
{
    //myStartWindow->signalLobbyPlayerLeft(playerId);
}

void GuiWrapper::SignalSelfGameInvitation(unsigned gameId, unsigned playerIdFrom)
{
    //myStartWindow->signalSelfGameInvitation(gameId, playerIdFrom);
}
void GuiWrapper::SignalPlayerGameInvitation(unsigned gameId, unsigned playerIdWho, unsigned playerIdFrom)
{
    //myStartWindow->signalPlayerGameInvitation(gameId, playerIdWho, playerIdFrom);
}
void GuiWrapper::SignalRejectedGameInvitation(unsigned gameId, unsigned playerIdWho, DenyGameInvitationReason reason)
{
    //myStartWindow->signalRejectedGameInvitation(gameId, playerIdWho, reason);
}

