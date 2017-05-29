#-------------------------------------------------
#
# Project created by QtCreator 2016-11-27T18:24:58
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ssp
TEMPLATE = app

QMAKE_CXXFLAGS += -Wno-unused-parameter -std=c++11

DEFINES += ENABLE_IPV6

INCLUDEPATH +=  . \
                play \
                pokerth \
                pokerth/config \
                pokerth/core \
                pokerth/engine \
                pokerth/engine/local_engine \
                pokerth/gui \
                pokerth/gui/qt \
                edit \
                third_party

DEPENDPATH +=   . \
                play \
                pokerth \
                pokerth/config \
                pokerth/core \
                pokerth/engine \
                pokerth/engine/local_engine \
                pokerth/gui \
                pokerth/gui/qt \
                edit


SOURCES  += main.cpp\
            pokerth/playerdata.cpp\
            pokerth/config/configfile.cpp \
            pokerth/engine/boardinterface.cpp\
            pokerth/engine/log.cpp\
            pokerth/engine/enginefactory.cpp\
            pokerth/engine/game.cpp\
            pokerth/engine/handinterface.cpp\
            pokerth/engine/berointerface.cpp\
            pokerth/engine/playerinterface.cpp\
            pokerth/engine/local_engine/localplayer.cpp\
            pokerth/engine/local_engine/replay.cpp\
            pokerth/engine/local_engine/tools.cpp\
            pokerth/engine/local_engine/localberopostriver.cpp\
            pokerth/engine/local_engine/localberopreflop.cpp\
            pokerth/engine/local_engine/localbero.cpp\
            pokerth/engine/local_engine/localberoflop.cpp\
            pokerth/engine/local_engine/cardsvalue.cpp\
            pokerth/engine/local_engine/localenginefactory.cpp\
            pokerth/engine/local_engine/localboard.cpp\
            pokerth/engine/local_engine/arraydata.cpp\
            pokerth/engine/local_engine/localberoriver.cpp\
            pokerth/engine/local_engine/localhand.cpp\
            pokerth/engine/local_engine/localberoturn.cpp\
            pokerth/engine/local_engine/localexception.cpp\
            pokerth/gui/guiinterface.cpp\
            pokerth/core/common/pokerthexception.cpp\
    pokerth/xxx_tmp.cpp \
    pokerth/gui/qt/guiwrapper.cpp \
    pokerth/gui/qt/gametableimpl.cpp \
    pokerth/gui/qt/mycardspixmaplabel.cpp \
    pokerth/gui/qt/playerbox.cpp \
    pokerth/gui/qt/myactionbutton.cpp \
    pokerth/gui/qt/mycashlabel.cpp \
    pokerth/gui/qt/guilog.cpp \
    play/mainwindow.cpp \
    play/videowidget.cpp \
    pokerstove/penum/CardDistribution.cpp \
    pokerstove/penum/ShowdownEnumerator.cpp \
    pokerstove/peval/Card.cpp \
    pokerstove/peval/CardSet.cpp \
    pokerstove/peval/CardSetGenerators.cpp \
    pokerstove/peval/PokerEvaluation.cpp \
    pokerstove/peval/PokerHand.cpp \
    pokerstove/peval/PokerHandEvaluator.cpp \
    pokerstove/peval/PokerHandEvaluator_Alloc.cpp \
    pokerstove/peval/Rank.cpp \
    pokerstove/peval/Suit.cpp \
    pokerstove/psworker.cpp \
    pokerth/gui/qt/cashimage.cpp \
    edit/clipeditor.cpp \
    edit/clipgraphicsview.cpp \
    play/backgroundwidget.cpp \
    play/previewproducer.cpp \
    play/previewfiledialog.cpp \
    util.cpp \
    play/soundplayer.cpp \
    play/videoframegrabber.cpp

HEADERS  += \
            pokerth/xxx_tmp.h\
            pokerth/game_defs.h\
            pokerth/playerdata.h\
            pokerth/gamedata.h\
            pokerth/config/configfile.h \
            pokerth/core/loghelper.h\
            pokerth/core/pokerthexception.h\
            pokerth/engine/boardinterface.h\
            pokerth/engine/log.h\
            pokerth/engine/berointerface.h\
            pokerth/engine/handinterface.h\
            pokerth/engine/engine_defs.h\
            pokerth/engine/game.h\
            pokerth/engine/enginefactory.h\
            pokerth/engine/playerinterface.h\
            pokerth/engine/local_engine/localbero.h\
            pokerth/engine/local_engine/localhand.h\
            pokerth/engine/local_engine/localberopostriver.h\
            pokerth/engine/local_engine/localberoturn.h\
            pokerth/engine/local_engine/tools.h\
            pokerth/engine/local_engine/localberoriver.h\
            pokerth/engine/local_engine/localexception.h\
            pokerth/engine/local_engine/arraydata.h\
            pokerth/engine/local_engine/localboard.h\
            pokerth/engine/local_engine/localenginefactory.h\
            pokerth/engine/local_engine/localberopreflop.h\
            pokerth/engine/local_engine/localplayer.h\
            pokerth/engine/local_engine/localberoflop.h\
            pokerth/engine/local_engine/cardsvalue.h\
            pokerth/engine/local_engine/engine_msg.h\
            pokerth/engine/local_engine/replay.h\
            pokerth/gui/guiinterface.h\
    pokerth/gui/qt/guiwrapper.h \
    pokerth/gui/qt/gametableimpl.h \
    pokerth/gui/qt/mycardspixmaplabel.h \
    pokerth/gui/qt/playerbox.h \
    pokerth/gui/qt/myactionbutton.h \
    pokerth/gui/qt/mycashlabel.h \
    pokerth/gui/qt/guilog.h \
    play/mainwindow.h \
    play/videowidget.h \
    pokerstove/penum/CardDistribution.h \
    pokerstove/penum/Odometer.h \
    pokerstove/penum/PartitionEnumerator.h \
    pokerstove/penum/ShowdownEnumerator.h \
    pokerstove/penum/SimpleDeck.hpp \
    pokerstove/peval/Card.h \
    pokerstove/peval/CardSet.h \
    pokerstove/peval/CardSetGenerators.h \
    pokerstove/peval/Holdem.h \
    pokerstove/peval/HoldemHandEvaluator.h \
    pokerstove/peval/PokerEvaluation.h \
    pokerstove/peval/PokerEvaluationTables.h \
    pokerstove/peval/PokerHand.h \
    pokerstove/peval/PokerHandEvaluation.h \
    pokerstove/peval/PokerHandEvaluator.h \
    pokerstove/peval/Rank.h \
    pokerstove/peval/Suit.h \
    pokerstove/util/combinations.h \
    pokerstove/util/lastbit.h \
    pokerstove/util/utypes.h \
    pokerstove/psworker.h \
    pokerth/gui/qt/cashimage.h \
    edit/clipeditor.h \
    edit/clipgraphicsview.h \
    play/backgroundwidget.h \
    play/previewproducer.h \
    play/previewfiledialog.h \
    util.h \
    edit/slidernowheel.h \
    edit/plaintextediteatwheel.h \
    play/soundplayer.h \
    play/videoframegrabber.h

FORMS    += \
    pokerth/gui/qt/gametableimpl.ui \
    play/mainwindow.ui \
    edit/clipeditor.ui \
    play/previewproducer.ui

DISTFILES += \
    TODO \
    edit/help.html \
    LICENSE.md \
    README.md

RESOURCES += \
    pokerth/gui/qt/resources/coins0.qrc \
    pokerth/gui/qt/resources/coins1.qrc \
    pokerth/gui/qt/resources/coins2.qrc \
    pokerth/gui/qt/resources/coins3.qrc \
    pokerth/gui/qt/resources/images.qrc \
    pokerth/gui/qt/resources/pokerth.qrc \
    edit/edit.qrc \
    play/resources/sounds.qrc \
    resources/fonts.qrc
