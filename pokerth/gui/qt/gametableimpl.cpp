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
#include "gametableimpl.h"
#include "ui_gametableimpl.h"

#include "mycardspixmaplabel.h"
//#include "mysetlabel.h"
//#include "myavatarlabel.h"
//#include "myactionbutton.h"
//#include "mycashlabel.h"
//#include "mynamelabel.h"
//#include "mychancelabel.h"
//#include "guilog.h"

#include "playerinterface.h"
#include "boardinterface.h"
#include "handinterface.h"
#include "game.h"
//#include "session.h"
#include "cardsvalue.h"

#include "configfile.h"
//#include "soundevents.h"
#include <gamedata.h>
#include <edit/clipeditor.h>

#include <cmath>

#include <QFile>
#include <QThread>
#include <QDebug>

#include "play/previewfiledialog.h"

using namespace std;

static int piecesOfAmount(int amount) {
    if (amount <= 0) {
        return 0;
    }
    int pieces = (amount-1) / 1000;
    if (pieces > 4) {
        pieces = 4;
    }
    return pieces;
}

static QString actionPixName(int action) {
    switch(action) {
    case 1:
        return "://misc/actionpics/action_fold.png";
    case 2:
        return "://misc/actionpics/action_check.png";
    case 3:
        return "://misc/actionpics/action_call.png";
    case 4:
        return "://misc/actionpics/action_bet.png";
    case 5:
        return "://misc/actionpics/action_raise.png";
    case 6:
        return "://misc/actionpics/action_allin.png";
    case 7:
        return "://misc/actionpics/action_winner.png";
    default:
        return "://misc/1pix.png";
    }
}


GameTableImpl::GameTableImpl(QWidget *parent)
    : QFrame(parent),
      gameSpeed(0),
      myActionIsBet(0),
      myActionIsRaise(0),
      pushButtonBetRaiseIsChecked(false),
      pushButtonCallCheckIsChecked(false),
      pushButtonFoldIsChecked(false),
      pushButtonAllInIsChecked(false),
      myButtonsAreCheckable(false),
      breakAfterCurrentHand(false),
      currentGameOver(true),
      betSliderChangedByInput(false),
      guestMode(false),
      myLastPreActionBetValue(0),
      waitingForVideo(0),
      startNewHandDefered(false),
      startZoomIn(false),
      humanPlayerStartCash(0),
      psThread(0),
      ui(new Ui::GameTableImpl)
{
}

void GameTableImpl::init(ConfigFile *c, Log *l) {
    myConfig = c;
    myLog = l;
    myGuiLog = new GuiLog(this, c);
    myGuiInterface = new GuiWrapper(myConfig, this, myGuiLog);

    //	this->setStyle(new QPlastiqueStyle);

    int i;
    //for statistic development
    for(i=0; i<15; i++) {
        statisticArray[i] = 0;
    }
    ////////////////////////////

    //myAppDataPath = QString::fromUtf8(myConfig->readConfigString("AppDataDir").c_str());

    ui->setupUi(this);

    //Sound
    //mySoundEventHandler = new SoundEvents(myConfig);

    //set myStyle to widgets wich needs it
    //label_chance->setMyStyle(myGameTableStyle);

    flipside = QPixmap::fromImage(QImage("://cards/flipside.png"));

    //Flipside Animation noch nicht erledigt
    flipHolecardsAllInAlreadyDone = false;

    //Toolboxen verstecken?
    //if (!myConfig->readConfigInt("ShowRightToolBox")) {
        //groupBox_RightToolBox->hide();
    //}
    //if (!myConfig->readConfigInt("ShowLeftToolBox")) {
        //groupBox_LeftToolBox->hide();
    //}

    //CardsChanceMonitor show/hide
    //if (!myConfig->readConfigInt("ShowCardsChanceMonitor")) {
        //tabWidget_Right->removeTab(2);
        //tabWidget_Right->setCurrentIndex(0);
    //}

    //stoptimer Objekt erstellen
    dealFlopCards0Timer = new QTimer(this);
    dealFlopCards1Timer = new QTimer(this);
    dealFlopCards2Timer = new QTimer(this);
    dealFlopCards3Timer = new QTimer(this);
    dealFlopCards4Timer = new QTimer(this);
    dealFlopCards5Timer = new QTimer(this);
    dealFlopCards6Timer = new QTimer(this);
    dealTurnCards0Timer = new QTimer(this);
    dealTurnCards1Timer = new QTimer(this);
    dealTurnCards2Timer = new QTimer(this);
    dealRiverCards0Timer = new QTimer(this);
    dealRiverCards1Timer = new QTimer(this);
    dealRiverCards2Timer = new QTimer(this);

    nextPlayerAnimationTimer = new QTimer(this);
    preflopAnimation1Timer = new QTimer(this);
    preflopAnimation2Timer = new QTimer(this);
    flopAnimation1Timer = new QTimer(this);
    flopAnimation2Timer = new QTimer(this);
    turnAnimation1Timer = new QTimer(this);
    turnAnimation2Timer = new QTimer(this);
    riverAnimation1Timer = new QTimer(this);
    riverAnimation2Timer = new QTimer(this);

    postRiverAnimation1Timer = new QTimer(this);
    postRiverRunAnimation1Timer = new QTimer(this);
    postRiverRunAnimation2Timer = new QTimer(this);
    postRiverRunAnimation2_flipHoleCards1Timer = new QTimer(this);
    postRiverRunAnimation2_flipHoleCards2Timer = new QTimer(this);
    postRiverRunAnimation3Timer = new QTimer(this);
    postRiverRunAnimation5Timer = new QTimer(this);
    potDistributeTimer = new QTimer(this);
    postRiverRunAnimation6Timer = new QTimer(this);

    blinkingStartButtonAnimationTimer = new QTimer(this);
    voteOnKickTimeoutTimer = new QTimer(this);
    enableCallCheckPushButtonTimer = new QTimer(this);

    dealFlopCards0Timer->setSingleShot(true);
    dealFlopCards1Timer->setSingleShot(true);
    dealFlopCards2Timer->setSingleShot(true);
    dealFlopCards3Timer->setSingleShot(true);
    dealFlopCards4Timer->setSingleShot(true);
    dealFlopCards5Timer->setSingleShot(true);
    dealFlopCards6Timer->setSingleShot(true);
    dealTurnCards0Timer->setSingleShot(true);
    dealTurnCards1Timer->setSingleShot(true);
    dealTurnCards2Timer->setSingleShot(true);
    dealRiverCards0Timer->setSingleShot(true);
    dealRiverCards1Timer->setSingleShot(true);
    dealRiverCards2Timer->setSingleShot(true);

    nextPlayerAnimationTimer->setSingleShot(true);
    preflopAnimation1Timer->setSingleShot(true);
    preflopAnimation2Timer->setSingleShot(true);
    flopAnimation1Timer->setSingleShot(true);
    flopAnimation2Timer->setSingleShot(true);
    turnAnimation1Timer->setSingleShot(true);
    turnAnimation2Timer->setSingleShot(true);
    riverAnimation1Timer->setSingleShot(true);
    riverAnimation2Timer->setSingleShot(true);

    postRiverAnimation1Timer->setSingleShot(true);
    postRiverRunAnimation1Timer->setSingleShot(true);
    postRiverRunAnimation2Timer->setSingleShot(true);
    postRiverRunAnimation3Timer->setSingleShot(true);
    postRiverRunAnimation5Timer->setSingleShot(true);
    postRiverRunAnimation6Timer->setSingleShot(true);

    enableCallCheckPushButtonTimer->setSingleShot(true);

    // buttonLabelArray init
    buttonLabelArray[0] = ui->textLabel_Button0;
    buttonLabelArray[1] = ui->textLabel_Button1;
    buttonLabelArray[2] = ui->textLabel_Button2;
    buttonLabelArray[3] = ui->textLabel_Button3;
    buttonLabelArray[4] = ui->textLabel_Button4;
    buttonLabelArray[5] = ui->textLabel_Button5;

    // cashLabelArray init
    cashLabelArray[0] = ui->textLabel_Cash0;
    cashLabelArray[1] = ui->textLabel_Cash1;
    cashLabelArray[2] = ui->textLabel_Cash2;
    cashLabelArray[3] = ui->textLabel_Cash3;
    cashLabelArray[4] = ui->textLabel_Cash4;
    cashLabelArray[5] = ui->textLabel_Cash5;
    for (i=0; i<MAX_NUMBER_OF_PLAYERS; i++) {
        //cashLabelArray[i]->setMyW(this);
    }

    // playerNameLabelArray init
    //playerNameLabelArray[0] = label_PlayerName0;
    //playerNameLabelArray[1] = label_PlayerName1;
    //playerNameLabelArray[2] = label_PlayerName2;
    //playerNameLabelArray[3] = label_PlayerName3;
    //playerNameLabelArray[4] = label_PlayerName4;
    //playerNameLabelArray[5] = label_PlayerName5;
    //playerNameLabelArray[6] = label_PlayerName6;
    //playerNameLabelArray[7] = label_PlayerName7;
    //playerNameLabelArray[8] = label_PlayerName8;
    //playerNameLabelArray[9] = label_PlayerName9;
    //for (i=0; i<MAX_NUMBER_OF_PLAYERS; i++) {
    //    playerNameLabelArray[i]->setMyW(this);
    //}

    // statusLabelArray init
    actionLabelArray[0] = ui->textLabel_Status0;
    actionLabelArray[1] = ui->textLabel_Status1;
    actionLabelArray[2] = ui->textLabel_Status2;
    actionLabelArray[3] = ui->textLabel_Status3;
    actionLabelArray[4] = ui->textLabel_Status4;
    actionLabelArray[5] = ui->textLabel_Status5;
    QPixmap onePix = QPixmap::fromImage(QImage(actionPixName(0)));
    for (int i = 0; i < 6; i++) {
        actionLabelArray[i]->setPixmap(onePix);
    }

    // GroupBoxArray init
    groupBoxArray[0] = ui->groupBox0;
    groupBoxArray[1] = ui->groupBox1;
    groupBoxArray[2] = ui->groupBox2;
    groupBoxArray[3] = ui->groupBox3;
    groupBoxArray[4] = ui->groupBox4;
    groupBoxArray[5] = ui->groupBox5;

    selectPlayerButtonArray[1] = ui->selectPlayer1;
    selectPlayerButtonArray[2] = ui->selectPlayer2;
    selectPlayerButtonArray[3] = ui->selectPlayer3;
    selectPlayerButtonArray[4] = ui->selectPlayer4;
    selectPlayerButtonArray[5] = ui->selectPlayer5;

    // boardCardsArray init
    boardCardsArray[0] = ui->pixmapLabel_cardBoard0;
    boardCardsArray[1] = ui->pixmapLabel_cardBoard1;
    boardCardsArray[2] = ui->pixmapLabel_cardBoard2;
    boardCardsArray[3] = ui->pixmapLabel_cardBoard3;
    boardCardsArray[4] = ui->pixmapLabel_cardBoard4;

    // holeCardsArray int
    holeCardsArray[0][0] = ui->pixmapLabel_card0a;
    holeCardsArray[0][1] = ui->pixmapLabel_card0b;
    holeCardsArray[1][0] = ui->pixmapLabel_card1a;
    holeCardsArray[1][1] = ui->pixmapLabel_card1b;
    holeCardsArray[2][0] = ui->pixmapLabel_card2a;
    holeCardsArray[2][1] = ui->pixmapLabel_card2b;
    holeCardsArray[3][0] = ui->pixmapLabel_card3a;
    holeCardsArray[3][1] = ui->pixmapLabel_card3b;
    holeCardsArray[4][0] = ui->pixmapLabel_card4a;
    holeCardsArray[4][1] = ui->pixmapLabel_card4b;
    holeCardsArray[5][0] = ui->pixmapLabel_card5a;
    holeCardsArray[5][1] = ui->pixmapLabel_card5b;

    labelCards[0] = ui->labelCards0;
    labelCards[1] = ui->labelCards1;
    labelCards[2] = ui->labelCards2;
    labelCards[3] = ui->labelCards3;
    labelCards[4] = ui->labelCards4;
    labelCards[5] = ui->labelCards5;
    labelProbability[0] = ui->labelProbability0;
    labelProbability[1] = ui->labelProbability1;
    labelProbability[2] = ui->labelProbability2;
    labelProbability[3] = ui->labelProbability3;
    labelProbability[4] = ui->labelProbability4;
    labelProbability[5] = ui->labelProbability5;

    //style Game Table
    refreshGameTableStyle();

    //Clear Focus
    //pushButton_break->setMinimumSize(width+10,20);
    //groupBox_LeftToolBox->clearFocus();
    //groupBox_RightToolBox->clearFocus();

    //set Focus to gametable
    this->setFocus();

    //this->installEventFilter(this);


    //Connects
    connect(dealFlopCards0Timer, SIGNAL(timeout()), this, SLOT( dealFlopCards1() ));
    connect(dealFlopCards1Timer, SIGNAL(timeout()), this, SLOT( dealFlopCards2() ));
    connect(dealFlopCards2Timer, SIGNAL(timeout()), this, SLOT( dealFlopCards3() ));
    connect(dealFlopCards3Timer, SIGNAL(timeout()), this, SLOT( dealFlopCards4() ));
    connect(dealFlopCards4Timer, SIGNAL(timeout()), this, SLOT( dealFlopCards5() ));
    connect(dealFlopCards5Timer, SIGNAL(timeout()), this, SLOT( dealFlopCards6() ));
    connect(dealFlopCards6Timer, SIGNAL(timeout()), this, SLOT( handSwitchRounds() ));
    connect(dealTurnCards0Timer, SIGNAL(timeout()), this, SLOT( dealTurnCards1() ));
    connect(dealTurnCards1Timer, SIGNAL(timeout()), this, SLOT( dealTurnCards2() ));
    connect(dealTurnCards2Timer, SIGNAL(timeout()), this, SLOT( handSwitchRounds() ));
    connect(dealRiverCards0Timer, SIGNAL(timeout()), this, SLOT( dealRiverCards1() ));
    connect(dealRiverCards1Timer, SIGNAL(timeout()), this, SLOT( dealRiverCards2() ));
    connect(dealRiverCards2Timer, SIGNAL(timeout()), this, SLOT( handSwitchRounds() ));

    connect(nextPlayerAnimationTimer, SIGNAL(timeout()), this, SLOT( handSwitchRounds() ));
    connect(preflopAnimation1Timer, SIGNAL(timeout()), this, SLOT( preflopAnimation1Action() ));
    connect(preflopAnimation2Timer, SIGNAL(timeout()), this, SLOT( preflopAnimation2Action() ));
    connect(flopAnimation1Timer, SIGNAL(timeout()), this, SLOT( flopAnimation1Action() ));
    connect(flopAnimation2Timer, SIGNAL(timeout()), this, SLOT( flopAnimation2Action() ));
    connect(turnAnimation1Timer, SIGNAL(timeout()), this, SLOT( turnAnimation1Action() ));
    connect(turnAnimation2Timer, SIGNAL(timeout()), this, SLOT( turnAnimation2Action() ));
    connect(riverAnimation1Timer, SIGNAL(timeout()), this, SLOT( riverAnimation1Action() ));
    connect(riverAnimation2Timer, SIGNAL(timeout()), this, SLOT( riverAnimation2Action() ));

    connect(postRiverAnimation1Timer, SIGNAL(timeout()), this, SLOT( postRiverAnimation1Action() ));
    connect(postRiverRunAnimation1Timer, SIGNAL(timeout()), this, SLOT( postRiverRunAnimation2() ));
    connect(postRiverRunAnimation2Timer, SIGNAL(timeout()), this, SLOT( postRiverRunAnimation3() ));
    connect(postRiverRunAnimation3Timer, SIGNAL(timeout()), this, SLOT( postRiverRunAnimation4() ));
    connect(potDistributeTimer, SIGNAL(timeout()), this, SLOT(postRiverRunAnimation5()));
    connect(postRiverRunAnimation5Timer, SIGNAL(timeout()), this, SLOT( postRiverRunAnimation6() ));
    connect(postRiverRunAnimation6Timer, SIGNAL(timeout()), this, SLOT( startNewHand() ));

    connect(blinkingStartButtonAnimationTimer, SIGNAL(timeout()), this, SLOT( blinkingStartButtonAnimationAction()));
    connect(enableCallCheckPushButtonTimer, SIGNAL(timeout()), this, SLOT(enableCallCheckPushButton()));

    //connect( actionConfigure_PokerTH, SIGNAL( triggered() ), this, SLOT( callSettingsDialog() ) );

    //connect( actionClose, SIGNAL( triggered() ), this, SLOT( closeGameTable()) );

   // connect( actionFullScreen, SIGNAL( triggered() ), this, SLOT( switchFullscreen() ) );

    //connect( actionShowHideChat, SIGNAL( triggered() ), this, SLOT( switchChatWindow() ) );
    //connect( actionShowHideHelp, SIGNAL( triggered() ), this, SLOT( switchHelpWindow() ) );
    //connect( actionShowHideLog, SIGNAL( triggered() ), this, SLOT( switchLogWindow() ) );
    //connect( actionShowHideAway, SIGNAL( triggered() ), this, SLOT( switchAwayWindow() ) );
    //connect( actionShowHideChance, SIGNAL( triggered() ), this, SLOT( switchChanceWindow() ) );

    connect( ui->pushButton_BetRaise, SIGNAL( clicked(bool) ), this, SLOT( pushButtonBetRaiseClicked(bool) ) );
    connect( ui->pushButton_Fold, SIGNAL( clicked(bool) ), this, SLOT( pushButtonFoldClicked(bool) ) );
    connect( ui->pushButton_CallCheck, SIGNAL( clicked(bool) ), this, SLOT( pushButtonCallCheckClicked(bool) ) );
    connect( ui->pushButton_AllIn, SIGNAL( clicked(bool) ), this, SLOT(pushButtonAllInClicked(bool) ) );
    connect( ui->horizontalSlider_bet, SIGNAL( valueChanged(int)), this, SLOT ( changeSpinBoxBetValue(int) ) );
    connect( ui->spinBox_betValue, SIGNAL( valueChanged(int)), this, SLOT ( spinBoxBetValueChanged(int) ) );

    connect(ui->selectPlayer1, SIGNAL (clicked()), this, SLOT (slotSelectPlayer1Clicked()));
    connect(ui->selectPlayer2, SIGNAL (clicked()), this, SLOT (slotSelectPlayer2Clicked()));
    connect(ui->selectPlayer3, SIGNAL (clicked()), this, SLOT (slotSelectPlayer3Clicked()));
    connect(ui->selectPlayer4, SIGNAL (clicked()), this, SLOT (slotSelectPlayer4Clicked()));
    connect(ui->selectPlayer5, SIGNAL (clicked()), this, SLOT (slotSelectPlayer5Clicked()));
    connect(ui->buttonSaveCompilation, SIGNAL (clicked()), this, SLOT (slotSaveCompilation()));

    connect(ui->buttonTabConfig, SIGNAL (clicked()), this, SLOT (slotButtonTabConfigClicked()));
    connect(ui->buttonTabPlay, SIGNAL (clicked()), this, SLOT (slotButtonTabPlayClicked()));
    connect(ui->buttonTabLog, SIGNAL (clicked()), this, SLOT (slotButtonTabLogClicked()));
    connect(ui->buttonTabChances, SIGNAL (clicked()), this, SLOT (slotButtonTabChancesClicked()));
    connect(ui->buttonTabError, SIGNAL (clicked()), this, SLOT (slotButtonTabErrorClicked()));

    connect( ui->horizontalSlider_speed, SIGNAL( valueChanged(int)), this, SLOT ( setGameSpeed(int) ) );
    connect( ui->pushButton_break, SIGNAL( clicked()), this, SLOT ( breakButtonClicked() ) ); // auch wieder starten!!!!

    connect( ui->checkBox_Cheat, SIGNAL( clicked()), this, SLOT ( slotCheckboxCheatClicked() ) );

    connect( ui->pushButton_Wine, SIGNAL( clicked()), this, SLOT ( slotButtonWineClicked() ) );

    //connect( tabWidget_Left, SIGNAL( currentChanged(int) ), this, SLOT( tabSwitchAction() ) ); //connect( lineEdit_ChatInput, SIGNAL( returnPressed () ), this, SLOT( sendChatMessage() ) );
    //connect( lineEdit_ChatInput, SIGNAL( textChanged (QString) ), this, SLOT( checkChatInputLength(QString) ) );
    //connect( lineEdit_ChatInput, SIGNAL( textEdited (QString) ), myChat, SLOT( setChatTextEdited() ) );

    //connect( radioButton_manualAction, SIGNAL( clicked() ) , this, SLOT( changePlayingMode() ) );
    //connect( radioButton_autoCheckFold, SIGNAL( clicked() ) , this, SLOT( changePlayingMode() ) );
    //connect( radioButton_autoCheckCallAny, SIGNAL( clicked() ), this, SLOT( changePlayingMode() ) );

    //connect( pushButton_voteOnKickYes, SIGNAL( clicked() ), this, SLOT( voteOnKickYes() ) );
    //connect( pushButton_voteOnKickNo, SIGNAL( clicked() ), this, SLOT( voteOnKickNo() ) );

    //connect( pushButton_showMyCards, SIGNAL( clicked() ), this, SLOT( sendShowMyCardsSignal() ) );
    //for(i=0; i<=9; i++)connect( playerTipLabelArray[i], SIGNAL( linkActivated(QString) ), playerAvatarLabelArray[i], SLOT(startChangePlayerTip(QString) ) );
    //for(i=0; i<=9; i++) {
    //    for(int j=1; j<=5; j++) {
    //        connect( playerStarsArray[j][i], SIGNAL( linkActivated(QString) ), playerAvatarLabelArray[i], SLOT(setPlayerRating(QString) ) );
    //    }
    //}
    //Nachrichten Thread-Save

    connect(this, SIGNAL(signalInitGui(int)), this, SLOT(initGui(int)));
    connect(this, SIGNAL(signalRefreshSet()), this, SLOT(refreshSet()));
    connect(this, SIGNAL(signalRefreshCash()), this, SLOT(refreshCash()));
    connect(this, SIGNAL(signalRefreshAction(int, int)), this, SLOT(refreshAction(int, int)));
    connect(this, SIGNAL(signalRefreshChangePlayer()), this, SLOT(refreshChangePlayer()));
    connect(this, SIGNAL(signalRefreshPot()), this, SLOT(refreshPot()));
    connect(this, SIGNAL(signalRefreshGroupbox(int, int)), this, SLOT(refreshGroupbox(int, int)));
    connect(this, SIGNAL(signalRefreshAll()), this, SLOT(refreshAll()));
    connect(this, SIGNAL(signalRefreshPlayerName()), this, SLOT(refreshPlayerName()));
    connect(this, SIGNAL(signalRefreshButton()), this, SLOT(refreshButton()));
    connect(this, SIGNAL(signalRefreshGameLabels(int)), this, SLOT(refreshGameLabels(int)));
    connect(this, SIGNAL(signalSetPlayerAvatar(int, QString)), this, SLOT(setPlayerAvatar(int, QString)));
    connect(this, SIGNAL(signalGuiUpdateDone()), this, SLOT(guiUpdateDone()));
    connect(this, SIGNAL(signalMeInAction()), this, SLOT(meInAction()));
    connect(this, SIGNAL(signalDisableMyButtons()), this, SLOT(disableMyButtons()));
    connect(this, SIGNAL(signalUpdateMyButtonsState()), this, SLOT(updateMyButtonsState()));
    connect(this, SIGNAL(signalDealBeRoCards(int)), this, SLOT(dealBeRoCards(int)));
    connect(this, SIGNAL(signalDealHoleCards()), this, SLOT(dealHoleCards()));
    connect(this, SIGNAL(signalDealFlopCards0()), this, SLOT(dealFlopCards0()));
    connect(this, SIGNAL(signalDealTurnCards0()), this, SLOT(dealTurnCards0()));
    connect(this, SIGNAL(signalDealRiverCards0()), this, SLOT(dealRiverCards0()));
    connect(this, SIGNAL(signalNextPlayerAnimation()), this, SLOT(nextPlayerAnimation()));
    connect(this, SIGNAL(signalBeRoAnimation2(int)), this, SLOT(beRoAnimation2(int)));
    connect(this, SIGNAL(signalPreflopAnimation1()), this, SLOT(preflopAnimation1()));
    connect(this, SIGNAL(signalPreflopAnimation2()), this, SLOT(preflopAnimation2()));
    connect(this, SIGNAL(signalFlopAnimation1()), this, SLOT(flopAnimation1()));
    connect(this, SIGNAL(signalFlopAnimation2()), this, SLOT(flopAnimation2()));
    connect(this, SIGNAL(signalTurnAnimation1()), this, SLOT(turnAnimation1()));
    connect(this, SIGNAL(signalTurnAnimation2()), this, SLOT(turnAnimation2()));
    connect(this, SIGNAL(signalRiverAnimation1()), this, SLOT(riverAnimation1()));
    connect(this, SIGNAL(signalRiverAnimation2()), this, SLOT(riverAnimation2()));
    connect(this, SIGNAL(signalPostRiverAnimation1()), this, SLOT(postRiverAnimation1()));
    connect(this, SIGNAL(signalPostRiverRunAnimation1()), this, SLOT(postRiverRunAnimation1()));
    connect(this, SIGNAL(signalPostRiverShowCards(unsigned)), this, SLOT(showHoleCards(unsigned)));
    connect(this, SIGNAL(signalFlipHolecardsAllIn()), this, SLOT(flipHolecardsAllIn()));
    connect(this, SIGNAL(signalNextRoundCleanGui()), this, SLOT(nextRoundCleanGui()));

    psThread = new QThread(this);
    psWorker = new PSWorker();
    psWorker->moveToThread(psThread);
    //connect(worker, SIGNAL (fatalError(QString)), this, SLOT (slotFatalError(QString)));
    //connect(worker, SIGNAL (error(QString)), this, SLOT (slotError(QString)));
    //connect(worker, SIGNAL (info(QString)), this, SLOT (slotInfo(QString)));
    connect(psWorker, SIGNAL (signalHandProbability(QVector<int>, QVector<double>)), this, SLOT (slotHandProbability(QVector<int>, QVector<double>)));
    connect(this, SIGNAL(signalHandChanged(QVector<int>)), psWorker, SLOT (slotHandChanged(QVector<int>)));
    connect(psThread, SIGNAL (finished()), psThread, SLOT (deleteLater()));
    connect(psThread, SIGNAL (finished()), psWorker, SLOT (deleteLater()));
    psThread->start(QThread::LowPriority);

    QSettings settings;
    if (settings.contains("gametable/smallBlind")) {
        ui->horizontalSlider_speed->setValue(settings.value("gametable/speed").toInt());
        ui->editSmallBlind->setValue(settings.value("gametable/smallBlind").toInt());
        ui->riseBlinds->setValue(settings.value("gametable/riseBlind").toInt());
    }

    ui->selectPlayer1->setProperty("attention", "start");
    ui->selectPlayer1->setStyleSheet(ui->selectPlayer1->styleSheet());
}

GameTableImpl::~GameTableImpl()
{
    delete myGuiInterface;
    delete ui;
}

void GameTableImpl::prepareClose() {
    psThread->quit();
    //psThread->terminate();
    bool done = psThread->wait(1000);

    QSettings settings;
    settings.setValue("gametable/speed", ui->horizontalSlider_speed->value());
    settings.setValue("gametable/smallBlind", ui->editSmallBlind->value());
    settings.setValue("gametable/riseBlind", ui->riseBlinds->value());
}

void GameTableImpl::startNewGame() {
    for (int i = 1; i <= 5; i++) {
        selectPlayerButtonArray[i]->setProperty("attention", "normal");
        selectPlayerButtonArray[i]->setStyleSheet(selectPlayerButtonArray[i]->styleSheet());
    }
    GameData gameData;
    gameData.maxNumberOfPlayers = 6;
    gameData.startMoney = 5000;
    gameData.firstSmallBlind = ui->editSmallBlind->value();
    gameData.raiseIntervalMode = RAISE_ON_HANDNUMBER;
    gameData.raiseSmallBlindEveryHandsValue = ui->riseBlinds->value();
    gameData.raiseMode = DOUBLE_BLINDS;
    gameData.afterManualBlindsMode = AFTERMB_DOUBLE_BLINDS;
    gameData.guiSpeed = ui->horizontalSlider_speed->value();

    StartData startData;
    if (tsFileName[2].isEmpty()) {
        startData.numberOfPlayers = 2;
    } else if (tsFileName[3].isEmpty()) {
        startData.numberOfPlayers = 3;
    } else if (tsFileName[4].isEmpty()) {
        startData.numberOfPlayers = 4;
    } else if (tsFileName[5].isEmpty()) {
        startData.numberOfPlayers = 5;
    } else {
        startData.numberOfPlayers = 6;
    }
    startData.startDealerPlayerId = 0;

    currentGame.reset();

    myGuiInterface->initGui(gameData.guiSpeed);

    PlayerDataList playerDataList;
    for(int i = 0; i < startData.numberOfPlayers; i++) {
        //Namen und Avatarpfad abfragen
        ostringstream myName;
        if (i==0) {
            myName << "MyName";
        } else {
            myName << "Opponent" << i << "Name";
        }
        ostringstream myAvatar;
        if (i==0) {
            myAvatar << "MyAvatar";
        } else {
            myAvatar << "Opponent" << i << "Avatar";
        }

        //PlayerData erzeugen
        // UniqueId = PlayerNumber for local games.
        std::shared_ptr<PlayerData> playerData(new PlayerData(
                    i,
                    i,
                    i == 0 ? PLAYER_TYPE_HUMAN : PLAYER_TYPE_COMPUTER,
                    PLAYER_RIGHTS_NORMAL,
                    i == 0));
        ostringstream name;
        if (i == 0) {
            name << "Human Player";
        } else {
            name << "Player" << i;
        }
        playerData->SetName(name.str());
        //playerData->SetName(myConfig->readConfigString(myName.str()));
        //playerData->SetAvatarFile(myConfig->readConfigString(myAvatar.str()));

        playerDataList.push_back(playerData);
    }
    // EngineFactory erstellen
    std::shared_ptr<EngineFactory> factory(new LocalEngineFactory(myConfig)); // LocalEngine erstellen

    currentGame.reset(new Game(myGuiInterface, factory, playerDataList, gameData, startData, 0 /*currentGameNum*/, myLog));

    PlayerListConstIterator it_c;
    PlayerList seatsList = currentGame->getSeatsList();
    for (it_c=seatsList->begin(); it_c!=seatsList->end(); ++it_c) {
        if ((*it_c)->getMyID()) {
            waitingForVideo |= 1 << (*it_c)->getMyID();
            qDebug() << "emit New Game" << (*it_c)->getMyID();
            signalPlayerAction((*it_c)->getMyID(), JobActionNewGame, piecesOfAmount((*it_c)->getMyCash()), true);
            refreshButton();
        }
    }

    //PlayerList seatsList1 = currentGame->getSeatsList();
    //for (it_c=seatsList1->begin(); it_c!=seatsList1->end(); ++it_c) {
    //    if ((*it_c)->getMyID()) {
    //    } else {
    //        (*it_c)->setMyCash(1000);
    //    }
    //}


    //// SPIEL-SCHLEIFE
    //currentGame->initHand();
    //currentGame->startHand();
    // SPIEL-SCHLEIFE
    breakAfterCurrentHand=false;
    startNewHand();
}

void GameTableImpl::initGui(int speed)
{
    //kill running Singleshots!!!
    stopTimer();

    guiGameSpeed = speed;
    setSpeeds();

    /*

    label_handNumber->setText(HandString+":");
    label_gameNumber->setText(GameString+":");

    //set WindowTitle dynamically
    QString titleString = "";
    assert(myStartWindow->getSession());
    if(myStartWindow->getSession()->getGameType() == Session::GAME_TYPE_INTERNET || myStartWindow->getSession()->getGameType() == Session::GAME_TYPE_NETWORK) {
        GameInfo info(myStartWindow->getSession()->getClientGameInfo(myStartWindow->getSession()->getClientCurrentGameId()));
        titleString = QString::fromUtf8(info.name.c_str())+" - ";
    }

    //show human player buttons
    for(int i=0; i<6; i++) {
        userWidgetsArray[i]->show();
    }

    //set speeds for local game and for first network game
    if( !myStartWindow->getSession()->isNetworkClientRunning() || (myStartWindow->getSession()->isNetworkClientRunning() && !myStartWindow->getSession()->getCurrentGame()) ) {
        */
        /*
    }

    //set session for chat
    myChat->setSession(this->getSession());

    this->setWindowTitle(QString(titleString + tr("PokerTH %1 - The Open-Source Texas Holdem Engine").arg(POKERTH_BETA_RELEASE_STRING)));

    label_Pot->setText(PotString);
    label_Total->setText(TotalString+":");
    label_Sets->setText(BetsString+":");
    groupBox_RightToolBox->setDisabled(false);
    groupBox_LeftToolBox->setDisabled(false);

    //set minimum gui speed to prevent gui lags on fast inet games
    if( myStartWindow->getSession()->isNetworkClientRunning() ) {
        horizontalSlider_speed->setMinimum(speed);
    } else {
        horizontalSlider_speed->setMinimum(1);
    }

    */
    ui->horizontalSlider_speed->setValue(guiGameSpeed);
}

//refresh-Funktionen
void GameTableImpl::refreshSet()
{
    /*
    std::shared_ptr<Game> currentGame = myStartWindow->getSession()->getCurrentGame();

    PlayerListConstIterator it_c;
    PlayerList seatsList = currentGame->getSeatsList();
    for (it_c=seatsList->begin(); it_c!=seatsList->end(); ++it_c) {
        if( (*it_c)->getMySet() == 0 )
            setLabelArray[(*it_c)->getMyID()]->setText("");
        else
            setLabelArray[(*it_c)->getMyID()]->setText("$"+QString("%L1").arg((*it_c)->getMySet()));
    }
    */
}

void GameTableImpl::refreshButton()
{
    //qDebug() << "refressButton" << waitingForVideo;
    QPixmap dealerButton = QPixmap::fromImage(QImage("://misc/dealer_puck.png"));
    QPixmap smallblindButton = QPixmap::fromImage(QImage("://misc/small_blind_puck.png"));
    QPixmap bigblindButton = QPixmap::fromImage(QImage("://misc/big_blind_puck.png"));
    QPixmap onePix = QPixmap::fromImage(QImage("://misc/1pix.png"));
    QPixmap hourglass = QPixmap::fromImage(QImage("://misc/hourglass.png"));

    PlayerListConstIterator it_c;
    PlayerList seatsList = currentGame->getSeatsList();
    PlayerList activePlayerList = currentGame->getActivePlayerList();
    for (it_c=seatsList->begin(); it_c!=seatsList->end(); ++it_c) {
        if (startNewHandDefered && (waitingForVideo & (1<<(*it_c)->getMyID()))) {
            buttonLabelArray[(*it_c)->getMyID()]->setPixmap(hourglass);
            //qDebug() << "refressButton" << (*it_c)->getMyID() << "hourglass";
        } else if( (*it_c)->getMyActiveStatus() ) {
            //qDebug() << "refressButton" << (*it_c)->getMyID() << "active";
            if( activePlayerList->size() > 2 ) {
                switch ( (*it_c)->getMyButton() ) {

                case 1 :
                    buttonLabelArray[(*it_c)->getMyID()]->setPixmap(dealerButton);
                    break;
                case 2 : {
                    //if(myConfig->readConfigInt("ShowBlindButtons"))
                        buttonLabelArray[(*it_c)->getMyID()]->setPixmap(smallblindButton);
                    //else
                    //    buttonLabelArray[(*it_c)->getMyID()]->setPixmap(onePix);
                }
                break;
                case 3 : {
                    //if(myConfig->readConfigInt("ShowBlindButtons"))
                        buttonLabelArray[(*it_c)->getMyID()]->setPixmap(bigblindButton);
                    //else
                    //    buttonLabelArray[(*it_c)->getMyID()]->setPixmap(onePix);
                }
                break;
                default:
                    buttonLabelArray[(*it_c)->getMyID()]->setPixmap(onePix);

                }
            } else {
                switch ((*it_c)->getMyButton()) {

                case 2 :
                    buttonLabelArray[(*it_c)->getMyID()]->setPixmap(dealerButton);
                    break;
                case 3 : {
                    //if(myConfig->readConfigInt("ShowBlindButtons"))
                        buttonLabelArray[(*it_c)->getMyID()]->setPixmap(bigblindButton);
                    //else
                    //    buttonLabelArray[(*it_c)->getMyID()]->setPixmap(onePix);
                }
                break;
                default:
                    buttonLabelArray[(*it_c)->getMyID()]->setPixmap(onePix);

                }
            }
        } else {
            //qDebug() << "refressButton" << (*it_c)->getMyID() << "empty";
            buttonLabelArray[(*it_c)->getMyID()]->setPixmap(onePix);
        }
    }
}


void GameTableImpl::refreshPlayerName()
{
    /*
    if(myStartWindow->getSession()->getCurrentGame()) {

        std::shared_ptr<Game> currentGame = myStartWindow->getSession()->getCurrentGame();
        PlayerListConstIterator it_c;
        PlayerList seatsList = currentGame->getSeatsList();
        for (it_c=seatsList->begin(); it_c!=seatsList->end(); ++it_c) {

            //collect needed infos
            bool guest = myStartWindow->getSession()->getClientPlayerInfo((*it_c)->getMyUniqueID()).isGuest;
            bool computerPlayer = false;
            if((*it_c)->getMyType() == PLAYER_TYPE_COMPUTER) {
                computerPlayer = true;
            }
            QString nick = QString::fromUtf8((*it_c)->getMyName().c_str());

            //check SeatStates and refresh
            switch(getCurrentSeatState((*it_c))) {

            case SEAT_ACTIVE: {
                playerNameLabelArray[(*it_c)->getMyID()]->setText(nick, false, guest, computerPlayer );
            }
            break;
            case SEAT_AUTOFOLD: {
                playerNameLabelArray[(*it_c)->getMyID()]->setText(nick, true, guest, computerPlayer );
            }
            break;
            case SEAT_STAYONTABLE: {
                playerNameLabelArray[(*it_c)->getMyID()]->setText(nick, true, guest, computerPlayer );
            }
            break;
            case SEAT_CLEAR: {
                playerNameLabelArray[(*it_c)->getMyID()]->setText("");
            }
            break;
            default: {
                playerNameLabelArray[(*it_c)->getMyID()]->setText("");
            }
            }
        }
    }

    playerAvatarLabelArray[0]->refreshTooltips();
    */
}

void GameTableImpl::refreshPlayerAvatar()
{
    /*
    if(myStartWindow->getSession()->getCurrentGame()) {

        QPixmap onePix = QPixmap::fromImage(QImage(myAppDataPath +"gfx/gui/misc/1px.png"));

        std::shared_ptr<Game> currentGame = myStartWindow->getSession()->getCurrentGame();
        int seatPlace;
        PlayerListConstIterator it_c;
        PlayerList seatsList = currentGame->getSeatsList();
        for (it_c=seatsList->begin(), seatPlace=0; it_c!=seatsList->end(); ++it_c, seatPlace++) {

            //set uniqueID
            playerAvatarLabelArray[(*it_c)->getMyID()]->setMyUniqueId((*it_c)->getMyUniqueID());

            //get CountryString
            QString countryString(QString(myStartWindow->getSession()->getClientPlayerInfo((*it_c)->getMyUniqueID()).countryCode.c_str()).toLower());
            countryString = QString(":/cflags/cflags/%1.png").arg(countryString);

            //get AvatarPic
            QFile myAvatarFile(QString::fromUtf8((*it_c)->getMyAvatar().c_str()));
            QPixmap avatarPic;
            if((*it_c)->getMyAvatar() == "" || !myAvatarFile.exists()) {
                avatarPic = QPixmap::fromImage(QImage(myGameTableStyle->getDefaultAvatar()));
            } else {
                avatarPic = QPixmap::fromImage(QImage(QString::fromUtf8((*it_c)->getMyAvatar().c_str())));
            }

            //check SeatStates and refresh
            switch(getCurrentSeatState((*it_c))) {

            case SEAT_ACTIVE: {
                playerAvatarLabelArray[(*it_c)->getMyID()]->setPixmapAndCountry(avatarPic, countryString, seatPlace);
            }
            break;
            case SEAT_AUTOFOLD: {
//				qDebug() << seatPlace << "AVATAR AUTOFOLD";
                playerAvatarLabelArray[(*it_c)->getMyID()]->setPixmapAndCountry(avatarPic, countryString, seatPlace, true);
            }
            break;
            case SEAT_STAYONTABLE: {
//				qDebug() << seatPlace << "AVATAR STAYONTABLE";
                playerAvatarLabelArray[(*it_c)->getMyID()]->setPixmapAndCountry(avatarPic, countryString, seatPlace, true);
            }
            break;
            case SEAT_CLEAR: {
                playerAvatarLabelArray[(*it_c)->getMyID()]->setPixmap(onePix);
            }
            break;
            default: {
                playerAvatarLabelArray[(*it_c)->getMyID()]->setPixmap(onePix);
            }
            }
        }
    }
    */
}

void GameTableImpl::setPlayerAvatar(int myID, QString myAvatar)
{
    /*
    if(myStartWindow->getSession()->getCurrentGame()) {

        std::shared_ptr<PlayerInterface> tmpPlayer = myStartWindow->getSession()->getCurrentGame()->getPlayerByUniqueId(myID);
        if (tmpPlayer.get()) {

            QFile myAvatarFile(myAvatar);
            if(myAvatarFile.exists()) {
                playerAvatarLabelArray[tmpPlayer->getMyID()]->setPixmap(myAvatar);
                tmpPlayer->setMyAvatar(myAvatar.toUtf8().constData());
            } else {
                playerAvatarLabelArray[tmpPlayer->getMyID()]->setPixmap(QPixmap::fromImage(QImage(myGameTableStyle->getDefaultAvatar())));
                tmpPlayer->setMyAvatar("");
            }
        }
    }
    */
}

void GameTableImpl::refreshAction(int playerID, int playerAction)
{
    QPixmap onePix = QPixmap::fromImage(QImage(actionPixName(0)));

    //QStringList actionArray;
    //actionArray << "" << "fold" << "check" << "call" << "bet" << "raise" << "allin";
    //qDebug() << "Action:" << actionArray[playerAction] << playerID;

    if(playerID == -1 || playerAction == -1) {

        PlayerListConstIterator it_c;
        PlayerList seatsList = currentGame->getSeatsList();
        for (it_c=seatsList->begin(); it_c!=seatsList->end(); ++it_c) {
            actionLabelArray[(*it_c)->getMyID()]->setPixmap(QPixmap::fromImage(QImage(actionPixName((*it_c)->getMyAction()))));
            //qDebug() << "AP" << (*it_c)->getMyAction();

            if ((*it_c)->getMyAction()==1) {
                if((*it_c)->getMyID() != 0) {
                    holeCardsArray[(*it_c)->getMyID()][0]->setPixmap(onePix, false);
                    holeCardsArray[(*it_c)->getMyID()][1]->setPixmap(onePix, false);
                }
            }
        }
    } else {
        actionLabelArray[playerID]->setPixmap(QPixmap::fromImage(QImage(actionPixName(playerAction))));
        //qDebug() << "AP" << playerAction;

            //play sounds if exist
            //if(myConfig->readConfigInt("PlayGameActions"))
            //    mySoundEventHandler->playSound(actionArray[playerAction].toStdString(), playerID);

        if (playerAction == 1) { // FOLD
            if (playerID == 0) {
                holeCardsArray[0][0]->startFadeOut(10);
                holeCardsArray[0][1]->startFadeOut(10);
                int cash = currentGame->getPlayerByNumber(playerID)->getMyCash();
                //qDebug() << "emit Fold" << playerID << cash;
                if (cash == 0) {
                    signalPlayerAction(playerID, JobActionFold, -1, false);
                    signalPlayerAction(playerID, JobActionDead, -1, false);
                } else {
                    signalPlayerAction(playerID, JobActionFold, -1, false);
                }
                std::shared_ptr<PlayerInterface> humanPlayer = currentGame->getSeatsList()->front();
                int lost = humanPlayerStartCash - humanPlayer->getMyCash();
                if (lost > 0 /*currentGame->getStartSmallBlind()*/) {
                    startZoomIn = false;
                }

            } else {
                holeCardsArray[playerID][0]->setPixmap(onePix, false);
                holeCardsArray[playerID][1]->setPixmap(onePix, false);
                int cash = currentGame->getPlayerByNumber(playerID)->getMyCash();
                //qDebug() << "emit Fold" << playerID << cash;
                if (cash == 0) {
                    signalPlayerAction(playerID, JobActionFold, piecesOfAmount(currentGame->getPlayerByNumber(playerID)->getMyCash()), false);
                    signalPlayerAction(playerID, JobActionDead, piecesOfAmount(currentGame->getPlayerByNumber(playerID)->getMyCash()), false);
                } else {
                    waitingForVideo |= 1 << playerID;
                    signalPlayerAction(playerID, JobActionFold, piecesOfAmount(currentGame->getPlayerByNumber(playerID)->getMyCash()), true);
                    refreshButton();
                }
            }
            refreshCardsChance(currentGame->getCurrentHand()->getCurrentRound(), false);
        } else if (playerAction == 2) {
            signalPlayerAction(playerID, JobActionCheck, -1, false);
        } else if (playerAction == 3) {
            signalPlayerAction(playerID, JobActionCall, -1, false);
        } else if (playerAction == 4) {
            signalPlayerAction(playerID, JobActionBet, -1, false);
        } else if (playerAction == 5) {
            signalPlayerAction(playerID, JobActionRaise, -1, false);
        } else if (playerAction == 6) {
            signalPlayerAction(playerID, JobActionAllIn, -1, false);
        }
    }
}

void GameTableImpl::refreshCash(bool handDone)
{
    //qDebug() << "  refreshCash" << handDone;
    //bool transparent = true;
    PlayerListConstIterator it_c;
    PlayerList seatsList = currentGame->getSeatsList();
    for (it_c=seatsList->begin(); it_c!=seatsList->end(); ++it_c) {

        //check SeatStates and refresh
        switch(getCurrentSeatState((*it_c))) {

        case SEAT_ACTIVE: {
            //qDebug() << (*it_c)->getMyID() << "CASH ACTIVE";
            cashLabelArray[(*it_c)->getMyID()]->setAmount((*it_c)->getMyCash(), handDone);
        }
        break;
        case SEAT_AUTOFOLD: {
            //qDebug() << (*it_c)->getMyID() << "CASH AUTOFOLD"; //TODO transparent
            cashLabelArray[(*it_c)->getMyID()]->setAmount((*it_c)->getMyCash(), false);
            //cashLabelArray[(*it_c)->getMyID()]->setText("$"+QString("%L1").arg((*it_c)->getMyCash()), transparent);
        }
        break;
        case SEAT_STAYONTABLE: {
            //cashLabelArray[(*it_c)->getMyID()]->setAmount(0, handDone);
        }
        break;
        case SEAT_CLEAR: {
            //cashLabelArray[(*it_c)->getMyID()]->setAmount(0, handDone);
        }
        break;
        default: {
            //cashLabelArray[(*it_c)->getMyID()]->setAmount(0, handDone);
        }
        }
    }

    std::shared_ptr<PlayerInterface> humanPlayer = currentGame->getSeatsList()->front();
    int cash = humanPlayer->getMyCash();
    if (humanPlayer->getMyCash() >= 2 * currentGame->getStartSmallBlind()) {
        ui->pushButton_Wine->setEnabled(true);
    } else {
        ui->pushButton_Wine->setEnabled(false);
    }


}

void GameTableImpl::refreshGroupbox(int playerID, int status)
{
    int j;

    if(playerID == -1 || status == -1) {
        PlayerListConstIterator it_c;
        PlayerList seatsList = currentGame->getSeatsList();
        for (it_c=seatsList->begin(); it_c!=seatsList->end(); ++it_c) {
            if((*it_c)->getMyTurn()) {
                //Groupbox glow wenn der Spiele dran ist.
                groupBoxArray[(*it_c)->getMyID()]->setActive(true);
            } else {
                //Groupbox auf Hintergrundfarbe setzen wenn der Spiele nicht dran aber aktiv ist.
                if((*it_c)->getMyActiveStatus()) {
                    if((*it_c)->getMyID()==0) {
                        //show buttons
                        for(j=0; j<6; j++) {
                            //userWidgetsArray[j]->show();
                        }
                    }
                    groupBoxArray[(*it_c)->getMyID()]->setActive(false);
                } else {
                    //Groupbox verdunkeln wenn der Spiele inactive ist.
                    if((*it_c)->getMyID()==0) {
                        //hide buttons
                        for(j=0; j<6; j++) {
                            //userWidgetsArray[j]->hide();
                        }
                    }
                    groupBoxArray[(*it_c)->getMyID()]->setActive(false);
                }
            }
        }
    } else {
        switch(status) {
            //inactive
        case 0: {
            if (!playerID) {
                //hide buttons
                for(j=0; j<6; j++) {
                    userWidgetsArray[j]->hide();
                }
            }
            groupBoxArray[playerID]->setActive(false);
        }
        break;
        //active but fold
        case 1: {
            if (!playerID) {
                //show buttons
                for(j=0; j<6; j++) {
                    //userWidgetsArray[j]->show();
                }
            }
            groupBoxArray[playerID]->setActive(false);
        }
        break;
        //active in action
        case 2:  {
            groupBoxArray[playerID]->setActive(true);
        }
        break;
        //active not in action
        case 3:  {
            groupBoxArray[playerID]->setActive(false);
        }
        break;
        default:
        {}
        }
    }
}

void GameTableImpl::refreshGameLabels(int gameState)
{

    switch(gameState) {
    case 0: {
        //textLabel_handLabel->setText(PreflopString);
    }
    break;
    case 1: {
        //textLabel_handLabel->setText(FlopString);
    }
    break;
    case 2: {
        //textLabel_handLabel->setText(TurnString);
    }
    break;
    case 3: {
        //textLabel_handLabel->setText(RiverString);
    }
    break;
    case 4: {
        //textLabel_handLabel->setText("");
    }
    break;
    default: {
        //textLabel_handLabel->setText("!!! ERROR !!!");
    }
    }

    //label_handNumberValue->setText(QString::number(myStartWindow->getSession()->getCurrentGame()->getCurrentHand()->getMyID(),10));
    //label_gameNumberValue->setText(QString::number(myStartWindow->getSession()->getCurrentGame()->getMyGameID(),10));
}

void GameTableImpl::refreshAll()
{
    refreshSet();
    refreshButton();

    PlayerListConstIterator it_c;
    PlayerList seatsList = currentGame->getSeatsList();
    for (it_c=seatsList->begin(); it_c!=seatsList->end(); ++it_c) {
        refreshAction( (*it_c)->getMyID(), (*it_c)->getMyAction());
    }

    refreshCash();
    refreshGroupbox();
    refreshPlayerName();
    refreshPlayerAvatar();
}

void GameTableImpl::refreshChangePlayer()
{
    refreshSet();

    PlayerListConstIterator it_c;
    PlayerList seatsList = currentGame->getSeatsList();
    for (it_c=seatsList->begin(); it_c!=seatsList->end(); ++it_c) {
        refreshAction( (*it_c)->getMyID(), (*it_c)->getMyAction());
    }

    refreshCash();
}

void GameTableImpl::refreshPot()
{
    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();

    //textLabel_Sets->setText("$"+QString("%L1").arg(currentHand->getBoard()->getSets()));
    //textLabel_Pot->setText("$"+QString("%L1").arg(currentHand->getBoard()->getPot()));
    /*
    QString txt("Pot ");
    if (currentHand->getBoard()->getPot() > 0) {
        txt += QString("$%L1").arg(currentHand->getBoard()->getPot());
        if (currentHand->getBoard()->getSets() > 0) {
            txt += " + ";
        }
    }
    if (currentHand->getBoard()->getSets() > 0) {
        txt += QString("$%L1").arg(currentHand->getBoard()->getSets());
    }
    if (currentHand->getBoard()->getPot() == 0 && currentHand->getBoard()->getSets() == 0) {
        txt += " empty";
    }
    */
    QString txt("Pot empty");
    int total = currentHand->getBoard()->getPot() + currentHand->getBoard()->getSets();
    if (total > 0 || !breakAfterCurrentHand) {
        ui->cashImage->setAmount(total);
        txt = QString("Pot $%L1").arg(total);
    }
    ui->textLabel_Pot_Sets->setText(txt);
}

void GameTableImpl::guiUpdateDone()
{
    guiUpdateSemaphore.release();
}

void GameTableImpl::waitForGuiUpdateDone()
{
    guiUpdateSemaphore.acquire();
}

void GameTableImpl::dealHoleCards()
{
    if (startZoomIn) {
        signalPlayerAction(-1, JobActionZoomIn, -1, false);
    } else {
        signalPlayerAction(-1, JobActionZoomOut, -1, false);
    }

    int i,k;
    for (i=0; i<MAX_NUMBER_OF_PLAYERS; i++ ) {
        for ( k=0; k<=1; k++ ) {
            holeCardsArray[i][k]->setFadeOutAction(false);
            holeCardsArray[i][k]->stopFlipCardsAnimation();
        }
    }

    QPixmap onePix = QPixmap::fromImage(QImage(actionPixName(0)));

    //TempArrays
    QPixmap tempCardsPixmapArray[2];
    int tempCardsIntArray[2];

    // Karten der Gegner und eigene Karten austeilen
    int j;

    PlayerListConstIterator it_c;
    PlayerList seatsList = currentGame->getSeatsList();
    for (it_c=seatsList->begin(); it_c!=seatsList->end(); ++it_c) {
        if((*it_c)->getMyActiveStatus()) {
            int odds, call, raise;
            (*it_c)->getMyOdds(&odds, &call, &raise);
            JobAction r = JobActionTakeBad;
            if (odds >= (call / 2)) { //FIXME
                r = JobActionTake;
            }
            if (odds >= call) {
                r = JobActionTakeGood;
            }
            //qDebug() << QString("emit Cards%1").arg(r == JobActionTakeBad ? "Bad" : (r == JobActionTakeGood ? "Good" : "")) << (*it_c)->getMyID() << odds << call << raise;
            signalPlayerAction((*it_c)->getMyID(), r, -1, false);
        } else {
            signalPlayerAction((*it_c)->getMyID(), JobActionShow, -1, false);
        }
        (*it_c)->getMyCards(tempCardsIntArray);
        for(j=0; j<2; j++) {
            if((*it_c)->getMyActiveStatus()) {
                if (( (*it_c)->getMyID() == 0) /*|| DEBUG_MODE*/ ) {
                    tempCardsPixmapArray[j].load("://cards/" + QString::number(tempCardsIntArray[j], 10) + ".png");
                    holeCardsArray[(*it_c)->getMyID()][j]->setPixmap(tempCardsPixmapArray[j],false);
                    holeCardsArray[(*it_c)->getMyID()][j]->setFront(tempCardsPixmapArray[j]);
                } else {
                    holeCardsArray[(*it_c)->getMyID()][j]->setPixmap(flipside, true);
                    holeCardsArray[(*it_c)->getMyID()][j]->setFlipsidePix(flipside);
                }
            } else {
                holeCardsArray[(*it_c)->getMyID()][j]->setPixmap(onePix, false);
            }
        }
    }

    //refresh CardsChanceMonitor Tool
    refreshCardsChance(GAME_STATE_PREFLOP, false);
}

void GameTableImpl::dealBeRoCards(int myBeRoID)
{
    //qDebug() << "dealBeRoCards";
    uncheckMyButtons();
    myButtonsCheckable(false);
    resetMyButtonsCheckStateMemory();
    clearMyButtons();

    ui->horizontalSlider_bet->setDisabled(true);
    ui->spinBox_betValue->setDisabled(true);

    switch(myBeRoID) {

    case 1: {
        signalPlayerAction(-1, JobActionDealFlopCards, -1, false);
        dealFlopCards0();
    }
    break;
    case 2: {
        signalPlayerAction(-1, JobActionDealTurnCards, -1, false);
        dealTurnCards0();
    }
    break;
    case 3: {
        signalPlayerAction(-1, JobActionDealRiverCards, -1, false);
        dealRiverCards0();
    }
    break;
    default: {
        cout << "dealBeRoCards() Error" << endl;
    }
    }
}


void GameTableImpl::dealFlopCards0()
{
    dealFlopCards0Timer->start(preDealCardsSpeed);
}

void GameTableImpl::dealFlopCards1()
{
    boardCardsArray[0]->setPixmap(flipside, true);
    dealFlopCards1Timer->start(dealCardsSpeed);
}

void GameTableImpl::dealFlopCards2()
{
    boardCardsArray[1]->setPixmap(flipside, true);
    dealFlopCards2Timer->start(dealCardsSpeed);
}

void GameTableImpl::dealFlopCards3()
{
    boardCardsArray[2]->setPixmap(flipside, true);
    dealFlopCards3Timer->start(dealCardsSpeed);
}

void GameTableImpl::dealFlopCards4()
{
    int tempBoardCardsArray[5];

    currentGame->getCurrentHand()->getBoard()->getMyCards(tempBoardCardsArray);
    QPixmap card = QPixmap::fromImage(QImage("://cards/" + QString::number(tempBoardCardsArray[0], 10) + ".png"));

    boardCardsArray[0]->startFlipCards(guiGameSpeed, card, flipside);
    dealFlopCards4Timer->start(dealCardsSpeed);
}

void GameTableImpl::dealFlopCards5()
{
    int tempBoardCardsArray[5];
    currentGame->getCurrentHand()->getBoard()->getMyCards(tempBoardCardsArray);
    QPixmap card = QPixmap::fromImage(QImage("://cards/" + QString::number(tempBoardCardsArray[1], 10) + ".png"));

    boardCardsArray[1]->startFlipCards(guiGameSpeed, card, flipside);
    dealFlopCards5Timer->start(dealCardsSpeed);
}

void GameTableImpl::dealFlopCards6()
{
    int tempBoardCardsArray[5];
    currentGame->getCurrentHand()->getBoard()->getMyCards(tempBoardCardsArray);
    QPixmap card = QPixmap::fromImage(QImage("://cards/" + QString::number(tempBoardCardsArray[2], 10) + ".png"));

    boardCardsArray[2]->startFlipCards(guiGameSpeed, card, flipside);

    // stable
    // wenn alle All In
    if(currentGame->getCurrentHand()->getAllInCondition()) {
        dealFlopCards6Timer->start(AllInDealCardsSpeed);
    } else { // sonst normale Variante
        updateMyButtonsState(0);  //mode 0 == called from dealberocards
        dealFlopCards6Timer->start(postDealCardsSpeed);
    }

    //refresh CardsChanceMonitor Tool
    refreshCardsChance(GAME_STATE_FLOP, true);
}

void GameTableImpl::dealTurnCards0()
{
    dealTurnCards0Timer->start(preDealCardsSpeed);
}

void GameTableImpl::dealTurnCards1()
{
    boardCardsArray[3]->setPixmap(flipside, true);
    dealTurnCards1Timer->start(dealCardsSpeed);
}

void GameTableImpl::dealTurnCards2()
{
    int tempBoardCardsArray[5];
    currentGame->getCurrentHand()->getBoard()->getMyCards(tempBoardCardsArray);
    QPixmap card = QPixmap::fromImage(QImage("://cards/" + QString::number(tempBoardCardsArray[3], 10) + ".png"));

    boardCardsArray[3]->startFlipCards(guiGameSpeed, card, flipside);

    // stable
    // wenn alle All In
    if(currentGame->getCurrentHand()->getAllInCondition()) {
        dealTurnCards2Timer->start(AllInDealCardsSpeed);
    } else { // sonst normale Variante
        updateMyButtonsState(0);  //mode 0 == called from dealberocards
        dealTurnCards2Timer->start(postDealCardsSpeed);
    }
    //refresh CardsChanceMonitor Tool
    refreshCardsChance(GAME_STATE_TURN, true);
}

void GameTableImpl::dealRiverCards0()
{
    dealRiverCards0Timer->start(preDealCardsSpeed);
}

void GameTableImpl::dealRiverCards1()
{
    boardCardsArray[4]->setPixmap(flipside, true);
    dealRiverCards1Timer->start(dealCardsSpeed);
}

void GameTableImpl::dealRiverCards2()
{
    int tempBoardCardsArray[5];
    currentGame->getCurrentHand()->getBoard()->getMyCards(tempBoardCardsArray);
    QPixmap card = QPixmap::fromImage(QImage("://cards/" + QString::number(tempBoardCardsArray[4], 10) + ".png"));

    boardCardsArray[4]->startFlipCards(guiGameSpeed, card, flipside);

    // stable
    // wenn alle All In
    if(currentGame->getCurrentHand()->getAllInCondition()) {
        dealRiverCards2Timer->start(AllInDealCardsSpeed);
    } else { // sonst normale Variante
        updateMyButtonsState(0);  //mode 0 == called from dealberocards
        dealRiverCards2Timer->start(postDealCardsSpeed);
    }
    //refresh CardsChanceMonitor Tool
    refreshCardsChance(GAME_STATE_RIVER, true);
}

void GameTableImpl::provideMyActions(int mode)
{
    QString pushButtonFoldString;
    QString pushButtonBetRaiseString;
    QString lastPushButtonBetRaiseString = ui->pushButton_BetRaise->text();
    QString pushButtonCallCheckString;
    QString pushButtonAllInString;
    QString lastPushButtonCallCheckString = ui->pushButton_CallCheck->text();
    int betProposal = 0;

    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();
    std::shared_ptr<PlayerInterface> humanPlayer = currentHand->getSeatsList()->front();
    PlayerList activePlayerList = currentHand->getActivePlayerList();

    //really disabled buttons if human player is fold/all-in or server-autofold... and not called from dealberocards
    if(humanPlayer->getMyCash() == 0 || (mode != 0 && (humanPlayer->getMyAction() == PLAYER_ACTION_ALLIN || humanPlayer->getMyAction() == PLAYER_ACTION_FOLD || (humanPlayer->getMySet() == currentHand->getCurrentBeRo()->getHighestSet() && (humanPlayer->getMyAction() != PLAYER_ACTION_NONE)))) || !humanPlayer->isSessionActive()) {
        ui->pushButton_BetRaise->setText("");
        ui->pushButton_CallCheck->setText("");
        ui->pushButton_Fold->setText("");
        ui->pushButton_AllIn->setText("");

        ui->horizontalSlider_bet->setDisabled(true);
        ui->spinBox_betValue->setDisabled(true);

        myButtonsCheckable(false);

        refreshActionButtonFKeyIndicator(1);
    } else {
        ui->horizontalSlider_bet->setEnabled(true);
        ui->spinBox_betValue->setEnabled(true);

        //show available actions on buttons
        if(currentHand->getCurrentRound() == 0) { // preflop

            if (humanPlayer->getMyCash()+humanPlayer->getMySet() > currentHand->getCurrentBeRo()->getHighestSet() && !currentHand->getCurrentBeRo()->getFullBetRule()) {
                pushButtonBetRaiseString = RaiseString+" $"+QString("%L1").arg(getMyBetAmount());
            }

            if (humanPlayer->getMySet()== currentHand->getCurrentBeRo()->getHighestSet() &&  humanPlayer->getMyButton() == 3) {
                pushButtonCallCheckString = CheckString;
            } else {
                pushButtonCallCheckString = CallString+" $"+QString("%L1").arg(getMyCallAmount());
                betProposal = getMyCallAmount();
            }

            pushButtonFoldString = FoldString;
            if(!currentHand->getCurrentBeRo()->getFullBetRule()) {
                pushButtonAllInString = AllInString;
            }
        } else { // flop,turn,river

            if (currentHand->getCurrentBeRo()->getHighestSet() == 0 && ui->pushButton_Fold->isCheckable() ) {
                pushButtonFoldString = CheckString+"/"+FoldString;
            } else {
                pushButtonFoldString = FoldString;
            }
            if (currentHand->getCurrentBeRo()->getHighestSet() == 0) {
                pushButtonCallCheckString = CheckString;
                pushButtonBetRaiseString = BetString+" $"+QString("%L1").arg(getMyBetAmount());
            }
            if (currentHand->getCurrentBeRo()->getHighestSet() > 0 && currentHand->getCurrentBeRo()->getHighestSet() > humanPlayer->getMySet()) {
                pushButtonCallCheckString = CallString+" $"+QString("%L1").arg(getMyCallAmount());
                betProposal = getMyCallAmount();
                if (humanPlayer->getMyCash()+humanPlayer->getMySet() > currentHand->getCurrentBeRo()->getHighestSet() && !currentHand->getCurrentBeRo()->getFullBetRule()) {
                    pushButtonBetRaiseString = RaiseString+" $"+QString("%L1").arg(getMyBetAmount());
                }
            }
            if(!currentHand->getCurrentBeRo()->getFullBetRule()) {
                pushButtonAllInString = AllInString;
            }
        }

        if(mode == 0) {
            if( humanPlayer->getMyAction() != PLAYER_ACTION_FOLD ) {
                pushButtonBetRaiseString = BetString+" $"+QString("%L1").arg(getMyBetAmount());
                pushButtonCallCheckString = CheckString;
                if( (activePlayerList->size() > 2 && humanPlayer->getMyButton() == BUTTON_SMALL_BLIND ) || ( activePlayerList->size() <= 2 && humanPlayer->getMyButton() == BUTTON_BIG_BLIND)) {
                    pushButtonFoldString = FoldString;
                } else {
                    pushButtonFoldString = CheckString+"/"+FoldString;
                }

                pushButtonAllInString = AllInString;
            } else {
                pushButtonBetRaiseString = "";
                pushButtonCallCheckString = "";
                pushButtonFoldString = "";
                pushButtonAllInString = "";
                ui->horizontalSlider_bet->setDisabled(true);
                ui->spinBox_betValue->setDisabled(true);

                myButtonsCheckable(false);

            }
        }

        //if text changed on checked button --> do something to prevent unwanted actions
        if(pushButtonCallCheckString != lastPushButtonCallCheckString) {

            if(ui->pushButton_CallCheck->isChecked()) {
                //uncheck a previous checked button to prevent unwanted action
                uncheckMyButtons();
                resetMyButtonsCheckStateMemory();
            }
            //disable button to prevent unwanted clicks (e.g. call allin)
            //if(myConfig->readConfigInt("AccidentallyCallBlocker")) {
            //    pushButton_CallCheck->setEatMyEvents(true);
            //    enableCallCheckPushButtonTimer->start(1000);
            //}

        }

        if(pushButtonBetRaiseString == "") {

            ui->horizontalSlider_bet->setDisabled(true);
            ui->spinBox_betValue->setDisabled(true);
        }

        ui->pushButton_Fold->setText(pushButtonFoldString);
        ui->pushButton_BetRaise->setText(pushButtonBetRaiseString);
        ui->pushButton_CallCheck->setText(pushButtonCallCheckString);
        ui->pushButton_AllIn->setText(pushButtonAllInString);

        refreshActionButtonFKeyIndicator();
        // 		myBetRaise();

        if(ui->pushButton_BetRaise->text().startsWith(RaiseString)) {

            ui->horizontalSlider_bet->setMinimum(currentHand->getCurrentBeRo()->getHighestSet() - humanPlayer->getMySet() + currentHand->getCurrentBeRo()->getMinimumRaise());
            ui->horizontalSlider_bet->setMaximum(humanPlayer->getMyCash());
            ui->horizontalSlider_bet->setSingleStep(10);
            ui->spinBox_betValue->setMinimum(currentHand->getCurrentBeRo()->getHighestSet() - humanPlayer->getMySet() + currentHand->getCurrentBeRo()->getMinimumRaise());
            ui->spinBox_betValue->setMaximum(humanPlayer->getMyCash());
            changeSpinBoxBetValue(ui->horizontalSlider_bet->value());

            myActionIsRaise = 1;
        } else if(ui->pushButton_BetRaise->text().startsWith(BetString)) {

            ui->horizontalSlider_bet->setMinimum(currentHand->getSmallBlind()*2);
            ui->horizontalSlider_bet->setMaximum(humanPlayer->getMyCash());
            ui->horizontalSlider_bet->setSingleStep(10);
            ui->spinBox_betValue->setMinimum(currentHand->getSmallBlind()*2);
            ui->spinBox_betValue->setMaximum(humanPlayer->getMyCash());
            changeSpinBoxBetValue(ui->horizontalSlider_bet->value());

            myActionIsBet = 1;
        } else {}


        //if value changed on bet/raise button --> uncheck to prevent unwanted actions
        int lastBetValue = lastPushButtonBetRaiseString.simplified().remove(QRegExp("[^0-9]")).toInt();

        if((lastBetValue < ui->horizontalSlider_bet->minimum() && ui->pushButton_BetRaise->isChecked())) {

            uncheckMyButtons();
            resetMyButtonsCheckStateMemory();
        }

        if (betProposal) {
            int pot = currentHand->getBoard()->getPot() + currentHand->getBoard()->getSets();
            ui->labelPotToBet->setText(QString::asprintf("$%d / $%d", pot, betProposal));
            ui->labelProbabilityPot->setText(QString::asprintf("%.1f%%", ((double)betProposal) / (betProposal + pot) * 100));
        } else {
            ui->labelPotToBet->setText("");
            ui->labelProbabilityPot->setText("");
        }

        ui->spinBox_betValue->setFocus();
        ui->spinBox_betValue->selectAll();

    }
}

void GameTableImpl::meInAction()
{
    myButtonsCheckable(false);

    ui->horizontalSlider_bet->setEnabled(true);
    ui->spinBox_betValue->setEnabled(true);

    myActionIsRaise = 0;
    myActionIsBet = 0;

    QString lastPushButtonFoldString = ui->pushButton_Fold->text();

    //paint actions on buttons
    provideMyActions();

    //do remembered action
    if( pushButtonBetRaiseIsChecked ) {
        ui->pushButton_BetRaise->click();
        pushButtonBetRaiseIsChecked = false;
    }
    if( pushButtonCallCheckIsChecked )  {
        ui->pushButton_CallCheck->click();
        pushButtonCallCheckIsChecked = false;
    }
    if( pushButtonFoldIsChecked ) {
        if(lastPushButtonFoldString == CheckString+"/"+FoldString && ui->pushButton_CallCheck->text() == CheckString) {
            ui->pushButton_CallCheck->click();
        } else {
            ui->pushButton_Fold->click();
        }
        pushButtonFoldIsChecked = false;
    }
    if( pushButtonAllInIsChecked ) {
        ui->pushButton_AllIn->click();
        pushButtonAllInIsChecked = false;
    }
}

void GameTableImpl::disableMyButtons()
{
    std::shared_ptr<PlayerInterface> humanPlayer = currentGame->getCurrentHand()->getSeatsList()->front();

    clearMyButtons();

    //clear userWidgets
    ui->horizontalSlider_bet->setDisabled(true);
    ui->spinBox_betValue->setDisabled(true);
    ui->horizontalSlider_bet->setMinimum(0);
    ui->horizontalSlider_bet->setMaximum(humanPlayer->getMyCash());
    ui->spinBox_betValue->setMinimum(0);
    ui->spinBox_betValue->setMaximum(humanPlayer->getMyCash());
    ui->spinBox_betValue->clear();
    ui->horizontalSlider_bet->setValue(0);
}

void GameTableImpl::myCallCheck()
{
    if(ui->pushButton_CallCheck->text().startsWith(CallString)) {
        myCall();
    }
    if(ui->pushButton_CallCheck->text() == CheckString) {
        myCheck();
    }
}

void GameTableImpl::myFold()
{
    if(ui->pushButton_Fold->text() == FoldString) {

        std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();
        std::shared_ptr<PlayerInterface> humanPlayer = currentHand->getSeatsList()->front();
        humanPlayer->setMyAction(PLAYER_ACTION_FOLD,true);
        humanPlayer->setMyTurn(0);

        //set that i was the last active player. need this for unhighlighting groupbox
        currentHand->setPreviousPlayerID(0);

        // 		statusBar()->clearMessage();

        //Spiel läuft weiter
        myActionDone();
    }
}

void GameTableImpl::myCheck()
{
    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();
    std::shared_ptr<PlayerInterface> humanPlayer = currentHand->getSeatsList()->front();
    humanPlayer->setMyTurn(0);
    humanPlayer->setMyAction(PLAYER_ACTION_CHECK,true);

    //set that i was the last active player. need this for unhighlighting groupbox
    currentHand->setPreviousPlayerID(0);

    // 	statusBar()->clearMessage();

    //Spiel läuft weiter
    myActionDone();
}

int GameTableImpl::getMyCallAmount()
{
    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();
    std::shared_ptr<PlayerInterface> humanPlayer = currentHand->getSeatsList()->front();
    int tempHighestSet = currentHand->getCurrentBeRo()->getHighestSet();

    if (humanPlayer->getMyCash() + humanPlayer->getMySet() <= tempHighestSet) {

        return humanPlayer->getMyCash();
    } else {
        return tempHighestSet - humanPlayer->getMySet();
    }
}

int GameTableImpl::getBetRaisePushButtonValue()
{
    int betValue = ui->pushButton_BetRaise->text().simplified().remove(QRegExp("[^0-9]")).toInt();
    return betValue;
}

int GameTableImpl::getMyBetAmount()
{
    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();
    std::shared_ptr<PlayerInterface> humanPlayer = currentHand->getSeatsList()->front();

    int betValue = getBetRaisePushButtonValue();
    int minimum;

    minimum = currentHand->getCurrentBeRo()->getHighestSet() - humanPlayer->getMySet() + currentHand->getCurrentBeRo()->getMinimumRaise();

    if(betValue < minimum) {
        return min(minimum,humanPlayer->getMyCash());
    } else {
        return betValue;
    }
}

void GameTableImpl::myCall()
{
    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();
    std::shared_ptr<PlayerInterface> humanPlayer = currentHand->getSeatsList()->front();

    int tempHighestSet = currentHand->getCurrentBeRo()->getHighestSet();

    if (humanPlayer->getMyCash() + humanPlayer->getMySet() <= tempHighestSet) {

        humanPlayer->setMySet(humanPlayer->getMyCash());
        humanPlayer->setMyCash(0);
        humanPlayer->setMyAction(PLAYER_ACTION_ALLIN,true);
    } else {
        humanPlayer->setMySet(tempHighestSet - humanPlayer->getMySet());
        humanPlayer->setMyAction(PLAYER_ACTION_CALL,true);
    }
    humanPlayer->setMyTurn(0);

    currentHand->getBoard()->collectSets();
    refreshPot();

    //set that i was the last active player. need this for unhighlighting groupbox
    currentHand->setPreviousPlayerID(0);

    // 	statusBar()->clearMessage();

    //Spiel läuft weiter
    myActionDone();
}

void GameTableImpl::mySet()
{
    if(ui->pushButton_BetRaise->text() != "") {

        std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();
        std::shared_ptr<PlayerInterface> humanPlayer = currentHand->getSeatsList()->front();

        int tempCash = humanPlayer->getMyCash();

        // 		cout << "Set-Value " << getBetRaisePushButtonValue() << endl;
        humanPlayer->setMySet(getBetRaisePushButtonValue());

        if (getBetRaisePushButtonValue() >= tempCash ) {

            humanPlayer->setMySet(humanPlayer->getMyCash());
            humanPlayer->setMyCash(0);
            humanPlayer->setMyAction(PLAYER_ACTION_ALLIN,true);

            // full bet rule
            if(currentHand->getCurrentBeRo()->getHighestSet() + currentHand->getCurrentBeRo()->getMinimumRaise() > humanPlayer->getMySet()) {
                currentHand->getCurrentBeRo()->setFullBetRule(true);
            }
        }

        if(myActionIsRaise) {
            //do not if allIn
            if(humanPlayer->getMyAction() != 6) {
                humanPlayer->setMyAction(PLAYER_ACTION_RAISE,true);
            }
            myActionIsRaise = 0;

            currentHand->getCurrentBeRo()->setMinimumRaise(humanPlayer->getMySet() - currentHand->getCurrentBeRo()->getHighestSet());
        }

        if(myActionIsBet) {
            //do not if allIn
            if(humanPlayer->getMyAction() != 6) {
                humanPlayer->setMyAction(PLAYER_ACTION_BET,true);
            }
            myActionIsBet = 0;

            currentHand->getCurrentBeRo()->setMinimumRaise(humanPlayer->getMySet());
        }

        currentHand->getCurrentBeRo()->setHighestSet(humanPlayer->getMySet());

        humanPlayer->setMyTurn(0);

        currentHand->getBoard()->collectSets();
        refreshPot();

        // 		statusBar()->clearMessage();

        //set that i was the last active player. need this for unhighlighting groupbox
        currentHand->setPreviousPlayerID(0);

        // lastPlayerAction für Karten umblättern reihenfolge setzrn
        currentHand->setLastActionPlayerID(humanPlayer->getMyUniqueID());

        //Spiel läuft weiter
        myActionDone();
    }
}

void GameTableImpl::myAllIn()
{
    if(ui->pushButton_AllIn->text() == AllInString) {

        std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();
        std::shared_ptr<PlayerInterface> humanPlayer = currentHand->getSeatsList()->front();

        humanPlayer->setMySet(humanPlayer->getMyCash());
        humanPlayer->setMyCash(0);
        humanPlayer->setMyAction(PLAYER_ACTION_ALLIN,true);

        // full bet rule
        if(currentHand->getCurrentBeRo()->getHighestSet() + currentHand->getCurrentBeRo()->getMinimumRaise() > humanPlayer->getMySet()) {
            currentHand->getCurrentBeRo()->setFullBetRule(true);
        }

        if(humanPlayer->getMySet() > currentHand->getCurrentBeRo()->getHighestSet()) {
            currentHand->getCurrentBeRo()->setMinimumRaise(humanPlayer->getMySet() - currentHand->getCurrentBeRo()->getHighestSet());

            currentHand->getCurrentBeRo()->setHighestSet(humanPlayer->getMySet());

            // lastPlayerAction für Karten umblättern reihenfolge setzrn
            currentHand->setLastActionPlayerID(humanPlayer->getMyUniqueID());
        }

        humanPlayer->setMyTurn(0);

        currentHand->getBoard()->collectSets();
        refreshPot();

        //set that i was the last active player. need this for unhighlighting groupbox
        currentHand->setPreviousPlayerID(0);

        //Spiel läuft weiter
        myActionDone();
    }
}


void GameTableImpl::pushButtonBetRaiseClicked(bool checked)
{
    if (ui->pushButton_BetRaise->isCheckable()) {
        if(checked) {
            ui->pushButton_CallCheck->setChecked(false);
            ui->pushButton_Fold->setChecked(false);
            ui->pushButton_AllIn->setChecked(false);

            pushButtonCallCheckIsChecked = false;
            pushButtonFoldIsChecked = false;
            pushButtonAllInIsChecked = false;

            pushButtonBetRaiseIsChecked = true;

            //if(!radioButton_manualAction->isChecked())
            //    radioButton_manualAction->click();
        } else {
            pushButtonBetRaiseIsChecked = false;
            myLastPreActionBetValue = 0;
        }
    } else {
        mySet();
    }
}

void GameTableImpl::pushButtonCallCheckClicked(bool checked)
{
    if (ui->pushButton_CallCheck->isCheckable()) {
        if(checked) {
            ui->pushButton_Fold->setChecked(false);
            ui->pushButton_BetRaise->setChecked(false);
            ui->pushButton_AllIn->setChecked(false);

            pushButtonAllInIsChecked = false;
            pushButtonFoldIsChecked = false;
            pushButtonBetRaiseIsChecked = false;

            pushButtonCallCheckIsChecked = true;

            //if(!radioButton_manualAction->isChecked())
            //    radioButton_manualAction->click();
        } else {
            pushButtonCallCheckIsChecked = false;
        }
    } else {
        myCallCheck();
    }
}

void GameTableImpl::pushButtonFoldClicked(bool checked)
{
    if (ui->pushButton_Fold->isCheckable()) {
        if(checked) {
            ui->pushButton_CallCheck->setChecked(false);
            ui->pushButton_BetRaise->setChecked(false);
            ui->pushButton_AllIn->setChecked(false);

            pushButtonAllInIsChecked = false;
            pushButtonCallCheckIsChecked = false;
            pushButtonBetRaiseIsChecked = false;

            pushButtonFoldIsChecked = true;

            //if(!radioButton_manualAction->isChecked())
            //    radioButton_manualAction->click();
        } else {
            pushButtonFoldIsChecked = false;
        }
    } else {
        myFold();
    }
}

void GameTableImpl::pushButtonAllInClicked(bool checked)
{
    if (ui->pushButton_AllIn->isCheckable()) {
        if(checked) {
            ui->pushButton_CallCheck->setChecked(false);
            ui->pushButton_BetRaise->setChecked(false);
            ui->pushButton_Fold->setChecked(false);

            pushButtonFoldIsChecked = false;
            pushButtonCallCheckIsChecked = false;
            pushButtonBetRaiseIsChecked = false;

            pushButtonAllInIsChecked = true;

            //if(!radioButton_manualAction->isChecked())
            //    radioButton_manualAction->click();
        } else {
            pushButtonAllInIsChecked = false;
        }
    } else {
        myAllIn();
    }
}

void GameTableImpl::myActionDone()
{
    // TODO: Should not call in networking game.
    disableMyButtons();

    nextPlayerAnimation();

    //prevent escape button working while allIn
    myActionIsRaise = 0;
    myActionIsBet = 0;
}

void GameTableImpl::nextPlayerAnimation()
{
    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();

    //refresh Change Player
    refreshSet();

    PlayerListConstIterator it_c;
    PlayerList seatsList = currentHand->getSeatsList();
    for (it_c=seatsList->begin(); it_c!=seatsList->end(); ++it_c) {
        if((*it_c)->getMyID() == currentHand->getPreviousPlayerID()) break;
    }

    if(currentHand->getPreviousPlayerID() != -1) {
        refreshAction(currentHand->getPreviousPlayerID(), (*it_c)->getMyAction());
    }
    refreshCash();

    //refresh actions for human player
    updateMyButtonsState();

    nextPlayerAnimationTimer->start(nextPlayerSpeed1);
}

void GameTableImpl::beRoAnimation2(int myBeRoID)
{

    switch(myBeRoID) {

    case 0: {
        preflopAnimation2();
    }
    break;
    case 1: {
        flopAnimation2();
    }
    break;
    case 2: {
        turnAnimation2();
    }
    break;
    case 3: {
        riverAnimation2();
    }
    break;
    default: {
        cout << "beRoAnimation2() Error" << endl;
    }
    }
}


void GameTableImpl::preflopAnimation1()
{
    preflopAnimation1Timer->start(nextPlayerSpeed2);
}
void GameTableImpl::preflopAnimation1Action()
{
    currentGame->getCurrentHand()->getCurrentBeRo()->run();
}

void GameTableImpl::preflopAnimation2()
{
    preflopAnimation2Timer->start(preflopNextPlayerSpeed);
}
void GameTableImpl::preflopAnimation2Action()
{
    currentGame->getCurrentHand()->getCurrentBeRo()->nextPlayer();
}


void GameTableImpl::flopAnimation1()
{
    flopAnimation1Timer->start(nextPlayerSpeed2);
}
void GameTableImpl::flopAnimation1Action()
{
    currentGame->getCurrentHand()->getCurrentBeRo()->run();
}

void GameTableImpl::flopAnimation2()
{
    flopAnimation2Timer->start(nextPlayerSpeed3);
}
void GameTableImpl::flopAnimation2Action()
{
    currentGame->getCurrentHand()->getCurrentBeRo()->nextPlayer();
}

void GameTableImpl::turnAnimation1()
{
    turnAnimation1Timer->start(nextPlayerSpeed2);
}
void GameTableImpl::turnAnimation1Action()
{
    currentGame->getCurrentHand()->getCurrentBeRo()->run();
}

void GameTableImpl::turnAnimation2()
{
    turnAnimation2Timer->start(nextPlayerSpeed3);
}
void GameTableImpl::turnAnimation2Action()
{
    currentGame->getCurrentHand()->getCurrentBeRo()->nextPlayer();
}

void GameTableImpl::riverAnimation1()
{
    riverAnimation1Timer->start(nextPlayerSpeed2);
}
void GameTableImpl::riverAnimation1Action()
{
    currentGame->getCurrentHand()->getCurrentBeRo()->run();
}

void GameTableImpl::riverAnimation2()
{
    riverAnimation2Timer->start(nextPlayerSpeed3);
}
void GameTableImpl::riverAnimation2Action()
{
    currentGame->getCurrentHand()->getCurrentBeRo()->nextPlayer();
}

void GameTableImpl::postRiverAnimation1()
{
    postRiverAnimation1Timer->start(nextPlayerSpeed2);
}
void GameTableImpl::postRiverAnimation1Action()
{
    currentGame->getCurrentHand()->getCurrentBeRo()->postRiverRun();
}

void GameTableImpl::postRiverRunAnimation1()
{
    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();
    PlayerListConstIterator it_c;
    PlayerList activePlayerList = currentHand->getActivePlayerList();
    std::shared_ptr<PlayerInterface> humanPlayer = currentHand->getSeatsList()->front();
    bool humanWin =  humanPlayer->getMyCash() > 0 &&  humanPlayer->getMyAction() != PLAYER_ACTION_FOLD && humanPlayer->getMyCardsValueInt() == currentHand->getCurrentBeRo()->getHighestCardsValue();
    bool humanFolded =  humanPlayer->getMyCash() == 0 || humanPlayer->getMyAction() == PLAYER_ACTION_FOLD;

    for(it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {

        if ((*it_c)->getMyID() == 0) {
            int cash = (*it_c)->getMyCash();
            if((*it_c)->getMyAction() != PLAYER_ACTION_FOLD && (*it_c)->getMyCardsValueInt() == currentHand->getCurrentBeRo()->getHighestCardsValue() ) {
                //qDebug() << "emit Win" << (*it_c)->getMyID() << cash;
                signalPlayerAction((*it_c)->getMyID(), JobActionWin, -1, false);
                startZoomIn = true;
            } else if((*it_c)->getMyAction() != PLAYER_ACTION_FOLD) {
                //qDebug() << "enmit Lose" << (*it_c)->getMyID() << cash;
                if (cash == 0) {
                    signalPlayerAction((*it_c)->getMyID(), JobActionLose, -1, false);
                    signalPlayerAction((*it_c)->getMyID(), JobActionDead, -1, false);
                } else {
                    signalPlayerAction((*it_c)->getMyID(), JobActionLose, -1, false);
                }
                startZoomIn = false;
            }
        } else {
            int cash = (*it_c)->getMyCash();
            if((*it_c)->getMyAction() != PLAYER_ACTION_FOLD && (*it_c)->getMyCardsValueInt() == currentHand->getCurrentBeRo()->getHighestCardsValue() ) {
                //qDebug() << "emit Win" << (*it_c)->getMyID() << cash;
                waitingForVideo |= 1 << (*it_c)->getMyID();
                signalPlayerAction((*it_c)->getMyID(), (humanWin || humanFolded) ? JobActionWin : JobActionWinYoulose, piecesOfAmount((*it_c)->getMyCash()), true);
                refreshButton();
            } else if((*it_c)->getMyAction() != PLAYER_ACTION_FOLD) {
                //qDebug() << "enmit Lose" << (*it_c)->getMyID() << cash;
                if (cash == 0) {
                    signalPlayerAction((*it_c)->getMyID(), JobActionLose, piecesOfAmount((*it_c)->getMyCash()), false);
                    signalPlayerAction((*it_c)->getMyID(), JobActionDead, piecesOfAmount((*it_c)->getMyCash()), false);
                } else {
                    waitingForVideo |= 1 << (*it_c)->getMyID();
                    signalPlayerAction((*it_c)->getMyID(), humanWin ? JobActionLoseYouWin : JobActionLose, piecesOfAmount((*it_c)->getMyCash()), true);
                    refreshButton();
                }
            }
        }
    }

    humanPlayerStartCash = humanPlayer->getMyCash();

    postRiverRunAnimation1Timer->start(postRiverRunAnimationSpeed);
}

void GameTableImpl::postRiverRunAnimation2()
{
    uncheckMyButtons();
    myButtonsCheckable(false);
    clearMyButtons();
    resetMyButtonsCheckStateMemory();

    ui->horizontalSlider_bet->setDisabled(true);
    ui->spinBox_betValue->setDisabled(true);

    int nonfoldPlayersCounter = 0;
    PlayerListConstIterator it_c;
    PlayerList activePlayerList = currentGame->getActivePlayerList();
    for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
        if ((*it_c)->getMyAction() != PLAYER_ACTION_FOLD)
            nonfoldPlayersCounter++;
    }
    if(nonfoldPlayersCounter!=1) {
        if(!flipHolecardsAllInAlreadyDone) {
            for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
                if((*it_c)->getMyAction() != PLAYER_ACTION_FOLD && (*it_c)->checkIfINeedToShowCards()) {

                    showHoleCards((*it_c)->getMyUniqueID());
                }
            }
            //Wenn einmal umgedreht dann fertig!!
            flipHolecardsAllInAlreadyDone = true;
        } else {
            for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
                if((*it_c)->getMyAction() != PLAYER_ACTION_FOLD) {
                    //set Player value (logging) for all in already shown cards
                    (*it_c)->setMyCardsFlip(1,3);
                }
            }
        }
        postRiverRunAnimation2Timer->start(postRiverRunAnimationSpeed);
    } else {
        postRiverRunAnimation3();
    }
}

void GameTableImpl::postRiverRunAnimation3()
{
    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();

    int nonfoldPlayerCounter = 0;
    PlayerListConstIterator it_c;

    PlayerList activePlayerList = currentHand->getActivePlayerList();
    for(it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
        if((*it_c)->getMyAction() != PLAYER_ACTION_FOLD) {
            nonfoldPlayerCounter++;
        }
    }

    list<unsigned> winners = currentHand->getBoard()->getWinners();

    for(it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
        if((*it_c)->getMyAction() != PLAYER_ACTION_FOLD && (*it_c)->getMyCardsValueInt() == currentHand->getCurrentBeRo()->getHighestCardsValue() ) {

            //Show "Winner" label
            actionLabelArray[(*it_c)->getMyID()]->setPixmap(QPixmap::fromImage(QImage(actionPixName(7))));
            //qDebug() << "AP" << "7";
            //show winnercards if more than one player is active TODO
            if ( nonfoldPlayerCounter != 1) {

                int j;
                int bestHandPos[5];
                (*it_c)->getMyBestHandPosition(bestHandPos);

                bool index0 = true;
                for(j=0; j<5; j++) {
                    if (bestHandPos[j] == 0 ) {
                        index0 = false;
                    }
                }
                if (index0) {
                    holeCardsArray[(*it_c)->getMyID()][0]->startFadeOut(guiGameSpeed); /* cout << "Fade Out index0" << endl; */
                }
                //index 1 testen
                bool index1 = true;
                for(j=0; j<5; j++) {
                    if (bestHandPos[j] == 1 ) {
                        index1 = false;
                    }
                }
                if (index1) {
                    holeCardsArray[(*it_c)->getMyID()][1]->startFadeOut(guiGameSpeed); /*cout << "Fade Out index1" << endl;*/
                }
                //index 2 testen
                bool index2 = true;
                for(j=0; j<5; j++) {
                    if (bestHandPos[j] == 2 ) {
                        index2 = false;
                    }
                }
                if (index2) {
                    boardCardsArray[0]->startFadeOut(guiGameSpeed); /*cout << "Fade Out index2" << endl;*/
                }
                //index 3 testen
                bool index3 = true;
                for(j=0; j<5; j++) {
                    if (bestHandPos[j] == 3 ) {
                        index3 = false;
                    }
                }
                if (index3) {
                    boardCardsArray[1]->startFadeOut(guiGameSpeed); /*cout << "Fade Out index3" << endl;*/
                }
                //index 4 testen
                bool index4 = true;
                for(j=0; j<5; j++) {
                    if (bestHandPos[j] == 4 ) {
                        index4 = false;
                    }
                }
                if (index4) {
                    boardCardsArray[2]->startFadeOut(guiGameSpeed); /*cout << "Fade Out index4" << endl;*/
                }
                //index 5 testen
                bool index5 = true;
                for(j=0; j<5; j++) {
                    if (bestHandPos[j] == 5 ) {
                        index5 = false;
                    }
                }
                if (index5) {
                    boardCardsArray[3]->startFadeOut(guiGameSpeed); /*cout << "Fade Out index5" << endl;*/
                }
                //index 6 testen
                bool index6 = true;
                for(j=0; j<5; j++) {
                    if (bestHandPos[j] == 6 ) {
                        index6 = false;
                    }
                }
                if (index6) {
                    boardCardsArray[4]->startFadeOut(guiGameSpeed); /*cout << "Fade Out index6" << endl;*/
                }
            }
            myGuiLog->logPlayerWinsMsg(QString::fromUtf8((*it_c)->getMyName().c_str()),(*it_c)->getLastMoneyWon(),true);
        } else {
            if( activePlayerList->size() != 1 && (*it_c)->getMyAction() != PLAYER_ACTION_FOLD) {
                //aufgedeckte Gegner auch ausblenden
                holeCardsArray[(*it_c)->getMyID()][0]->startFadeOut(guiGameSpeed);
                holeCardsArray[(*it_c)->getMyID()][1]->startFadeOut(guiGameSpeed);
            }
        }
    }

    // log side pot winners -> TODO
    list<unsigned>::iterator it_int;
    for(it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
        if((*it_c)->getMyAction() != PLAYER_ACTION_FOLD && (*it_c)->getMyCardsValueInt() != currentHand->getCurrentBeRo()->getHighestCardsValue() ) {

            for(it_int = winners.begin(); it_int != winners.end(); ++it_int) {
                if((*it_int) == (*it_c)->getMyUniqueID()) {
                    myGuiLog->logPlayerWinsMsg(QString::fromUtf8((*it_c)->getMyName().c_str()), (*it_c)->getLastMoneyWon(), false);
                }
            }

        }
    }

    for(it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
        if((*it_c)->getMyCash() == 0) {
            currentHand->getGuiInterface()->logPlayerSitsOut((*it_c)->getMyName());
        }
    }

//	textBrowser_Log->append("");

    postRiverRunAnimation3Timer->start(postRiverRunAnimationSpeed/2);
}

void GameTableImpl::postRiverRunAnimation4()
{
    distributePotAnimCounter=0;
    potDistributeTimer->start(winnerBlinkSpeed);
}

void GameTableImpl::postRiverRunAnimation5()
{
    /*
    std::shared_ptr<HandInterface> currentHand = myStartWindow->getSession()->getCurrentGame()->getCurrentHand();
    PlayerList activePlayerList = currentHand->getActivePlayerList();
    PlayerListConstIterator it_c;
    */
    if (distributePotAnimCounter<10) {
        /*
        if (distributePotAnimCounter==0 || distributePotAnimCounter==2 || distributePotAnimCounter==4 || distributePotAnimCounter==6 || distributePotAnimCounter==8) {


            for(it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {

                if((*it_c)->getMyAction() != PLAYER_ACTION_FOLD && (*it_c)->getMyCardsValueInt() == currentHand->getCurrentBeRo()->getHighestCardsValue() ) {

                    playerNameLabelArray[(*it_c)->getMyID()]->hide();
                }
            }
        } else {

            for(it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {

                if((*it_c)->getMyAction() != PLAYER_ACTION_FOLD && (*it_c)->getMyCardsValueInt() == currentHand->getCurrentBeRo()->getHighestCardsValue() ) {

                    playerNameLabelArray[(*it_c)->getMyID()]->show();
                }
            }
        }
        */
        distributePotAnimCounter++;
    } else {
        potDistributeTimer->stop();
        postRiverRunAnimation5Timer->start(gameSpeed);
    }
}

void GameTableImpl::postRiverRunAnimation6()
{
    //GUI HACK show every nick label
    int i;
    for (i=0; i<MAX_NUMBER_OF_PLAYERS; i++) {
        //playerNameLabelArray[i]->show();
    }

    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();

    refreshCash(true);
    refreshPot();


    // wenn nur noch ein Spieler aktive "neues Spiel"-Dialog anzeigen
    int playersPositiveCashCounter = 0;

    PlayerListConstIterator it_c;
    PlayerList activePlayerList = currentHand->getActivePlayerList();
    for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {

        if ((*it_c)->getMyCash() > 0) playersPositiveCashCounter++;
    }

    if (playersPositiveCashCounter==1) {

        for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {

            if ((*it_c)->getMyCash() > 0) {
                currentHand->getGuiInterface()->logPlayerWinGame((*it_c)->getMyName(),  currentGame->getMyGameID());
            }
        }

        currentGameOver = true;
        ui->pushButton_break->setDisabled(false);
        ui->pushButton_break->setProperty("attention", "start");
        ui->pushButton_break->setStyleSheet(ui->pushButton_break->styleSheet());
        ui->pushButton_break->setText("Start");
        ui->editSmallBlind->setEnabled(true);
        ui->riseBlinds->setEnabled(true);
        ui->horizontalSlider_bet->setEnabled(false);
        ui->spinBox_betValue->setEnabled(false);

        //qDebug() << "emit Game Over";

        std::shared_ptr<PlayerInterface> humanPlayer = currentHand->getSeatsList()->front();
        if (humanPlayer->getMyCash() > 0) {
            signalPlayerAction(0, JobActionIntro, -1, false);
        }
        signalPlayerAction(-1, JobActionGameOver, -1, false);

        return;
    }

    postRiverRunAnimation6Timer->start(newRoundSpeed);
}

void GameTableImpl::showHoleCards(unsigned playerId, bool allIn)
{
    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();
    //TempArrays
    QPixmap tempCardsPixmapArray[2];
    int tempCardsIntArray[2];
    int j;
    PlayerListConstIterator it_c;
    PlayerList activePlayerList = currentHand->getActivePlayerList();
    for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
        if((*it_c)->getMyUniqueID() == playerId) {
            (*it_c)->getMyCards(tempCardsIntArray);
            for(j=0; j<2; j++) {
                holeCardsArray[(*it_c)->getMyID()][j]->startFlipCards(guiGameSpeed, QPixmap::fromImage(QImage("://cards/" + QString::number(tempCardsIntArray[j], 10) + ".png")), flipside);
            }
            //set Player value (logging)
            if(currentHand->getCurrentRound() < GAME_STATE_RIVER || allIn) {
                (*it_c)->setMyCardsFlip(1,2); //for bero before postriver or allin just log the hole cards
            } else {
                (*it_c)->setMyCardsFlip(1,1); //for postriver log the value
            }
        }
    }
}

void GameTableImpl::flipHolecardsAllIn()
{
    if(!flipHolecardsAllInAlreadyDone && currentGame->getCurrentHand()->getCurrentRound() < GAME_STATE_RIVER) {
        //Aktive Spieler zählen --> wenn nur noch einer nicht-folded dann keine Karten umdrehen
        int nonfoldPlayersCounter = 0;
        PlayerListConstIterator it_c;
        PlayerList activePlayerList = currentGame->getActivePlayerList();
        for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
            if ((*it_c)->getMyAction() != PLAYER_ACTION_FOLD) nonfoldPlayersCounter++;
        }

        if(nonfoldPlayersCounter!=1) {
            for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
                if((*it_c)->getMyAction() != PLAYER_ACTION_FOLD) {

                    showHoleCards((*it_c)->getMyUniqueID());
                }
            }
        }

        //Wenn einmal umgedreht dann fertig!!
        flipHolecardsAllInAlreadyDone = true;
    }
}


void GameTableImpl::startNewHand()
{
    if (waitingForVideo) {
        startNewHandDefered = true;
        refreshButton();
        return;
    }
    startNewHandDefered = false;

    if( !breakAfterCurrentHand) {
        signalPlayerAction(-1, JobActionNewHand, -1, false);
        currentGame->initHand();
        currentGame->startHand();
    } else {
        ui->pushButton_break->setDisabled(false);
        ui->pushButton_break->setProperty("attention", "continue");
        ui->pushButton_break->setStyleSheet(ui->pushButton_break->styleSheet());
        ui->pushButton_break->setText("Start");
        breakAfterCurrentHand=false;
    }
}

void GameTableImpl::handSwitchRounds()
{
    currentGame->getCurrentHand()->switchRounds();
}
/*
static int startingHand[13][13] = {
    {5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5},
    {0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5},
    {0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 5, 5},
    {0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 5, 5, 5},
    {0, 0, 0, 0, 6, 0, 0, 0, 0, 5, 5, 5, 5},
    {0, 0, 0, 0, 0, 6, 0, 0, 5, 5, 5, 5, 5},
    {0, 0, 0, 0, 0, 5, 7, 5, 5, 5, 5, 5, 6},
    {0, 0, 0, 0, 0, 5, 5, 9, 5, 5, 5, 6, 6},
    {0, 0, 0, 0, 5, 5, 5, 5, 9, 5, 5, 6, 6},
    {0, 5, 5, 5, 5, 5, 5, 5, 5, 9, 6, 6, 6},
    {5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 9, 6, 6},
    {5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 9, 6},
    {5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 9}
};
*/
void GameTableImpl::nextRoundCleanGui()
{
    int i,j;

    // GUI bereinigen - Bilder löschen, Animationen unterbrechen
    QPixmap onePix = QPixmap::fromImage(QImage(actionPixName(0)));
    for (i=0; i<5; i++ ) {
        boardCardsArray[i]->setPixmap(onePix, false);
        boardCardsArray[i]->setFadeOutAction(false);
        boardCardsArray[i]->stopFlipCardsAnimation();

    }
    for (i=0; i<MAX_NUMBER_OF_PLAYERS; i++ ) {
        //timeoutLabelArray[i]->stopTimeOutAnimation();
        for ( j=0; j<=1; j++ ) {
            holeCardsArray[i][j]->setFadeOutAction(false);
            holeCardsArray[i][j]->stopFlipCardsAnimation();
        }
    }

    // for startNewGame during human player is active
    if(currentGame->getCurrentHand()->getSeatsList()->front()->getMyActiveStatus() == 1) {
        disableMyButtons();
    }
    /*
    textLabel_handLabel->setText("");
    */
    refreshAll();
    flipHolecardsAllInAlreadyDone = false;

    /*
    //Wenn Pause zwischen den Hands in der Konfiguration steht den Stop Button drücken!
    if (myConfig->readConfigInt("PauseBetweenHands") /X && blinkingStartButtonAnimationTimer->isActive() == false X/ && myStartWindow->getSession()->getGameType() == Session::GAME_TYPE_LOCAL) {
        pushButton_break->click();
    } else {
        //FIX STRG+N Bug
        pushButton_break->setEnabled(true);
        breakAfterCurrentHand=false;
    }
    */
    //Clean breakbutton
    ui->pushButton_break->setProperty("attention", "pause");
    ui->pushButton_break->setStyleSheet(ui->pushButton_break->styleSheet());
    ui->pushButton_break->setText("Pause");
    /*
    //Clear Statusbarmessage
    // 	statusBar()->clearMessage();

    //fix press mouse button during bankrupt with anti-peek-mode
    this->mouseOverFlipCards(false);
    */
    ui->horizontalSlider_bet->setDisabled(true);
    ui->spinBox_betValue->setDisabled(true);

    uncheckMyButtons();
    myButtonsCheckable(false);
    resetMyButtonsCheckStateMemory();
    clearMyButtons();
    //pushButton_showMyCards->hide();
}

void GameTableImpl::stopTimer()
{

    dealFlopCards0Timer->stop();
    dealFlopCards1Timer->stop();
    dealFlopCards2Timer->stop();
    dealFlopCards3Timer->stop();
    dealFlopCards4Timer->stop();
    dealFlopCards5Timer->stop();
    dealFlopCards6Timer->stop();
    dealTurnCards0Timer->stop();
    dealTurnCards1Timer->stop();
    dealTurnCards2Timer->stop();
    dealRiverCards0Timer->stop();
    dealRiverCards1Timer->stop();
    dealRiverCards2Timer->stop();

    nextPlayerAnimationTimer->stop();
    preflopAnimation1Timer->stop();
    preflopAnimation2Timer->stop();
    flopAnimation1Timer->stop();
    flopAnimation2Timer->stop();
    turnAnimation1Timer->stop();
    turnAnimation2Timer->stop();
    riverAnimation1Timer->stop();
    riverAnimation2Timer->stop();

    postRiverAnimation1Timer->stop();
    postRiverRunAnimation1Timer->stop();
    postRiverRunAnimation2Timer->stop();
    postRiverRunAnimation3Timer->stop();
    postRiverRunAnimation5Timer->stop();
    postRiverRunAnimation6Timer->stop();
    potDistributeTimer->stop();
}

void GameTableImpl::setSpeeds()
{

    gameSpeed = (11-guiGameSpeed)*10;
    dealCardsSpeed = (gameSpeed/2)*10; //milliseconds
    preDealCardsSpeed = dealCardsSpeed*2; //Zeit for Karten aufdecken auf dem Board (Flop, Turn, River)
    postDealCardsSpeed = dealCardsSpeed*3; //Zeit nach Karten aufdecken auf dem Board (Flop, Turn, River)
    AllInDealCardsSpeed = dealCardsSpeed*4; //Zeit nach Karten aufdecken auf dem Board (Flop, Turn, River) bei AllIn
    postRiverRunAnimationSpeed = gameSpeed*18;
    winnerBlinkSpeed = gameSpeed*3; //milliseconds
    newRoundSpeed = gameSpeed*35;
    nextPlayerSpeed1 = gameSpeed*10; // Zeit zwischen dem Setzen des Spielers und dem Verdunkeln
    nextPlayerSpeed2 = gameSpeed*4; // Zeit zwischen Verdunkeln des einen und aufhellen des anderen Spielers
    nextPlayerSpeed3 = gameSpeed*7; // Zeit bis zwischen Aufhellen und Aktion
    preflopNextPlayerSpeed = gameSpeed*10; // Zeit bis zwischen Aufhellen und Aktion im Preflop (etwas langsamer da nicht gerechnet wird. )
}

void GameTableImpl::breakButtonClicked()
{
    if (ui->pushButton_break->text() == tr("Pause")) {
        ui->pushButton_break->setDisabled(true);
        ui->pushButton_break->setProperty("attention", "checked");
        ui->pushButton_break->setStyleSheet(ui->pushButton_break->styleSheet());
        breakAfterCurrentHand=true;
    } else if (ui->pushButton_break->text() == tr("Start")) {
        ui->pushButton_break->setProperty("attention", "pause");
        ui->pushButton_break->setStyleSheet(ui->pushButton_break->styleSheet());
        ui->pushButton_break->setText("Pause");

        if(currentGameOver) {
            ui->editSmallBlind->setEnabled(false);
            ui->riseBlinds->setEnabled(false);
            ui->pushButton_Wine->setEnabled(true);
            ui->horizontalSlider_bet->setEnabled(true);
            ui->spinBox_betValue->setEnabled(true);

            //let the SoundEventHandler know that there is a new game
            //mySoundEventHandler->newGameStarts();

            currentGameOver = false;
            //myStartWindow->callNewGameDialog();
            slotButtonTabPlayClicked();
            startNewGame();
            //Bei Cancel nichts machen!!!
        } else {
            startNewHand();
        }
    }
}

void GameTableImpl::keyPressEvent ( QKeyEvent * event )
{
    // 	cout << event->key() << endl;

    //bool ctrlPressed = false;

    if ((event->modifiers() && ~Qt::KeypadModifier) == Qt::NoModifier) {
        if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return ) {
            if(ui->spinBox_betValue->hasFocus()) {
                ui->pushButton_BetRaise->click();
            }
        }
        if (event->key() == Qt::Key_F1) {
            ui->pushButton_Fold->click();
        }
        if (event->key() == Qt::Key_F2) {
            ui->pushButton_CallCheck->click();
        }
        if (event->key() == Qt::Key_F3 ) {
            ui->pushButton_BetRaise->click();
        }
        if (event->key() == Qt::Key_F4) {
            ui->pushButton_AllIn->click();
        }
        if (event->key() == Qt::Key_F5) {
            ui->buttonTabConfig->click();
        }
        if (event->key() == Qt::Key_F6) {
            ui->buttonTabPlay->click();
        }
        if (event->key() == Qt::Key_F7) {
            ui->buttonTabChances->click();
        }
        if (event->key() == Qt::Key_F8) {
            ui->buttonTabLog->click();
        }
        if (event->key() == Qt::Key_F9) {
            ui->buttonTabError->click();
        }
        if (event->key() == Qt::Key_F10) {
            ui->pushButton_break->click();
        }
    } else if (event->modifiers() == Qt::ControlModifier) {
        if (event->key() == Qt::Key_X) {
            ui->pushButton_Exit->click();
        }
        if (event->key() == Qt::Key_S) {
            ui->buttonSaveCompilation->click();
        }
        if (event->key() == Qt::Key_G) {
            ui->buttonGeneratePreviews->click();
        }
        if (event->key() == Qt::Key_E) {
            ui->buttonEdit->click();
        }
        if (event->key() == Qt::Key_O) {
            ui->selectPlayer1->click();
        }
        if (event->key() == Qt::Key_D) {
            ui->pushButton_Wine->click();
        }
    }

    /*
    if (event->key() == Qt::Key_F5) {
        pushButton_showMyCards->click();
    }

    if (event->key() == Qt::Key_F6) {
        radioButton_manualAction->click();
    }
    if (event->key() == Qt::Key_F7) {
        radioButton_autoCheckFold->click();
    }
    if (event->key() == Qt::Key_F8) {
        radioButton_autoCheckCallAny->click();
    }
    if (event->key() == Qt::Key_Shift) {
        if(myStartWindow->getSession()->getGameType() == Session::GAME_TYPE_LOCAL) {
            pushButton_break->click();
            //ctrlPressed = true;
        }
    }
    //    if (event->key() == Qt::Key_Escape && (myActionIsBet || myActionIsRaise)) {
    //            meInAction();
    //    }
    if (event->key() == Qt::Key_Up && lineEdit_ChatInput->hasFocus()) {
        if((keyUpDownChatCounter + 1) <= myChat->getChatLinesHistorySize()) {
            keyUpDownChatCounter++;
        }
        // 		std::cout << "Up keyUpDownChatCounter: " << keyUpDownChatCounter << "\n";
        myChat->showChatHistoryIndex(keyUpDownChatCounter);
    } else if(event->key() == Qt::Key_Down && lineEdit_ChatInput->hasFocus()) {
        if((keyUpDownChatCounter - 1) >= 0) {
            keyUpDownChatCounter--;
        }
        // 		std::cout << "Down keyUpDownChatCounter: " << keyUpDownChatCounter << "\n";
        myChat->showChatHistoryIndex(keyUpDownChatCounter);
    } else {
        keyUpDownChatCounter = 0;
    }
    */
}

void GameTableImpl::changePlayingMode()
{
    /*
    int mode = -1;

    if(radioButton_manualAction->isChecked()) {
        mode=0;
    }
    if(radioButton_autoCheckFold->isChecked()) {
        mode=2;
    }
    if(radioButton_autoCheckCallAny->isChecked()) {
        mode=1;
    }
    */

    /*	switch (mode) {

            // 		case 0: { statusBar()->showMessage(tr("Manual mode set. You've got to choose yourself now."), 5000); }
            break;
            // 		case 1: { statusBar()->showMessage(tr("Auto mode set: Check or call any."), 5000); }
            break;
            // 		case 2: { statusBar()->showMessage(tr("Auto mode set: Check or fold."), 5000); }
            break;
        default: {
            //cout << "changePlayingMode ERROR!!!!" << endl;
        }

        }*/

    //playingMode = mode;
}

bool GameTableImpl::eventFilter(QObject *obj, QEvent *event)
{
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

    if (/*obj == lineEdit_ChatInput && lineEdit_ChatInput->text() != "" && */event->type() == QEvent::KeyPress && keyEvent->key() == Qt::Key_Tab) {
        //myChat->nickAutoCompletition();
        return true;
    } else if (event->type() == QEvent::KeyPress && keyEvent->key() == Qt::Key_Back) {
        event->ignore();
        //closeGameTable();
        return true;
    } else if (event->type() == QEvent::Close) {
        event->ignore();
        //closeGameTable();
        return true;
    } else if (event->type() == QEvent::Resize) {
        //refreshSpectatorsDisplay();
        return true;
    } else {
        // pass the event on to the parent class
        return QWidget::eventFilter(obj, event);
    }
}

void GameTableImpl::switchHelpWindow()
{
    /*
    int tab = 0;
    if (groupBox_LeftToolBox->isHidden()) {
        tabWidget_Left->setCurrentIndex(tab);
        groupBox_LeftToolBox->show();
    }	else {
        if (tabWidget_Left->currentIndex() == tab) {
            groupBox_LeftToolBox->hide();
        } else {
            tabWidget_Left->setCurrentIndex(tab);
        }
    }
    */
}

void GameTableImpl::switchLogWindow()
{
    /*
    int tab = 0;
    if (groupBox_RightToolBox->isHidden()) {
        tabWidget_Right->setCurrentIndex(tab);
        groupBox_RightToolBox->show();
    }	else {
        if (tabWidget_Right->currentIndex() == tab) {
            groupBox_RightToolBox->hide();
        } else {
            tabWidget_Right->setCurrentIndex(tab);
        }
    }
    */
}

void GameTableImpl::switchChanceWindow()
{
    /*
    int tab = 2;
    if (groupBox_RightToolBox->isHidden()) {
        tabWidget_Right->setCurrentIndex(tab);
        groupBox_RightToolBox->show();
    }	else {
        if (tabWidget_Right->currentIndex() == tab) {
            groupBox_RightToolBox->hide();
        } else {
            tabWidget_Right->setCurrentIndex(tab);
        }
    }
    */
}

void GameTableImpl::switchFullscreen()
{
    /*
    if (this->isFullScreen()) {
        this->showNormal();
    } else {
        this->showFullScreen();
    }
    */
}

void GameTableImpl::blinkingStartButtonAnimationAction()
{
    /*
    QString style = pushButton_break->styleSheet();

    if(style.contains("QPushButton:enabled { background-color: #"+myGameTableStyle->getBreakLobbyButtonBgColor())) {
        myGameTableStyle->setBreakButtonStyle(pushButton_break,1);
    } else {
        myGameTableStyle->setBreakButtonStyle(pushButton_break,0);
    }
    */
}

void GameTableImpl::tabSwitchAction()
{
    /*
    switch(tabWidget_Left->currentIndex()) {

    case 1: {
        lineEdit_ChatInput->setFocus();
    }
    break;
    default: {
        lineEdit_ChatInput->clearFocus();
    }

    }
    */
}

void GameTableImpl::updateMyButtonsState(int mode)
{
    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();

    if(currentHand->getPreviousPlayerID() == 0) {
        myButtonsCheckable(false);
        clearMyButtons();
    } else {
        if(currentHand->getSeatsList()->front()->getMyAction() != PLAYER_ACTION_ALLIN) { // dont show pre-actions after flip cards when allin
            myButtonsCheckable(true);
            provideMyActions(mode);
        }
    }
}

void GameTableImpl::uncheckMyButtons()
{
    ui->pushButton_BetRaise->setChecked(false);
    ui->pushButton_CallCheck->setChecked(false);
    ui->pushButton_Fold->setChecked(false);
    ui->pushButton_AllIn->setChecked(false);
}

void GameTableImpl::resetMyButtonsCheckStateMemory()
{
    pushButtonCallCheckIsChecked = false;
    pushButtonFoldIsChecked = false;
    pushButtonAllInIsChecked = false;
    pushButtonBetRaiseIsChecked = false;
}

void GameTableImpl::clearMyButtons()
{
    refreshActionButtonFKeyIndicator(1);

    ui->pushButton_BetRaise->setText("");
    ui->pushButton_CallCheck->setText("");
    ui->pushButton_Fold->setText("");
    ui->pushButton_AllIn->setText("");
}

void GameTableImpl::myButtonsCheckable(bool state)
{
    std::shared_ptr<HandInterface> currentHand = currentGame->getCurrentHand();

    if(state) {
        //checkable

        // exception: full bet rule
        if(!currentHand->getCurrentBeRo()->getFullBetRule()) {
            ui->pushButton_BetRaise->setCheckable(true);
        }
        ui->pushButton_CallCheck->setCheckable(true);
        ui->pushButton_Fold->setCheckable(true);
        ui->pushButton_AllIn->setCheckable(true);

        //design
        //myGameTableStyle->setButtonsStyle(pushButton_BetRaise, pushButton_CallCheck, pushButton_Fold, pushButton_AllIn, 2);

        myButtonsAreCheckable = true;
    } else {
        //not checkable

        ui->pushButton_BetRaise->setCheckable(false);
        ui->pushButton_CallCheck->setCheckable(false);
        ui->pushButton_Fold->setCheckable(false);
        ui->pushButton_AllIn->setCheckable(false);

        QString hover;
        if(ui->pushButton_AllIn->text()==AllInString) {
            //myGameTableStyle->setButtonsStyle(pushButton_BetRaise, pushButton_CallCheck, pushButton_Fold, pushButton_AllIn, 0);
        } else {
            //myGameTableStyle->setButtonsStyle(pushButton_BetRaise, pushButton_CallCheck, pushButton_Fold, pushButton_AllIn, 1);
        }

        myButtonsAreCheckable = false;
    }
}

void GameTableImpl::showMaximized ()
{
    this->showFullScreen ();
}

void GameTableImpl::closeGameTable()
{
    /*
    if (myStartWindow->getMyServerGuiInterface() && myStartWindow->getMyServerGuiInterface()->getSession()->isNetworkServerRunning()) {

        MyMessageBox msgBox(QMessageBox::Warning, tr("Closing PokerTH during network game"),
                            tr("You are the hosting server. Do you want to close PokerTH anyway?"), QMessageBox::Yes | QMessageBox::No, this);

        if (msgBox.exec() == QMessageBox::Yes ) {
            myStartWindow->getSession()->terminateNetworkClient();
            stopTimer();
            if (myStartWindow->getMyServerGuiInterface()) myStartWindow->getMyServerGuiInterface()->getSession()->terminateNetworkServer();
            saveGameTableGeometry();
            myStartWindow->show();
            this->hide();
        }
    } else {

        bool close = true;

        if(myUniversalMessageDialog->checkIfMesssageWillBeDisplayed(CLOSE_GAMETABLE_QUESTION ) && (myStartWindow->getSession()->getGameType() == Session::GAME_TYPE_INTERNET || myStartWindow->getSession()->getGameType() == Session::GAME_TYPE_NETWORK ) && this->isVisible()) {
            if (myUniversalMessageDialog->exec(CLOSE_GAMETABLE_QUESTION , tr("Really want to exit?"), tr("PokerTH - Close Table?"), QPixmap(":/gfx/logoChip3D.png"), QDialogButtonBox::Yes|QDialogButtonBox::No, true) == QDialog::Rejected) {
                close = false;
            }
        }

        if(close) {
            //now really close the table
            myStartWindow->getSession()->terminateNetworkClient();
            stopTimer();
            saveGameTableGeometry();
            myStartWindow->show();
            this->hide();
        }
    }
    */
}

void GameTableImpl::changeSpinBoxBetValue(int value)
{
    if(betSliderChangedByInput) {
        //prevent interval cutting of spinBox_betValue input from code below
        betSliderChangedByInput = false;
    } else {

        if(ui->horizontalSlider_bet->value() == ui->horizontalSlider_bet->maximum()) {
            ui->spinBox_betValue->setValue(ui->horizontalSlider_bet->value());
        } else {

            int temp;
            if(ui->horizontalSlider_bet->maximum() <= 1000 ) {
                temp = (int)((value/10)*10);
            } else if(ui->horizontalSlider_bet->maximum() > 1000 && ui->horizontalSlider_bet->maximum() <= 10000) {
                temp = (int)((value/50)*50);
            } else if(ui->horizontalSlider_bet->maximum() > 10000 && ui->horizontalSlider_bet->maximum() <= 100000) {
                temp = (int)((value/500)*500);
            } else {
                temp = (int)((value/5000)*5000);
            }

            if(temp < ui->horizontalSlider_bet->minimum())
                ui->spinBox_betValue->setValue(ui->horizontalSlider_bet->minimum());
            else
                ui->spinBox_betValue->setValue(temp);
        }
    }
}

void GameTableImpl::spinBoxBetValueChanged(int value)
{
    if(ui->horizontalSlider_bet->isEnabled()) {

        QString betRaise = ui->pushButton_BetRaise->text().section(" ",0 ,0);

        if(value >= ui->horizontalSlider_bet->minimum()) {

            if(value > ui->horizontalSlider_bet->maximum()) { // print the maximum
                ui->pushButton_BetRaise->setText(betRaise + " $" + QString("%L1").arg(ui->horizontalSlider_bet->maximum()));
                betSliderChangedByInput = true;
                ui->horizontalSlider_bet->setValue(ui->horizontalSlider_bet->maximum());
            } else { // really print the value
                ui->pushButton_BetRaise->setText(betRaise + " $" + QString("%L1").arg(value));
                betSliderChangedByInput = true;
                ui->horizontalSlider_bet->setValue(value);
            }
        } else { // print the minimum
            ui->pushButton_BetRaise->setText(betRaise + " $" + QString("%L1").arg(ui->horizontalSlider_bet->minimum()));
            betSliderChangedByInput = true;
            ui->horizontalSlider_bet->setValue(ui->horizontalSlider_bet->minimum());
        }
    }
}

void GameTableImpl::refreshCardsChance(GameState bero, bool emitOdds)
{
    if (emitOdds) {
        PlayerListConstIterator it_c;
        PlayerList seatsList = currentGame->getSeatsList();
        for (it_c=seatsList->begin(); it_c!=seatsList->end(); ++it_c) {
            if((*it_c)->getMyActiveStatus()) {
                int odds, call, raise;
                (*it_c)->getMyOdds(&odds, &call, &raise);
                JobAction r = JobActionCardsBad;
                if (odds >= call / 2) { // FIXME
                    r = JobActionCards;
                }
                if (odds >= call) {
                    r = JobActionCardsGood;
                }
                //qDebug() << QString("emit Cards%1").arg(r == JobActionCardsBad ? "Bad" : (r == JobActionCardsGood ? "Good" : "")) << (*it_c)->getMyID() << odds << call << raise;
                signalPlayerAction((*it_c)->getMyID(), r, -1, false);
            }
        }
    }

    if (ui->toolTab->currentIndex() == 3) {
        int cards[5];
        QVector<int> hand(CardVectorSize, -1);

        if (bero == GAME_STATE_FLOP) {
            currentGame->getCurrentHand()->getBoard()->getMyCards(cards);
            hand[CardIndexFlop] = cards[0];
            hand[CardIndexFlop+1] = cards[1];
            hand[CardIndexFlop+2] = cards[2];
        } else if (bero == GAME_STATE_TURN) {
            currentGame->getCurrentHand()->getBoard()->getMyCards(cards);
            hand[CardIndexFlop] = cards[0];
            hand[CardIndexFlop+1] = cards[1];
            hand[CardIndexFlop+2] = cards[2];
            hand[CardIndexTurn] = cards[3];
        } else if (bero == GAME_STATE_RIVER) {
            currentGame->getCurrentHand()->getBoard()->getMyCards(cards);
            hand[CardIndexFlop] = cards[0];
            hand[CardIndexFlop+1] = cards[1];
            hand[CardIndexFlop+2] = cards[2];
            hand[CardIndexTurn] = cards[3];
            hand[CardIndexRiver] = cards[4];
        }

        PlayerListConstIterator it_c;
        PlayerList activePlayerList = currentGame->getActivePlayerList();
        for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
            if ((*it_c)->getMyAction() != PLAYER_ACTION_FOLD) {
                if (ui->checkBox_Cheat->isChecked() || (*it_c)->getMyID() == 0) {
                    (*it_c)->getMyCards(cards);
                    hand[2*(*it_c)->getMyID()] = cards[0];
                    hand[2*(*it_c)->getMyID()+1] = cards[1];
                } else  {
                    hand[2*(*it_c)->getMyID()] = -2;
                    hand[2*(*it_c)->getMyID()] = -2;
             }
            }
        }
        signalHandChanged(hand);
    }

    /*
    if(myConfig->readConfigInt("ShowCardsChanceMonitor")) {

        std::shared_ptr<PlayerInterface> humanPlayer = myStartWindow->getSession()->getCurrentGame()->getSeatsList()->front();
        if(humanPlayer->getMyActiveStatus()) {
            int boardCards[5];
            int holeCards[2];

            humanPlayer->getMyCards(holeCards);
            myStartWindow->getSession()->getCurrentGame()->getCurrentHand()->getBoard()->getMyCards(boardCards);

            if(humanPlayer->getMyAction() == PLAYER_ACTION_FOLD) {
#ifdef GUI_800x480
                tabs.label_chance->refreshChance(CardsValue::calcCardsChance(bero, holeCards, boardCards), true);
#else
                label_chance->refreshChance(CardsValue::calcCardsChance(bero, holeCards, boardCards), true);
#endif
            } else {
#ifdef GUI_800x480
                tabs.label_chance->refreshChance(CardsValue::calcCardsChance(bero, holeCards, boardCards), false);
#else
                label_chance->refreshChance(CardsValue::calcCardsChance(bero, holeCards, boardCards), false);
#endif
            }
        } else {
#ifdef GUI_800x480
            tabs.label_chance->resetChance();
#else
            label_chance->resetChance();
#endif
        }
    }
    */
}

void GameTableImpl::refreshActionButtonFKeyIndicator(bool clear)
{
    /*
    if(clear) {
        pushButton_AllIn->setFKeyText("");
        pushButton_BetRaise->setFKeyText("");
        pushButton_CallCheck->setFKeyText("");
        pushButton_Fold->setFKeyText("");
    } else {
#ifndef GUI_800x480
        if(myConfig->readConfigInt("AlternateFKeysUserActionMode") == 0 ) {
            if(!pushButton_AllIn->text().isEmpty()) pushButton_AllIn->setFKeyText("F4");
            if(!pushButton_BetRaise->text().isEmpty()) pushButton_BetRaise->setFKeyText("F3");
            if(!pushButton_CallCheck->text().isEmpty()) pushButton_CallCheck->setFKeyText("F2");
            if(!pushButton_Fold->text().isEmpty()) pushButton_Fold->setFKeyText("F1");
        } else {
            if(!pushButton_AllIn->text().isEmpty()) pushButton_AllIn->setFKeyText("F1");
            if(!pushButton_BetRaise->text().isEmpty()) pushButton_BetRaise->setFKeyText("F2");
            if(!pushButton_CallCheck->text().isEmpty()) pushButton_CallCheck->setFKeyText("F3");
            if(!pushButton_Fold->text().isEmpty()) pushButton_Fold->setFKeyText("F4");
        }
#endif
    }
    */
}

void GameTableImpl::refreshGameTableStyle()
{
    ui->label_CardHolder0->setPixmap(QPixmap::fromImage(QImage("://danuxi1/cardholder_flop.png")));
    ui->label_CardHolder1->setPixmap(QPixmap::fromImage(QImage("://danuxi1/cardholder_flop.png")));
    ui->label_CardHolder2->setPixmap(QPixmap::fromImage(QImage("://danuxi1/cardholder_flop.png")));
    ui->label_CardHolder3->setPixmap(QPixmap::fromImage(QImage("://danuxi1/cardholder_turn.png")));
    ui->label_CardHolder4->setPixmap(QPixmap::fromImage(QImage("://danuxi1/cardholder_river.png")));
        CallString = "Call";
        CheckString = "Check";
        BetString = "Bet";
        RaiseString = "Raise";
        FoldString = "Fold";
        AllInString = "All-In";
        PotString = "Pot";
        TotalString = "Total";
        BetsString = "Bets";
        GameString = "Game";
        HandString = "Hand";
        PreflopString = "Preflop";
        FlopString = "Flop";
        TurnString = "Turn";
        RiverString = "River";

        ui->pushButton_break->setProperty("attention", "start");

}

SeatState GameTableImpl::getCurrentSeatState(std::shared_ptr<PlayerInterface> player)
{
    if(player->getMyActiveStatus()) {
        if(player->isSessionActive()) {
            return SEAT_ACTIVE;
        } else {
            return SEAT_AUTOFOLD;
        }
    } else {
        return SEAT_CLEAR;
    }
    return SEAT_UNDEFINED;
}

void GameTableImpl::soundEvent_blindsWereSet(int sbSet)
{
    /*
    mySoundEventHandler->blindsWereSet(sbSet);
    */
}

void GameTableImpl::enableCallCheckPushButton()
{
    /*
    pushButton_CallCheck->setEatMyEvents(false);
    */
}

void GameTableImpl::slotSelectPlayer1Clicked() {
    selectPlayerClicked(1);
}
void GameTableImpl::slotSelectPlayer2Clicked() {
    selectPlayerClicked(2);
}
void GameTableImpl::slotSelectPlayer3Clicked() {
    selectPlayerClicked(3);
}
void GameTableImpl::slotSelectPlayer4Clicked() {
    selectPlayerClicked(4);
}
void GameTableImpl::slotSelectPlayer5Clicked() {
    selectPlayerClicked(5);
}

void GameTableImpl::selectPlayerClicked(int seat) {
    QSettings settings;
    QString dir;
    if (settings.contains("directory/video")) {
        dir = settings.value("directory/video").toString();
    } else {
        dir = QDir::homePath();
    }

    PreviewFileDialog dialog(this, "Select a player timestamp file", dir, "Timestamps, Compilations (*.ts *.tsc)");
    dialog.setFileMode(QFileDialog::ExistingFiles);

    if (dialog.exec()) {
        QStringList fileNamesRaw;
        fileNamesRaw = dialog.selectedFiles();
        if (fileNamesRaw.isEmpty()) { //FIXME
            tsFileName[seat] = "";
            groupBoxArray[seat]->setTsFileName("");
            selectPlayerButtonArray[seat]->setText("Select...");
            signalPlayerSelected(seat, "");
            return;
        }
        if (fileNamesRaw[0].endsWith(".tsc")) {
            seat = 1;
        }
        int startSeat = seat;
        QStringList fileNames = Util::expandCompilations(fileNamesRaw, &settings);
        for (int i = 0; i < fileNames.length() && i < 5; i++) {
            if (fileNames[i].endsWith(".ts")) {
                tsFileName[seat] = fileNames[i];
                QString name(tsFileName[seat]);
                name.remove(QRegExp("(^.*/)|([.]ts$)"));
                name.remove(QRegExp("^[-_0-9\\s]*"));
                name.remove(QRegExp("-.*$"));
                name = name.left(1).toUpper() + name.mid(1);
                selectPlayerButtonArray[seat]->setText(name);

                groupBoxArray[seat]->setTsFileName(tsFileName[seat]);
                //videoWorker[2]->readTimestampFile(playerFileName[2]);
                signalPlayerSelected(seat, tsFileName[seat]);
            } else {
                tsFileName[seat] = "";
                groupBoxArray[seat]->setTsFileName("");
                selectPlayerButtonArray[seat]->setText("Select...");
                signalPlayerSelected(seat, "");
            }
            seat++;
            if (seat > 5) {
                seat = 1;
            }
            if (seat == startSeat) {
                i = 99;
            }
        }
    }

    if (tsFileName[1].isEmpty()) {
        ui->pushButton_break->setDisabled(true);
        selectPlayerButtonArray[1]->setProperty("attention", currentGameOver ? "start" : "normal");
        selectPlayerButtonArray[1]->setStyleSheet(selectPlayerButtonArray[1]->styleSheet());
        selectPlayerButtonArray[2]->setDisabled(true);
        selectPlayerButtonArray[3]->setDisabled(true);
        selectPlayerButtonArray[4]->setDisabled(true);
        selectPlayerButtonArray[5]->setDisabled(true);
    } else if (tsFileName[2].isEmpty()) {
        ui->pushButton_break->setDisabled(false);
        selectPlayerButtonArray[1]->setProperty("attention", "normal");
        selectPlayerButtonArray[1]->setStyleSheet(selectPlayerButtonArray[1]->styleSheet());
        selectPlayerButtonArray[2]->setProperty("attention", currentGameOver ? "start" : "normal");
        selectPlayerButtonArray[2]->setStyleSheet(selectPlayerButtonArray[2]->styleSheet());
        selectPlayerButtonArray[2]->setDisabled(false);
        selectPlayerButtonArray[3]->setDisabled(true);
        selectPlayerButtonArray[4]->setDisabled(true);
        selectPlayerButtonArray[5]->setDisabled(true);
    } else if (tsFileName[3].isEmpty()) {
        ui->pushButton_break->setDisabled(false);
        selectPlayerButtonArray[1]->setProperty("attention", "normal");
        selectPlayerButtonArray[1]->setStyleSheet(selectPlayerButtonArray[1]->styleSheet());
        selectPlayerButtonArray[2]->setProperty("attention", "normal");
        selectPlayerButtonArray[2]->setStyleSheet(selectPlayerButtonArray[2]->styleSheet());
        selectPlayerButtonArray[2]->setDisabled(false);
        selectPlayerButtonArray[3]->setProperty("attention", currentGameOver ? "start" : "normal");
        selectPlayerButtonArray[3]->setStyleSheet(selectPlayerButtonArray[3]->styleSheet());
        selectPlayerButtonArray[3]->setDisabled(false);
        selectPlayerButtonArray[4]->setDisabled(true);
        selectPlayerButtonArray[5]->setDisabled(true);
    } else if (tsFileName[4].isEmpty()) {
        ui->pushButton_break->setDisabled(false);
        selectPlayerButtonArray[1]->setProperty("attention", "normal");
        selectPlayerButtonArray[1]->setStyleSheet(selectPlayerButtonArray[1]->styleSheet());
        selectPlayerButtonArray[2]->setProperty("attention", "normal");
        selectPlayerButtonArray[2]->setStyleSheet(selectPlayerButtonArray[2]->styleSheet());
        selectPlayerButtonArray[2]->setDisabled(false);
        selectPlayerButtonArray[3]->setProperty("attention", "normal");
        selectPlayerButtonArray[3]->setStyleSheet(selectPlayerButtonArray[3]->styleSheet());
        selectPlayerButtonArray[3]->setDisabled(false);
        selectPlayerButtonArray[4]->setProperty("attention", currentGameOver ? "start" : "normal");
        selectPlayerButtonArray[4]->setStyleSheet(selectPlayerButtonArray[4]->styleSheet());
        selectPlayerButtonArray[4]->setDisabled(false);
        selectPlayerButtonArray[5]->setDisabled(true);
    } else if (tsFileName[5].isEmpty()) {
        ui->pushButton_break->setDisabled(false);
        selectPlayerButtonArray[1]->setProperty("attention", "normal");
        selectPlayerButtonArray[1]->setStyleSheet(selectPlayerButtonArray[1]->styleSheet());
        selectPlayerButtonArray[2]->setProperty("attention", "normal");
        selectPlayerButtonArray[2]->setStyleSheet(selectPlayerButtonArray[2]->styleSheet());
        selectPlayerButtonArray[2]->setDisabled(false);
        selectPlayerButtonArray[3]->setProperty("attention", "normal");
        selectPlayerButtonArray[3]->setStyleSheet(selectPlayerButtonArray[3]->styleSheet());
        selectPlayerButtonArray[3]->setDisabled(false);
        selectPlayerButtonArray[4]->setProperty("attention", "normal");
        selectPlayerButtonArray[4]->setStyleSheet(selectPlayerButtonArray[4]->styleSheet());
        selectPlayerButtonArray[4]->setDisabled(false);
        selectPlayerButtonArray[4]->setProperty("attention", currentGameOver ? "start" : "normal");
        selectPlayerButtonArray[4]->setStyleSheet(selectPlayerButtonArray[5]->styleSheet());
        selectPlayerButtonArray[5]->setDisabled(false);
    } else {
        ui->pushButton_break->setDisabled(false);
        selectPlayerButtonArray[1]->setProperty("attention", "normal");
        selectPlayerButtonArray[1]->setStyleSheet(selectPlayerButtonArray[1]->styleSheet());
        selectPlayerButtonArray[2]->setProperty("attention", "normal");
        selectPlayerButtonArray[2]->setStyleSheet(selectPlayerButtonArray[2]->styleSheet());
        selectPlayerButtonArray[2]->setDisabled(false);
        selectPlayerButtonArray[3]->setProperty("attention", "normal");
        selectPlayerButtonArray[3]->setStyleSheet(selectPlayerButtonArray[3]->styleSheet());
        selectPlayerButtonArray[3]->setDisabled(false);
        selectPlayerButtonArray[4]->setProperty("attention", "normal");
        selectPlayerButtonArray[4]->setStyleSheet(selectPlayerButtonArray[4]->styleSheet());
        selectPlayerButtonArray[4]->setDisabled(false);
        selectPlayerButtonArray[4]->setProperty("attention", "normal");
        selectPlayerButtonArray[4]->setStyleSheet(selectPlayerButtonArray[5]->styleSheet());
        selectPlayerButtonArray[5]->setDisabled(false);
    }
}

void GameTableImpl::slotSaveCompilation() {
    QSettings settings;
    QString dir;
    if (settings.contains("directory/video")) {
        dir = settings.value("directory/video").toString();
    } else {
        dir = QDir::homePath();
    }
    QFileDialog dialog(this, "Select a file to save the compilation", dir, "Compilations (*.tsc)");
    dialog.setFileMode(QFileDialog::AnyFile);

    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if (!fileNames.isEmpty()) {
            QString name(fileNames[0]);
            if (!name.endsWith(".tsc")) {
                name.append(".tsc");
            }
            QFile outFile(name);
            outFile.open(QIODevice::WriteOnly | QFile::Truncate);
            QTextStream out(&outFile);
            QFileInfo fi(name);
            QDir d = fi.absoluteDir();
            for (int i = 1; i <= 5; i++) {
                if (!tsFileName[i].isEmpty()) {
                    out << d.relativeFilePath(tsFileName[i]);
                }
                out << "\n";
            }
            out.flush();
            outFile.close();

            dir = fi.absoluteDir().absolutePath();
            settings.setValue("directory/video", dir);
        }
    }
}

void GameTableImpl::setButtonTab(QPushButton *active) {
    ui->buttonTabConfig->setChecked(active == ui->buttonTabConfig);
    ui->buttonTabPlay->setChecked(active == ui->buttonTabPlay);
    ui->buttonTabLog->setChecked(active == ui->buttonTabLog);
    ui->buttonTabChances->setChecked(active == ui->buttonTabChances);
    ui->buttonTabError->setChecked(active == ui->buttonTabError);
}

void GameTableImpl::slotButtonTabConfigClicked() {
    ui->toolTab->setCurrentIndex(0);
    setButtonTab(ui->buttonTabConfig);
    signalHandChanged(QVector<int>());
}

void GameTableImpl::slotButtonTabPlayClicked() {
    ui->toolTab->setCurrentIndex(1);
    setButtonTab(ui->buttonTabPlay);
    signalHandChanged(QVector<int>());
}

void GameTableImpl::slotButtonTabLogClicked() {
    ui->toolTab->setCurrentIndex(2);
    setButtonTab(ui->buttonTabLog);
    signalHandChanged(QVector<int>());
}

void GameTableImpl::slotButtonTabChancesClicked() {
    ui->toolTab->setCurrentIndex(3);
    slotCheckboxCheatClicked();
    setButtonTab(ui->buttonTabChances);
    slotCheckboxCheatClicked();
}

void GameTableImpl::slotButtonTabErrorClicked() {
    ui->toolTab->setCurrentIndex(4);
    setButtonTab(ui->buttonTabError);
    signalHandChanged(QVector<int>());
}

void GameTableImpl::slotPlayerActionDone(int seat) {

    int old = waitingForVideo;
    waitingForVideo &= ~(1 << seat);
    refreshButton();
    //qDebug() << ">>> slotPlayerActionDone" << seat << startNewHandDefered << old << waitingForVideo;
    if (startNewHandDefered && old && !waitingForVideo) {
        startNewHand();
    }
}

void GameTableImpl::slotCheckboxCheatClicked() {
    if (currentGame && currentGame->getCurrentHand()) {
        refreshCardsChance(currentGame->getCurrentHand()->getCurrentRound(), false);
    }
}

void GameTableImpl::slotShowInfo(QString txt) {
    ui->textBrowser_Log->append("<span style=\"color:#000040;\">"+txt+"</span>");
}

void GameTableImpl::slotShowError(QString txt)
{
   ui->textBrowser_Error->append("<b><span style=\"color:#200000;\">"+txt+"</span></b>");
   slotButtonTabErrorClicked();
}

void GameTableImpl::slotShowFatalError(QString txt)
{
   ui->textBrowser_Error->append("<b><span style=\"color:#200000;\">Fatal: "+txt+"</span></b>");
   slotButtonTabErrorClicked();
}

static void translateCardCode(int cardCode, QString &cardString, bool markEmpty)
{
    if (cardCode < 0) {
        if (markEmpty) {
            cardString += "--";
        }
    } else {
        int value = cardCode%13;
        int color = cardCode/13;

        switch (value) {

        case 0:
            cardString += "2";
            break;
        case 1:
            cardString += "3";
            break;
        case 2:
            cardString += "4";
            break;
        case 3:
            cardString += "5";
            break;
        case 4:
            cardString += "6";
            break;
        case 5:
            cardString += "7";
            break;
        case 6:
            cardString += "8";
            break;
        case 7:
            cardString += "9";
            break;
        case 8:
            cardString += "T";
            break;
        case 9:
            cardString += "J";
            break;
        case 10:
            cardString += "Q";
            break;
        case 11:
            cardString += "K";
            break;
        case 12:
            cardString += "A";
            break;
        }

        switch (color) {

        case 0:
            //cardString += "d";
            cardString += QChar(0x2666);
            break;
        case 1:
            //cardString += "h";
            cardString += QChar(0x2665);
            break;
        case 2:
            //cardString += "s";
            cardString += QChar(0x2660);
            break;
        case 3:
            //cardString += "c";
            cardString += QChar(0x2663);
            break;
        }
    }
}

static int valOfSuit[] {2, 1, 0, 4};

static bool cardLess(int c1, int c2) {
    if (c1 % 13 < c2 % 13)  {
        return true;
    }
    if (c1 % 13 == c2 % 13)  {
        if (valOfSuit[c1 / 13] < valOfSuit[c2 / 13]) {
            return true;
        }
    }
    return false;
}

void GameTableImpl::slotHandProbability(QVector<int> cards, QVector<double> probs) {
    //qDebug() << "slotHandProbability";
    for (int i = 0; i < 6; i++) {
        if (cards[2*i] >= 0) {
            QString s;
            if (cardLess(cards[2*i], cards[2*i+1])) {
                translateCardCode(cards[2*i+1], s, false);
                translateCardCode(cards[2*i], s, false);
            } else {
                translateCardCode(cards[2*i], s, false);
                translateCardCode(cards[2*i+1], s, false);
            }
            labelCards[i]->setText(s);
            labelProbability[i]->setText(QString::asprintf("%.1f%%", 100 * probs[i]));
        } else if (cards[2*i] == -2) {
            labelCards[i]->setText("????");
            labelProbability[i]->setText("--.-%");
        } else {
            labelCards[i]->setText("");
            labelProbability[i]->setText("");
        }
    }
    //QString bs;
    //translateCardCode(cards[CardIndexFlop], bs, true);
    //translateCardCode(cards[CardIndexFlop+1], bs, true);
    //translateCardCode(cards[CardIndexFlop+1], bs, true);
    //translateCardCode(cards[CardIndexTurn], bs, true);
    //translateCardCode(cards[CardIndexRiver], bs, true);
    //ui->labelCardsBoard->setText(bs);
}

void GameTableImpl::slotButtonWineClicked() {
    int bp = 1;

    std::shared_ptr<PlayerInterface> humanPlayer = currentGame->getSeatsList()->front();
    if (humanPlayer->getMyCash() >= 2 * currentGame->getStartSmallBlind()) {
        signalPlayerAction(-1, JobActionDrink, -1, false);
        humanPlayer->setMyCash(humanPlayer->getMyCash() - currentGame->getStartSmallBlind());
        humanPlayer->setMyRoundStartCash(humanPlayer->getMyRoundStartCash() - currentGame->getStartSmallBlind());
        refreshCash(false);
        provideMyActions(-1);
    }
}
