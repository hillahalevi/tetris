//
// Created by hilla on 6/10/19.
//
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>
#define GRID_SIZE 20
#define HORIZANTAL 0
#define VERTICAL 1
#define FLIP 'w'
#define LEFT 'a'
#define RIGHT 'd'
#define DOWN 's'
#define QUIT 'q'
#define SIGN '*'
#define SPACE ' '
typedef struct tetris_game_board {
    char grid[GRID_SIZE][GRID_SIZE];
    int row;
    int col;
    int position; // vertical : 1 |horizontal : 0
} TetriStruct;

TetriStruct tetris;
void InitializeGame(TetriStruct* tetris);
void CreateGrid(TetriStruct* tetris);
void Move(int signum);
void KeyCommand(int signum);
void ChangePosition(TetriStruct *tetris);
void PlaceShape(TetriStruct* tetris, int row, int col, int isChanged);
void locate(TetriStruct* tetris);
void ShowGrid(TetriStruct* tetris);
void ClearShape(TetriStruct* tetris);

int main() {
    InitializeGame(&tetris);
    ShowGrid(&tetris);
    // Signal moving the shape down.
    signal(SIGALRM, Move);
    // Wait one second before the next signal-The signal sent is SIGALRM.
    alarm(1);
    // Signal to check the key press.
    signal(SIGUSR2, KeyCommand);
    while (1) {
        /* wait until the process gets a signal.*/
        pause();
    }
}

/**
 * initializes game flow
 * @param tetris
 */
void InitializeGame(TetriStruct* tetris) {
    locate(tetris);
    CreateGrid(tetris);
    PlaceShape(tetris, tetris->row, tetris->col, 0);

}
/**
 * locate it in the middle of top row.
 * @param tetris
 */
void locate(TetriStruct* tetris){
    tetris->position = 0;
    tetris->row = 0;
    tetris->col = 9;

}

/**
 * move the shape closer to the bottom every second.
 * @param signum
 */
void Move(int signum) {
    signal(SIGALRM, Move);
    // keep the signal for entering this function and moving the shape down.
    alarm(1);
    // one step
    PlaceShape(&tetris, tetris.row + 1, tetris.col, 0);
    ShowGrid(&tetris);
}

/**
 * act as the key command.
 * @param signum
 */
void KeyCommand(int signum) {
    signal(SIGUSR2, KeyCommand);
    char key = (char) getchar();
    switch (key){
        case DOWN:
            PlaceShape(&tetris, tetris.row + 1, tetris.col, 0);
            break;
        case RIGHT:
            PlaceShape(&tetris, tetris.row, tetris.col + 1, 0);
            break;
        case LEFT:
            PlaceShape(&tetris, tetris.row, tetris.col - 1, 0);
            break;
        case FLIP:
            ChangePosition(&tetris);
            break;
        case QUIT: exit(1);
        default:
            break;
    }
    ShowGrid(&tetris);
}


/**
 * flip
 * @param tetris
 */
void ChangePosition(TetriStruct *tetris) {
    ClearShape(tetris);
    int prevRow = tetris->row;
    int prevCol = tetris->col;
    if (tetris->position == HORIZANTAL) {
        tetris->position = VERTICAL;
        PlaceShape(tetris, prevRow - 1, prevCol + 1, 1);
    } else  {
        int x = prevRow + 1;
        int y = prevCol - 1;
        // check borders
        if (y + 2 >= 20 - 1) {
            y = 20 - 4;
        } else if (y < 1) {
            y = 1;
        }
        tetris->position = HORIZANTAL;
        PlaceShape(tetris, x, y, 1);
    }
}

/**
 * build the grid
 * @param tetris
 */
void CreateGrid(TetriStruct* tetris) {
    int i, j;
    for (i = 0; i < 20 - 1; i++) {
        for (j = 0; j < 20; j++) {
            if (j == 0 || j == 20 - 1) {
                tetris->grid[i][j] = SIGN;
                continue;
            }
            // Fill the rest with ' ' .
            tetris->grid[i][j] = SPACE;
        }
    }
    for (j = 0; j < 20; j++) {
        tetris->grid[20 - 1][j] = SIGN;
    }
}


/**
 * show the grid to the screen.
 * @param tetris
 */
void ShowGrid(TetriStruct* tetris) {
    system("clear");
    int i,j;
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 20; j++) {
            printf("%c", tetris->grid[i][j]);
        }
        printf("\n");
    }
}

/**
 * delete the moving shape
 * @param tetris
 */
void ClearShape(TetriStruct* tetris) {
    if (tetris->position == HORIZANTAL) {
        tetris->grid[tetris->row][tetris->col] = SPACE;
        tetris->grid[tetris->row][tetris->col + 1] = SPACE;
        tetris->grid[tetris->row][tetris->col + 2] = SPACE;
    } else {
        tetris->grid[tetris->row][tetris->col] = SPACE;
        tetris->grid[tetris->row + 1][tetris->col] = SPACE;
        tetris->grid[tetris->row + 2][tetris->col] = SPACE;
    }
}

/**
 * replace the shape
 * @param tetris
 * @param row
 * @param col
 * @param isChanged
 */
void PlaceShape(TetriStruct* tetris, int row, int col, int isChanged) {

    if (tetris->position == HORIZANTAL) {
        if (col <= 0)
            return;
        if (col + 2 >= 20 -1 )
            return;
    } else{
        if (col <= 0)
            return;
        if (col >= 20 - 1)
            return;
    }
    // clear previous shape if we didn't already cleared it
    if (!isChanged) {
        ClearShape(tetris);
    }
    tetris->row = row;
    tetris->col = col;

    if (tetris->position == HORIZANTAL) {
        tetris->grid[row][col] = SPACE;
        tetris->grid[row][col + 1] = SPACE;
        tetris->grid[row][col + 2] = SPACE;
        // Once we hit the bottom, re-initialize
        if (row >= 20 - 1) {
            InitializeGame(tetris);
        }
    } else {
        tetris->grid[row][col] = SPACE;
        tetris->grid[row + 1][col] = SPACE;
        tetris->grid[row + 2][col] = SPACE;
        // Once we hit the bottom, re-initialize
        if (row + 2 >= 20 - 1) {
            InitializeGame(tetris);
        }
    }
}

