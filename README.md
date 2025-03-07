# Wordle Game in C

This is a simple Wordle-like console game written in C. It allows you to guess 5-letter words and gives you color-coded feedback (green, yellow, red) for each guessed letter.

## Files

- **game.h**: The header file that declares the core functions and enumerations.
- **game.c**: Contains the core game logic and dictionary storage.
- **main.c**: The main entry point, orchestrating the game loop and calling functions from `game.c`.
- **words.txt**: A list of valid 5-letter words, one word per line.

## How to Compile

```bash
gcc main.c game.c -o wordle_game
