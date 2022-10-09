//
// Created by Angelo van Meurs on 11/04/2022.
//

#ifndef TETRIS_STRUCTS_H
#define TETRIS_STRUCTS_H


typedef struct Shape
/* Collective name for 2d shapes containing 1-16 blocks.
 * Classic naming such as tetromino or pentomino is confusing.
 *
 * Each Shape has a predefined shape in "defs_init".
 *
 * To spawn a Shape into the matrix it must have an
 * x/y position. This is the position of (x,y) = (0,0).
 * Simply put, the bottom left corner.
 *
 * Rotation and flipped is reserved. */
{
    int x, y, shape, rotation, flipped;
    char symbol;
    struct Block *tail;

}   Shape;


typedef struct Block
/* Single block inside Shape. Can link to form shapes.*/
{
    int x, y;
    struct Block *prev;

}   Block;


typedef struct Vars {

    /* List of static variables set during init. */

    int matrix_w, matrix_h;

}   Vars;


#endif //TETRIS_STRUCTS_H