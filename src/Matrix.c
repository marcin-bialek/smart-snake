#include "Matrix.h"
#include <stdlib.h>
#include <string.h>


Matrix matrixNew(matrix_size_t rows, matrix_size_t columns) {
    Matrix self = malloc(sizeof(struct Matrix));
    self->rows = rows;
    self->columns = columns;
    self->size = rows * columns;
    self->values = malloc(self->size * sizeof(matrix_number_t));
    return self;
}


Matrix matrixCopy(Matrix other) {
    Matrix self = matrixNew(other->rows, other->columns);
    memcpy(self->values, other->values, self->size * sizeof(matrix_number_t));
    return self;
}


Matrix matrixZeros(matrix_size_t rows, matrix_size_t columns) {
    Matrix self = matrixNew(rows, columns);
    bzero(self->values, self->size * sizeof(matrix_number_t));
    return self;
}


Matrix matrixOnes(matrix_size_t rows, matrix_size_t columns) {
    Matrix self = matrixNew(rows, columns);
    
    for(matrix_index_t i = 0; i < self->size; i++) {
        self->values[i] = 1.0;
    }

    return self;
}


Matrix matrixIdentity(matrix_size_t size) {
    Matrix self = matrixNew(size, size);
    matrix_number_t* p = self->values;

    for(matrix_index_t i = 0; i < self->rows; i++) {
        for(matrix_index_t j = 0; j < self->columns; j++) {
            *p = (i == j) ? 1.0 : 0.0;
            p += 1;
        }
    }

    return self;
}


void matrixDelete(Matrix self) {
    free(self->values);
    free(self);
}


inline matrix_number_t matrixGet(Matrix self, matrix_index_t i, matrix_index_t j) {
    return self->values[i * self->columns + j];
}


inline void matrixSet(Matrix self, matrix_index_t i, matrix_index_t j, matrix_number_t value) {
    self->values[i * self->columns + j] = value;
}


Matrix matrixAddInPlace(Matrix first, Matrix second) {
    if(first->rows != second->rows || first->columns != second->columns) {
        return NULL;
    }

    for(matrix_index_t i = 0; i < first->size; i++) {
        first->values[i] = first->values[i] + second->values[i];
    }

    return first;
}


Matrix matrixAdd(Matrix first, Matrix second) {
    if(first->rows != second->rows || first->columns != second->columns) {
        return NULL;
    }

    return matrixAddInPlace(matrixCopy(first), second);
}


Matrix matrixSubstractInPlace(Matrix first, Matrix second) {
    if(first->rows != second->rows || first->columns != second->columns) {
        return NULL;
    }

    for(matrix_index_t i = 0; i < first->size; i++) {
        first->values[i] = first->values[i] - second->values[i];
    }

    return first;
}


Matrix matrixSubstract(Matrix first, Matrix second) {
    if(first->rows != second->rows || first->columns != second->columns) {
        return NULL;
    }

    return matrixSubstractInPlace(matrixCopy(first), second);
}


Matrix matrixMultiplyInPlace(Matrix matrix, matrix_number_t scalar) {
    for(matrix_index_t i = 0; i < matrix->size; i++) {
        matrix->values[i] = matrix->values[i] * scalar;
    }

    return matrix;
}


Matrix matrixMultiply(Matrix matrix, matrix_number_t scalar) {
    return matrixMultiplyInPlace(matrixCopy(matrix), scalar);
}


Matrix matrixDot(Matrix first, Matrix second) {
    if(first->columns != second->rows) {
        return NULL;
    }

    Matrix result = matrixZeros(first->rows, second->columns);
    matrix_number_t* c = result->values;

    for(matrix_index_t i = 0; i < first->rows; i++) {
        for(matrix_index_t j = 0; j < second->columns; j++) {
            matrix_number_t* a = first->values + (i * first->columns);
            matrix_number_t* b = second->values + j;

            for(matrix_index_t r = 0; r < first->columns; r++) {
                *c += *a * *b;
                a += 1;
                b += second->columns;
            }

            c += 1;
        }
    }

    return result;
}

