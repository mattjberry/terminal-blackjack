# Prompts and Design Decisions

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

**Reworked**
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

