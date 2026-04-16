/* Created by Matt Berry */

#ifndef GAME_H
#define GAME_H

#include <optional>
#include "card/deck.h"
#include "player/player.h"
#include "player/dealer.h"
#include "io/persistence.h"
#include "io/inputHandler.h"
#include "io/outputHandler.h"

/* Top-level game controller — owns all objects and runs the main loop */
class Game {
public:
    Game();
    void run();

private:
    std::optional<Deck> deck;
    Player player;
    Dealer dealer;
    PersistenceManager persistence;
    InputHandler input;

    /* Fresh deck at the start of every round */
    void newDeck();

    /* ── Round stages ── */
    void playRound();
    void dealInitialCards();

    /* Returns true if round should continue (no instant BJ resolution) */
    bool handleEarlyResolution();

    /* Handle insurance / even-money offer. Returns insurance bet placed. */
    void handleInsurance();

    /* Player decision loop for one hand; returns true if hand is active
     * (not bust/surrendered/doubled).  firstAction=true on initial call. */
    void playerTurn(int handIdx, bool fromSplit = false);

    void dealerTurn();
    void resolveRound();

    /* ── Helpers ── */
    void checkEndConditions(); /* win/loss state; may exit */
    Card deal();               /* shorthand: deck->dealCard() */

    /* Dealer quip from a random pool */
    void dealerQuip(const std::string& moment);

    /* Per-hand outcomes (parallel to player.getHandCount()) */
    enum class Outcome { Win, Lose, Push, Blackjack, Surrender };
    std::vector<Outcome> outcomes;
    std::vector<int> payouts;   /* net delta to balance (negative = loss) */
};

#endif
