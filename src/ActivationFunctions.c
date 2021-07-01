#include "ActivationFunctions.h"
#include <math.h>


static Matrix relu(Matrix input, void* context) {
    for(matrix_index_t i = 0; i < input->size; i++) {
        input->values[i] *= (input->values[i] > 0.0) ? 1.0 : 0.0;
    }

    return input;
}


static Matrix leakyRelu(Matrix input, void* context) {
    for(matrix_index_t i = 0; i < input->size; i++) {
        input->values[i] *= (input->values[i] > 0.0) ? 1.0 : 0.01;
    }

    return input;
}


static Matrix softmax(Matrix input, void* context) {
    matrix_number_t sum = 0.0;

    for(matrix_index_t i = 0; i < input->size; i++) {
        input->values[i] = expf(input->values[i]);
        sum += input->values[i];
    }

    for(matrix_index_t i = 0; i < input->size; i++) {
        input->values[i] = input->values[i] / sum;
    }

    return input;
}


static Matrix sigmoid(Matrix input, void* context) {
    for(matrix_index_t i = 0; i < input->size; i++) {
        input->values[i] = 1.0f / (1.0f + expf(-input->values[i]));
    }

    return input;
}


const struct ActivationFunctions ActivationFunctions = {
    .relu = relu,
    .leakyRelu = leakyRelu,
    .softmax = softmax,
    .sigmoid = sigmoid
};
