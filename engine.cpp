
#include "engine.h"
#include <QStringList>
#include <QRegularExpression>

bool CheckAlienAction::fold(const ListCards& hand, const ListCards& desk) const {
    if (!checkFold) return false;
    return checkFold(hand, desk);
}

Combination getConcatCombo(const ListCards& cards1, const ListCards& cards2) {

    vector<Card> nativeCards;
    nativeCards.reserve(cards1.size() + cards2.size());

    for (const IdxCard& card: cards1)
        nativeCards.push_back(Card(card));

    for (const IdxCard& card: cards2)
        nativeCards.push_back(Card(card));

    Combination res;
    getCombo(res, nativeCards);
    return res;
}

// достраивает srcDesk до 5 и сравнивает результат.
int calcOneRandomVariant_genDesk(
        Deck& deck,
        const ListCards& myHand,
        const ListCards& alienHand,
        const ListCards& srcDesk) {

    int szSrcDesk = srcDesk.size();

    if (szSrcDesk >= 5)
        return compareCombo(
            getConcatCombo(myHand, srcDesk),
            getConcatCombo(alienHand, srcDesk));

    // 1. достраивает desk до 5, если desk меньше
    ListCards newDesk(srcDesk);
    while (newDesk.size() < 5)
        newDesk.push_back(deck.takeRandom());

    // 2. расчет комбинаций
    Combination myCombo = getConcatCombo(myHand, newDesk);
    Combination alienCombo = getConcatCombo(alienHand, newDesk);

    // 3. возвращает карты в колоду
    for (int ii = szSrcDesk; ii < newDesk.size(); ++ii)
        deck.recall(newDesk.at(ii));

    // 4. сравнение комбинаций
    return compareCombo(myCombo, alienCombo);
}

int calcOneRandomVariant_genAlienAndDesk(
        Deck& deck,
        const ListCards& myHand,
        const ListCards& srcDesk,
        const CheckAlienAction& check) {

    // 1. генерирует расклад противника
    ListCards alienHand = deck.takeRandomCount(2);

    int r = -2;

    // 2. проверяет желание противника отказаться
    if (!check.fold(alienHand, srcDesk))
        // 3. достраивает стол
        r = calcOneRandomVariant_genDesk(deck, myHand, alienHand, srcDesk);

    // 4. возврат в колоду противника
    deck.recallList(alienHand);

    return r;
}

ResProb calcProbability(
        Deck& deck,
        const ListCards& myHand,
        const ListCards& srcDesk,
        const CheckAlienAction& check,
        int cInterate) {

    if (myHand.size() < 2 || srcDesk.size() < 2)
        return {};

    ResProb r;

    for (; r.cAll < cInterate; ++r.cAll) {
        int c = calcOneRandomVariant_genAlienAndDesk(deck, myHand, srcDesk, check);
        if (c <= -2)
            --r.cAll;
        else if (c > 0)
            ++r.cWin;
        else if (c < 0)
            ++r.cLoss;
    }

    return r;
}

void splitHand(
        const ListCards& cards,
        ListCards& hand,
        ListCards& desk) {

    for (auto& icard: cards) {
        if (hand.size() >= 2) break;
        hand.push_back(icard);
    }

    for (int ii = hand.size(); ii < cards.size(); ++ii)
        desk.push_back(cards.at(ii));
}

ResProb calcAbstractProbability(
        const ListCards& cards, int cIterate) {

    Deck deck;
    deck.takeList(cards);

    ListCards myHand;
    ListCards desk;
    splitHand(cards, myHand, desk);

    return calcProbability(deck, myHand, desk, {}, cIterate);
}

ResProb calcPseudoRealProbability(
        const ListCards& cards,
        double probAlienFold,
        int cIterate, int cAlienIterate) {

    Deck deck;
    deck.takeList(cards);

    ListCards myHand;
    ListCards desk;
    splitHand(cards, myHand, desk);

    CheckAlienAction alienCheck;

    Deck alienTurn;
    alienTurn.takeList(desk);

    alienCheck.checkFold = [&](const ListCards& alienHand, const ListCards& desk) -> bool {

        alienTurn.takeList(alienHand);

        auto r = calcProbability(deck, alienHand, desk, {}, cAlienIterate);

        alienTurn.recallList(alienHand);

        if (r.probWin() <= probAlienFold)
            return true;

        return false;
    };

    return calcProbability(deck, myHand, desk, alienCheck, cIterate);
}

QString listNamesCards(const ListCards& cards) {

    QStringList r;
    for (const IdxCard& card: cards)
        r += QString::fromStdString(Card(card).getShortName());
    return r.join(" ");
}

QString listNumericNamesCards(const ListCards& cards) {

    QStringList r;
    for (const IdxCard& card: cards)
        r += QString::fromStdString(Card(card).getNumericName());

    return r.join(" ");
}

ListCards parseCards(const QString& sCards) {

    vector<IdxCard> cards;

    for (const QString& _sCard: sCards.split(" ")) {
        QString sCard = _sCard.trimmed();
        if (sCard.isEmpty()) continue;

        Card card(sCard.toStdString());
        if (card.isValid())
            cards.push_back( card.getIndex() );
    }

    return cards;
}

ListCards parseNumericCards(const QString& sCards) {
    vector<IdxCard> cards;

    QRegularExpression re("[-+ ]+");

    for (const QString& _sCard: sCards.split(re)) {
        QString sCard = _sCard.trimmed();
        if (sCard.isEmpty()) continue;

        Card card;
        card.setNumericName(sCard.toStdString());

        if (card.isValid())
            cards.push_back( card.getIndex() );
    }

    return cards;
}





