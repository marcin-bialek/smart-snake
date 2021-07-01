#pragma once 
#include "wasm.h"
#include "Matrix.h"
#include "ActivationFunctions.h"
#include "List.h"


typedef void (*NetworkLayerInitializer)(Matrix matrix, void* context);

typedef struct {
    size_t units;
    size_t inputSize;
    ActivationFunction activation;
    NetworkLayerInitializer initializer;
    Matrix weights;
    Matrix bias;
} NetworkLayerParams;

struct NetworkLayer;
typedef struct NetworkLayer* NetworkLayer;

struct NetworkLayer {
    NetworkLayerParams params;
    Matrix weights;
    Matrix bias;
    ActivationFunction activation;
};

struct Network {
    size_t inputSize;
    size_t outputSize;
    List layers;
};

typedef struct Network* Network;


WASM_EXPORT Network networkCreate(void);
WASM_EXPORT Network networkCopy(Network network);
WASM_EXPORT void networkDelete(Network network);
WASM_EXPORT void networkAddLayer(Network network, NetworkLayerParams params);
WASM_EXPORT void networkCompile(Network network);
WASM_EXPORT Matrix networkFit(Network network, Matrix input);
