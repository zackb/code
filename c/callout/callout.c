#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

/* say -o hi.wav --data-format=LEF32@8000 "hello" */
#define JAB_SAY "jab "
#define CROSS_SAY "cross "
#define LEFT_HOOK_SAY "left hook "
#define RIGHT_UPPERCUT_SAY "right uppercut "
#define LEFT_UPPERCUT_SAY "left uppercut "
#define RIGHT_HOOK_SAY "right hook "

#define BUFFER_SIZE 1024

int total_punches = 0;

typedef enum    {
    NONE = 0,
    JAB = 1,
    CROSS = 2,
    LEFT_HOOK = 3,
    RIGHT_UPPERCUT = 4,
    LEFT_UPPERCUT = 5,
    RIGHT_HOOK = 6
} punch;


typedef struct  {
    punch punches[7];
} combo;

const combo combos[] = {
    {1},
    {1, 1},
    {1, 2},
    {1, 1, 2},
    {1, 2, 1, 2},
    {1, 2, 1, 2, 3},
    {1, 2, 2},
    {1, 1, 1},
    {1, 1, 3},
    {1, 2, 3},
    {1, 1, 3, 3},
    {1, 3, 6},
    {1, 3, 5},
    {1, 6},
    {1, 4},
    {1, 4, 5},
    {2},
    {2, 3},
    {3, 2},
    {3, 6},
    {2, 4, 6},
    {2, 6, 4},
    {1, 2, 3, 4, 5, 6}
};

const char *punch_say(punch p)  {

    const char* say = NULL;

    switch (p)  {
        case JAB:
            say = JAB_SAY;
            break;
        case CROSS:
            say = CROSS_SAY;
            break;
        case LEFT_HOOK:
            say = LEFT_HOOK_SAY;
            break;
        case RIGHT_UPPERCUT:
            say = RIGHT_UPPERCUT_SAY;
            break;
        case LEFT_UPPERCUT:
            say = LEFT_UPPERCUT_SAY;
            break;
        case RIGHT_HOOK:
            say = RIGHT_HOOK_SAY;
            break;
        default:
            break;
    }

    return say;
}

void say(const char *phrase)    {
    char tosay[BUFFER_SIZE];
    snprintf(tosay, BUFFER_SIZE, "say %s", phrase);
    system(tosay);
}

void callout(combo c)   {
    char combo_say[BUFFER_SIZE] = {0};
    int i = 0;
    for (i = 0; i < sizeof(c.punches) / sizeof(punch); i++) {
        punch p = c.punches[i];
        total_punches++;
        if (p == NONE) {
            break;
        }
        const char *say = punch_say(p);
        strncat(combo_say, say, strlen(say));
    }

    say(combo_say);
}

combo choose_combo()    {
    int max = sizeof(combos) / sizeof(combo);
    int chosen = rand() % max;
    return combos[chosen];
}

void sleep_rand(int max)   {
    //sleep(rand() % max);
    sleep((rand()%(max-1))+1);
}

void do_round(long minutes)    {
    time_t start = time(NULL);
    time_t end = start + (minutes * 60);
    int total_punches_start = total_punches;
    while (1)    {
        time_t now = time(NULL);
        if ((end - now) < 1)  {
            break;
        }
        callout(choose_combo());
        sleep_rand(5);
    }
    say("stop");
    int total_punches_round = total_punches - total_punches_start;
    char phrase[BUFFER_SIZE];
    snprintf(phrase, BUFFER_SIZE, "total punches that round %i", total_punches_round);
    say(phrase);
    snprintf(phrase, BUFFER_SIZE, "total punches %i", total_punches);
    say(phrase);
}

void countdown(int min, int rest)    {
    char phrase[BUFFER_SIZE];
    snprintf(phrase, BUFFER_SIZE, "Starting callout %i minute rounds %i second rest", min, rest);
    printf("%s\n", phrase);
    say(phrase);

    int i = 0;
    for (i = 10; i > 0; i--)    {
        printf("Starting in %i seconds\n", i);
        snprintf(phrase, BUFFER_SIZE, "%i", i);
        say(phrase);
        sleep(1);
    }
    say("start");
}

void usage()    {
    printf("Usage: callout <min> <rest seconds>\n");
}

int main(int argc, const char **argv)   {
    int min = 3, rest = 60;
    if (argc < 3)   {
        usage();
        return 1;
    }
    min = atoi(argv[1]);
    rest = atoi(argv[2]);

    srand(time(NULL));
    total_punches = 0;

    while (1)   {
        countdown(min, rest);
        do_round(min);
        sleep(rest);
    }
    return 0;
}
