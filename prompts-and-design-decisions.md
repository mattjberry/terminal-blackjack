# Prompts and Design Decisions

This document includes all prompts used in generating this project and their output where applicable. All prompts that heavily modified code map to their own pull request. Some prompts were purely imformational.

## Initial Prompt
**Original**
I want to create a simple offline blackjack simulation game where players can play against a virtual dealer from their computers terminal. I have attached a pdf detailing the rules and basic flow of the game. 

Each round, the player should be able to chose their bet size, with a minimum bet of 10$, increasing a rate of 2x. Then the player and the dealer will receive their cards. The player than has the option to hit or stand, as well as split or double down where applicable. Decisions are made via single key text entry. Ie. hit = H, upper or lower case. Players start with $500. If a player reaches a cash value greater or equal to $100,000 they have “beaten the dealer”. 

_Requirements_
1. Cards are dealt randomly from a deck that is refreshed at the beginning of each round.
2. User selections are made via single key text input.
3. Player cash value carries over from round to round.
4. Player cash value carries over on subsequent executions of the game.
5. If a player has less than $10 they have lost and can reset to $500.
6. If a player exceeds $100,000 they have won the game.
7. All rules of the game are observed (ex. Dealer stands when >= 17).
8. When the game launches, players are challenged with “Can you beat the evil blackjack dealer!?”

_Flexibles_
1. Exact terminal outputs
2. How the cash score persists between runs (Database vs Docker)

_Nice To Haves_
1. Bet size selection using arrow keys
2. Text prompts and dealer dialog should have an air of silliness to them. 

Originally I planned to write this game myself in C++. I haven’t had the time to do that so I decided to turn this project into an experiment with AI driven development. I have some existing code written in C++ as well as a Makefile. I’d like to continue to use C++ even if it isn’t maybe the most intuitive language for this use case. I think that the best approach for saving a user score between runs of the game would be a docker volume, but that approach is flexible. 

**Reworked - Optimized by Claude for use in Claude Code**
PLAN ONLY — do not write any code yet.
Before writing a single line, produce a detailed implementation plan for the project described below. The plan should cover: project structure, class/module breakdown, data flow, persistence approach, third-party dependencies (if any), and any decisions made on the "flexibles" listed below. Flag any ambiguities or risks.

Project: Terminal Blackjack in C++
Build a single-player terminal blackjack game playable offline from a CLI. The player competes against a virtual dealer. A PDF of the full rules is attached — all rules must be observed.

_Hard Constraints_
* Language: C++ (existing code and a Makefile are already in place — the plan must account for integrating with them, not replacing them)
* Persistence: Player cash balance must survive both round-to-round and between separate executions of the binary. Preferred approach is a Docker volume, but this is open for the plan to evaluate and recommend
* Input: All player decisions use single-key entry (H = hit, S = stand, D = double down, P = split, etc.) — case-insensitive. Nice-to-have: bet selection via arrow keys
* Minimum bet: $10, with bet sizes scaling at 2× increments (e.g. $10, $20, $40…)
* Starting balance: $500

_Win/Loss Conditions_
* Win: Balance ≥ $100,000 → display "You have beaten the evil blackjack dealer!"
* Loss: Balance < $10 → offer reset to $500
* Launch message: "Can you beat the evil blackjack dealer!?"

_Functional Requirements (all non-negotiable)_
1. Deck is reshuffled fresh at the start of every round
2. Player balance persists across rounds and across separate runs of the game
3. All blackjack rules are enforced (dealer stands on hard 17+, soft 17 behaviour should be defined in the plan, split/double down eligibility, blackjack pays 3:2, insurance if applicable per the PDF)
4. Game launches with the challenge message above

_Flexibles (plan should make a recommendation for each)_
* Exact terminal output formatting and layout
* Persistence mechanism: Docker volume vs. flat file vs. other — evaluate trade-offs given C++ and offline usage

_Nice-to-Haves (implement if straightforward, otherwise note as future work)_
* Bet selection using arrow keys / interactive selector
* Silly, flavourful dealer dialogue and text prompts throughout

_Deliver the plan as:_
1. Recommended file/directory structure
2. Class or module breakdown with responsibilities
3. Game loop flow (round lifecycle, state transitions)
4. Persistence design (what is stored, where, format, how it's read/written)
5. Input handling design (especially arrow key consideration)
6. Open questions or risks to flag before implementation begins

**Output - See claude-plan.md**
Notable design decisions applied:
* Dealer hits on soft 17
* Insurance peek uses peekForBlackjack() — hole card stays visually hidden during player turn
* Even money offer shown when player has BJ and dealer shows Ace
* Bet amounts: $10 × 2ⁿ up to balance, navigable with ↑/↓ arrows
* Balance persisted after every round to ~/.terminal_blackjack/save.dat

**Refactoring**
Directory groupings:
Directory	Contents	Rationale
src/card/	suit, value, card, deck, hand	All pure card-data types — no game logic, no I/O
src/player/	player, dealer	Both model a participant holding a hand
src/game/	game	Isolated orchestration layer
src/io/	inputHandler, outputHandler, persistence	All external interaction (terminal + disk)
src/ root	main.h, main.cpp	Entry point
Include strategy: GCC searches the including file's own directory first before checking -I paths, so same-directory includes (card.cpp → "card.h") required no changes. Only the 5 headers with cross-directory dependencies needed updating. -I. became -Isrc as the single root.
Makefile pattern rule: $(patsubst src/%.cpp, build/%.o, ...) means build/ automatically mirrors src/ — build/card/hand.o, build/io/persistence.o, etc. The @mkdir -p $(dir $@) creates whatever subdirs are needed at compile time.

**Arrow Handling Bug Fix**
Nice, refactor/restructure-src-dirs branch  has been merged and deleted. I've found a bug with handling of the arrow keys. Bug appears both in the standard Mac terminal and in the Warp app terminal. Arrow keys simply don't register an input and the bet stays at $10. Check the existing code for a bug that may be handling this incorrectly, otherwise we may have to change the handling to other keys. My first choice for an alternative is '-' and '+', where we also interpret '=' as a '+' to avoid having to press shift. Create a new branch from main to address this issue.

**Output**
Found it. There are two bugs in readEscapeSequence():

Line 54 — root cause: tcsetattr(STDIN_FILENO, TCSAFLUSH, &timed) uses TCSAFLUSH, which discards all pending input at the moment it's called. By the time readEscapeSequence runs, the [A bytes from the arrow key are already sitting in the kernel buffer — TCSAFLUSH throws them away before we can read them.
Lines 58/62/66 — secondary: setRawMode() is called as a "restore" but it starts with tcgetattr(STDIN_FILENO, &origSettings), overwriting the saved original terminal settings with whatever is current (the timed settings). The destructor would then restore the wrong state.
Even if we patched both (swap TCSAFLUSH → TCSANOW, store raw settings separately), escape sequence handling stays fragile — a bare Escape keypress would block the input loop waiting for bytes that never arrive. The -/+/= approach sidesteps all of it. Implementing that now.

**Improving Output Variations**
Before we close this branch, I want to add a couple more things. For each of the dealer dialogs, add more possible options matching their respective circumstances, for example:

* When choosing the bet size, the dealer always says "The odds are never in your favour." Add three more possible dialog options with varying levels of optimism. 
This will help the game feel more alive.

**Output**
Done. Here's what was added across all five pools:

quipsStart (bet selection) — +6, grouped by tone:

Pessimistic: "Every chip you place is a chip closer to ruin." / "Place your bet. The outcome is already decided." / "I've seen that look before. It doesn't end well."
Almost encouraging: "A bold wager. I respect that... for now." / "Everyone wins their first round or two." / "Perhaps today is your day. Stranger things have happened."
quipsBust — +3: "And just like that, it's over." / "You were so close. Then you weren't." / "The cards don't lie."

quipsPlayerWin — +3: "Take your winnings. While you still have them." / "Don't smile too wide. The night is young." / "Luck favours the foolish. For now."

quipsBlackjack — +3: "Twenty-one on the deal. You got lucky." / "Charming. Truly." / "I've seen better hands lose everything."

quipsDealerBust — +3: "The deck betrayed me. It won't happen again." / "Consider yourself fortunate." / "A minor inconvenience."

Ready to open the PR whenever you are.

**Consistent Player Options**
Just a small tweak I'd like to make,  on a new branch. Currently the player options display as [H]it [S]tand [D]ouble Down, then [P]split and [Q]surrender. This looks a bit awkward. Can you change it to [P] - Split and [Q] - Surrender

**Concerning Windows Compatibility**
Since we removed the arrow key handling, is there anything preventing this project from running on Windows as well as the already supported mac and linux?

**Output**
No — removing the arrow keys removed the escape-sequence parsing but the two POSIX-only dependencies that were always the real blockers are still there:

1. inputHandler — termios / unistd.h (src/io/inputHandler.cpp)

Raw single-key input (no Enter required, no echo) is implemented via tcgetattr/tcsetattr and read(STDIN_FILENO, ...). None of those exist on Windows. The Windows equivalent is _getch() from <conio.h>, or GetConsoleMode()/SetConsoleMode() from the Win32 console API.

2. persistence — sys/stat.h, two-argument mkdir, HOME (src/io/persistence.cpp)

mkdir(path, 0700) is the POSIX two-argument form. Windows only has _mkdir(path) (one argument, from <direct.h>).
getenv("HOME") is not set by default on Windows — the equivalent is USERPROFILE or APPDATA.
So the README restriction stays accurate. Making the game portable to Windows would require conditional compilation (#ifdef _WIN32) in both of those files — doable, but not trivial. The termios swap is the bigger of the two since it touches every keystroke.
