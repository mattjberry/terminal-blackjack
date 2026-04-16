# Terminal Blackjack

A full functional blackjack game playable in the terminal!\
Originally I was planning on creating this little game as an experiment for learning C++. After getting some structure fleshed out I ran out of free time to work on it and the project instead sat unfinished for quite some time. Coming back to it, I decided to convert this project into an experiment for AI-driven development. I used Claude Code to write all code, building off the skeleton I made. All prompts and decisions are logged in the corresponding file. 

## Building

```
make
./terminal_blackjack
```

Requires **g++ with C++17** support. Uses POSIX terminal APIs (`termios`, `unistd.h`) — **macOS and Linux only**. Not supported on Windows.

## Persistence

Your balance is saved automatically after every round to `~/.terminal_blackjack/save.dat`.
