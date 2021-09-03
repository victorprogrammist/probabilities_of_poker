
#include "deck.h"
#include <algorithm>
#include <cassert>

Deck::Deck() {
    for (int ii = 0; ii < 52; ++ii)
        residueDeck.push_back(ii);
}

void Deck::checkFullAss() const {

    vector<IdxCard> test(residueDeck);

    std::sort(test.begin(), test.end());

    assert( test.size() == 52 );
    for (int ii = 0; ii < test.size(); ++ii)
        assert( test.at(ii) == ii );
}

IdxCard Deck::takeByIndex(int idx) {
    IdxCard res = residueDeck.at(idx);
    residueDeck.erase(residueDeck.begin() + idx);
    return res;
}

int Deck::findCard(IdxCard card) const {
    auto itr = std::find(residueDeck.begin(), residueDeck.end(), card);
    if (itr == residueDeck.end())
        return -1;
    return itr - residueDeck.begin();
}

IdxCard Deck::takeOne(IdxCard card) {
    // изъятие одной указанной карты
    int idx = findCard(card);
    assert( idx >= 0 );
    return takeByIndex(idx);
}

void Deck::takeList(const ListCards& cards) {
    // изъятие списка указанных карт
    for (const auto& card: cards)
        takeOne(card);
}

IdxCard Deck::takeRandom() {
    // изъятие одной случайной карты
    int idx = rand() % residueCount();
    return takeByIndex(idx);
}

ListCards Deck::takeRandomCount(int cnt) {
    ListCards res;
    res.reserve(cnt);
    for (int ii = 0; ii < cnt; ++ii)
        res.push_back(takeRandom());
    return res;
}

void Deck::recall(IdxCard card) {
    // возврат карты обратно в пачку неизвестных
    residueDeck.push_back(card);
}

void Deck::recallList(const ListCards& cards) {
    for (const IdxCard& card: cards)
        recall(card);
}
