/* Created by Matt Berry */

#include "dealer.h"

Dealer::Dealer() : holeHidden(false) {}

void Dealer::receiveCard(Card c, bool faceDown) {
    hand.addCard(c);
    if (faceDown) holeHidden = true;
}

Hand& Dealer::getHand() { return hand; }
const Hand& Dealer::getHand() const { return hand; }

bool Dealer::shouldHit() const {
    int v = hand.value();
    if (v < 17) return true;
    /* Dealer hits on soft 17 */
    if (v == 17 && hand.isSoft()) return true;
    return false;
}

bool Dealer::peekForBlackjack() const {
    /* Check without changing holeHidden — used for insurance resolution */
    return hand.isBlackjack();
}

void Dealer::revealHoleCard() {
    holeHidden = false;
}

bool Dealer::isHoleCardHidden() const {
    return holeHidden;
}

const Card& Dealer::upCard() const {
    /* The up-card is the second card dealt (index 1).
     * Index 0 is the hole card dealt face-down first. */
    return hand.getCards()[1];
}

void Dealer::reset() {
    hand.clear();
    holeHidden = false;
}
