/* Created by Matt Berry */

#include "card.h"

/* Initialize card 
 * Pretty much the only method we need here */
Card::Card(Value v, Suit s)
  : value(v)
  , suit(s)     
{}

// Really not sure how I feel about this syntax
