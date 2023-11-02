#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stddef.h>
#include <sys/types.h>

typedef double elem_t;

namespace LinkedList {
    const size_t DEFAULT_CAPACITY = 10;

    enum ListErrorCode {
        NO_LIST_ERRORS          = 0,
        LIST_NULL_POINTER       = 1 << 0,
        PREV_NULL_POINTER       = 1 << 1,
        NEXT_NULL_POINTER       = 1 << 2,
        DATA_NULL_POINTER       = 1 << 3,
        FREE_LIST_ERROR         = 1 << 4,
        WRONG_INDEX             = 1 << 5,
        GRAPHVIZ_BUFFER_ERROR   = 1 << 6,
        LOG_FILE_ERROR          = 1 << 7,
    };

    struct List {
        elem_t *data;

        ssize_t *next;
        ssize_t *prev;

        ssize_t capacity;

        ssize_t tail;
        ssize_t freeElem;

        ListErrorCode errors;
    };

    ListErrorCode InitList    (List *list, size_t capacity = DEFAULT_CAPACITY);
    ListErrorCode DestroyList (List *list);
    ListErrorCode InsertAfter (List *list, ssize_t insertIndex, ssize_t *newIndex, elem_t element);
    ListErrorCode DeleteValue (List *list, ssize_t deleteIndex);
    ListErrorCode VerifyList  (List *list);
    ListErrorCode DumpList    (List *list, char *logFolder);

}
#endif
