#include "game.h"
#include <stdio.h>

int main(void) {
    /* 1) Initialize random number generator. */
    init_random();

    /* 2) Load the dictionary from file. */
    int count = load_dictionary("words.txt");
    if (count < 0) {
        fprintf(stderr, "Failed to load dictionary from wordList.txt\n");
        return 1;
    }
    if (count == 0) {
        fprintf(stderr, "No 5-letter words found in the file.\n");
        return 1;
    }

    /* 3) Pick one random secret word. */
    char *secret = select_word(count);

    /* 4) Repeatedly prompt the user for a guess until the game ends. */
    while (game_active()) {
        guess_cycle(secret);
    }

    /* 5) Print the final result. */
    printf("\nThe correct word was: '%s'\n", secret);
    if (game_success()) {
        printf("Congrats, you guessed correctly!\n");
    } else {
        printf("You lost. Better luck next time.\n");
    }

    return 0;
}
