/* Created by Matt Berry */

#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <termios.h>
#include <vector>

/* RAII wrapper for raw terminal mode.
 * Saves terminal state on construction, restores on destruction.
 * All player input flows through this class. */
class InputHandler {
public:
    InputHandler();
    ~InputHandler();

    /* Read a single character; returns it uppercased */
    char readKey();

    /* Interactive bet selector.
     * Displays the current bet inline. Press - to decrease, +/= to increase,
     * Enter to confirm. Returns the chosen bet amount. */
    int selectBet(int balance);

private:
    struct termios origSettings;
    bool rawModeActive;

    void setRawMode();
    void restoreMode();

    /* Read a raw byte without upper-casing (used internally) */
    char readRaw();
};

#endif
