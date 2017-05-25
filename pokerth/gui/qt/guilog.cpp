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
#include "guilog.h"

#include "gametableimpl.h"
#include <handinterface.h>
#include <game.h>
#include <cardsvalue.h>
#include <game_defs.h>

using namespace std;

GuiLog::GuiLog(GameTableImpl* w, ConfigFile *c) :
    QObject(w),
    myW(w),
    myConfig(c)
{
    //myAppDataPath = QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str());

    connect(this, SIGNAL(signalLogPlayerActionMsg(QString, int, int)), this, SLOT(logPlayerActionMsg(QString, int, int)));
    connect(this, SIGNAL(signalLogNewGameHandMsg(int, int)), this, SLOT(logNewGameHandMsg(int, int)));
    connect(this, SIGNAL(signalLogNewBlindsSetsMsg(int, int, QString, QString)), this, SLOT(logNewBlindsSetsMsg(int, int, QString, QString)));
    connect(this, SIGNAL(signalLogPlayerWinsMsg(QString, int, bool)), this, SLOT(logPlayerWinsMsg(QString, int, bool)));
    connect(this, SIGNAL(signalLogPlayerSitsOut(QString)), this, SLOT(logPlayerSitsOut(QString)));
    connect(this, SIGNAL(signalLogDealBoardCardsMsg(int, int, int, int, int, int)), this, SLOT(logDealBoardCardsMsg(int, int, int, int, int, int)));
    connect(this, SIGNAL(signalLogFlipHoleCardsMsg(QString, int, int, int, QString)), this, SLOT(logFlipHoleCardsMsg(QString, int, int, int, QString)));
    connect(this, SIGNAL(signalLogPlayerLeftMsg(QString, int)), this, SLOT(logPlayerLeftMsg(QString, int)));
    connect(this, SIGNAL(signalLogPlayerJoinedMsg(QString)), this, SLOT(logPlayerJoinedMsg(QString)));
    connect(this, SIGNAL(signalLogNewGameAdminMsg(QString)), this, SLOT(logNewGameAdminMsg(QString)));
    connect(this, SIGNAL(signalLogSpectatorLeftMsg(QString, int)), this, SLOT(logSpectatorLeftMsg(QString, int)));
    connect(this, SIGNAL(signalLogSpectatorJoinedMsg(QString)), this, SLOT(logSpectatorJoinedMsg(QString)));
    connect(this, SIGNAL(signalLogPlayerWinGame(QString, int)), this, SLOT(logPlayerWinGame(QString, int)));
    //connect(this, SIGNAL(signalFlushLogAtGame(int)), this, SLOT(flushLogAtGame(int)));
    //connect(this, SIGNAL(signalFlushLogAtHand()), this, SLOT(flushLogAtHand()));


    lastGameID = 0;
}

GuiLog::~GuiLog()
{
}

void GuiLog::logPlayerActionMsg(QString msg, int action, int setValue)
{

    switch (action) {

    case 1:
        msg += " folds.";
        break;
    case 2:
        msg += " checks.";
        break;
    case 3:
        msg += " calls ";
        break;
    case 4:
        msg += " bets ";
        break;
    case 5:
        msg += " bets ";
        break;
    case 6:
        msg += " is all in with ";
        break;
    default:
        msg += "ERROR";
    }

    if (action >= 3) {
        msg += "$"+QString::number(setValue,10)+".";
    }

    myW->ui->textBrowser_Log->append("<span style=\"color:#000000;\">"+msg+"</span>");
}

void GuiLog::logNewGameHandMsg(int gameID, int handID)
{
    myW->ui->textBrowser_Log->append("<span style=\"color:#000000; font-weight:bold\">## Game: "+QString::number(gameID,10)+" | Hand: "+QString::number(handID,10)+" ##</span>");
}

void GuiLog::logNewBlindsSetsMsg(int sbSet, int bbSet, QString sbName, QString bbName)
{
    myW->ui->textBrowser_Log->append("<span style=\"color:#000000;\">"+sbName+" posts small blind ($"+QString::number(sbSet,10)+")</span>");
    myW->ui->textBrowser_Log->append("<span style=\"color:#000000;\">"+bbName+" posts big blind ($"+QString::number(bbSet,10)+")</span>");
}

void GuiLog::logPlayerWinsMsg(QString playerName, int pot, bool main)
{
    if(main) {
        myW->ui->textBrowser_Log->append("<span style=\"color:#000000;\">"+playerName+" wins $"+QString::number(pot,10)+"</span>");
    } else {
        myW->ui->textBrowser_Log->append("<span style=\"color:#000000;\">"+playerName+" wins $"+QString::number(pot,10)+" (side pot)</span>");
    }
}

void GuiLog::logPlayerSitsOut(QString playerName)
{

    myW->ui->textBrowser_Log->append("<i><span style=\"color:#000000;\">"+playerName+" sits out</span></i>");
}

void GuiLog::logDealBoardCardsMsg(int roundID, int card1, int card2, int card3, int card4, int card5)
{
    QString round;

    switch (roundID) {

    case 1:
        round = "Flop";
        myW->ui->textBrowser_Log->append("<span style=\"color:#000000;\">--- "+round+" --- "+"["+translateCardCode(card1).at(0)+translateCardCode(card1).at(1)+","+translateCardCode(card2).at(0)+translateCardCode(card2).at(1)+","+translateCardCode(card3).at(0)+translateCardCode(card3).at(1)+"]</span>");
        break;
    case 2:
        round = "Turn";
        myW->ui->textBrowser_Log->append("<span style=\"color:#000000;\">--- "+round+" --- "+"["+translateCardCode(card1).at(0)+translateCardCode(card1).at(1)+","+translateCardCode(card2).at(0)+translateCardCode(card2).at(1)+","+translateCardCode(card3).at(0)+translateCardCode(card3).at(1)+","+translateCardCode(card4).at(0)+translateCardCode(card4).at(1)+"]</span>");
        break;
    case 3:
        round = "River";
        myW->ui->textBrowser_Log->append("<span style=\"color:#000000;\">--- "+round+" --- "+"["+translateCardCode(card1).at(0)+translateCardCode(card1).at(1)+","+translateCardCode(card2).at(0)+translateCardCode(card2).at(1)+","+translateCardCode(card3).at(0)+translateCardCode(card3).at(1)+","+translateCardCode(card4).at(0)+translateCardCode(card4).at(1)+","+translateCardCode(card5).at(0)+translateCardCode(card5).at(1)+"]</span>");
        break;
    default:
        round = "ERROR";
    }
}

void GuiLog::logFlipHoleCardsMsg(QString playerName, int card1, int card2, int cardsValueInt, QString showHas)
{

    QString tempHandName;

    if (cardsValueInt != -1) {
        tempHandName = CardsValue::determineHandName(cardsValueInt,myW->currentGame->getActivePlayerList()).c_str();
        myW->ui->textBrowser_Log->append("<span style=\"color:#000000;\">"+playerName+" "+showHas+" ["+translateCardCode(card1).at(0)+translateCardCode(card1).at(1)+","+translateCardCode(card2).at(0)+translateCardCode(card2).at(1)+"] - \""+tempHandName+"\"</span>");
    } else {
        myW->ui->textBrowser_Log->append("<span style=\"color:#000000;\">"+playerName+" "+showHas+" ["+translateCardCode(card1).at(0)+translateCardCode(card1).at(1)+","+translateCardCode(card2).at(0)+translateCardCode(card2).at(1)+"]</span>");
    }
}

void GuiLog::logPlayerLeftMsg(QString playerName, int wasKicked)
{
    QString action;
    if(wasKicked) action = "was kicked from";
    else action = "has left";
    myW->ui->textBrowser_Log->append( "<span style=\"color:#000000;\"><i>"+playerName+" "+action+" the game!</i></span>");
}

void GuiLog::logNewGameAdminMsg(QString playerName)
{

    myW->ui->textBrowser_Log->append( "<i><span style=\"color:#000000;\">"+playerName+" is game admin now!</span></i>");
}

void GuiLog::logPlayerJoinedMsg(QString playerName)
{
    myW->ui->textBrowser_Log->append("<span style=\"color:#000000;\"><i>"+playerName+" has joined the game!</i></span>");
}

void GuiLog::logSpectatorLeftMsg(QString playerName, int wasKicked)
{
    // TODO
}

void GuiLog::logSpectatorJoinedMsg(QString playerName)
{
    // TODO
}

void GuiLog::logPlayerWinGame(QString playerName, int gameID)
{
    myW->ui->textBrowser_Log->append( "<i><b>"+playerName+" wins game " + QString::number(gameID,10)  +"!</i></b><br>");
}

QStringList GuiLog::translateCardCode(int cardCode)
{

    int value = cardCode%13;
    int color = cardCode/13;

    QStringList cardString;

    switch (value) {

    case 0:
        cardString << "2";
        break;
    case 1:
        cardString << "3";
        break;
    case 2:
        cardString << "4";
        break;
    case 3:
        cardString << "5";
        break;
    case 4:
        cardString << "6";
        break;
    case 5:
        cardString << "7";
        break;
    case 6:
        cardString << "8";
        break;
    case 7:
        cardString << "9";
        break;
    case 8:
        cardString << "T";
        break;
    case 9:
        cardString << "J";
        break;
    case 10:
        cardString << "Q";
        break;
    case 11:
        cardString << "K";
        break;
    case 12:
        cardString << "A";
        break;
    default:
        cardString << "ERROR";
    }

    switch (color) {

    case 0:
        cardString << "<font size=+1><b>&diams;</b></font>";
        break;
    case 1:
        cardString << "<font size=+1><b>&hearts;</b></font>";
        break;
    case 2:
        cardString << "<font size=+1><b>&spades;</b></font>";
        break;
    case 3:
        cardString << "<font size=+1><b>&clubs;</b></font>";
        break;
    default:
        cardString << "ERROR";
    }

    return cardString;
}
