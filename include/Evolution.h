#pragma once 
#include "wasm.h"
#include "Network.h"


typedef struct {
    size_t populationSize;
    size_t snakeBoardSize;
    size_t snakeRaysCount;
    size_t networkHiddenLayersCount;
    size_t networkUnitsCount;
    ActivationFunction networkActivation;
    float f;
    bool normalizeMatrices;
    bool chooseBest;
} EvolutionParams;

typedef struct {
    Network network;
    float score;
} Chromosome;

struct Evolution {
    EvolutionParams params;
    size_t bestIndex;
    Chromosome* population;
};

typedef struct Evolution* Evolution;


WASM_EXPORT Evolution evolutionCreate(EvolutionParams params);
WASM_EXPORT void evolutionDelete(Evolution evolution);
WASM_EXPORT void evolutionInit(Evolution evolution);
WASM_EXPORT void evolutionNextGeneration(Evolution evolution);
WASM_EXPORT float evolutionGetBestScore(Evolution evolution);
WASM_EXPORT Network evolutionGetBestNetwork(Evolution evolution);

WASM_EXPORT Evolution jsEvolutionCreate(void);
