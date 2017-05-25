#include "psworker.h"

#include <vector>

#include <QDebug>

#include <sys/time.h>
#include <math.h>

//QString remaining(int deck[], int pos) {
//    QString cardString;
//    QString sep;
//    for (int i = 0; i <= pos; i++) {
//        cardString += sep;
//        sep = " ";
//        cardString += QString::fromStdString(Card(deck[i]).str());
//    }
//    return cardString;
//}

PSWorker::PSWorker(QObject *parent) :
    QObject(parent),
    res(6, 0),
    resSum(6, 0)
{
    pokerHandEvaluator = PokerHandEvaluator::alloc("h");

    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotNextRandom()));
}

void PSWorker::slotHandChanged(QVector<int> argCards) {
    //qDebug() << "slotHandChanged" << argCards.size();
    timer->stop();
    if (argCards.isEmpty()) {
        return;
    }

    cards = argCards;

    //QString s;
    //translateCardCode(cards[0], s, true);
    //translateCardCode(cards[1], s, true);
    //s += " ";
    //translateCardCode(cards[2], s, true);
    //translateCardCode(cards[3], s, true);
    //s += " ";
    //translateCardCode(cards[4], s, true);
    //translateCardCode(cards[5], s, true);
    //s += " ";
    //translateCardCode(cards[6], s, true);
    //translateCardCode(cards[7], s, true);
    //s += " ";
    //translateCardCode(cards[8], s, true);
    //translateCardCode(cards[9], s, true);
    //s += " ";
    //translateCardCode(cards[10], s, true);
    //translateCardCode(cards[11], s, true);
    //s += " ";
    //translateCardCode(cards[12], s, true);
    //translateCardCode(cards[13], s, true);
    //translateCardCode(cards[14], s, true);
    //translateCardCode(cards[15], s, true);
    //translateCardCode(cards[16], s, true);

    //qDebug() << "slotHandChanged" << s;
    //qDebug() << "slotHandChanged";

    numSeats = 0;
    for (int i = 0; i < 6; i++) {
         if ((int)(cards[2*i]) != -1) {
             numSeats++;
         }
    }
    randomCards = 0;

    if (numSeats >= 2) {
        holeCards = std::vector<CardDistribution>(numSeats);
        int ii = 0;
        for (int i = 0; i < 6; i++) {
            if (cards[2*i] >= 0) {
                QString s;
                translateCardCode(cards[2*i], s);
                translateCardCode(cards[2*i+1], s);
                QByteArray sa = s.toLatin1();
                holeCards[ii].parse(sa.data());
                holeSeat[ii] = i;
                ii++;
            } else if (cards[2*i] == -2) {
                randomCards |= 1 << ii;
                holeSeat[ii] = i;
                ii++;
            }
        }
        QString bs;
        translateCardCode(cards[CardIndexFlop], bs);
        translateCardCode(cards[CardIndexFlop+1], bs);
        translateCardCode(cards[CardIndexFlop+2], bs);
        translateCardCode(cards[CardIndexTurn], bs);
        translateCardCode(cards[CardIndexRiver], bs);
        QByteArray bsa = bs.toLatin1();
        boardCards = CardSet(bsa.data());

        for (int i = 0; i < 6; i++) {
            res[i] = 0;
            resSum[i] = 0;
        }
        resSumCount = 0;
        calculate();
    } else {
        for (int i = 0; i < 6; i++) {
            if ((int)(cards[2*i]) >= 0) {
                res[i] = 1;
            } else {
                res[i] = 0;
            }
        }
        signalHandProbability(cards, res);
    }
    //qDebug() << "slotHandChanged done";
}

void PSWorker::slotNextRandom() {
    //qDebug() << "slotNextRandom";
     if (randomCards) {
        calculate();
     }
     //qDebug() << "slotNextRandom done";
}

void PSWorker::calculate() {
    //qDebug() << "calculate";
    //struct timeval t0;
    //struct timeval t1;
    //float elapsed;
    //gettimeofday(&t0, 0);

    if (randomCards) {
        CardSet usedCards;
        std::vector<Card> board = boardCards.cards();
        for (size_t i = 0; i < board.size(); i++) {
            usedCards.insert(board[i]);
            //qDebug() << "BoardCard" << QString::fromStdString(board[i].str());
        }
        for (int ii = 0; ii < numSeats; ii++) {
            if ((randomCards & 1 << ii) == 0) {
                const std::vector<Card> &cards = holeCards[ii][0].cards();
                usedCards.insert(cards[0]);
                usedCards.insert(cards[1]);
                //qDebug() << "HoleCard" << ii << QString::fromStdString(cards[0].str());
                //qDebug() << "HoleCard" << ii << QString::fromStdString(cards[1].str());
            }
        }

        int deck[52];
        for (int i = 0; i < 52; i++) {
            deck[i] = i;
        }
        int randPos = 51;
        std::vector<Card> used = usedCards.cards();
        for (int i = used.size() - 1; i >= 0; i--) {
            deck[used[i].code()] = deck[randPos];
            deck[randPos] = used[i].code();
            randPos--;
        }
        //qDebug() << remaining(deck, randPos);

        for (int ii = 0; ii < numSeats; ii++) {
            if (randomCards & 1 << ii) {
                int tmp = rand() % (randPos + 1);
                CardSet cs;
                int c = deck[tmp];
                deck[tmp] = deck[randPos];
                deck[randPos] = c;
                randPos--;
                cs.insert(Card(c));
                tmp = rand() % (randPos + 1);
                c = deck[tmp];
                deck[tmp] = deck[randPos];
                deck[randPos] = c;
                randPos--;
                cs.insert(Card(c));
                holeCards[ii] = CardDistribution(cs);
                const std::vector<Card> &cards = holeCards[ii][0].cards();
                //qDebug() << "HoleCard rand" << ii << QString::fromStdString(cards[0].str());
                //qDebug() << "HoleCard rand" << ii << QString::fromStdString(cards[1].str()) << remaining(deck, randPos);
            }
        }
    }

    std::vector<EquityResult> result = showdown.calculateEquity(holeCards, boardCards, pokerHandEvaluator);

    //gettimeofday(&t1, 0);
    //elapsed = (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
    //qDebug() << "Elapsed:" << elapsed;

    double total = 0;
    for (int i = 0; i < numSeats; i++) {
        total += result[i].winShares;
        total += result[i].tieShares;
    }

    if (total == 0) {
        int bp = 1;
    }

    //for (int i = 0; i < numSeats; i++) {
    //    qDebug() << "..." << holeSeat[i] << ((result[i].winShares + result[i].tieShares) / total);
    //}

    for (int i = 0; i < numSeats; i++) {
        resSum[holeSeat[i]] += ((result[i].winShares + result[i].tieShares) / total);
    }
    resSumCount++;
    for (int i = 0; i < 6; i++) {
        res[i] = resSum[i] / resSumCount;
    }
    signalHandProbability(cards, res);

    if (randomCards) {
        timer->start(100);
    }
    //qDebug() << "calculate done";
}

void PSWorker::translateCardCode(int cardCode, QString &cardString, bool markEmpty)
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
            cardString += "d";
            break;
        case 1:
            cardString += "h";
            break;
        case 2:
         cardString += "s";
            break;
        case 3:
            cardString += "c";
            break;
        }
    }
}

