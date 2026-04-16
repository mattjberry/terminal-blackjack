/* Created by Matt Berry */

#include "persistence.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <sys/stat.h>

static const int DEFAULT_BALANCE = 500;

PersistenceManager::PersistenceManager()
    : savePath(buildSavePath()) {}

std::string PersistenceManager::buildSavePath() const {
    const char* home = std::getenv("HOME");
    std::string base = (home != nullptr) ? std::string(home) : "/tmp";
    return base + "/.terminal_blackjack/save.dat";
}

void PersistenceManager::ensureDirectoryExists() const {
    const char* home = std::getenv("HOME");
    std::string dir = (home != nullptr) ? std::string(home) : "/tmp";
    dir += "/.terminal_blackjack";
    /* mkdir returns -1 if directory already exists, which is fine */
    mkdir(dir.c_str(), 0700);
}

bool PersistenceManager::saveExists() const {
    std::ifstream f(savePath);
    return f.good();
}

int PersistenceManager::loadBalance() const {
    std::ifstream f(savePath);
    if (!f.is_open()) return DEFAULT_BALANCE;

    std::string line;
    while (std::getline(f, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        if (key == "balance") {
            try {
                return std::stoi(val);
            } catch (...) {
                return DEFAULT_BALANCE;
            }
        }
    }
    return DEFAULT_BALANCE;
}

void PersistenceManager::saveBalance(int balance) const {
    ensureDirectoryExists();
    std::ofstream f(savePath, std::ios::trunc);
    if (f.is_open()) {
        f << "balance=" << balance << "\n";
    }
}
