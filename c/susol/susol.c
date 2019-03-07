#include <stdio.h>
#include <stdlib.h>

#define SIZE 9
#define EMPTY 0

int usage();
void read_board();
int solve_board();
void print_board();

int board[SIZE][SIZE] = {{EMPTY}};

int main(int argc, const char **argv) {
    if (argc != 2) 
        return usage();
    read_board(argv[1]);
    solve_board();
    print_board();
    return 0;
}

int solve_board() {
    // find the x and y to try
   int x, y;

    // see if the board has an empty space and set the x and y
    int empty = 0;
    for (x = 0; x < SIZE; x++) {
        for (y = 0; y < SIZE; y++) {
            if (board[x][y] == EMPTY) {
                empty = 1;
                break;
            }
        }
        if (empty == 1) break;
    }
    if (empty == 0) return 1;

    // track numbers already used in row and cols
    int numbers[SIZE + 1] = { 0 };
    for (int i = 0; i < SIZE; i++) {
        numbers[board[x][i]] = 1;
        numbers[board[i][y]] = 1;
    }

    // track numbers already used in the unit
    int ux = (x / 3) * 3;
    int uy = (y / 3) * 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            numbers[board[ux + i][uy + j]] = 1;

    // find unused numbers and see if they fit
    for (int i = 1; i <= SIZE; i++) {
        // find an available number
        if (numbers[i] == 0) {
            // see if it works
            board[x][y] = i;
            if (solve_board() == 1)
                return 1;
            // otherwise reset and try the next number
            board[x][y] = EMPTY;
        }
    }
    return 0;
}

void read_board(char *path) {
    FILE *file = fopen(path, "r");
    char c;
    int i = 0, j = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c == ',') continue;
        if (c == '\n') {
            i += 1;
            j = 0;
            continue;
        }
        if (c == 'x') board[i][j++] = EMPTY;
        else board[i][j++] = c - '0';
    }
    fclose(file);
}

void print_board() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%d", board[i][j]);
            if (j < SIZE - 1) printf(",");
        }
        printf("\n");
    }
}

int usage() {
    printf("usage: ./susol <input file>\n");
    return 1;
}
