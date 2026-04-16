/* Created by Matt Berry */

#include "hand.h"

int cardPoints(Value v) {
    switch (v) {
        case Value::Ace:   return 11;
        case Value::Two:   return 2;
        case Value::Three: return 3;
        case Value::Four:  return 4;
        case Value::Five:  return 5;
        case Value::Six:   return 6;
        case Value::Seven: return 7;
        case Value::Eight: return 8;
        case Value::Nine:  return 9;
        case Value::Ten:
        case Value::Jack:
        case Value::Queen:
        case Value::King:  return 10;
    }
    return 0;
}

void Hand::addCard(Card c) {
    cards.push_back(c);
}

int Hand::value() const {
    int total = 0;
    int aces = 0;

    for (const Card& c : cards) {
        int pts = cardPoints(c.value);
        total += pts;
        if (c.value == Value::Ace) aces++;
    }

    /* Convert aces from 11 to 1 as needed to stay at or below 21 */
    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }

    return total;
}

bool Hand::isBlackjack() const {
    return cards.size() == 2 && value() == 21;
}

bool Hand::isBust() const {
    return value() > 21;
}

bool Hand::isSoft() const {
    int total = 0;
    int aces = 0;

    for (const Card& c : cards) {
        total += cardPoints(c.value);
        if (c.value == Value::Ace) aces++;
    }

    /* An ace is still counted as 11 if we haven't needed to reduce it */
    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }

    return aces > 0; /* at least one ace still contributing 11 */
}

bool Hand::isPair() const {
    return cards.size() == 2 && cards[0].value == cards[1].value;
}

bool Hand::canDoubleDown() const {
    return cards.size() == 2;
}

int Hand::size() const {
    return static_cast<int>(cards.size());
}

const std::vector<Card>& Hand::getCards() const {
    return cards;
}

void Hand::clear() {
    cards.clear();
}
