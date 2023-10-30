#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

typedef double elem_t;

namespace LinkedList {
    const size_t DEFAULT_CAPACITY = 10;

    enum ListErrorCode {
        NO_LIST_ERRORS    = 0,
        LIST_NULL_POINTER = 1 << 0,
        PREV_NULL_POINTER = 1 << 1,
        NEXT_NULL_POINTER = 1 << 2,
        DATA_NULL_POINTER = 1 << 3,
    };

    struct List {
        elem_t *data;

        size_t *next;
        size_t *prev;

        size_t capacity;

        size_t head;
        size_t tail;

        ListErrorCode errors;
    };

    ListErrorCode InitList    (List *list, size_t capacity = DEFAULT_CAPACITY);
    ListErrorCode DestroyList (List *list);
    ListErrorCode InsertAfter (List *list, size_t insertIndex, size_t *newIndex);
    ListErrorCode DeleteValue (List *list, size_t deleteIndex);
    ListErrorCode VerifyList  (List *list);
    ListErrorCode DumpList    (List *list);

}
#endif
