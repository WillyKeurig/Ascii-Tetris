//
// Created by arcva on 11/04/2022.
//

#ifndef TETRIS_DEFS_INIT_H
#define TETRIS_DEFS_INIT_H

#define TRUE                1
#define FALSE               0

#define DEBUG_MODE          FALSE

#define BASE_SPEED          500      // ms

//  MACROS
#define COORDS(  obj)    obj->x   ,  obj->y
#define COORDS_N(obj)    obj->x   ,  obj->y+1
#define COORDS_E(obj)    obj->x+1 ,  obj->y
#define COORDS_S(obj)    obj->x   ,  obj->y-1
#define COORDS_W(obj)    obj->x-1 ,  obj->y

//  SIZES, LENGTHS AND DIMENSIONS
#define MATRIX_OVF          4   // off-screen for spawn
#define MATRIX_W            20
#define MATRIX_H            19  // includes OVerFlow
#define MAX_SHAPES          7
#define MAX_SHAPES_EXTRA    18
#define MAX_ROTATION        3
#define SHAPE_W             4
#define SHAPE_H             4
#define BP_BITS             16
#define MATRIX_SIZE         sizeof(char*) * MATRIX_W

//  GENERAL
#define EXTRA               TRUE   // turn on extras, currently only adds more shapes
#define EMPTY_CELL          '.'
#define SPAWN_X             (int) (MATRIX_W / 2) - 2 // offset 2 (half of shape width)
#define SPAWN_Y             MATRIX_H - MATRIX_OVF
#define X_MASK              0b0011  // counts x w/ bp width 4
#define Y_MASK              0b1100  // counts y w/ bp width 4

//  KEYS
#define UP_ARROW            72  // ascii
#define RIGHT_ARROW         77
#define DOWN_ARROW          80
#define LEFT_ARROW          75
#define ESC                 27
#define N_BUTTON            UP_ARROW
#define E_BUTTON            RIGHT_ARROW
#define S_BUTTON            DOWN_ARROW
#define W_BUTTON            LEFT_ARROW
#define CW_BUTTON           'd'
#define CCW_BUTTON          'a'
#define DROP_BUTTON         ' '
#define QUIT_BUTTON         ESC


// "N" conflicts with <windows.h> - unfortunately
// <sysinfoapi.h> doesn't compile on my MinGW gcc (smaller more specific header file)
#define dN          10       // insert 1 to avoid
#define dE          11       // collision with CW value
#define dS          12       // which functionaly uses
#define dW          13       // the variable value

#define CW          1
#define CCW         3

//  SHAPE - ID
#define L_ID        0
#define J_ID        1
#define I_ID        2
#define O_ID        3
#define T_ID        4
#define S_ID        5
#define Z_ID        6

//  SHAPE - ID EXTRA
#define XF_ID       7
#define XL_ID       8
#define XN_ID       9
#define XP_ID       10
#define XT_ID       11
#define XU_ID       12
#define XV_ID       13
#define XW_ID       14
#define XX_ID       15
#define XY_ID       16
#define XZ_ID       17

//  SHAPE - CHAR REPRESENTATION
#define L_CH        'L'
#define J_CH        'J'
#define I_CH        'I'
#define O_CH        'O'
#define T_CH        'T'
#define S_CH        'S'
#define Z_CH        'Z'

//  SHAPE - CHAR REPRESENTATION EXTRA
#define XF_CH       'F'
#define XL_CH       'E'
#define XN_CH       'N'
#define XP_CH       'P'
#define XT_CH       'B'
#define XU_CH       'U'
#define XV_CH       'V'
#define XW_CH       'W'
#define XX_CH       'X'
#define XY_CH       'Y'
#define XZ_CH       'M'


//  SHAPE - BLUEPRINT
#define L_BP        0b\
0000\
0100\
0100\
0110
#define J_BP        0b\
0000\
0010\
0010\
0110
#define I_BP        0b\
0100\
0100\
0100\
0100
#define O_BP        0b\
0000\
0110\
0110\
0000
#define T_BP        0b\
0000\
0100\
1110\
0000
#define S_BP        0b\
0000\
0100\
0110\
0010
#define Z_BP        0b\
0000\
0010\
0110\
0100

//  SHAPES - BLUEPRINT EXTRA
#define XF_BP       0b\
0000\
0100\
1110\
0010
#define XL_BP       0b\
0100\
0100\
0100\
0110
#define XN_BP       0b\
0000\
1100\
0111\
0000
#define XP_BP       0b\
0000\
0110\
0110\
0100
#define XT_BP       0b\
0000\
1110\
0100\
0100
#define XU_BP       0b\
0000\
1010\
1110\
0000
#define XV_BP       0b\
0000\
0111\
0100\
0100
#define XW_BP       0b\
0000\
0010\
0110\
1100
#define XX_BP       0b\
0000\
0100\
1110\
0100
#define XY_BP       0b\
0010\
0110\
0010\
0010
#define XZ_BP       0b\
0000\
1100\
0100\
0110


//  SHAPE ARRAYS
int IDS[] = {
        L_ID,
        J_ID,
        I_ID,
        O_ID,
        T_ID,
        S_ID,
        Z_ID,

        XF_ID,
        XL_ID,
        XN_ID,
        XP_ID,
        XT_ID,
        XU_ID,
        XV_ID,
        XW_ID,
        XX_ID,
        XY_ID,
        XZ_ID
};

char CHARACTERS[] = {
        L_CH,
        J_CH,
        I_CH,
        O_CH,
        T_CH,
        S_CH,
        Z_CH,

        XF_CH,
        XL_CH,
        XN_CH,
        XP_CH,
        XT_CH,
        XU_CH,
        XV_CH,
        XW_CH,
        XX_CH,
        XY_CH,
        XZ_CH
};

uint16_t BLUEPRINTS[] = {
        L_BP,
        J_BP,
        I_BP,
        O_BP,
        T_BP,
        S_BP,
        Z_BP,

        XF_BP,
        XL_BP,
        XN_BP,
        XP_BP,
        XT_BP,
        XV_BP,
        XU_BP,
        XW_BP,
        XX_BP,
        XY_BP,
        XZ_BP
};

#endif //TETRIS_DEFS_INIT_H
