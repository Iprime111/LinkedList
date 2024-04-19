#ifndef LINKED_LIST_DEFINITIONS_HPP_
#define LINKED_LIST_DEFINITIONS_HPP_

#include <stddef.h>
#include <sys/types.h>

namespace LinkedList {
    const size_t REALLOC_SCALE = 2;

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

    template <typename elem_t>
    struct List {
        elem_t *data        = NULL;

        ssize_t *next       = NULL;
        ssize_t *prev       = NULL;

        ssize_t capacity    = -1;

        ssize_t freeElem    = -1;

        ListErrorCode errors;
        CallingFileData creationData;
    };

    template <typename elem_t>
    ListErrorCode InitList_    (List <elem_t> *list, size_t capacity, CallingFileData creationData);
    template <typename elem_t>
    ListErrorCode DestroyList_ (List <elem_t> *list);
    template <typename elem_t>
    ListErrorCode InsertAfter_ (List <elem_t> *list, ssize_t insertIndex, ssize_t *newIndex, elem_t element, CallingFileData callData);
    template <typename elem_t>
    ListErrorCode DeleteValue_ (List <elem_t> *list, ssize_t deleteIndex, CallingFileData callData);
    template <typename elem_t>
    ListErrorCode VerifyList_  (List <elem_t> *list);
    template <typename elem_t>
    ListErrorCode DumpList_    (List <elem_t> *list, char *logFolder, CallingFileData callData);
    template <typename elem_t>
    ListErrorCode FindValueInListSlowImplementation_ (List <elem_t> *list, elem_t value, ssize_t *index, CallingFileData callData);

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
