#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

typedef enum {
    VALID_OK,
    VALID_BAD_INPUT,
    VALID_NOT_FOUND
} ValidationStatus;

typedef enum {
    R_GREEN,
    R_YELLOW,
    R_RED
} CharComparison;

static const int WORDS_CAPACITY = 2272;

/* Color codes */
static const char *CLR_GREEN  = "\e[38;2;255;255;255;1m\e[48;2;106;170;100;1m"
static const char *CLR_YELLOW = "\e[38;2;255;255;255;1m\e[48;2;201;180;88;1m"
static const char *CLR_RED    = "\e[38;2;255;255;255;1m\e[48;2;220;20;60;1m"
static const char *CLR_STOP   = "\e[0;39m"

/* ===========================
   FUNCTION PROTOTYPES
   =========================== */

/* 1) Load 5-letter words from file into an internal dictionary. */
int load_dictionary(const char *filename);

/* 2) Initialize random number generator (using PID). */
void init_random(void);

/* 3) Select one random word from the loaded dictionary. */
char *select_word(int count);

/* 4) The main "per-turn" routine: prompt user, read guess, validate, compare. */
void guess_cycle(char *secret);

/* 5) Check if the game is still active (hasn't ended). */
bool game_active(void);

/* 6) Check if the player has succeeded (guessed the word). */
bool game_success(void);

#endif /* GAME_H */
