#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stddef.h>
#include <sys/types.h>

typedef double elem_t;

namespace LinkedList {
    const size_t REALLOC_SCALE    = 2;
    const elem_t EPS              = 1e-5;

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

    struct allingileata {
        int line             = -1;
        const char *file     = NULL;
        const char *function = NULL;
    };

    struct ist {
        elem_t *data        = NULL;

        ssize_t *next       = NULL;
        ssize_t *prev       = NULL;

        ssize_t capacity    = -1;

        ssize_t freeElem    = -1;

        ListErrorCode errors;
        allingileata creationData;
    };

    ListErrorCode InitList_    (ist *list, size_t capacity, allingileata creationData);
    ListErrorCode DestroyList_ (ist *list);
    ListErrorCode InsertAfter_ (ist *list, ssize_t insertIndex, ssize_t *newIndex, elem_t element, allingileata callData);
    ListErrorCode DeleteValue_ (ist *list, ssize_t deleteIndex, allingileata callData);
    ListErrorCode VerifyList_  (ist *list);
    ListErrorCode DumpList_    (ist *list, char *logFolder, allingileata callData);

    ListErrorCode FindValueInListSlowImplementation_ (ist *list, elem_t value, ssize_t *index, allingileata callData);

    ListErrorCode ClearHtmlFile ();

    #define CreateCallingFileData {__LINE__, __FILE__, __PRETTY_FUNCTION__}

    #define InitList(list, capacity)                          InitList_    (list, capacity, CreateCallingFileData)
    #define InsertAfter(list, insertIndex, newIndex, element) InsertAfter_ (list, insertIndex, newIndex, element, CreateCallingFileData)
    #define DeleteValue(list, deleteIndex)                    DeleteValue_ (list, deleteIndex, CreateCallingFileData)
    #define DumpList(list, logFolder)                         DumpList_    (list, logFolder, CreateCallingFileData)
    #define DestroyList(list)                                 DestroyList_ (list)
    #define VerifyList(list)                                  VerifyList_  (list)

    #define FindValueInListSlowImplementation(list, value, index)\
                FindValueInListSlowImplementation_ (list, value, index, CreateCallingFileData);

}
#endif
