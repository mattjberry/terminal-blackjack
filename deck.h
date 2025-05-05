/* Created by Matt Berry */

#ifndef DECK_H
#define DECK_H

#include <vector>
#include <random>
#include <chrono>
#include "card.h"

/* Represents a deck of standard playing cards (no jokers) */
class Deck {

  public:
    Deck();
    Card dealCard();

  private:
    std::vector<Card> cards;
};

#endif
