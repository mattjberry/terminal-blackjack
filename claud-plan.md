# Terminal Blackjack — Implementation Plan

## Context

The goal is a fully-featured single-player terminal blackjack game in C++. The card and deck infrastructure already exists; the plan covers all the missing layers needed to turn that foundation into a playable game. The "evil blackjack dealer" framing is intentional flavour — rules should slightly favour the house (e.g., dealer hits soft 17).

---

## Recommended File / Directory Structure

All new files sit alongside the existing ones in the project root (flat layout, consistent with current Makefile pattern):

```
TerminalBlackjack/
├── Makefile                ← update SRCS list
├── README.md
├── suit.h                  ← existing, unchanged
├── value.h                 ← existing, unchanged
├── card.h / card.cpp       ← existing, unchanged
├── deck.h / deck.cpp       ← existing, unchanged
├── hand.h / hand.cpp       ← NEW
├── player.h / player.cpp   ← NEW
├── dealer.h / dealer.cpp   ← NEW
├── game.h / game.cpp       ← NEW (main game loop)
├── persistence.h / persistence.cpp  ← NEW
├── inputHandler.h / inputHandler.cpp   ← implement (scaffolded)
├── outputHandler.h / outputHandler.cpp ← implement (scaffolded)
├── main.h                  ← minor update to include new headers
└── main.cpp                ← replace stub with Game instantiation
```

**Makefile change** — add new source files to `SRCS`:
```makefile
SRCS= main.cpp card.cpp deck.cpp hand.cpp player.cpp dealer.cpp \
      game.cpp persistence.cpp inputHandler.cpp outputHandler.cpp
```

---

## Class / Module Breakdown

### `Hand` (hand.h / hand.cpp)
Single hand of cards — shared by both player and dealer.

| Method | Purpose |
|--------|---------|
| `void addCard(Card)` | Append a card |
| `int value() const` | Best value ≤ 21; if all options bust, return smallest bust value |
| `bool isBlackjack() const` | Exactly 2 cards and value == 21 |
| `bool isBust() const` | value() > 21 |
| `bool isSoft() const` | Has an ace currently counted as 11 |
| `bool isPair() const` | Exactly 2 cards of the same face `Value` (split eligibility) |
| `bool canDoubleDown() const` | Exactly 2 cards |
| `int size() const` | Number of cards |
| `const vector<Card>& cards() const` | Read-only access for rendering |

**Ace logic in `value()`**: count all aces as 11, then for each ace convert it to 1 until total ≤ 21 or all aces are at 1. `isSoft()` returns true if at least one ace is still contributing 11.

**Value mapping** (important — existing `Value` enum assigns sequential ints, not blackjack point values):
- `Ace` → 11 (or 1 as needed)
- `Two`–`Ten` → face value (enum int matches point value for Two–Ten)
- `Jack`, `Queen`, `King` → 10

---

### `Player` (player.h / player.cpp)
Holds the player's balance and one or more hands (splits create extra hands).

| Member/Method | Purpose |
|--------------|---------|
| `int getBalance() / setBalance()` | Cash balance |
| `void addHand(int bet)` | Add a new hand (for splits) |
| `Hand& getHand(int idx)` | Access hand by index |
| `int getHandBet(int idx)` | Bet for a specific hand |
| `void setHandBet(int idx, int b)` | Update per-hand bet (after double) |
| `int getHandCount()` | Number of active hands |
| `void clearHands()` | Reset between rounds |
| `bool handIsSplitAce(int idx)` | Flag set when a split aces hand gets only 1 card |

---

### `Dealer` (dealer.h / dealer.cpp)
Dealer hand with hole-card visibility toggle and AI hit logic.

| Member/Method | Purpose |
|--------------|---------|
| `void receiveCard(Card, bool faceDown)` | Add card; hole card is face-down |
| `Hand& getHand()` | Access dealer hand |
| `bool shouldHit() const` | True if value ≤ 16, **or** if hand is soft 17 |
| `void revealHoleCard()` | Flip hole card face-up |
| `bool holeCardVisible() const` | Used by OutputHandler |

**Soft 17 rule** → **Dealer hits on soft 17.** Rationale: consistent with modern casino standard and the "evil dealer" theme (slight house edge vs. standing on soft 17).

---

### `InputHandler` (inputHandler.h / inputHandler.cpp)
RAII wrapper around POSIX `termios` raw mode. macOS/Linux only (`unistd.h`, `termios.h`).

| Method | Purpose |
|--------|---------|
| Constructor | Save terminal state, enable raw mode (`~ECHO \| ~ICANON`) |
| Destructor | Restore terminal state |
| `char readKey()` | Block until a single byte, return it (uppercased for uniformity) |
| `ArrowKey readArrowOrEnter()` | Read full escape sequence for arrow-key bet selector |
| `int selectBet(int balance)` | Interactive arrow-key bet picker (see below) |

**Arrow key protocol**: Arrow keys send 3-byte sequences `\033[A/B/C/D`. After seeing `\033`, read 2 more bytes with a short timeout. If it's a `[A` or `[B`, return Up/Down; otherwise treat as regular key. Enter/Return (`\r` or `\n`) confirms.

**Bet selector**: Displays a single line:
```
  Bet: $160   [↑/↓ to change, Enter to confirm]
```
Available amounts: $10 × powers of 2 that don't exceed `balance`. Up arrow advances to next amount; Down arrow goes back. Redraws in place using `\r`.

**Key map**:
| Key | Action |
|-----|--------|
| H | Hit |
| S | Stand |
| D | Double down |
| P | Split |
| Q | Surrender |
| I | Insurance (when prompted) |
| Y / N | Yes / No confirmations |
| Enter | Confirm bet |

---

### `OutputHandler` (outputHandler.h / outputHandler.cpp)
All display logic. Uses ANSI escape codes for colour and cursor control.

| Method | Purpose |
|--------|---------|
| `static void clearScreen()` | `\033[2J\033[H` |
| `static void displayWelcome()` | Launch message + flavour |
| `static void displayTable(Dealer&, Player&, int activeHandIdx)` | Full round view |
| `static void displayResult(string msg)` | Win/loss/push line per hand |
| `static void displayGameOver(bool won)` | Win/loss final screen |
| `static void displayDealerDialogue(string)` | Flavour text line |
| `static string cardStr(Card, bool hidden)` | e.g. `[A♠]` or `[?]` |
| `static string handStr(Hand&)` | Space-separated cards + `= N` |

**Card colours**: ♥ ♦ → red (`\033[31m`), ♣ ♠ → reset (`\033[0m`). Hidden card renders as `[?]`.

**Table layout**:
```
══════════════════════════════════════
  TERMINAL BLACKJACK  │  Balance: $480
══════════════════════════════════════

  DEALER: [?] [7♣]   (showing: 7)

  YOUR HAND: [A♠] [K♥]  = 21  ← BLACKJACK!

  Bet: $20   Pot: $20

  "So... you think you can beat me?"

  [H]it  [S]tand  [D]ouble  [Q]uit hand
```

---

### `PersistenceManager` (persistence.h / persistence.cpp)
**Recommended approach: flat text file** (not Docker volume).

Docker volumes are appropriate for containerized services, not standalone CLI binaries. A flat file has zero dependencies, survives reboots, works offline, and is trivially portable.

**Save location**: `~/.terminal_blackjack/save.dat`  
**Format**: One key=value per line:
```
balance=500
```
**Implementation**:
- `std::string getSavePath()` — joins `getenv("HOME")` + `"/.terminal_blackjack/save.dat"`
- On `loadBalance()`: open file; if missing, return 500; parse `balance=N`
- On `saveBalance(int b)`: `mkdir -p` the directory (via `mkdir()` syscall with mode 0700), then `std::ofstream` overwrite
- Thread safety: irrelevant (single-player offline)

---

### `Game` (game.h / game.cpp)
Orchestrates everything; holds all the above as members.

```cpp
class Game {
    Deck deck;
    Player player;
    Dealer dealer;
    PersistenceManager persistence;
    InputHandler input;
    OutputHandler output;
public:
    void run();
private:
    void playRound();
    void dealInitialCards();
    void handleInsurance();
    bool playerTurn(int handIdx);     // returns true if hand can still be played
    void dealerTurn();
    void resolveRound();
    void checkEndConditions();        // $100k win / < $10 loss
};
```

---

## Game Loop Flow

```
run()
  │
  ├─ displayWelcome()   "Can you beat the evil blackjack dealer!?"
  ├─ player.setBalance(persistence.loadBalance())
  │
  └─ loop:
       checkEndConditions()
         ├─ balance ≥ $100,000 → displayGameOver(won=true) → exit
         └─ balance < $10 → offer reset to $500 or quit
       
       displayTable(empty state)
       bet = input.selectBet(player.getBalance())
       player.clearHands()
       player.addHand(bet)
       dealer.reset()
       deck = Deck()                  ← fresh shuffle every round
       
       dealInitialCards()
         ├─ player gets 2 cards (face up)
         └─ dealer gets 2 cards (1 up, 1 hidden)
       
       [Insurance check]
       if dealer up-card is Ace:
         if player has Blackjack → offer "even money" (1:1) → collect & skip to next round
         else → offer insurance side-bet (up to bet/2); store aside
       
       if dealer has Blackjack:
         reveal hole card
         resolve insurance bets (pay 2:1 to insured players)
         if player also has Blackjack → push
         else → player loses (insurance may soften loss)
         → skip player/dealer turns
       
       if player has Blackjack (dealer does not):
         pay 3:2, round over
       
       else: normal round
         for each player hand (handIdx 0..N):
           playerTurn(handIdx):
             loop:
               displayTable()
               available actions depend on state:
                 always: [H]it, [S]tand
                 if canDoubleDown: [D]ouble
                 if isPair and splitCount < 3: [P]plit
                 if first action of hand: [Q]surrender
               
               process action:
                 Hit    → addCard; if bust → end hand
                 Stand  → end hand
                 Double → charge extra bet, deal 1 card, end hand
                 Split  → move 1 card to new hand, deal 1 card to each,
                           mark as split-ace if applicable,
                           recurse into each sub-hand
                 Surrender → refund bet/2, mark surrendered, end hand
         
         dealerTurn():
           reveal hole card
           while dealer.shouldHit(): deal card
         
         resolveRound():
           for each non-surrendered hand:
             if player bust    → lose bet
             if dealer bust    → win bet (1:1)
             if player > dealer → win bet (1:1)
             if player < dealer → lose bet
             if equal          → push (return bet)
           settle insurance bets
       
       persistence.saveBalance(player.getBalance())
       displayResults()
       prompt "Another round? [Y/N]"
```

---

## Persistence Design

| Field | Type | Notes |
|-------|------|-------|
| `balance` | int | Player's cash in dollars |

- **File**: `~/.terminal_blackjack/save.dat`
- **Read**: once on startup; missing file → default $500
- **Write**: immediately after every round resolves (before displaying result screen)
- **Format**: `balance=500\n` — trivial to parse with `getline` + `find('=')`
- **Directory creation**: `mkdir(path, 0700)` — silently succeeds if dir already exists

No external libraries (no JSON, no SQLite). The format is human-readable if the player wants to inspect or manually edit their balance.

---

## Split Rules Summary

| Rule | Decision |
|------|---------|
| Split eligibility | Two cards of the **same** `Value` enum (Jack ≠ Queen even though both are worth 10) |
| Max splits | 3 (4 hands total) |
| Split Aces | Each new hand receives exactly 1 card; no further hit/double/split allowed |
| Re-split Aces | Not allowed |
| Double after split | Allowed (except on split Aces) |
| Blackjack after split | Pays 1:1 (not 3:2) — standard rule |

---

## Input Design Summary

- **Raw mode**: `termios` RAII class in `InputHandler` constructor/destructor
- **Single-key**: all game decisions read with one keystroke, no Enter required
- **Case-insensitive**: `toupper()` applied to every read byte
- **Arrow key bet selector**: reads 3-byte escape sequence; falls back gracefully if not an arrow key
- **Bet amounts**: `$10 × 2^n` sequence up to balance — `$10, $20, $40, $80, $160, $320, $640 …`

---

## Flavour / Dealer Dialogue (Nice-to-Have)

Implement as a static array of dealer quips in `OutputHandler`. One is selected randomly (seeded RNG) at key moments:
- Round start: *"Welcome to my table… if you dare."*
- Player busts: *"Too greedy. The house always wins."*
- Player wins: *"Beginner's luck. It won't last."*
- Blackjack: *"...I'll remember that."*
- Player hits $100k: *"You have beaten the evil blackjack dealer!"*

---

## Open Questions / Risks

| # | Risk / Ambiguity | Recommendation |
|---|-----------------|----------------|
| 1 | **`Value` enum ints don't match blackjack point values** — `Jack=11`, `Queen=12`, `King=13` in the enum, but all worth 10 in blackjack. | Handle in `Hand::value()` with a helper `cardPoints(Value)` that maps Jack/Queen/King → 10. Do not change the enum. |
| 2 | **Split eligibility — same Value or same point value?** Standard casino rule is same face value (Jack ≠ Queen), but some allow any two 10-value cards. | Same `Value` enum value only. Simpler, standard, and avoids ambiguity. |
| 3 | **`Deck` is constructed once in `Game`.** To reshuffle per round, `Game` must re-create the `Deck` object each round. | Declare `deck` as `std::optional<Deck>` or a pointer, reset it at the top of `playRound()`. |
| 4 | **`main.h` uses `using namespace std` in a header** — not ideal practice. | Leave unchanged for now (existing file); avoid adding more `using namespace` statements in new headers. |
| 5 | **Arrow-key support on non-ANSI terminals** — rare but possible. | If escape sequence is malformed, fall back to treating input as a regular key and ignore the arrow attempt. |
| 6 | **Insurance side-bet UX** — needs a distinct prompt and clear accounting. | Show a dedicated insurance prompt line; store as a separate `int insuranceBet` on `Player`. |
| 7 | **Windows portability** | Not in scope. `termios` is POSIX only. Note this in README. |
| 8 | **Bet exceeds balance edge case** | Clamp bet selector to never exceed `player.getBalance()`. Also validate before charging double-down/split. |
