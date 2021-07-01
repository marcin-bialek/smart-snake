#pragma once 
#include <stddef.h>
#include <stdbool.h>


struct ListNode;
typedef struct ListNode* ListNode;

struct List {
    size_t size;
    ListNode head;
    ListNode tail;
};

typedef struct List* List;

typedef struct {
    List list;
    bool present;
    void* value;
    ListNode node;
} ListIterator;

typedef void (*ListForEachCallback)(List list, size_t index, void* value, void* context);
typedef bool (*ListSomeCallback)(List list, size_t index, void* value, void* context);


List listCreate(void);
void listDelete(List list);
void listAppend(List list, void* value);
void listInsert(List list, size_t position, void* value);
void* listGet(List list, size_t position);
void* listFirst(List list);
void* listLast(List list);
void* listPop(List list, size_t position);
void* listPopFirst(List list);
void* listPopLast(List list);
void listForEach(List list, ListForEachCallback callback, void* context);
bool listSome(List list, ListSomeCallback callback, void* context);
ListIterator listGetIterator(List list);
void listIteratorNext(ListIterator* iterator);
