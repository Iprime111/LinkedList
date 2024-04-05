#include <cmath>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>
#include <sys/types.h>

#include "LinkedList.h"
#include "GraphVizDump.h"
#include "CustomAssert.h"
#include "Logger.h"

#ifndef NDEBUG
    #define ON_DEBUG(...) __VA_ARGS__
#else
    #define ON_DEBUG(...)
#endif

#define Verification(list, callData)                        \
    ON_DEBUG (                                              \
        do {                                                \
            ListErrorCode errorCode_ = VerifyList_ (list);  \
            if (errorCode_ != NO_LIST_ERRORS) {             \
                ON_DEBUG (DumpList_ (list, ".", callData)); \
                RETURN errorCode_;                          \
            }                                               \
        } while (0)                                         \
    )

namespace LinkedList {

    ListErrorCode InitList_ (List *list, size_t capacity, CallingFileData creationData) {
        PushLog (3);

        if (!list) {
            RETURN LIST_NULL_POINTER;
        }

        list->capacity = (ssize_t) capacity + 1;

        list->next = (ssize_t *) calloc ((size_t) list->capacity, sizeof (ssize_t));
        list->prev = (ssize_t *) calloc ((size_t) list->capacity, sizeof (ssize_t));
        list->data = (elem_t *)  calloc ((size_t) list->capacity, sizeof (elem_t));

        #define CheckForNull(expression, error) if (!(expression)) {RETURN error;}

        CheckForNull (list->prev, PREV_NULL_POINTER);
        CheckForNull (list->next, NEXT_NULL_POINTER);
        CheckForNull (list->data, DATA_NULL_POINTER);

        #undef CheckForNull

        list->prev [0] = 0;
        list->next [0] = 0;

        list->freeElem = 1;

        for (ssize_t listIndex = list->freeElem; listIndex < list->capacity; listIndex++) {
            list->next [listIndex] = (listIndex + 1) % list->capacity;
            list->prev [listIndex] = -1;
        }

        list->creationData = creationData;

        Verification (list, creationData);

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode DestroyList_ (List *list) {
        PushLog (3);

        if (!list) {
            RETURN LIST_NULL_POINTER;
        }

        #define ZeroMemory(arrayPointer) memset (arrayPointer, 0, (size_t) list->capacity * sizeof (elem_t))

        ZeroMemory (list->data);
        ZeroMemory (list->prev);
        ZeroMemory (list->next);

        free (list->data);
        free (list->prev);
        free (list->next);

        #undef ZeroMemory

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode InsertAfter_ (List *list, ssize_t insertIndex, ssize_t *newIndex, elem_t element, CallingFileData callData) {
        PushLog (3);

        custom_assert (newIndex, pointer_is_null, WRONG_INDEX);

        Verification (list, callData);

        if (insertIndex < 0 || insertIndex >= list->capacity) {
            RETURN WRONG_INDEX;
        }

        if (list->prev [insertIndex] == -1) {
            RETURN WRONG_INDEX;
        }

        if (list->freeElem == 0) {
            RETURN INVALID_CAPACITY;
        }

        *newIndex = list->freeElem;
        list->freeElem = list->next [list->freeElem];

        list->prev [list->next [insertIndex]] = *newIndex;

        list->next [*newIndex]   = list->next [insertIndex];
        list->next [insertIndex] = *newIndex;
        list->prev [*newIndex]   = insertIndex;
        list->data [*newIndex]   = element;

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode DeleteValue_ (List *list, ssize_t deleteIndex, CallingFileData callData) {
        PushLog (3);

        Verification (list, callData);

        if (deleteIndex <= 0) {
            RETURN WRONG_INDEX;
        }

        if (list->prev [deleteIndex] == -1) {
            RETURN WRONG_INDEX;
        }

        list->prev [list->next [deleteIndex]] = list->prev [deleteIndex];
        list->next [list->prev [deleteIndex]] = list->next [deleteIndex];

        list->next [deleteIndex]    = list->freeElem;
        list->prev [deleteIndex]    = -1;
        list->freeElem              = deleteIndex;

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode VerifyList (List *list) {
        PushLog (3);

        #define WriteErrors(list, errorCodes)  (list)->errors = (ListErrorCode) ((list)->errors | (errorCodes))
        #define ReturnErrors(list, errorCodes) WriteErrors (list, errorCodes); RETURN (list)->errors
        #define ErrorCheck(condition, errorCodes)   \
            do {                                    \
                if (!(condition)) {                 \
                    WriteErrors (list, errorCodes); \
                }                                   \
            } while (0)

        if (!list) {
            RETURN LIST_NULL_POINTER;
        }

        ErrorCheck (list->data,                                             DATA_NULL_POINTER);
        ErrorCheck (list->prev,                                             PREV_NULL_POINTER);
        ErrorCheck (list->next,                                             NEXT_NULL_POINTER);
        ErrorCheck (list->capacity >= 0,                                    INVALID_CAPACITY);
        ErrorCheck (list->next [0] >= 0 && list->next [0] < list->capacity, INVALID_HEAD);
        ErrorCheck (list->prev [0] >= 0 && list->prev [0] < list->capacity, INVALID_TAIL);
        ErrorCheck (list->freeElem >= 0 && list->freeElem < list->capacity, FREE_LIST_ERROR);

        ssize_t freeIndex = list->freeElem;

        while (freeIndex > 0) {
            ErrorCheck (list->prev [freeIndex] <= 0, FREE_LIST_ERROR);

            freeIndex = list->next [freeIndex];
        }

        #undef WriteErrors
        #undef ReturnErrors
        #undef ErrorCheck

        RETURN list->errors;
    }

    ListErrorCode FindValueInListSlowImplementation_ (List *list, elem_t value, ssize_t *index, CallingFileData callData) {
        PushLog (3);

        for (ssize_t elementIndex = list->next [0]; elementIndex != 0; elementIndex = list->next [elementIndex]) {
            if (abs (list->data [elementIndex] - value) < EPS) {
                *index = elementIndex;
                RETURN NO_LIST_ERRORS;
            }
        }

        *index = -1;
        RETURN NO_LIST_ERRORS;
    }
}
