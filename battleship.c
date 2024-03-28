#include <stdlib.h>
#include <stdbool.h>

#define UNKNOWN 0
#define MISS -1
#define HIT 1
#define NOSHIP -1
#define BOARDSIZE 10

typedef struct Ship {
    int row;
    int col;
    int size;
    bool vertical;
    int life;
} Ship;

Ship* createShip(int s){
    Ship* ship = (Ship*)malloc(sizeof(Ship));
    if (!ship) return NULL;
    ship->row = 0;
    ship->col = 0;
    ship->size = s;
    ship->vertical = true;
    ship->life = s;
    return ship;
}

typedef struct GameState {
    int row;
    int col;
    bool gameOver;
    bool placementRound;
    bool player1turn;
    bool player1win;
    bool colsel;
    Ship* playerships[2][5]; //player 1's info at index 0, player 2's info at index 1
    int playerlife[2];
    int shipboard[2][BOARDSIZE][BOARDSIZE]; //player 1's info at index 0, player 2's info at index 1
    int shotboard[2][BOARDSIZE][BOARDSIZE];
    
} GameState;

GameState* createGameState(){
    GameState* gameState = (GameState*)malloc(sizeof(GameState));
    if (!gameState) return NULL;
    gameState->row = 0;
    gameState->col = 0;
    gameState->gameOver = false;
    gameState->placementRound = true;
    gameState->player1turn = true;
    gameState->player1win = false;
    gameState->colsel = true;
    for (int player = 0; player < 2; player++){
        gameState->playerships[player][0] = createShip(5);
        if (!(gameState->playerships[player][0])) return NULL;

        gameState->playerships[player][1] = createShip(4);
        if (!(gameState->playerships[player][1])) return NULL;
        
        gameState->playerships[player][2] = createShip(3);
        if (!(gameState->playerships[player][2])) return NULL;
        
        gameState->playerships[player][3] = createShip(3);
        if (!(gameState->playerships[player][3])) return NULL;
        
        gameState->playerships[player][4] = createShip(2);
        if (!(gameState->playerships[player][4])) return NULL;

        gameState->playerlife[player] = 5;
        
        for (int i = 0; i < BOARDSIZE; i++){
            for (int j = 0; j < BOARDSIZE; j++){
                gameState->shipboard[player][i][j] = NOSHIP;
                gameState->shotboard[player][i][j] = NOSHIP;
            }
        }
    }
    return gameState;
}

void deleteGameState(GameState* gameState){
    for (int player = 0; player < 2; player++){
        for (int ship = 0; ship < 5; ship++){
            free(gameState->playerships[player][ship]);
            gameState->playerships[player][ship] = NULL;
        }
    }
    free(gameState);
    gameState = NULL;
}

void plot_pixel(int x, int y, short int line_color);
void clear_screen();
void wait_for_vsync();

// New game functions
void draw_board(GameState* gamestate);

volatile int pixel_buffer_start; // global variable
short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
short int Buffer2[240][512];

// Relevant subroutines

// Plot single pixel
void plot_pixel(int x, int y, short int line_color)
{
    volatile short int *one_pixel_address;
        
        one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
        
        *one_pixel_address = line_color;
}

// Clear entire screen
void clear_screen(){
    for (int x = 0; x < 320; x++){
        for (int y = 0; y < 240; y++){
            plot_pixel(x,y,0);
        }
    }
}

// Used for animations: maintain status while waiting
void wait_for_vsync(){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    *pixel_ctrl_ptr = 1;
    int status = *(pixel_ctrl_ptr + 3);
    while ((status & 0x01) != 0) {
        status = *(pixel_ctrl_ptr + 3);
    }
}

void draw_board(GameState* gamestate){
    // define how big the blocks will be
    // define location of each board on the screen
    // DRAW

}

int getSWNum(volatile int* SW_ptr, int orig){
    int sw = *SW_ptr;
    if (sw & 0x200) return 0;
    if (sw & 0x100) return 1;
    if (sw & 0x80) return 2;
    if (sw & 0x40) return 3;
    if (sw & 0x20) return 4;
    if (sw & 0x10) return 5;
    if (sw & 0x8) return 6;
    if (sw & 0x4) return 7;
    if (sw & 0x2) return 8;
    if (sw & 0x1) return 9;
    return orig;
}

int getKEYPress(volatile int* KEY_ptr){
    int key = *(KEY_ptr + 3);
    if (key & 0b1000){
        *(KEY_ptr + 3) = key | 0b1000;
        return 3;
    }
    if (key & 0b100){
        *(KEY_ptr + 3) = key | 0b100;
        return 2;
    }
    if (key & 0b10){
        *(KEY_ptr + 3) = key | 0b10;
        return 1;
    }
    if (key & 0b1){
        *(KEY_ptr + 3) = key | 0b1;
        return 0;
    }
    return -1;
}

bool checkLegalShipPlacement(GameState* gameState, int turn, Ship* ship){
    bool valid = true;
    if (ship->vertical){
        if (ship->row + ship->size > BOARDSIZE) ship->row = BOARDSIZE - ship->size;
        for (int i = 0; i < ship->size; i++){
            if (gameState->shipboard[turn][ship->row + i][ship->col] != NOSHIP){
                // call a function to highlight overlapping square
                valid = false;
            }
        }
    } else {
        if (ship->col + ship->size > BOARDSIZE) ship->col = BOARDSIZE - ship->size;
        for (int i = 0; i < ship->size; i++){
            if (gameState->shipboard[turn][ship->row][ship->col + i] != NOSHIP){
                // call a function to highlight overlapping square
                valid = false;
            }
        }
    }
    return valid;
    
}

int main(){
    volatile int *SW_ptr = 0xFF200040;
    volatile int *KEY_ptr = 0xFF200050;
    while(1){
        GameState* gameState = createGameState();
        if (!gameState) break;
        
        int shipToPlace = 0;
        int turn;
        
        while (gameState->placementRound){
            // players take turns placing ships
            turn = gameState->player1turn ? 0 : 1;
            Ship* ship = gameState->playerships[turn][shipToPlace];

            int key = getKEYPress(KEY_ptr);
            if (key == 3) gameState->colsel = !(gameState->colsel);
            if (key == 2) ship->vertical = !(ship->vertical);
            if (gameState->colsel){
                ship->col = getSWNum(SW_ptr, ship->col);
            } else {
                ship->row = getSWNum(SW_ptr, ship->row);
            }

            if (key == 1 && checkLegalShipPlacement(gameState, turn, ship)){
                if (ship->vertical){
                    for (int i = 0; i < ship->size; i++){
                        gameState->shipboard[turn][ship->row + i][ship->col] = shipToPlace;
                    }
                } else {
                    for (int i = 0; i < ship->size; i++){
                        gameState->shipboard[turn][ship->row][ship->col + i] = shipToPlace;
                    }
                }
                
                if (gameState->player1turn){
                    gameState->player1turn = false;
                } else {
                    shipToPlace++;
                    gameState->player1turn = true;
                    if (shipToPlace == 5) gameState->placementRound = false;
                }
            }

        }

        while (!(gameState->gameOver)){
            // players take turns shooting each other's ships

        }

        if (gameState->player1win){
            // draw victory screen for player 1
        } else {
            // victory for player 2
        }

        while (1) { // create a loop until player presses button to start next game

        }

        deleteGameState(gameState);

    }

    return 0;
}