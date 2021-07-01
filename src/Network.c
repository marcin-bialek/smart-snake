#include "Network.h"
#include <stdlib.h>


Network networkCreate(void) {
    Network self = malloc(sizeof(struct Network));
    self->layers = listCreate();
    return self;
}


Network networkCopy(Network self) {
    Network copy = networkCreate();
    copy->inputSize = self->inputSize;
    copy->outputSize = self->outputSize;

    for(ListIterator i = listGetIterator(self->layers); i.present; listIteratorNext(&i)) {
        NetworkLayer layer = i.value;

        networkAddLayer(copy, (NetworkLayerParams){
            .weights = matrixCopy(layer->weights),
            .bias = matrixCopy(layer->bias),
            .activation = layer->activation
        });
    }

    networkCompile(copy);
    return copy;
}


void networkDelete(Network self) {
    for(ListIterator i = listGetIterator(self->layers); i.present; listIteratorNext(&i)) {
        matrixDelete(((NetworkLayer)i.value)->weights);
        matrixDelete(((NetworkLayer)i.value)->bias);
        free(i.value);
    }

    listDelete(self->layers);
    free(self);
}


void networkAddLayer(Network self, NetworkLayerParams params) {
    NetworkLayer layer = malloc(sizeof(struct NetworkLayer));
    layer->params = params;
    layer->weights = NULL;
    layer->bias = NULL;
    layer->activation = params.activation;
    listAppend(self->layers, layer);
}


static void compileLayer(List layers, size_t i, NetworkLayer layer, size_t* size) {
    if(layer->params.weights) {
        layer->weights = layer->params.weights;
    }
    else {
        layer->weights = matrixNew(layer->params.units, *size);
        layer->params.initializer(layer->weights, NULL);
    }

    if(layer->params.bias) {
        layer->bias = layer->params.bias;
    }
    else {
        layer->bias = matrixNew(layer->params.units, 1);
        layer->params.initializer(layer->bias, NULL);
    }

    *size = layer->params.units;
}


void networkCompile(Network self) {
    NetworkLayer inputLayer = listFirst(self->layers);
    size_t size = inputLayer->params.inputSize;
    listForEach(self->layers, (ListForEachCallback)compileLayer, &size);
    self->inputSize = inputLayer->params.inputSize;
    self->outputSize = size;
}


static void layerFit(List layers, size_t i, NetworkLayer layer, Matrix* x) {
    Matrix y = matrixDot(layer->weights, *x);
    matrixAddInPlace(y, layer->bias);
    layer->activation(y, NULL);

    if(i > 0) {
        matrixDelete(*x);
    }

    *x = y;
}


Matrix networkFit(Network self, Matrix input) {
    Matrix x = input;
    listForEach(self->layers, (ListForEachCallback)layerFit, &x);
    return x;
}

