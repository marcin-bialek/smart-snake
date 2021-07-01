#pragma once 
#include "wasm.h"
#include "List.h"
#include "Matrix.h"
#include <stddef.h>


typedef enum {
    SNAKE_UP,
    SNAKE_RIGHT,
    SNAKE_DOWN,
    SNAKE_LEFT
} SnakeDirection;

typedef struct {
    int x;
    int y;
} SnakePoint;

struct Snake {
    size_t boardSize;
    size_t bodyLength;
    size_t ticks;
    List body;
    SnakePoint food;
    float maxDistance;
    float maxDistanceInv;
    size_t raysCount;
    struct { float dx, dy; }* rays;
    SnakeDirection previousDirection;
};

typedef struct Snake* Snake;


WASM_EXPORT Snake snakeCreate(size_t boardSize, size_t raysCount);
WASM_EXPORT void snakeDelete(Snake snake);
WASM_EXPORT long snakeMove(Snake snake, SnakeDirection direction);
WASM_EXPORT Matrix snakeCalcDistanceMatrix(Snake snake);
