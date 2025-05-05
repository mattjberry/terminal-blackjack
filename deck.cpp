/* Created by Matt Berry */

#include "deck.h"

/* Builds a deck of cards and shuffles it */
Deck::Deck() {
  // Array representations of both enums so we can iterate over
  constexpr Suit suitsArr[] = {
    Suit::Clubs,
    Suit::Diamonds,
    Suit::Hearts,
    Suit::Spades
  };

  constexpr Value valuesArr[] = {
    Value::Ace,
    Value::Two,
    Value::Three,
    Value::Four,
    Value::Five,
    Value::Six,
    Value::Seven,
    Value::Eight,
    Value::Nine,
    Value::Ten,
    Value::Jack,
    Value::Queen,
    Value::King
  };

  // build deck
  for (Suit s : suitsArr) {
    for (Value v : valuesArr) {
      this->cards.emplace_back(v, s);
    }
  }

  // generate random seed using clock time
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 rng(seed);    // silly name but ok
  /* Since there is no reason for the deck to be rebuilt within the same
   * millisecond there is very little chance of a duplicated shuffle pattern */

  // shuffle
  std::shuffle(this->cards.begin(), this->cards.end(), seed);
}

/* Deal one card, removing from the deck
 * always goes from back of vector for simplicity */
Card Deck::dealCard() {
  Card top = this->cards.back();
  this->cards.pop_back();
  return top;
}
