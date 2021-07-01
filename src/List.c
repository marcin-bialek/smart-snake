#include "List.h"
#include <stdlib.h>


struct ListNode {
    void* value;
    ListNode previous;
    ListNode next;
};


static ListNode nodeCreate(void* value, ListNode previous, ListNode next) {
    ListNode node = malloc(sizeof(struct ListNode));
    node->value = value;
    node->previous = previous;
    node->next = next;
    return node;
}


static ListNode getNode(List self, size_t position) {
    ListNode node = NULL;
    
    if(position <= self->size / 2) {
        for(node = self->head->next; position--; node = node->next);
    }
    else {
        position = position > self->size ? 0 : self->size - position;
        for(node = self->tail; position--; node = node->previous);
    }

    return node;
}


List listCreate(void) {
    List self = malloc(sizeof(struct List));
    self->size = 0;
    self->head = nodeCreate(NULL, NULL, NULL);
    self->tail = nodeCreate(NULL, self->head, NULL);
    self->head->next = self->tail;
    return self;
}


void listDelete(List self) {
    for(ListNode node = self->head, next; node; node = next) {
        next = node->next;
        free(node);
    }

    free(self);
}


void listInsert(List self, size_t position, void* value) {
    ListNode node = getNode(self, position);
    ListNode newNode = nodeCreate(value, node->previous, node);
    newNode->previous->next = newNode;
    newNode->next->previous = newNode;
    self->size += 1;
}


void listAppend(List self, void* value) {
    listInsert(self, self->size, value);
}


void* listGet(List self, size_t position) {
    ListNode node = getNode(self, position);
    return node->next ? node->value : NULL;
}


void* listFirst(List self) {
    return listGet(self, 0);
}


void* listLast(List self) {
    return listGet(self, self->size);
}


void* listPop(List self, size_t position) {
    position = position >= self->size ? self->size - 1 : position;
    ListNode node = getNode(self, position);

    if(node->next == NULL) {
        return NULL;
    }

    node->previous->next = node->next;
    node->next->previous = node->previous;
    void* value = node->value;
    free(node);
    self->size--;
    return value;
}


void* listPopFirst(List self) {
    return listPop(self, 0);
}


void* listPopLast(List self) {
    return listPop(self, self->size);
}


void listForEach(List self, ListForEachCallback callback, void* context) {
    ListNode node = self->head->next;
    
    for(size_t i = 0; node->next; i++) {
        callback(self, i, node->value, context);
        node = node->next;
    }
}


bool listSome(List self, ListSomeCallback callback, void* context) {
    ListNode node = self->head->next;
    
    for(size_t i = 0; node->next; i++) {
        if(callback(self, i, node->value, context)) {
            return true;
        }

        node = node->next;
    }

    return false;
}


ListIterator listGetIterator(List self) {
    return (ListIterator){
        .list = self,
        .node = self->head->next,
        .value = self->head->next->value,
        .present = self->head->next->next != NULL 
    };
}


void listIteratorNext(ListIterator* iterator) {
    if(iterator->present) {
        iterator->node = iterator->node->next;
        iterator->value = iterator->node->value;
        iterator->present = iterator->node->next != NULL;
    }
}
