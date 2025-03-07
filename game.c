#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ===========================
   INTERNAL GLOBALS
   =========================== */

/* Holds all loaded words (up to WORDS_CAPACITY). Each is exactly 5 letters. */
static char dictionary[WORDS_CAPACITY][5];

/* For each position (0..4), track if it is matched. */
static bool matched[5];

/* Number of guesses so far. */
static int rounds = 0;

/* True if the game is ongoing, false if it ended. */
static bool active = true;

/* True if the user guessed the word correctly. */
static bool success = false;


/* ===========================
   FORWARD-DECLARATIONS OF HELPER FUNCTIONS
   =========================== */

/* Safely read a line from stdin into a heap buffer (caller frees). */
static char *read_input(void);

/* Check if guessed word is valid (5 letters, in dictionary). */
static ValidationStatus check_validity(char *word);

/* Compare entire guess to the secret word. */
static CharComparison *compare_guess(char *guess, char *secret);

/* Print color-coded feedback for each letter of the guess. */
static void display_feedback(CharComparison *results, char *guess, char *secret);

/* Show partial progress (correct letters) and guesses remaining. */
static void show_prompt(char *secret);

/* Helper: does char c appear anywhere in str? */
static bool find_in_string(char c, char *str);

/* Compare guess character to secret word at position idx. */
static CharComparison compare_char(char guess, int idx, char *secret);


/* ===========================
   EXTERNAL FUNCTIONS
   =========================== */

/* 1) Load dictionary from file, storing any 5-letter words in 'dictionary'. */
int load_dictionary(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return -1;
    }
    int index = 0;
    char line[32];
    while (fgets(line, sizeof(line), fp)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        /* Keep only lines of length 5. */
        if (len == 5) {
            strncpy(dictionary[index], line, 5);
            index++;
            if (index >= WORDS_CAPACITY) {
                break;
            }
        }
    }
    fclose(fp);
    return index; /* how many words loaded */
}

/* 2) Seed random number generator (with PID). */
void init_random(void) {
    srand(getpid());
}

/* 3) Select a random word from the dictionary. 'count' is how many were loaded. */
char *select_word(int count) {
    static char chosen[6];
    int r = rand() % count;
    strncpy(chosen, dictionary[r], 5);
    chosen[5] = '\0'; /* null-terminate */
    return chosen;
}

/* 4) The main routine for each guess. */
void guess_cycle(char *secret) {
    /* If the game is already not active, return. */
    if (!active) return;
    
    show_prompt(secret);
    char *guess = read_input();

    /* Validate length and dictionary membership. */
    ValidationStatus st = check_validity(guess);
    if (st == VALID_BAD_INPUT) {
        printf("Bad input (must be 5 letters)\n");
        rounds++;
        free(guess);
        return;
    }
    if (st == VALID_NOT_FOUND) {
        printf("No such word in dictionary\n");
        rounds++;
        free(guess);
        return;
    }

    /* Compare guess to secret. */
    CharComparison *res = compare_guess(guess, secret);
    display_feedback(res, guess, secret);

    /* Check how many positions are matched. */
    int correct_count = 0;
    for (int i = 0; i < 5; i++) {
        if (matched[i]) {
            correct_count++;
        }
    }

    if (correct_count == 5) {
        success = true;
        active  = false;
    } else {
        rounds++;
        if (rounds >= 5) {
            success = false;
            active  = false;
        }
    }
    free(guess);
}

/* 5) Return if the game is still active. */
bool game_active(void) {
    return active;
}

/* 6) Return if the user has guessed the word. */
bool game_success(void) {
    return success;
}


/* ===========================
   INTERNAL (STATIC) FUNCTIONS
   =========================== */

/* Safely read a line from stdin into a heap buffer. Caller frees. */
static char *read_input(void) {
    char *buf = malloc(64);
    if (!buf) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }
    if (!fgets(buf, 64, stdin)) {
        free(buf);
        fprintf(stderr, "Error or EOF reading input.\n");
        exit(EXIT_FAILURE);
    }
    /* strip newline if present */
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') {
        buf[len-1] = '\0';
    }
    return buf;
}

/* Check if guessed word is 5 letters and in dictionary. */
static ValidationStatus check_validity(char *word) {
    if (strlen(word) != 5) {
        return VALID_BAD_INPUT;
    }
    bool found = false;
    for (int i = 0; i < WORDS_CAPACITY; i++) {
        if (strncmp(dictionary[i], word, 5) == 0) {
            found = true;
            break;
        }
    }
    return found ? VALID_OK : VALID_NOT_FOUND;
}

/* Compare entire guess to secret. Returns an array of CharComparison. */
static CharComparison *compare_guess(char *guess, char *secret) {
    static CharComparison results[5];
    for (int i = 0; i < 5; i++) {
        results[i] = compare_char(guess[i], i, secret);
    }
    return results;
}

/* Display color-coded result for each letter in the guess. */
static void display_feedback(CharComparison *results, char *guess, char *secret) {
    for (int i = 0; i < 5; i++) {
        switch (results[i]) {
            case R_GREEN:
                printf("%s%c%s", CLR_GREEN,  guess[i], CLR_STOP);
                break;
            case R_YELLOW:
                printf("%s%c%s", CLR_YELLOW, guess[i], CLR_STOP);
                break;
            case R_RED:
            default:
                printf("%s%c%s", CLR_RED,    guess[i], CLR_STOP);
                break;
        }
    }
    printf("\n");
}

/* Show matched letters or '-' if not matched, plus how many guesses remain. */
static void show_prompt(char *secret) {
    for (int i = 0; i < 5; i++) {
        if (matched[i]) {
            printf("%c", secret[i]);
        } else {
            printf("-");
        }
    }
    printf("\n\n%d> ", (5 - rounds)); 
    fflush(stdout);
}

/* Return true if c is found anywhere in str. */
static bool find_in_string(char c, char *str) {
    return (strchr(str, c) != NULL);
}

/* Compare guess[i] to secret[i]. Return R_GREEN, R_YELLOW, or R_RED. */
static CharComparison compare_char(char guess, int idx, char *secret) {
    if (guess == secret[idx]) {
        matched[idx] = true;
        return R_GREEN;
    }
    else if (find_in_string(guess, secret)) {
        return R_YELLOW;
    }
    return R_RED;
}
