#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stddef.h>
#include <sys/types.h>
#include <math.h>

typedef double elem_t;

namespace LinkedList {
    const elem_t EPS              = 1e-5;

    enum ListErrorCode {
        NO_LIST_ERRORS          = 0,
        LIST_NULL_POINTER       = 1 << 0,
        GRAPHVIZ_BUFFER_ERROR   = 1 << 1,
        LOG_FILE_ERROR          = 1 << 2,
        INVALID_CAPACITY        = 1 << 3,
        INVALID_HEAD            = 1 << 4,
        INVALID_TAIL            = 1 << 5,
        NODE_NULL_POINTER       = 1 << 6,
    };

    struct CallingFileData {
        int line             = -1;
        const char *file     = NULL;
        const char *function = NULL;
    };

    struct Node {
        elem_t data = NAN;

        Node *next = NULL;
        Node *prev = NULL;
    };

    struct List {
        Node *head      = NULL;
        Node *tail      = NULL;

        ssize_t capacity = 0;

        ListErrorCode errors;
        CallingFileData creationData;
    };

    ListErrorCode InitList_    (List *list, CallingFileData creationData);
    ListErrorCode DestroyList_ (List *list);
    ListErrorCode InsertAfter_ (List *list, Node *insertPointer, Node **newPointer, elem_t element, CallingFileData callData);
    ListErrorCode DeleteValue_ (List *list, Node *deletePointer, CallingFileData callData);
    ListErrorCode VerifyList_  (List *list);
    ListErrorCode DumpList_    (List *list, char *logFolder, CallingFileData callData);

    ListErrorCode FindValueInListSlowImplementation_ (List *list, elem_t value, Node **foundValuePointer, CallingFileData callData);

    #define CreateCallingFileData {__LINE__, __FILE__, __PRETTY_FUNCTION__}

    #define InitList(list)                                          InitList_    (list, CreateCallingFileData)
    #define InsertAfter(list, insertPointer, newPointer, element)   InsertAfter_ (list, insertPointer, newPointer, element, CreateCallingFileData)
    #define DeleteValue(list, deletePointer)                        DeleteValue_ (list, deletePointer, CreateCallingFileData)
    #define DumpList(list, logFolder)                               DumpList_    (list, logFolder, CreateCallingFileData)
    #define DestroyList(list)                                       DestroyList_ (list)
    #define VerifyList(list)                                        VerifyList_  (list)

    #define FindValueInListSlowImplementation(list, value, foundValuePointer)\
                FindValueInListSlowImplementation_ (list, value, foundValuePointer, CreateCallingFileData);

}
#endif
