#pragma once
#include "wasm.h"
#include <stddef.h>


typedef size_t matrix_size_t;
typedef size_t matrix_index_t;
typedef float matrix_number_t;

struct Matrix {
    matrix_size_t rows; 
    matrix_size_t columns;  
    matrix_size_t size;
    matrix_number_t* values; 
};

typedef struct Matrix* Matrix;

 
WASM_EXPORT Matrix matrixNew(matrix_size_t rows, matrix_size_t columns);
WASM_EXPORT Matrix matrixCopy(Matrix other);
WASM_EXPORT Matrix matrixZeros(matrix_size_t rows, matrix_size_t columns);
WASM_EXPORT Matrix matrixOnes(matrix_size_t rows, matrix_size_t columns);
WASM_EXPORT Matrix matrixIdentity(matrix_size_t size);
WASM_EXPORT void matrixDelete(Matrix matrix);

WASM_EXPORT matrix_number_t matrixGet(Matrix matrix, matrix_index_t i, matrix_index_t j);
WASM_EXPORT void matrixSet(Matrix matrix, matrix_index_t i, matrix_index_t j, matrix_number_t value);

WASM_EXPORT Matrix matrixAdd(Matrix first, Matrix second);
WASM_EXPORT Matrix matrixAddInPlace(Matrix first, Matrix second);
WASM_EXPORT Matrix matrixSubstract(Matrix first, Matrix second);
WASM_EXPORT Matrix matrixSubstractInPlace(Matrix first, Matrix second);
WASM_EXPORT Matrix matrixMultiply(Matrix matrix, matrix_number_t scalar);
WASM_EXPORT Matrix matrixMultiplyInPlace(Matrix matrix, matrix_number_t scalar);
WASM_EXPORT Matrix matrixDot(Matrix first, Matrix second);
