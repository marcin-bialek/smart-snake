#pragma once 
#include "Matrix.h"


typedef Matrix (*ActivationFunction)(Matrix input, void* context);

struct ActivationFunctions {
    const ActivationFunction relu;
    const ActivationFunction leakyRelu;
    const ActivationFunction softmax;
    const ActivationFunction sigmoid;
};

extern const struct ActivationFunctions ActivationFunctions;
