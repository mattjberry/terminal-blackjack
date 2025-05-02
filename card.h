/* Created by Matt Berry */

#ifndef CARD_H
#define CARD_H

#include "suit.h"
#include "value.h"

/* Represents a single playing card */
class Card {

public:
  // Constructor
  Card(Value v, Suit s);

  // Face Value
  Value value;
  // Suit
  Suit suit;
};

#endif
