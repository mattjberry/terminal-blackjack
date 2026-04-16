/* Created by Matt Berry */

#ifndef DEALER_H
#define DEALER_H

#include "card/hand.h"

/* Represents the dealer — manages hole-card visibility and AI hit logic */
class Dealer {
public:
    Dealer();

    /* Add a card; pass faceDown=true for the hole card */
    void receiveCard(Card c, bool faceDown = false);

    Hand& getHand();
    const Hand& getHand() const;

    /* True if value ≤ 16, or hand is a soft 17 (hits on soft 17) */
    bool shouldHit() const;

    /* Peek at the hole card without changing visibility.
     * Returns true if dealer has a natural blackjack. */
    bool peekForBlackjack() const;

    /* Flip the hole card face-up */
    void revealHoleCard();

    bool isHoleCardHidden() const;

    /* The card that is visible to the player (second card dealt, index 1) */
    const Card& upCard() const;

    void reset();

private:
    Hand hand;
    bool holeHidden;
};

#endif
