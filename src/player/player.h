/* Created by Matt Berry */

#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "card/hand.h"

/* Represents the human player — holds balance and one or more hands (splits) */
class Player {
public:
    explicit Player(int startingBalance);

    int  getBalance() const;
    void setBalance(int b);

    /* Begin a new round: add a starting hand with this bet */
    void startRound(int bet);

    /* Reset all hands for a new round */
    void clearHands();

    /* Split hand at idx: move second card to a new hand, both get a fresh card
     * from the caller via addCardToHand(). Returns index of new hand. */
    int splitHand(int idx);

    Hand& getHand(int idx);
    const Hand& getHand(int idx) const;
    int getHandCount() const;

    int  getHandBet(int idx) const;
    void setHandBet(int idx, int b);

    /* True if this hand was created from splitting aces (only 1 card allowed) */
    bool handIsSplitAce(int idx) const;
    void markSplitAce(int idx);

    /* Insurance side-bet (separate from hand bets) */
    int  getInsuranceBet() const;
    void setInsuranceBet(int b);

    /* Number of splits performed this round (max 3) */
    int getSplitCount() const;

private:
    int balance;
    int insuranceBet;

    std::vector<Hand> hands;
    std::vector<int>  bets;
    std::vector<bool> splitAceFlags;
    int splitCount;
};

#endif
