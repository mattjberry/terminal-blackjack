/* Created by Matt Berry */

#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H

#include <string>
#include "hand.h"
#include "player.h"
#include "dealer.h"

/* All display logic — uses ANSI escape codes for colour.
 * Methods are static; no state needed. */
class OutputHandler {
public:
    static void clearScreen();
    static void displayWelcome();

    /* Full table view during a round.
     * activeHandIdx: which player hand is currently being played (-1 = none) */
    static void displayTable(const Dealer& dealer,
                             const Player& player,
                             int activeHandIdx);

    /* Show the outcome of a hand after resolution */
    static void displayHandResult(int handIdx, const std::string& result,
                                  int amount);

    /* Win/loss end-game screen */
    static void displayGameOver(bool won);

    /* Print a dealer quip */
    static void displayDealerDialogue(const std::string& line);

    /* Prompt for another round */
    static void displayPlayAgainPrompt();

    /* Prompt for insurance */
    static void displayInsurancePrompt(int maxBet);

    /* Prompt for even money offer */
    static void displayEvenMoneyPrompt();

    /* Prompt when balance is too low */
    static void displayBrokePrompt();

    /* ── Card / hand helpers ── */

    /* e.g. "A♠" or "?" when hidden */
    static std::string cardStr(const Card& c, bool hidden = false);

    /* Space-separated cards followed by " = N" */
    static std::string handStr(const Hand& h);

    /* Dealer's visible hand: hole card as [?], then up-card */
    static std::string dealerHandStr(const Dealer& d);

private:
    /* ANSI colour helpers */
    static std::string red(const std::string& s);
    static std::string bold(const std::string& s);
    static std::string reset();

    static std::string suitStr(Suit s);
    static std::string valueStr(Value v);
};

#endif
