
#ifndef __CARD_ENGINE_H__
#define __CARD_ENGINE_H__

#include "card.h"
#include "combination.h"
#include "deck.h"
#include "fdiv.h"

#include <functional>
#include <QString>

using std::function;

Combination getCombo(const ListCards& cards1, const ListCards& cards2);

struct CheckAlienAction {

    function<bool(const ListCards& hand, const ListCards& desk)> checkFold;

    bool fold(const ListCards& hand, const ListCards& desk) const;
};

int calcRandomVariant(
        Deck& deck,
        const ListCards& myHand,
        const ListCards& srcDesk);

struct ResProb {

    int cAll = 0;
    int cWin = 0;
    int cLoss = 0;

    double probWin() const {
        if (!cAll) return 0;
        return FDIV(cWin, cAll); }

    double probLoss() const {
        if (!cAll) return 0;
        return FDIV(cLoss, cAll); }
};

ResProb calcProbability(
        Deck& deck,
        const ListCards& myHand,
        const ListCards& srcDesk,
        const CheckAlienAction& check,
        int cInterate);

ResProb calcAbstractProbability(
        const ListCards& cards, int cIterate);

ResProb calcPseudoRealProbability(
        const ListCards& cards,
        double probAlienFold,
        int cIterate, int cAlienIterate);

ListCards parseCards(const QString& sCards);
ListCards parseNumericCards(const QString& sCards);

QString listNamesCards(const ListCards& cards);
QString listNumericNamesCards(const ListCards& cards);

#endif // __CARD_ENGINE_H__

