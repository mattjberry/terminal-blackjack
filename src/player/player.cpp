/* Created by Matt Berry */

#include "player.h"

Player::Player(int startingBalance)
    : balance(startingBalance), insuranceBet(0), splitCount(0) {}

int Player::getBalance() const { return balance; }
void Player::setBalance(int b) { balance = b; }

void Player::startRound(int bet) {
    clearHands();
    hands.emplace_back();
    bets.push_back(bet);
    splitAceFlags.push_back(false);
}

void Player::clearHands() {
    hands.clear();
    bets.clear();
    splitAceFlags.clear();
    splitCount = 0;
    insuranceBet = 0;
}

int Player::splitHand(int idx) {
    /* Move the second card of hand[idx] to a brand-new hand */
    Card movedCard = hands[idx].getCards()[1];

    /* Rebuild hand[idx] with only its first card */
    Card firstCard = hands[idx].getCards()[0];
    hands[idx].clear();
    hands[idx].addCard(firstCard);

    /* Create the new hand with the moved card */
    Hand newHand;
    newHand.addCard(movedCard);

    int newIdx = static_cast<int>(hands.size());
    hands.push_back(newHand);
    bets.push_back(bets[idx]);         /* same bet as original */
    splitAceFlags.push_back(false);    /* caller will markSplitAce if needed */

    splitCount++;
    return newIdx;
}

Hand& Player::getHand(int idx) { return hands[idx]; }
const Hand& Player::getHand(int idx) const { return hands[idx]; }
int Player::getHandCount() const { return static_cast<int>(hands.size()); }

int  Player::getHandBet(int idx) const { return bets[idx]; }
void Player::setHandBet(int idx, int b) { bets[idx] = b; }

bool Player::handIsSplitAce(int idx) const { return splitAceFlags[idx]; }
void Player::markSplitAce(int idx) { splitAceFlags[idx] = true; }

int  Player::getInsuranceBet() const { return insuranceBet; }
void Player::setInsuranceBet(int b)  { insuranceBet = b; }

int Player::getSplitCount() const { return splitCount; }
