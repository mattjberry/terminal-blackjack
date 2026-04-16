/* Created by Matt Berry */

#include "inputHandler.h"
#include <unistd.h>
#include <cctype>
#include <cstdio>
#include <iostream>

InputHandler::InputHandler() : rawModeActive(false) {
    setRawMode();
}

InputHandler::~InputHandler() {
    restoreMode();
}

void InputHandler::setRawMode() {
    tcgetattr(STDIN_FILENO, &origSettings);
    struct termios raw = origSettings;
    /* Disable canonical mode and echo */
    raw.c_lflag &= ~(ICANON | ECHO);
    /* Return immediately after 1 byte */
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    rawModeActive = true;
}

void InputHandler::restoreMode() {
    if (rawModeActive) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &origSettings);
        rawModeActive = false;
    }
}

char InputHandler::readRaw() {
    char c = 0;
    if (read(STDIN_FILENO, &c, 1) < 0) c = 0;
    return c;
}

char InputHandler::readKey() {
    char c = readRaw();
    return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}

char InputHandler::readEscapeSequence() {
    /* Set a short timeout so we don't block forever waiting for
     * the rest of an escape sequence */
    struct termios timed = origSettings;
    timed.c_lflag &= ~(ICANON | ECHO);
    timed.c_cc[VMIN]  = 0;
    timed.c_cc[VTIME] = 1; /* 0.1 second */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &timed);

    char seq[2] = {0, 0};
    if (read(STDIN_FILENO, &seq[0], 1) != 1) {
        setRawMode();
        return '\0';
    }
    if (read(STDIN_FILENO, &seq[1], 1) != 1) {
        setRawMode();
        return '\0';
    }

    setRawMode();

    if (seq[0] == '[') {
        if (seq[1] == 'A') return 'U'; /* up arrow   */
        if (seq[1] == 'B') return 'D'; /* down arrow */
    }
    return '\0';
}

int InputHandler::selectBet(int balance) {
    /* Build list: $10 * 2^n capped at balance */
    std::vector<int> bets;
    for (int b = 10; b <= balance; b *= 2) {
        bets.push_back(b);
    }
    if (bets.empty()) bets.push_back(balance);

    int idx = 0;

    auto draw = [&]() {
        /* UTF-8 up/down arrows ↑↓ */
        std::cout << "\r  Bet: $" << bets[idx]
                  << "   [\xe2\x86\x91/\xe2\x86\x93 to change, Enter to confirm]   "
                  << std::flush;
    };

    draw();

    while (true) {
        char c = readRaw();

        if (c == '\r' || c == '\n') {
            std::cout << std::endl;
            return bets[idx];
        }

        if (c == '\033') {
            char arrow = readEscapeSequence();
            if (arrow == 'U' && idx < static_cast<int>(bets.size()) - 1) {
                idx++;
                draw();
            } else if (arrow == 'D' && idx > 0) {
                idx--;
                draw();
            }
        }
    }
}
