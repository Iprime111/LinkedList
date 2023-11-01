#include <cmath>
#include <cstring>
#include <stdlib.h>

#include "LinkedList.h"
#include "CustomAssert.h"
#include "Logger.h"

#define Verification(list)                              \
    do {                                                \
        ListErrorCode errorCode_ = VerifyList (list);   \
        if (errorCode_ != NO_LIST_ERRORS) {             \
            DumpList (list);                            \
            RETURN errorCode_;                          \
        }                                               \
    }while (0)

#ifndef NDEBUG
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

namespace LinkedList {
    ListErrorCode InitList (List *list, size_t capacity) {
        PushLog (3);

        if (!list) {
            RETURN LIST_NULL_POINTER;
        }

        list->head = 0;
        list->tail = 0;

        list->capacity = capacity + 1;

        list->next = (size_t *) calloc (list->capacity, sizeof (size_t));
        list->prev = (size_t *) calloc (list->capacity, sizeof (size_t));
        list->data = (elem_t *) calloc (list->capacity, sizeof (elem_t));

        list->data [0] = NAN;
        list->prev [0] = 0;
        list->next [0] = 0;

        list->freeElem = 1;

        for (size_t listIndex = list->freeElem; listIndex < list->capacity; listIndex++) {
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

        memset (list->data, 0, list->capacity * sizeof (elem_t));
        memset (list->prev, 0, list->capacity * sizeof (size_t));
        memset (list->next, 0, list->capacity * sizeof (size_t));

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

        if (!list->data)
            ReturnErrors (list, DATA_NULL_POINTER);

        if (!list->prev)
            ReturnErrors (list, PREV_NULL_POINTER);

        if (!list->next)
            ReturnErrors (list, NEXT_NULL_POINTER);

        size_t freeIndex = list->freeElem;

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

    ListErrorCode InsertAfter (List *list, size_t insertIndex, size_t *newIndex, elem_t element) {
        PushLog (3);

        Verification (list);

        if (insertIndex < 0 || insertIndex >= list->capacity) {
            RETURN WRONG_INDEX;
        }

        if (list->freeElem == 0) {
            // TODO realloc
        }

        *newIndex = list->freeElem;
        list->freeElem = list->next [list->freeElem];

        list->next [insertIndex] = *newIndex;
        list->prev [*newIndex]   = insertIndex;
        list->data [*newIndex]   = element;

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode DeleteValue (List *list, size_t deleteIndex) {
        PushLog (3);

        VerifyList (list);

        if (deleteIndex <= 0) {
            RETURN WRONG_INDEX;
        }

        list->prev [list->next [deleteIndex]] = list->prev [deleteIndex];
        list->next [list->prev [deleteIndex]] = list->next [deleteIndex];

        list->next [list->freeElem] = deleteIndex;
        list->prev [deleteIndex]    = list->freeElem;
        list->freeElem              = deleteIndex;

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode DumpList (List *list) {
        PushLog (3);

        RETURN NO_LIST_ERRORS;
    }
}
