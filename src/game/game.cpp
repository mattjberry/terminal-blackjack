/* Created by Matt Berry */

#include "game.h"
#include <iostream>
#include <chrono>
#include <random>
#include <cstdlib>

/* ── Dealer quip pools ── */

static std::mt19937 quipRng(
    static_cast<unsigned>(
        std::chrono::system_clock::now().time_since_epoch().count()));

static const char* quipsStart[] = {
    /* pessimistic */
    "Another victim approaches.",
    "The odds are never in your favour.",
    "Let's see how long you last.",
    "I've never lost. Not once.",
    "Every chip you place is a chip closer to ruin.",
    /* neutral / dry */
    "Place your bet. The outcome is already decided.",
    "I've seen that look before. It doesn't end well.",
    /* almost encouraging */
    "A bold wager. I respect that... for now.",
    "Everyone wins their first round or two.",
    "Perhaps today is your day. Stranger things have happened.",
};

static const char* quipsBust[] = {
    "Too greedy. The house always wins.",
    "Tsk tsk. You should have stopped.",
    "Over 21. What a shame.",
    "Greed is a virtue... for me.",
    "And just like that, it's over.",
    "You were so close. Then you weren't.",
    "The cards don't lie.",
};

static const char* quipsPlayerWin[] = {
    "Beginner's luck. It won't last.",
    "Enjoy it. It's the last time.",
    "I let you win that one.",
    "Interesting.",
    "Take your winnings. While you still have them.",
    "Don't smile too wide. The night is young.",
    "Luck favours the foolish. For now.",
};

static const char* quipsBlackjack[] = {
    "...I'll remember that.",
    "Hmph. Don't get used to it.",
    "Natural 21. Well played... this time.",
    "Twenty-one on the deal. You got lucky.",
    "Charming. Truly.",
    "I've seen better hands lose everything.",
};

static const char* quipsDealerBust[] = {
    "...that was intentional.",
    "I grow tired of this hand.",
    "Even I make mistakes. Rarely.",
    "The deck betrayed me. It won't happen again.",
    "Consider yourself fortunate.",
    "A minor inconvenience.",
};

template<typename T, std::size_t N>
static std::string pickQuip(const T (&pool)[N]) {
    std::uniform_int_distribution<std::size_t> dist(0, N - 1);
    return pool[dist(quipRng)];
}

/* ── Game ── */

Game::Game()
    : player(500) {}

void Game::newDeck() {
    deck.emplace();
}

Card Game::deal() {
    return deck->dealCard();
}

void Game::run() {
    OutputHandler::displayWelcome();
    input.readKey(); /* wait for any key */

    player.setBalance(persistence.loadBalance());

    while (true) {
        checkEndConditions();
        playRound();
    }
}

void Game::checkEndConditions() {
    if (player.getBalance() >= 100000) {
        OutputHandler::displayGameOver(true);
        persistence.saveBalance(player.getBalance());
        std::exit(0);
    }
    if (player.getBalance() < 10) {
        OutputHandler::displayBrokePrompt();
        char c = input.readKey();
        if (c == 'Y') {
            player.setBalance(500);
            persistence.saveBalance(500);
        } else {
            OutputHandler::displayGameOver(false);
            persistence.saveBalance(player.getBalance());
            std::exit(0);
        }
    }
}

void Game::playRound() {
    newDeck();
    player.clearHands();
    dealer.reset();
    outcomes.clear();
    payouts.clear();

    OutputHandler::displayTable(dealer, player, -1);
    dealerQuip("start");

    int bet = input.selectBet(player.getBalance());
    player.setBalance(player.getBalance() - bet);
    player.startRound(bet);

    dealInitialCards();
    OutputHandler::displayTable(dealer, player, 0);

    /* Check for insurance / early resolution */
    if (handleEarlyResolution()) {
        /* Round ended early (blackjack / insurance scenario) */
        persistence.saveBalance(player.getBalance());
        OutputHandler::displayPlayAgainPrompt();
        char c = input.readKey();
        if (c != 'Y') { persistence.saveBalance(player.getBalance()); std::exit(0); }
        return;
    }

    /* Normal play */
    for (int i = 0; i < player.getHandCount(); i++) {
        playerTurn(i);
        OutputHandler::displayTable(dealer, player, -1);
    }

    /* Dealer plays if any hand is still live */
    bool anyLive = false;
    for (int i = 0; i < player.getHandCount(); i++) {
        const Hand& h = player.getHand(i);
        if (!h.isBust() && outcomes.size() <= static_cast<size_t>(i)) {
            anyLive = true;
        }
    }
    if (anyLive) dealerTurn();

    resolveRound();
    persistence.saveBalance(player.getBalance());

    OutputHandler::displayTable(dealer, player, -1);
    /* Print results */
    for (int i = 0; i < static_cast<int>(outcomes.size()); i++) {
        std::string label;
        switch (outcomes[i]) {
            case Outcome::Win:       label = "WIN";       break;
            case Outcome::Lose:      label = "LOSE";      break;
            case Outcome::Push:      label = "PUSH";      break;
            case Outcome::Blackjack: label = "BLACKJACK"; break;
            case Outcome::Surrender: label = "SURRENDER"; break;
        }
        OutputHandler::displayHandResult(i, label, payouts[i]);
    }
    std::cout << "\n  Balance: $" << player.getBalance() << "\n";

    OutputHandler::displayPlayAgainPrompt();
    char c = input.readKey();
    std::cout << "\n";
    if (c != 'Y') { persistence.saveBalance(player.getBalance()); std::exit(0); }
}

void Game::dealInitialCards() {
    /* Deal order matches standard blackjack.
     * Dealer hole card is index 0 (hidden); up-card is index 1 (visible).
     * Dealer::upCard() and dealerHandStr() both expect this layout. */
    player.getHand(0).addCard(deal());
    dealer.receiveCard(deal(), true);   /* hole card — index 0, hidden */
    player.getHand(0).addCard(deal());
    dealer.receiveCard(deal(), false);  /* up-card — index 1, visible */
}

bool Game::handleEarlyResolution() {
    bool dealerShowsAce = (dealer.upCard().value == Value::Ace);
    bool playerBJ = player.getHand(0).isBlackjack();

    if (dealerShowsAce) {
        if (playerBJ) {
            /* Even money offer */
            OutputHandler::displayTable(dealer, player, -1);
            OutputHandler::displayEvenMoneyPrompt();
            char c = input.readKey();
            std::cout << "\n";
            if (c == 'Y') {
                /* 1:1 payout — player already had bet deducted */
                int bet = player.getHandBet(0);
                player.setBalance(player.getBalance() + bet * 2);
                outcomes.push_back(Outcome::Win);
                payouts.push_back(bet);
                dealerQuip("player_win");
                return true;
            }
            /* Declined even money — fall through to check dealer BJ */
        } else {
            handleInsurance();
        }

        /* Peek at dealer hole card without revealing it to the player */
        if (dealer.peekForBlackjack()) {
            dealer.revealHoleCard(); /* now safe to show */
            OutputHandler::displayTable(dealer, player, -1);
            std::cout << "  Dealer has Blackjack!\n";

            /* Resolve insurance */
            int insurancePayout = 0;
            if (player.getInsuranceBet() > 0) {
                /* Return original bet + 2:1 profit (insurance pays 2:1).
                 * The original insurance bet was already deducted when placed,
                 * so we must return it here alongside the winnings. */
                int insOriginal = player.getInsuranceBet();
                insurancePayout = insOriginal + insOriginal * 2;
                player.setBalance(player.getBalance() + insurancePayout);
                player.setInsuranceBet(0);
                std::cout << "  Insurance pays $" << (insOriginal * 2)
                          << " (2:1 on $" << insOriginal << ")\n";
            }

            if (playerBJ) {
                /* Push — return original bet */
                int bet = player.getHandBet(0);
                player.setBalance(player.getBalance() + bet);
                outcomes.push_back(Outcome::Push);
                payouts.push_back(0);
                std::cout << "  Push — both have Blackjack.\n";
            } else {
                /* Player loses — bet was already deducted */
                int bet = player.getHandBet(0);
                outcomes.push_back(Outcome::Lose);
                payouts.push_back(-bet);
            }
            return true;
        }

        /* Dealer doesn't have BJ — insurance lost, hole card stays hidden, play on */
        if (player.getInsuranceBet() > 0) {
            std::cout << "  Dealer does not have Blackjack. Insurance lost.\n";
            player.setInsuranceBet(0);
        }
    } else if (playerBJ) {
        /* Dealer up-card is not an Ace — instant 3:2 payout */
        dealer.revealHoleCard();
        if (!dealer.getHand().isBlackjack()) {
            int bet = player.getHandBet(0);
            int prize = bet + (bet * 3 / 2); /* original bet + 3:2 profit */
            player.setBalance(player.getBalance() + prize);
            outcomes.push_back(Outcome::Blackjack);
            payouts.push_back(bet * 3 / 2);
            OutputHandler::displayTable(dealer, player, -1);
            dealerQuip("blackjack");
            return true;
        }
        /* Both have BJ — push */
        int bet = player.getHandBet(0);
        player.setBalance(player.getBalance() + bet);
        outcomes.push_back(Outcome::Push);
        payouts.push_back(0);
        OutputHandler::displayTable(dealer, player, -1);
        std::cout << "  Push — both have Blackjack.\n";
        return true;
    }

    return false;
}

void Game::handleInsurance() {
    int maxInsurance = player.getHandBet(0) / 2;
    if (maxInsurance < 1 || player.getBalance() < 1) return;

    OutputHandler::displayInsurancePrompt(maxInsurance);
    char c = input.readKey();
    std::cout << "\n";
    if (c == 'Y') {
        int insAmt = maxInsurance;
        if (insAmt > player.getBalance()) insAmt = player.getBalance();
        player.setInsuranceBet(insAmt);
        player.setBalance(player.getBalance() - insAmt);
        std::cout << "  Insurance bet: $" << insAmt << "\n";
    }
}

void Game::playerTurn(int handIdx, bool fromSplit) {
    bool firstAction = true;

    while (true) {
        Hand& h = player.getHand(handIdx);

        if (h.isBust()) {
            dealerQuip("bust");
            return;
        }

        /* Split aces only get one card each */
        if (player.handIsSplitAce(handIdx) && h.size() == 2) {
            return; /* stand automatically */
        }

        OutputHandler::displayTable(dealer, player, handIdx);

        /* Build action menu */
        bool canDouble  = h.canDoubleDown() && player.getBalance() >= player.getHandBet(handIdx);
        bool canSplit   = h.isPair()
                       && player.getSplitCount() < 3
                       && player.getBalance() >= player.getHandBet(handIdx);
        bool canSurrender = firstAction && !fromSplit;

        std::cout << "  Actions: [H]it  [S]tand";
        if (canDouble)    std::cout << "  [D]ouble";
        if (canSplit)     std::cout << "  [P] - Split";
        if (canSurrender) std::cout << "  [Q] - Surrender";
        std::cout << "  " << std::flush;

        char key = input.readKey();
        std::cout << "\n";
        firstAction = false;

        if (key == 'H') {
            h.addCard(deal());
        } else if (key == 'S') {
            return;
        } else if (key == 'D' && canDouble) {
            int extra = player.getHandBet(handIdx);
            player.setBalance(player.getBalance() - extra);
            player.setHandBet(handIdx, extra * 2);
            h.addCard(deal());
            return; /* one card only */
        } else if (key == 'P' && canSplit) {
            bool isAceSplit = (h.getCards()[0].value == Value::Ace);
            int newIdx = player.splitHand(handIdx);

            /* Each split hand gets one new card */
            player.getHand(handIdx).addCard(deal());
            player.getHand(newIdx).addCard(deal());

            /* Charge the extra bet */
            player.setBalance(player.getBalance() - player.getHandBet(newIdx));

            if (isAceSplit) {
                player.markSplitAce(handIdx);
                player.markSplitAce(newIdx);
            }

            /* Play each sub-hand */
            playerTurn(handIdx, true);
            playerTurn(newIdx, true);
            return; /* splits handled recursively */
        } else if (key == 'Q' && canSurrender) {
            int bet = player.getHandBet(handIdx);
            int refund = bet / 2;
            player.setBalance(player.getBalance() + refund);
            outcomes.resize(std::max(static_cast<int>(outcomes.size()), handIdx + 1), Outcome::Lose);
            payouts.resize(std::max(static_cast<int>(payouts.size()), handIdx + 1), 0);
            outcomes[handIdx] = Outcome::Surrender;
            payouts[handIdx]  = -(bet - refund);
            return;
        }
        /* Unrecognised key: re-prompt */
    }
}

void Game::dealerTurn() {
    dealer.revealHoleCard();
    OutputHandler::displayTable(dealer, player, -1);

    while (dealer.shouldHit()) {
        dealer.getHand().addCard(deal());
        OutputHandler::displayTable(dealer, player, -1);
    }

    if (dealer.getHand().isBust()) {
        dealerQuip("dealer_bust");
    }
}

void Game::resolveRound() {
    int dealerVal = dealer.getHand().value();
    bool dealerBust = dealer.getHand().isBust();

    for (int i = 0; i < player.getHandCount(); i++) {
        /* Skip hands already resolved (surrender, early BJ) */
        if (i < static_cast<int>(outcomes.size())) continue;

        const Hand& h = player.getHand(i);
        int bet = player.getHandBet(i);
        int playerVal = h.value();

        outcomes.push_back(Outcome::Lose);
        payouts.push_back(-bet);

        if (h.isBust()) {
            /* Already a loss — bet was deducted at start */
        } else if (dealerBust || playerVal > dealerVal) {
            player.setBalance(player.getBalance() + bet * 2);
            outcomes.back() = Outcome::Win;
            payouts.back()  = bet;
        } else if (playerVal == dealerVal) {
            player.setBalance(player.getBalance() + bet);
            outcomes.back() = Outcome::Push;
            payouts.back()  = 0;
        }
        /* else player loses — balance already reduced at bet time */
    }
}

void Game::dealerQuip(const std::string& moment) {
    std::string line;
    if (moment == "start")       line = pickQuip(quipsStart);
    else if (moment == "bust")   line = pickQuip(quipsBust);
    else if (moment == "player_win") line = pickQuip(quipsPlayerWin);
    else if (moment == "blackjack")  line = pickQuip(quipsBlackjack);
    else if (moment == "dealer_bust") line = pickQuip(quipsDealerBust);
    else return;
    OutputHandler::displayDealerDialogue(line);
}
