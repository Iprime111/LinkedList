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

namespace LinkedList {
    ListErrorCode InitList (List *list, size_t capacity) {
        PushLog (3);

        if (!list) {
            RETURN LIST_NULL_POINTER;
        }

        list->capacity = capacity + 1;

        list->next = (size_t *) calloc (list->capacity, sizeof (size_t));
        list->prev = (size_t *) calloc (list->capacity, sizeof (size_t));
        list->data = (elem_t *) calloc (list->capacity, sizeof (elem_t));

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

        #define WriteErrors(list, errorCodes) (list)->errors = (ListErrorCode) ((list)->errors | (errorCodes))

        if (!list) {
            RETURN LIST_NULL_POINTER;
        }

        if (!list->data)
            WriteErrors (list, DATA_NULL_POINTER);

        if (!list->prev)
            WriteErrors (list, PREV_NULL_POINTER);

        if (!list->next)
            WriteErrors (list, NEXT_NULL_POINTER);

        #undef WriteErrors

        RETURN list->errors;
    }

    ListErrorCode DumpList (List *list) {
        PushLog (3);

        RETURN NO_LIST_ERRORS;
    }
}
