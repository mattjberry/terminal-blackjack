/* Created by Matt Berry */

#ifndef HAND_H
#define HAND_H

#include <vector>
#include "card.h"

/* Returns the blackjack point value of a card's face value.
 * Ace returns 11 (caller handles soft/hard reduction).
 * Jack/Queen/King all return 10. */
int cardPoints(Value v);

/* Represents a hand of playing cards for one participant */
class Hand {
public:
    void addCard(Card c);

    /* Best hand value ≤ 21; if impossible, smallest bust value */
    int value() const;

    /* Exactly 2 cards and value == 21 */
    bool isBlackjack() const;

    /* value() > 21 */
    bool isBust() const;

    /* Has at least one Ace currently counted as 11 */
    bool isSoft() const;

    /* Exactly 2 cards of the same Value enum (split eligibility) */
    bool isPair() const;

    /* Exactly 2 cards (double-down eligibility) */
    bool canDoubleDown() const;

    int size() const;

    const std::vector<Card>& getCards() const;

    void clear();

private:
    std::vector<Card> cards;
};

#endif
