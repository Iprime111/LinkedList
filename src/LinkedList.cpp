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

#define Verification(list, callData)                    \
    do {                                                \
        ListErrorCode errorCode_ = VerifyList_ (list);  \
        if (errorCode_ != NO_LIST_ERRORS) {             \
            ON_DEBUG (DumpList_ (list, ".", callData)); \
            RETURN errorCode_;                          \
        }                                               \
    }while (0)

namespace LinkedList {

    ListErrorCode InitList_ (List *list, CallingFileData creationData) {
        PushLog (3);

        if (!list) {
            RETURN LIST_NULL_POINTER;
        }

        list->creationData = creationData;
        list->head = (Node *) calloc (1, sizeof (Node));

        list->head->data = NAN;
        list->head->next = NULL;
        list->head->prev = NULL;


        list->tail = list->head;
        list->capacity = 1;

        Verification (list, creationData);

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode DestroyList_ (List *list) {
        PushLog (3);

        if (!list) {
            RETURN LIST_NULL_POINTER;
        }

        Node *nextNode = list->head;

        while (nextNode) {
            Node *node = nextNode;
            nextNode = nextNode->next;
            free (node);
        }

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode InsertAfter_ (List *list, Node *insertPointer, Node **newPointer, elem_t element, CallingFileData callData) {
        PushLog (3);

        custom_assert (newPointer,    pointer_is_null, NODE_NULL_POINTER);
        custom_assert (insertPointer, pointer_is_null, NODE_NULL_POINTER);

        Verification (list, callData);

        *newPointer = (Node *) calloc (1, sizeof (Node));

        if (!newPointer) {
            RETURN NODE_NULL_POINTER;
        }

        (*newPointer)->data = element;

        if (insertPointer->next) {
            insertPointer->next->prev = *newPointer;
        }

        (*newPointer)->next = insertPointer->next;
        insertPointer->next = *newPointer;
        (*newPointer)->prev = insertPointer;

        list->capacity++;

        if (list->tail == insertPointer) {
            list->tail = *newPointer;
        }

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode DeleteValue_ (List *list, Node *deletePointer, CallingFileData callData) {
        PushLog (3);

        custom_assert (deletePointer,               pointer_is_null, NODE_NULL_POINTER);
        custom_assert (deletePointer != list->head, pointer_is_null, NODE_NULL_POINTER);

        Verification (list, callData);

        if (!deletePointer) {
            RETURN NODE_NULL_POINTER;
        }

        if (deletePointer == list->tail) {
            list->tail = deletePointer->prev;
        }

        if (deletePointer->next) {
            deletePointer->next->prev = deletePointer->prev;
        }

        if (deletePointer->prev) {
            deletePointer->prev->next = deletePointer->next;
        }

        free (deletePointer);
        list->capacity--;

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode VerifyList (List *list) {
        PushLog (3);

        #define WriteErrors(list, errorCodes)  (list)->errors = (ListErrorCode) ((list)->errors | (errorCodes))
        #define ReturnErrors(list, errorCodes) WriteErrors (list, errorCodes); RETURN (list)->errors

        if (!list) {
            RETURN LIST_NULL_POINTER;
        }

        if (!list->head) {
            ReturnErrors (list, INVALID_HEAD);
        }

        if (!list->tail) {
            WriteErrors (list, INVALID_TAIL);
        }

        ssize_t capacity = 0;
        Node *nodePointer = list->head;

        while (nodePointer) {
            capacity++;
            nodePointer = nodePointer->next;
        }

        if (capacity != list->capacity) {
            WriteErrors(list, INVALID_CAPACITY);
        }

        RETURN list->errors;
    }

    ListErrorCode FindValueInListSlowImplementation_ (List *list, elem_t value, Node **foundElementpointer, CallingFileData callData) {
        PushLog (3);

        for (Node *elementPointer = list->head; elementPointer != NULL; elementPointer = elementPointer->next) {
            if (abs (elementPointer->data - value) < EPS) {
                *foundElementpointer = elementPointer;
                RETURN NO_LIST_ERRORS;
            }
        }

        *foundElementpointer = NULL;
        RETURN NO_LIST_ERRORS;
    }
}
