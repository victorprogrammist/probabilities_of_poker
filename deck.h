
#ifndef __DECK_H__
#define __DECK_H__

#include <vector>

using std::vector;

using IdxCard = int;
using ListCards = vector<IdxCard>;


// этот класс представляет собой колоду с изъятием.
struct Deck {

    ListCards residueDeck;

    Deck();

    void checkFullAss() const;

    bool existsCart(IdxCard card) const { return findCard(card) >= 0; }

    int residueCount() const { return residueDeck.size(); }

    IdxCard takeByIndex(int idx);

    int findCard(IdxCard card) const;

    IdxCard takeOne(IdxCard card);
    void takeList(const ListCards& cards);

    IdxCard takeRandom();
    ListCards takeRandomCount(int cnt);

    void recall(IdxCard card);
    void recallList(const ListCards& cards);
};

#endif
