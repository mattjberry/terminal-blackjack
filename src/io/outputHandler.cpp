/* Created by Matt Berry */

#include "outputHandler.h"
#include <iostream>
#include <iomanip>

/* ── ANSI helpers ── */

std::string OutputHandler::red(const std::string& s) {
    return "\033[31m" + s + "\033[0m";
}

std::string OutputHandler::bold(const std::string& s) {
    return "\033[1m" + s + "\033[0m";
}

std::string OutputHandler::reset() {
    return "\033[0m";
}

/* ── Card rendering ── */

std::string OutputHandler::suitStr(Suit s) {
    switch (s) {
        case Suit::Hearts:   return "\xe2\x99\xa5"; /* ♥ */
        case Suit::Diamonds: return "\xe2\x99\xa6"; /* ♦ */
        case Suit::Clubs:    return "\xe2\x99\xa3"; /* ♣ */
        case Suit::Spades:   return "\xe2\x99\xa0"; /* ♠ */
    }
    return "?";
}

std::string OutputHandler::valueStr(Value v) {
    switch (v) {
        case Value::Ace:   return "A";
        case Value::Two:   return "2";
        case Value::Three: return "3";
        case Value::Four:  return "4";
        case Value::Five:  return "5";
        case Value::Six:   return "6";
        case Value::Seven: return "7";
        case Value::Eight: return "8";
        case Value::Nine:  return "9";
        case Value::Ten:   return "10";
        case Value::Jack:  return "J";
        case Value::Queen: return "Q";
        case Value::King:  return "K";
    }
    return "?";
}

std::string OutputHandler::cardStr(const Card& c, bool hidden) {
    if (hidden) return "[?]";
    std::string face = valueStr(c.value) + suitStr(c.suit);
    if (c.suit == Suit::Hearts || c.suit == Suit::Diamonds) {
        return "[" + red(face) + "]";
    }
    return "[" + face + "]";
}

std::string OutputHandler::handStr(const Hand& h) {
    std::string out;
    for (const Card& c : h.getCards()) {
        if (!out.empty()) out += " ";
        out += cardStr(c);
    }
    out += "  = " + std::to_string(h.value());
    if (h.isBlackjack()) out += bold("  BLACKJACK!");
    else if (h.isBust())  out += "  BUST";
    return out;
}

std::string OutputHandler::dealerHandStr(const Dealer& d) {
    const auto& cards = d.getHand().getCards();
    if (cards.empty()) return "(no cards)";

    std::string out;
    for (int i = 0; i < static_cast<int>(cards.size()); i++) {
        if (!out.empty()) out += " ";
        bool hide = (i == 0 && d.isHoleCardHidden());
        out += cardStr(cards[i], hide);
    }

    if (!d.isHoleCardHidden()) {
        out += "  = " + std::to_string(d.getHand().value());
        if (d.getHand().isBust()) out += "  BUST";
    } else {
        /* Show only the up-card value */
        out += "  (showing: " + std::to_string(cardPoints(d.upCard().value)) + ")";
    }
    return out;
}

/* ── Screen layout ── */

void OutputHandler::clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

void OutputHandler::displayWelcome() {
    clearScreen();
    std::cout << "\n";
    std::cout << bold("  \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90") << "\n";
    std::cout << bold("         TERMINAL BLACKJACK") << "\n";
    std::cout << bold("  \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90") << "\n";
    std::cout << "\n";
    std::cout << "  Can you beat the evil blackjack dealer!?\n";
    std::cout << "\n";
    std::cout << "  \"Welcome to my table... if you dare.\"\n";
    std::cout << "\n";
    std::cout << "  Press any key to begin.\n";
    std::cout << std::flush;
}

void OutputHandler::displayTable(const Dealer& dealer,
                                  const Player& player,
                                  int activeHandIdx) {
    clearScreen();
    std::cout << "\n";
    /* Header */
    std::cout << bold("  ══════════════════════════════════════════════════") << "\n";
    std::cout << bold("   TERMINAL BLACKJACK")
              << "   │   Balance: $" << player.getBalance() << "\n";
    std::cout << bold("  ══════════════════════════════════════════════════") << "\n";
    std::cout << "\n";

    /* Dealer */
    std::cout << "  DEALER:  " << dealerHandStr(dealer) << "\n";
    std::cout << "\n";

    /* Player hands */
    for (int i = 0; i < player.getHandCount(); i++) {
        const Hand& h = player.getHand(i);
        std::string label = (player.getHandCount() > 1)
            ? "  HAND " + std::to_string(i + 1) + ":"
            : "  YOUR HAND:";
        if (i == activeHandIdx) label = bold(label + " >");
        else                    label = label + "  ";
        std::cout << label << "  " << handStr(h)
                  << "   Bet: $" << player.getHandBet(i) << "\n";
    }

    std::cout << "\n";
}

void OutputHandler::displayHandResult(int handIdx,
                                       const std::string& result,
                                       int amount) {
    std::string sign = (amount >= 0) ? "+" : "";
    std::cout << "  Hand " << (handIdx + 1) << ": "
              << bold(result)
              << "  (" << sign << amount << ")\n";
}

void OutputHandler::displayGameOver(bool won) {
    clearScreen();
    std::cout << "\n";
    if (won) {
        std::cout << bold("  You have beaten the evil blackjack dealer!") << "\n";
        std::cout << "\n";
        std::cout << "  Remarkable. I'll remember this.\n";
    } else {
        std::cout << bold("  You're broke. The house always wins.") << "\n";
        std::cout << "\n";
        std::cout << "  Better luck next time... if you dare return.\n";
    }
    std::cout << "\n";
}

void OutputHandler::displayDealerDialogue(const std::string& line) {
    std::cout << "\n  \"" << line << "\"\n\n";
}

void OutputHandler::displayPlayAgainPrompt() {
    std::cout << "  Another round? [Y/N]  " << std::flush;
}

void OutputHandler::displayInsurancePrompt(int maxBet) {
    std::cout << "  Dealer shows an Ace. Insurance? (up to $" << maxBet
              << ")  [Y/N]  " << std::flush;
}

void OutputHandler::displayEvenMoneyPrompt() {
    std::cout << "  You have Blackjack! Take even money (1:1) instead of 3:2?  [Y/N]  "
              << std::flush;
}

void OutputHandler::displayBrokePrompt() {
    std::cout << "\n  You're running low. Reset balance to $500?  [Y/N]  "
              << std::flush;
}
