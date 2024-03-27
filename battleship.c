#include <stdlib.h>
#include <stdbool.h>

#define UNKNOWN 0
#define MISS -1
#define HIT 1

typedef struct Ship {
    int row;
    int col;
    int size;
    bool vertical;
    int life;
} Ship;

typedef struct GameState {
    int row;
    int col;
    bool gameOver;
    bool player1turn;
    bool player1win;
    Ship playerships[2][5]; //player 1's info at index 0, player 2's info at index 1
    int playerlife[2];
    int shipboard[2][10][10]; //player 1's info at index 0, player 2's info at index 1
    int shotboard[2][10][10];
    
} GameState;

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

