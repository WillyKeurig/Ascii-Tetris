#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <conio.h>
#include "structs.h"
#include "defs.h"


#ifdef _WIN32
//#include <sysinfoapi.h>
#include <windows.h>
#endif


/*  ERROR
 *  11  -   Empty shape blueprint defined
 *  12  -   Invalid direction parameter
 *  13  -   No Windows detected
 */


/* Global vars to avoid excessive use of pointer args.
 *
 * After researching as to WHY global vars are frowned upon, the main issues
 * appear to be difficult debugging and dangerous threading. Since my program
 * is (and will remain) small I chose less code complexity over ease
 * of debugging. Also, I am not implementing any threads.
 */

Shape *shape;
char **matrix;
int score;
SYSTEMTIME prev_turn;


// init
void init();
void init_matrix ();
void init_shape ();

// utilities
int shape_block_amt ();
uint16_t random_blueprint ();
Block *tail_coord_from_bp (uint16_t blueprint);
int reversed (int direction);

// core mechanics
void move_object (int direction);
void control (int dropped);
void embed_blocks ();
void rotate (int direction);
void drop_row (int row);
void drop_floating_rows ();
void delete_row (int row);
int delete_full_rows ();

// rule checks
int move_legal (int direction);
int shape_out_of_bounds ();
int rotation_out_of_bounds (int direction);
int move_out_of_bounds (int direction);
int collision ();
int game_over ();

// clock and time
void delay (int ms);
void reset_clock();
int wait_clock();
int next_clock ();
int win_ms_dif (SYSTEMTIME previous, SYSTEMTIME current);

// print
void print_matrix ();
void print_score ();
void refresh ();

// free
void free_matrix ();
void free_shape ();

// debug
void debug_print_coords ();


void init_matrix ()
/* Allocate memory location to global matrix.
 * Fill with EMPTY_CELL */
{
    int x, y;

    matrix = calloc(MATRIX_W, sizeof(char*));

    for (x=0; x < MATRIX_W; x++)
    {
        matrix[x] = calloc(MATRIX_H, sizeof(char));
    }

    for (x=0; x < MATRIX_W; x++)
    {
        for (y=0; y < MATRIX_H; y++)
        {
            matrix[x][y] = EMPTY_CELL;
        }
    }
}


void init_shape ()
/* Instantiate shape struct Shape for use in matrix */
{
    shape = calloc(1, sizeof(Shape));
    shape->tail = tail_coord_from_bp(random_blueprint());
    shape->symbol = CHARACTERS[shape->shape];
    shape->x = SPAWN_X;
    shape->y = SPAWN_Y;
}


uint16_t random_blueprint ()
/* Return a random blueprint for a shape */
{
    int random;
    if (EXTRA)
        random = rand() % MAX_SHAPES_EXTRA;
    else
        random = rand() % MAX_SHAPES;   //  modulo bias insignificant

    shape->shape = random;
    return BLUEPRINTS[random];
}


Block *link (Block *prev, int x, int y)
/* Link to linked Block list. Returns a pointer of a
 * new Block. This new Block is linked through a pointer
 * to the prev(ious) Block. */
{
    Block *new = calloc(1, sizeof(Block));
    new-> x = x;
    new-> y = y;
    new-> prev = prev;
    return new;
}


Block *tail_coord_from_bp (uint16_t blueprint)
/* Returns a pointer to the tail of a linked Block
* list. This list contains Coords matching the blueprint.
* Each x/y pair represents a cell in the matrix.
*
* Shape blueprints are a boolean representation of block
* Coords that belong to the Shape. For convenience,
* this function is used to translate
* those bit representations to Coords. */
{
    if (!blueprint)
        exit(11);

    uint16_t bit;
    uint8_t i, x, y;
    Block *coord = NULL;                // first coord has no prev

    bit = 0x8000;                       // first bit of uint16

    for (i = 0; i < BP_BITS; i++)       // iterate bits 15:0
    {
        if (blueprint & bit)            // if cell in blueprint is not empty.
        {                               // below is conversion of single bits to coords, while creating linked list
            x = i & X_MASK;             // x mask counts every 4th step. y mask counts every step up to 4
            y = (~i & Y_MASK) >> 2;     // invert to keep position y1 at bottom
            coord = link(coord, x, y);
        }
        bit >>= 1;                      // next lower bit
    }

    return coord;
}


void move_object (int direction)
/* Move object based on direction */
{
    switch (direction)
    {
        case dN: shape->y++; break;  // illegal control
        case dE: shape->x++; break;
        case dS: shape->y--; break;
        case dW: shape->x--; break;
        case CW: rotate(CW); break;
        case CCW: rotate(CCW); break;
        //default: exit(12);
    }
}


void free_matrix ()
/* Free memory allocated to matrix */
{
    int x;
    for (x=0; x < MATRIX_W; x++)
    {
        free(matrix[x]);
        matrix[x] = NULL;
    }
    free(matrix);
    matrix = NULL;
}


int shape_block_amt ()
/* Returns amount of blocks in shape */
{
    int i=0;
    Block *block = shape->tail;

    while (block != NULL)
    {
        i++;
        block = block->prev;
    }
    return i;
}


int win_ms_dif (SYSTEMTIME previous, SYSTEMTIME current)
/* Return time difference in ms. Uses SYSTEMTIME variables */
{
    int prev, cur;
    prev = previous.wMilliseconds;
    cur = current.wMilliseconds;

    if (prev > cur)     // if a new second has started, prev will have more ms than cur
        return cur + (1000 - prev);

    return cur - prev;
}


int next_clock ()
/* Checks if enough time has passed to go to the next turn/clock */
{
    static int running;

    if (_WIN32)
    {
        SYSTEMTIME cur_turn;

        if (!running)           // set prev_turn if game just started (static)
        {
            running = TRUE;
            GetSystemTime(&prev_turn);
            return FALSE;
        }

        else
        {
            GetSystemTime(&cur_turn);
            if (win_ms_dif(prev_turn, cur_turn) > BASE_SPEED) // if difference in turns > game speed
            {
                GetSystemTime(&prev_turn);                  // reset prev_turn
                return TRUE;
            }
            return FALSE;
        }
    }

    else        // if not running windows, E13 (time functions very OS dependent)
        exit(13);
}


int wait_clock()
/* Pauses until next turn/clock */
{
    if (_WIN32)
    {
        SYSTEMTIME cur_turn;
        while (TRUE)
        {
            GetSystemTime(&cur_turn);
            if (win_ms_dif(prev_turn, cur_turn) > BASE_SPEED)
                return TRUE;
        }
    }
}


void reset_clock()
/* Sets previous turn time to real time */
{
    GetSystemTime(&prev_turn);
}


void print_matrix ()
/* Print matrix  */
{
    int x, y, match, height;

    height = MATRIX_H - MATRIX_OVF -1;  // OVerFlow is area in which new shapes spawn. Not visible to player

    if (DEBUG_MODE)                     // if debug, show OVF area
        height += MATRIX_OVF;

    if (shape != NULL)                  // if shape present: check where to print blocks in shape
    {                                   // if match, print symbol of the shape. Else print matrix cell
        for (y=height; y>=0; y--)       // iterate matrix
        {
            for (x=0; x<MATRIX_W; x++)
            {
                match = FALSE;
                Block *block = shape->tail;

                while(block!=NULL)      // iterate blocks in shape
                {
                    if (block->x + shape->x == x && block->y + shape->y == y)
                    {
                        match = TRUE;   // TRUE if coord is occupied by block in shape
                        break;
                    }
                    block = block->prev;
                }

                if (match)
                    printf("%c ", shape->symbol);
                else
                    printf("%c ", matrix[x][y]);
            }
            printf("\n");
        }
    }

    else        // if no shape in matrix, only print matrix
    {
        for (y=0; y<MATRIX_H; y++)
        {
            for (x=0; x<MATRIX_W; x++)
            {
                printf("%c", matrix[x][y]);
            }
            printf("\n");
        }
    }
}


void print_score ()
/* Print score above the game. Aligns right */
{
    int i, digits, str_size, used_chars, score_tmp;
    char *str;

    i = digits = str_size = 0;
    used_chars = 6;         // "score:"
    score_tmp = score+1;    // plus 1 bc score = 0 has no digits

    while (score_tmp != 0)
    {
        score_tmp /= 10;    // counts digits
        digits++;           // add to total
    }

    used_chars += digits;
    str_size = MATRIX_W*2 - used_chars;
    str = calloc(str_size, sizeof(char));

    for (i; i<str_size-1; i++)
    {
        str[i] = ' ';       // padding
    }

    printf("Score:%s%d\n", str, score);
}


void refresh ()
/* Clear screen and print new game state */
{
    system("cls");
    print_score();
    print_matrix();
}


void rotate (int direction)
/* Rotate a shape in a direction */
{
    int i, x;
    Block *block;

    for (i=0; i<direction; i++)         // if direction is CCW (val: 3) rotate CW 3 times. CW = 1 rotation
    {
        block = shape->tail;
        while (block != NULL)
        {
            x = block->x;               // these lines represent a simple way to rotate a 4x4 grid CW
            block->x = block->y;
            block->y = 3 - x;
            block = block->prev;
        }
    }
}


void init ()
/* init random seed and score */
{
    srand(time(NULL));
    score = 0;
}


void delay (int ms)
/* Delay game in ms */
{
    clock_t start_time = clock();
    while (clock() - start_time < ms *(CLOCKS_PER_SEC/1000));
}


int shape_out_of_bounds ()
/* Check if a shape is outside the matrix in the current state */
{
    int x, y;
    Block *block = shape->tail;

    while (block != NULL)
    {
        x = block->x + shape->x;         // get absolute position
        y = block->y + shape->y;

        if (
                x < 0           ||      // if over West edge
                x >= MATRIX_W   ||      // East
                y < 0           ||      // South
                y >= MATRIX_H           // North
            )
            return TRUE;

        block = block->prev;
    }
    return FALSE;
}


int collision ()
/* If current state has a block in shape on the same coord as a non-empty matrix cell, return TRUE */
{
    int x, y;
    Block *block = shape->tail;

    while (block != NULL)
    {
        x = block->x + shape->x;    // block coord + absolute shape position
        y = block->y + shape->y;

        if (matrix[x][y] != EMPTY_CELL)
            return TRUE;

        block = block->prev;
    }
    return FALSE;
}


int game_over ()
/* If any shape block reaches upper row, return game over bool */
{
    int x;
    for (x=0; x < MATRIX_W; x++)
    {
        if (matrix[x][MATRIX_H-MATRIX_OVF] != EMPTY_CELL) // last visible row
            return TRUE;
    }
    return FALSE;
}


void embed_blocks ()
/* Embeds blocks to appropriate coord in matrix */
{
    int x,y;
    Block *block;

    block = shape->tail;

    while (block != NULL)
    {
        x = block->x + shape->x;    // block coord in shape + absolute position shape
        y = block->y + shape->y;

        matrix[x][y] = shape->symbol; // embed symbol to matrix cell

        block = block->prev;
    }

    score += shape_block_amt();
}


void free_shape ()
/* Free memory allocated to shape and blocks in shape */
{
    Block *prev, *cur = shape->tail;
    while (cur !=NULL)
    {
        prev = cur->prev;
        free(cur);
        cur = prev;
    }
    free(prev);
    free(shape);
}


int rotation_out_of_bounds (int direction)
/* Check if rotation function will move any block in the shape outside the matrix */
{
    int legal;

    rotate(direction);
    legal = shape_out_of_bounds();
    rotate(4 - direction);

    return legal;
}


int move_out_of_bounds (int direction)
/* Check if move function will move any block in the shape outside the matrix */
{
    int x, y;
    Block *block = shape->tail;

    while (block != NULL)
    {
        x = block->x + shape->x;  // add block pos. to relative pos.
        y = block->y + shape->y;

        switch (direction)
        {
            case dN:
                if (y == MATRIX_H-1)        // Check if bordering North
                    return 1;
                break;

            case dE:
                if (x == MATRIX_W-1)        // East
                    return 1;
                break;

            case dS:
                if (y == 0)                 // South
                    return 1;
                break;

            case dW:
                if (x == 0)                 // West
                    return 1;
                break;

            case CW:
                if (rotation_out_of_bounds(CW))
                    return 1;
                break;

            case CCW:
                if (rotation_out_of_bounds(CCW))
                    return 1;
                break;
        }

        block = block->prev;
    }
    return 0;
}


void control (int dropped)
/* Handles all user inputs */
{
    switch (getch())
    {
        case QUIT_BUTTON:
            free_matrix();
            free_shape();
            exit(0);

        case N_BUTTON:
            if (move_legal(dN))
                move_object(dN);
            break;

        case E_BUTTON:
            if (move_legal(dE))
                move_object(dE);
            break;

        case S_BUTTON:
            if (move_legal(dS))
                move_object(dS);
            break;

        case W_BUTTON:
            if (move_legal(dW))
                move_object(dW);
            break;

        case CW_BUTTON:
            if (move_legal(CW))
                rotate(CW);
            break;

        case CCW_BUTTON:
            if (move_legal(CCW))
                rotate(CCW);
            break;

        case DROP_BUTTON:
            if (dropped)            // prevent infinite drop floating
                break;

            while (!collision())    // move down till collision
                move_object(dS);
            move_object(dN);// move 1 back up

            refresh();              // print dropped block
            reset_clock();          // give full clock time for input

            if (kbhit())
                control(TRUE);  // last move after drop
            break;
    }
}


int reversed (int direction)
/* Return opposite direction */
{
    switch (direction)
    {
        case dN: return dS;
        case dE: return dW;
        case dS: return dN;
        case dW: return dE;
        case CW: return CCW;
        case CCW: return CW;
    }
}


int move_legal (int direction)
/* Check if moving a shape collides with non-empty cells in matrix */
{
    if (move_out_of_bounds(direction))
        return FALSE;

    move_object(direction);
    if (collision()) {
        move_object(reversed(direction));
        return FALSE;
    }
    move_object(reversed(direction));
    return TRUE;
}


void drop_row (int row)
/* Drops all symbols in matrix row 1 step */
{
    int x,y;

    for (y=row; y < MATRIX_H-MATRIX_OVF-1; y++)
    {
        for (x=0; x < MATRIX_W; x++)
        {
            matrix[x][y] = matrix[x][y+1];
        }
    }
}


void drop_floating_rows ()
/* Drops ALL rows that are above empty rows */
{
    int x, y, empty, dropped;

    for (y=MATRIX_H-1; y >= 0; y--)    // in reverse because rows change when dropping
    {
        empty = TRUE;

        for (x=0; x < MATRIX_W; x++)
        {
            if (matrix[x][y] != EMPTY_CELL)
            {
                empty = FALSE;
                break;
            }
        }

        if (empty)                      // if all cells in row empty
        {
            drop_row(y);
            dropped = TRUE;
        }
    }

    if (dropped)
    {
        refresh();
        reset_clock();  // reset turn start
        wait_clock();   // waits full turn if after reset
    }
}


void delete_row (int row)
/* Empty row in matrix */
{
    int x;

    for (x=0; x < MATRIX_W; x++)
    {
        matrix[x][row] = EMPTY_CELL;
    }
}


int delete_full_rows ()
/*  Iterates rows and sets all cells to EMPTY_CELL char  */
{
    int x,y, full, deleted;

    deleted = 0;

    for (y=0; y < MATRIX_H; y++)        // row
    {
        full = TRUE;                    // true until proven false
        for (x=0; x < MATRIX_W; x++)    // col
        {
            if (matrix[x][y] == EMPTY_CELL)
            {                           // if any empty
                full = FALSE;           // not full
                break;
            }
        }
        if (full)
        {
            delete_row(y);
            deleted += 1;
        }
    }

    if (deleted)
    {                                   // exp multiplier for multiple row bonus
        score += deleted * deleted * MATRIX_W;
        refresh();                      // show deleted rows
        reset_clock();                  // wait until next clock
        wait_clock();
    }

    return deleted;
}


void debug_print_coords ()
/* Used to print shape coords while playing */
{
    Block *block = shape->tail;
    while (block != NULL)
    {
        printf("x: %d\ty: %d\n",
               block->x + shape->x, block->y + shape->y);

        block = block->prev;
    }
    block = NULL;
}


int main ()
{
    init();
    init_shape();
    init_matrix();

    system("cls");

    for(;;)
    {
        refresh();

        if (DEBUG_MODE)                     // if debug always display coords in shape
            debug_print_coords();

        delay(10);

        if (kbhit())                        // if input buffer has user input
            control(FALSE);

        if (next_clock())
        {
            score += 1;
            move_object(dS);        // force move
        }

        if (collision() || shape_out_of_bounds())   // if forced move collides
        {
            move_object(dN);        // move shape back
            embed_blocks();                 // embed shape in matrix
            init_shape();
        }

        if (delete_full_rows())             // check if any row is full
            drop_floating_rows();           // move down all rows above deleted

        if (game_over())
        {
            printf("\nGAME OVER\n\n");
            system("pause");
            exit(0);
        }
    }
}