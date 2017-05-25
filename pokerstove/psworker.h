#ifndef PSWORKER_H
#define PSWORKER_H

#include <QObject>
#include <QVector>
#include <QTimer>

#include <pokerstove/peval/PokerHandEvaluator.h>
#include <pokerstove/peval/Card.h>
#include <pokerstove/peval/CardSet.h>
#include <pokerstove/penum/CardDistribution.h>
#include <pokerstove/penum/ShowdownEnumerator.h>

using namespace pokerstove;

enum CardIndex {
    CardIndexHumanPlayer = 0,
    CardIndexPlayer1 = 2,
    CardIndexPlayer2 = 4,
    CardIndexPlayer3 = 6,
    CardIndexPlayer4 = 8,
    CardIndexPlayer5 = 10,
    CardIndexFlop = 12,
    CardIndexTurn = 15,
    CardIndexRiver = 16,
    CardVectorSize = 17
};

class PSWorker : public QObject
{
    Q_OBJECT
public:
    explicit PSWorker(QObject *parent = 0);

signals:

public slots:
    void slotHandChanged(QVector<int> cards);

private slots:
    void slotNextRandom();

signals:
    void signalHandProbability(QVector<int> cards, QVector<double> probs);

private:
    ShowdownEnumerator showdown;
    std::vector<CardDistribution> holeCards;
    CardSet boardCards;
    QVector<double> res;
    QVector<double> resSum;
    int resSumCount;
    QVector<int> cards;
    int randomCards = 0;
    int numSeats;
    int holeSeat[6];
    QTimer *timer;

    PokerHandEvaluator::eval_ptr pokerHandEvaluator;

    void translateCardCode(int cardCode, QString &cardString, bool markEmpty = false);
    void calculate();
};

#endif // PSWORKER_H
