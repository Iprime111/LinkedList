#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stddef.h>
#include <sys/types.h>

typedef double elem_t;

namespace LinkedList {
    const size_t REALLOC_SCALE    = 2;

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
        INVALID_CAPACITY        = 1 << 8,
        INVALID_HEAD            = 1 << 9,
        INVALID_TAIL            = 1 << 10,
    };

    struct CallingFileData {
        int line             = -1;
        const char *file     = NULL;
        const char *function = NULL;
    };

    struct List {
        elem_t *data        = NULL;

        ssize_t *next       = NULL;
        ssize_t *prev       = NULL;

        ssize_t capacity    = -1;

        ssize_t freeElem    = -1;

        ListErrorCode errors;
        CallingFileData creationData;
    };

    ListErrorCode InitList    (List *list, size_t capacity, CallingFileData creationData);
    ListErrorCode DestroyList (List *list);
    ListErrorCode InsertAfter (List *list, ssize_t insertIndex, ssize_t *newIndex, elem_t element, CallingFileData callData);
    ListErrorCode DeleteValue (List *list, ssize_t deleteIndex, CallingFileData callData);
    ListErrorCode VerifyList  (List *list);
    ListErrorCode DumpList    (List *list, char *logFolder, CallingFileData callData);

    ListErrorCode FindValueInListSlowImplementation (List *list, elem_t value);

    #define CreateCallingFileData {__LINE__, __FILE__, __PRETTY_FUNCTION__}

    #define InitList_(list, capacity)                          InitList (list, capacity, CreateCallingFileData)
    #define InsertAfter_(list, insertIndex, newIndex, element) InsertAfter (list, insertIndex, newIndex, element, CreateCallingFileData)
    #define DeleteValue_(list, deleteIndex)                    DeleteValue (list, deleteIndex, CreateCallingFileData)
    #define DumpList_(list, logFolder)                         DumpList (list, logFolder, CreateCallingFileData)

}
#endif
