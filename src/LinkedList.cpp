#include <cmath>
#include <cstring>
#include <stdlib.h>

#include "LinkedList.h"
#include "GraphVizDump.h"
#include "CustomAssert.h"
#include "Logger.h"

#ifndef NDEBUG
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

#define Verification(list)                              \
    do {                                                \
        ListErrorCode errorCode_ = VerifyList (list);   \
        if (errorCode_ != NO_LIST_ERRORS) {             \
            ON_DEBUG (DumpList (list, "."));            \
            RETURN errorCode_;                          \
        }                                               \
    }while (0)

namespace LinkedList {
    ListErrorCode InitList (List *list, size_t capacity) {
        PushLog (3);

        if (!list) {
            RETURN LIST_NULL_POINTER;
        }

        list->tail = 0;

        list->capacity = (ssize_t) capacity + 1;

        list->next = (ssize_t *) calloc ((size_t) list->capacity, sizeof (ssize_t));
        list->prev = (ssize_t *) calloc ((size_t) list->capacity, sizeof (ssize_t));
        list->data = (elem_t *)  calloc ((size_t) list->capacity, sizeof (elem_t));

        list->data [0] = NAN;
        list->prev [0] = 0;
        list->next [0] = 0;

        list->freeElem = 1;

        for (ssize_t listIndex = list->freeElem; listIndex < list->capacity; listIndex++) {
            list->next [listIndex] = (listIndex + 1) % list->capacity;
            list->prev [listIndex] = -1;
        }

        Verification (list);

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode DestroyList (List *list) {
        PushLog (3);

        if (!list) {
            RETURN LIST_NULL_POINTER;
        }

        memset (list->data, 0, (size_t) list->capacity * sizeof (elem_t));
        memset (list->prev, 0, (size_t) list->capacity * sizeof (size_t));
        memset (list->next, 0, (size_t) list->capacity * sizeof (size_t));

        free (list->data);
        free (list->prev);
        free (list->next);

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode VerifyList (List *list) {
        PushLog (3);

        #define WriteErrors(list, errorCodes)  (list)->errors = (ListErrorCode) ((list)->errors | (errorCodes))
        #define ReturnErrors(list, errorCodes) WriteErrors (list, errorCodes); RETURN (list)->errors

        if (!list) {
            RETURN LIST_NULL_POINTER;
        }

        if (!list->data) {
            ReturnErrors (list, DATA_NULL_POINTER);
        }

        if (!list->prev) {
            ReturnErrors (list, PREV_NULL_POINTER);
        }

        if (!list->next) {
            ReturnErrors (list, NEXT_NULL_POINTER);
        }

        ssize_t freeIndex = list->freeElem;

        while (freeIndex > 0) {
            if (list->prev [freeIndex] > 0) {
                WriteErrors (list, FREE_LIST_ERROR);
            }

            freeIndex = list->next [freeIndex];
        }

        #undef WriteErrors
        #undef ReturnErrors

        RETURN list->errors;
    }

    ListErrorCode InsertAfter (List *list, ssize_t insertIndex, ssize_t *newIndex, elem_t element) {
        PushLog (3);

        custom_assert (newIndex, pointer_is_null, WRONG_INDEX);

        Verification (list);

        if (insertIndex < 0 || insertIndex >= list->capacity) {
            RETURN WRONG_INDEX;
        }

        if (list->freeElem == 0) {
            // TODO realloc
        }

        *newIndex = list->freeElem;
        list->freeElem = list->next [list->freeElem];

        list->next [*newIndex]   = list->next [insertIndex];
        list->next [insertIndex] = *newIndex;
        list->prev [*newIndex]   = insertIndex;
        list->data [*newIndex]   = element;

        if (list->tail == insertIndex)
            list->tail = *newIndex;

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode DeleteValue (List *list, ssize_t deleteIndex) {
        PushLog (3);

        VerifyList (list);

        if (deleteIndex <= 0) {
            RETURN WRONG_INDEX;
        }

        if (list->prev [deleteIndex] == -1) {
            RETURN WRONG_INDEX;
        }

        if (list->tail == deleteIndex)
            list->tail = list->prev [deleteIndex];

        list->prev [list->next [deleteIndex]] = list->prev [deleteIndex];
        list->next [list->prev [deleteIndex]] = list->next [deleteIndex];

        list->next [deleteIndex]    = list->freeElem;
        list->prev [deleteIndex]    = -1;
        list->freeElem              = deleteIndex;

        RETURN NO_LIST_ERRORS;
    }
}
