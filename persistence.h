/* Created by Matt Berry */

#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <string>

/* Loads and saves the player's balance to ~/.terminal_blackjack/save.dat */
class PersistenceManager {
public:
    PersistenceManager();

    /* Returns saved balance, or 500 if no save file exists */
    int loadBalance() const;

    /* Overwrites save file with current balance */
    void saveBalance(int balance) const;

    bool saveExists() const;

private:
    std::string savePath;

    /* Builds path from $HOME; creates directory if needed on first save */
    std::string buildSavePath() const;
    void ensureDirectoryExists() const;
};

#endif
