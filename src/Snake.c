#include "Snake.h"
#include "random.h"
#include <stdlib.h>
#include <math.h>


static SnakePoint* makePoint(int x, int y) {
    SnakePoint* point = malloc(sizeof(SnakePoint));
    point->x = x;
    point->y = y;
    return point;
}


static bool isBodyPart(Snake self, SnakePoint* p) {
    for(ListIterator i = listGetIterator(self->body); i.present; listIteratorNext(&i)) {
        if(((SnakePoint*)i.value)->x == p->x && ((SnakePoint*)i.value)->y == p->y) {
            return true;
        }
    }

    return false;
}


static bool isBodyPartWithoutHead(Snake self, SnakePoint* p) {
    ListIterator i = listGetIterator(self->body);
    listIteratorNext(&i);

    for(; i.present; listIteratorNext(&i)) {
        if(((SnakePoint*)i.value)->x == p->x && ((SnakePoint*)i.value)->y == p->y) {
            return true;
        }
    }

    return false;
}


static void dropFood(Snake self) {
    do {
        self->food.x = randomInt(0, self->boardSize - 1);
        self->food.y = randomInt(0, self->boardSize - 1);
    } while(isBodyPart(self, &self->food));
}


static void calcRays(Snake self) {
    const float da = 2 * M_PI / self->raysCount;
    float a = 0.0;
    
    for(size_t i = 0; i < self->raysCount; i++) {
        self->rays[i].dx = sinf(a);
        self->rays[i].dy = cosf(a);
        a += da;
    }
}


static void calcFoodAngleAndDistance(Snake self, Matrix distanceMatrix) {
    SnakePoint* head = listFirst(self->body);
    const float dx = (float)self->food.x - head->x;
    const float dy = (float)self->food.y - head->y;
    const float a = atan2f(dx, dy) / M_PI;
    const float d = sqrtf(dx * dx + dy * dy) / self->maxDistance;
    matrixSet(distanceMatrix, 2 * self->raysCount, 0, a);
    matrixSet(distanceMatrix, 2 * self->raysCount + 1, 0, d);
}


typedef struct {
    SnakePoint min;
    SnakePoint max;
} Bounds;


static void calcSnakeBounds(List list, size_t i, SnakePoint* part, Bounds* bounds) {
    if(part->x < bounds->min.x) bounds->min.x = part->x;
    if(part->x > bounds->max.x) bounds->max.x = part->x;
    if(part->y < bounds->min.y) bounds->min.y = part->y;
    if(part->y > bounds->max.y) bounds->max.y = part->y;
}


static void calcBodyAndWallDistance(Snake self, Matrix distanceMatrix) {
    Bounds bounds = {
        .min.x = self->boardSize,
        .min.y = self->boardSize,
        .max.x = 0,
        .max.y = 0
    };

    listForEach(self->body, (ListForEachCallback)calcSnakeBounds, &bounds);
    SnakePoint* head = listFirst(self->body);

    for(size_t i = 0; i < self->raysCount; i++) { 
        float d = 0.0;
        float xf = head->x;
        float yf = head->y;
        SnakePoint pp = { xf, yf };
        SnakePoint p = { xf, yf };
        bool tb = true;

        while(0 <= p.x && p.x < self->boardSize && 0 <= p.y && p.y < self->boardSize) {
            if(p.x != pp.x || p.y != pp.y) {
                d += 1.0 / self->boardSize;

                if(tb && bounds.min.x <= p.x && p.x <= bounds.max.x && bounds.min.y <= p.y && p.y <= bounds.max.y) {
                    if(isBodyPartWithoutHead(self, &p)) {
                        matrixSet(distanceMatrix, self->raysCount + i, 0, d);
                        tb = false;
                    }
                }
                else if(p.x == self->food.x && p.y == self->food.y) {
                    matrixSet(distanceMatrix, 2 * self->raysCount + i, 0, d);
                }  
            }
            
            xf += self->rays[i].dx;
            yf += self->rays[i].dy;
            pp.x = p.x;
            pp.y = p.y;
            p.x = roundf(xf);
            p.y = roundf(yf); 
        }

        matrixSet(distanceMatrix, i, 0, d);
    }
}


Snake snakeCreate(size_t boardSize, size_t raysCount) {
    Snake self = malloc(sizeof(struct Snake));
    self->boardSize = boardSize;
    self->bodyLength = 1;
    self->ticks = 0;
    self->body = listCreate();
    listAppend(self->body, makePoint(boardSize / 2, boardSize / 2));
    dropFood(self);

    self->maxDistance = M_SQRT2 * (float)boardSize;
    self->maxDistanceInv = 1.0 / self->maxDistance;
    self->raysCount = raysCount;
    self->rays = malloc(2 * raysCount * sizeof(float));
    calcRays(self);

    return self;
}


void snakeDelete(Snake self) {
    for(ListIterator i = listGetIterator(self->body); i.present; listIteratorNext(&i)) {
        free(i.value);
    }

    listDelete(self->body);
    free(self->rays);
    free(self);
}


long snakeMove(Snake self, SnakeDirection direction) {
    SnakePoint* head = listFirst(self->body);
    SnakePoint* part = makePoint(head->x, head->y);

    if(self->bodyLength > 1) {
        if(direction == SNAKE_UP && self->previousDirection == SNAKE_DOWN) direction = self->previousDirection;
        else if(direction == SNAKE_DOWN && self->previousDirection == SNAKE_UP) direction = self->previousDirection;
        else if(direction == SNAKE_LEFT && self->previousDirection == SNAKE_RIGHT) direction = self->previousDirection;
        else if(direction == SNAKE_RIGHT && self->previousDirection == SNAKE_LEFT) direction = self->previousDirection;
    }

    switch(direction) {
        case SNAKE_UP: part->y += 1; break;
        case SNAKE_RIGHT: part->x += 1; break;
        case SNAKE_DOWN: part->y -= 1; break;
        case SNAKE_LEFT: part->x -= 1; break;
    }

    listInsert(self->body, 0, part);

    if(part->x == self->food.x && part->y == self->food.y) {
        self->bodyLength += 1;
        dropFood(self);
    }
    else {
        free(listPopLast(self->body));
        
        if(part->x < 0 || part->y < 0 || self->boardSize <= part->x || self->boardSize <= part->y) {
            return -1;
        }
        else if(isBodyPartWithoutHead(self, part)) {
            return -2;
        }
    }

    self->ticks += 1;
    self->previousDirection = direction;
    return self->ticks;
}


Matrix snakeCalcDistanceMatrix(Snake self) {
    Matrix distanceMatrix = matrixOnes(3 * self->raysCount, 1);
    // calcFoodAngleAndDistance(self, distanceMatrix);
    calcBodyAndWallDistance(self, distanceMatrix);
    return distanceMatrix;
}
