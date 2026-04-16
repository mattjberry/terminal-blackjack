# terminal-blackjack
Blackjack game developed as a fun project to learn C++

## Building

```
make
./terminal_blackjack
```

Requires **g++ with C++17** support. Uses POSIX terminal APIs (`termios`, `unistd.h`) — **macOS and Linux only**. Not supported on Windows.

## Persistence

Your balance is saved automatically after every round to `~/.terminal_blackjack/save.dat`.
