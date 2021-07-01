#include "Evolution.h"
#include "Snake.h"
#include "random.h"
#include <stdlib.h>
#include <math.h>


static void randomInitializer(Matrix matrix, void* ctx) {
    for(matrix_index_t i = 0; i < matrix->size; i++) {
        matrix->values[i] = 2.0 * randomFloat() - 1.0;
    }
}


static Network makeRandomNetwork(Evolution self) {
    Network network = networkCreate();

    networkAddLayer(network, (NetworkLayerParams){
        .inputSize = 3 * self->params.snakeRaysCount,
        .units = self->params.networkUnitsCount,
        .activation = self->params.networkActivation,
        .initializer = randomInitializer
    });

    for(size_t i = 0; i < self->params.networkHiddenLayersCount; i++) {
        networkAddLayer(network, (NetworkLayerParams){
            .units = self->params.networkUnitsCount,
            .activation = self->params.networkActivation,
            .initializer = randomInitializer
        });   
    }

    networkAddLayer(network, (NetworkLayerParams){
        .units = 4,
        .activation = ActivationFunctions.softmax,
        .initializer = randomInitializer
    });

    networkCompile(network);
    return network;
}


static matrix_index_t getMaxIndex(Matrix matrix) {
    matrix_index_t maxIndex = 0;

    for(matrix_index_t i = 1; i < matrix->size; i++) {
        if(matrix->values[i] > matrix->values[maxIndex]) {
            maxIndex = i;
        }
    }

    return maxIndex;
}


static float evaluate(Evolution self, Network network) {
    Snake snake = snakeCreate(self->params.snakeBoardSize, self->params.snakeRaysCount);
    size_t length = 1;
    size_t life = 100;
    size_t ticks = 0;
    long r;

    while(life > 0) {
        life -= 1;
        ticks += 1;

        Matrix distanceMatrix = snakeCalcDistanceMatrix(snake);
        Matrix output = networkFit(network, distanceMatrix);
        SnakeDirection direction = getMaxIndex(output);
        matrixDelete(distanceMatrix);
        matrixDelete(output);

        if((r = snakeMove(snake, direction)) >= 0) {
            if(snake->body->size > length) {
                life = 100;
                length = snake->body->size;
            }
        }
        else {
            break;
        }
    }

    snakeDelete(snake);
    return (powf(length, 1.5) - (float)ticks / 100.0); // * (r == -1 ? 0.75 : 1.0);
}


static float evaluateAvg(Evolution self, Network network) {
    float score = 0.0;

    for(int i = 0; i < 5; i++) {
        score += evaluate(self, network);
    }

    return score / 5;
}


Evolution evolutionCreate(EvolutionParams params) {
    Evolution self = malloc(sizeof(struct Evolution));
    self->params = params;
    self->bestIndex = 0;
    self->population = malloc(params.populationSize * sizeof(Chromosome));
    return self;
}


void evolutionDelete(Evolution self) {
    for(size_t i = 0; i < self->params.populationSize; i++) {
        networkDelete(self->population[i].network);
    }

    free(self->population);
    free(self);
}


void evolutionInit(Evolution self) {
    for(size_t i = 0; i < self->params.populationSize; i++) {
        self->population[i].network = makeRandomNetwork(self);
        self->population[i].score = evaluateAvg(self, self->population[i].network);

        if(self->population[i].score > self->population[self->bestIndex].score) {
            self->bestIndex = i;
        }
    }
}


static Matrix crossMatrices(Matrix a, Matrix b) {
    for(matrix_index_t i = 0; i < a->size; i++) {
        a->values[i] = randomFloat() < 0.5 ? a->values[i] : b->values[i];
    }

    return a;
}


static Matrix normalizeMatrix(Matrix a) {
    matrix_number_t max = 0.0;

    for(matrix_index_t i = 0; i < a->size; i++) {
        if(a->values[i] > max) {
            max = fabsf(a->values[i]); 
        }
    }

    if(max > 1.0) {
        for(matrix_index_t i = 0; i < a->size; i++) {
            a->values[i] = a->values[i] / max;
        }
    }

    return a;
}


void evolutionNextGeneration(Evolution self) {
    Chromosome* newPopulation = malloc(self->params.populationSize * sizeof(Chromosome));
    Chromosome* best = &self->population[self->bestIndex];
    size_t newBestIndex = 0;

    for(size_t i = 0; i < self->params.populationSize; i++) {
        Network m = networkCreate();
        Chromosome* a = &self->population[randomInt(0, self->params.populationSize - 1)];
        Chromosome* b = &self->population[randomInt(0, self->params.populationSize - 1)];

        if(self->params.chooseBest == false) {
            best = &self->population[randomInt(0, self->params.populationSize - 1)];
        }

        ListIterator p = listGetIterator(best->network->layers);
        ListIterator q = listGetIterator(a->network->layers);
        ListIterator r = listGetIterator(b->network->layers);

        while(p.present) {
            NetworkLayer pl = p.value;
            NetworkLayer ql = q.value;
            NetworkLayer rl = r.value;

            Matrix weights = matrixSubstract(rl->weights, ql->weights);
            Matrix bias = matrixSubstract(rl->bias, ql->bias);
            matrixMultiplyInPlace(weights, self->params.f);
            matrixMultiplyInPlace(bias, self->params.f);
            matrixAddInPlace(weights, pl->weights);
            matrixAddInPlace(bias, pl->bias);
            crossMatrices(weights, pl->weights);
            crossMatrices(bias, pl->bias);

            if(self->params.normalizeMatrices) {
                normalizeMatrix(weights);
                normalizeMatrix(bias);
            }

            networkAddLayer(m, (NetworkLayerParams){
                .weights = weights,
                .bias = bias,
                .activation = pl->activation
            });

            listIteratorNext(&p);
            listIteratorNext(&q);
            listIteratorNext(&r);
        }

        networkCompile(m);
        const float score = evaluateAvg(self, m);

        if(score > self->population[i].score) {
            newPopulation[i].network = m;
            newPopulation[i].score = score;
        }
        else {
            networkDelete(m);
            newPopulation[i].network = networkCopy(self->population[i].network);
            newPopulation[i].score = self->population[i].score;
        }

        if(newPopulation[i].score > newPopulation[newBestIndex].score) {
            newBestIndex = i;
        }
    }

    for(size_t i = 0; i < self->params.populationSize; i++) {
        networkDelete(self->population[i].network);
    }

    free(self->population);
    self->population = newPopulation;
    self->bestIndex = newBestIndex;
}


float evolutionGetBestScore(Evolution self) {
    return self->population[self->bestIndex].score;
}


Network evolutionGetBestNetwork(Evolution self) {
    return self->population[self->bestIndex].network;
}


Evolution jsEvolutionCreate(void) {
    return evolutionCreate((EvolutionParams){
        .populationSize = 100,
        .snakeBoardSize = 15,
        .snakeRaysCount = 4,
        .networkHiddenLayersCount = 0,
        .networkUnitsCount = 32,
        .networkActivation = ActivationFunctions.leakyRelu,
        .f = 0.5,
        .normalizeMatrices = true,
        .chooseBest = false
    });
}
