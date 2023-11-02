#include <cstddef>
#include <cstdio>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "GraphVizDump.h"
#include "Buffer.h"
#include "CustomAssert.h"
#include "LinkedList.h"
#include "Logger.h"

namespace LinkedList {

    static ListErrorCode DumpNode            (List *list, ssize_t nodeIndex, Buffer <char> *graphvizBuffer);
    static ListErrorCode DumpNodeConnections (List *list, ssize_t nodeIndex, Buffer <char> *graphvizBuffer);
    static ListErrorCode WriteDumpHeader     (List *list, Buffer <char> *graphvizBuffer);
    static char         *GetLogFilename      (char *logFolder);

    #define CheckWriteErrors(buffer, data)                                                          \
                do  {                                                                               \
                    if (WriteStringToBuffer (buffer, data) != BufferErrorCode::NO_BUFFER_ERRORS) {  \
                        DestroyBuffer (buffer);                                                     \
                        RETURN GRAPHVIZ_BUFFER_ERROR;                                               \
                    }                                                                               \
                } while (0)

    ListErrorCode DumpList (List *list, char *logFolder) {
        PushLog (3);

        ListErrorCode verificationResult = VerifyList (list);

        if (verificationResult & (LIST_NULL_POINTER | DATA_NULL_POINTER | PREV_NULL_POINTER | NEXT_NULL_POINTER)) {
            RETURN verificationResult;
        }

        Buffer <char> graphvizBuffer = {};
        if (InitBuffer (&graphvizBuffer, 1) != BufferErrorCode::NO_BUFFER_ERRORS) {
            RETURN GRAPHVIZ_BUFFER_ERROR;
        }

        WriteDumpHeader (list, &graphvizBuffer);

        for (ssize_t nodeIndex = 0; nodeIndex < list->capacity; nodeIndex++) {
            DumpNode (list, nodeIndex, &graphvizBuffer);
        }

        CheckWriteErrors (&graphvizBuffer, "\n");

        for (ssize_t nodeIndex = 0; nodeIndex < list->capacity; nodeIndex++) {
            DumpNodeConnections (list, nodeIndex, &graphvizBuffer);
        }

        CheckWriteErrors (&graphvizBuffer, "}");

        char *filename = GetLogFilename (logFolder);

        if (!filename) {
            RETURN LOG_FILE_ERROR;
        }

        FILE *logFile = fopen (filename, "w");
        fwrite (graphvizBuffer.data, graphvizBuffer.currentIndex, sizeof (char), logFile);
        fclose (logFile);

        free (filename);
        DestroyBuffer (&graphvizBuffer);

        RETURN NO_LIST_ERRORS;
    }

    static ListErrorCode DumpNode (List *list, ssize_t nodeIndex, Buffer <char> *graphvizBuffer) {
        PushLog (3);

        custom_assert (list,           pointer_is_null, LIST_NULL_POINTER);
        custom_assert (graphvizBuffer, pointer_is_null, GRAPHVIZ_BUFFER_ERROR);

        char indexBuffer [MAX_INDEX_LENGTH] = "";
        snprintf (indexBuffer, MAX_INDEX_LENGTH, "%ld", nodeIndex);

        CheckWriteErrors (graphvizBuffer, "\t");
        CheckWriteErrors (graphvizBuffer, indexBuffer);
        CheckWriteErrors (graphvizBuffer, " [style=\"filled, rounded\" fillcolor=\"" DUMP_NODE_COLOR "\" shape=\"record\" color=\"");

        if (list->prev [nodeIndex] < 0) {
            CheckWriteErrors (graphvizBuffer, DUMP_FREE_NODE_OUTLINE_COLOR);
        } else {
            CheckWriteErrors (graphvizBuffer, DUMP_NODE_OUTLINE_COLOR);
        }

        CheckWriteErrors (graphvizBuffer, "\" label=\"");

        char nodeDataBuffer [MAX_NODE_DATA_LENGTH] = "";

        snprintf (nodeDataBuffer, MAX_NODE_DATA_LENGTH, "<index> index: %ld | <data> data: %lf | {<prev> prev: %ld |<next> next: %ld} \"",
                    nodeIndex, list->data [nodeIndex], list->prev [nodeIndex], list->next [nodeIndex]);

        CheckWriteErrors (graphvizBuffer, nodeDataBuffer);

        CheckWriteErrors (graphvizBuffer, "];\n");

        RETURN NO_LIST_ERRORS;
    }

    static ListErrorCode DumpNodeConnections (List *list, ssize_t nodeIndex, Buffer <char> *graphvizBuffer) {
        PushLog (3);

        custom_assert (list,           pointer_is_null, LIST_NULL_POINTER);
        custom_assert (graphvizBuffer, pointer_is_null, GRAPHVIZ_BUFFER_ERROR);

        // Dump next connection

        char indexBuffer [MAX_INDEX_LENGTH] = "";

        CheckWriteErrors (graphvizBuffer, "\t");

        snprintf (indexBuffer, MAX_INDEX_LENGTH, "%ld", nodeIndex);

        CheckWriteErrors (graphvizBuffer, indexBuffer);
        CheckWriteErrors (graphvizBuffer, " -> ");

        snprintf (indexBuffer, MAX_INDEX_LENGTH, "%ld", list->next [nodeIndex]);

        CheckWriteErrors (graphvizBuffer, indexBuffer);
        CheckWriteErrors (graphvizBuffer, " [color=\"");

        if (list->next [nodeIndex] == 0) {
            CheckWriteErrors (graphvizBuffer, DUMP_NULL_CONNECTION_COLOR);
        } else {
            CheckWriteErrors (graphvizBuffer, DUMP_NEXT_CONNECTION_COLOR);
        }

        CheckWriteErrors (graphvizBuffer, "\"];\n");

        // Dump prev connection

        if (list->prev [nodeIndex] < 0) {
            RETURN NO_LIST_ERRORS;
        }

        CheckWriteErrors (graphvizBuffer, "\t");

        snprintf (indexBuffer, MAX_INDEX_LENGTH, "%ld", nodeIndex);

        CheckWriteErrors (graphvizBuffer, indexBuffer);
        CheckWriteErrors (graphvizBuffer, " -> ");

        snprintf (indexBuffer, MAX_INDEX_LENGTH, "%ld", list->prev [nodeIndex]);

        CheckWriteErrors (graphvizBuffer, indexBuffer);
        CheckWriteErrors (graphvizBuffer, " [color=\"" DUMP_PREV_CONNECTION_COLOR "\"];\n");

        RETURN NO_LIST_ERRORS;
    }

    static ListErrorCode WriteDumpHeader (List *list, Buffer <char> *graphvizBuffer) {
        PushLog (4);

        custom_assert (graphvizBuffer, pointer_is_null, GRAPHVIZ_BUFFER_ERROR);

        CheckWriteErrors (graphvizBuffer, "digraph {\n\trankdir=\"LR\";\n\tbgcolor=\"" DUMP_BACKGROUND_COLOR "\";\n\t");

        char indexBuffer [MAX_INDEX_LENGTH] = "";

        for (ssize_t nodeIndex = 0; nodeIndex < list->capacity - 1; nodeIndex++) {
            snprintf (indexBuffer, MAX_INDEX_LENGTH, "%ld", nodeIndex);
            CheckWriteErrors (graphvizBuffer, indexBuffer);

            CheckWriteErrors (graphvizBuffer, " -> ");
        }

        snprintf (indexBuffer, MAX_INDEX_LENGTH, "%ld", list->capacity - 1);
        CheckWriteErrors (graphvizBuffer, indexBuffer);

        CheckWriteErrors (graphvizBuffer, " [weight=999999 color=\"" DUMP_BACKGROUND_COLOR "\"];\n");

        CheckWriteErrors (graphvizBuffer, "\tHeader [style=\"filled, rounded\" fillcolor=\"" DUMP_NODE_COLOR "\" shape=\"record\" color=\"" DUMP_HEADER_NODE_COLOR "\" label=\"");

        char nodeDataBuffer [MAX_NODE_DATA_LENGTH] = "";

        snprintf (nodeDataBuffer, MAX_NODE_DATA_LENGTH, "<head> head: 0 | <tail> tail: %ld | <free> free: %ld \"];\n",
                    list->tail, list->freeElem);

        CheckWriteErrors (graphvizBuffer, nodeDataBuffer);

        CheckWriteErrors (graphvizBuffer, "\tHeader:head -> 0 [color=\"" DUMP_HEADER_NODE_COLOR "\"weight=2];\n");

        CheckWriteErrors (graphvizBuffer, "\tHeader:tail -> ");
        snprintf (indexBuffer, MAX_INDEX_LENGTH, "%ld", list->tail);
        CheckWriteErrors (graphvizBuffer, indexBuffer);
        CheckWriteErrors (graphvizBuffer, " [color=\"" DUMP_HEADER_NODE_COLOR "\" weight=2];\n");

        CheckWriteErrors (graphvizBuffer, "\tHeader:free -> ");
        snprintf (indexBuffer, MAX_INDEX_LENGTH, "%ld", list->freeElem);
        CheckWriteErrors (graphvizBuffer, indexBuffer);
        CheckWriteErrors (graphvizBuffer, " [color=\"" DUMP_HEADER_NODE_COLOR "\" weight=2];\n\n");

        RETURN NO_LIST_ERRORS;
    }

    static char *GetLogFilename (char *logFolder) {
        PushLog (4);

        time_t currentTime = time (NULL);
        tm localTime = *localtime (&currentTime);

        char *filename = (char *) calloc (FILENAME_MAX, sizeof (char));

        if (!filename) {
            RETURN NULL;
        }

        snprintf (filename, FILENAME_MAX, "%s/%.2d-%.2d-%.4d_%.2d:%.2d:%.2d.dot", logFolder, localTime.tm_mday, localTime.tm_mon,
                    localTime.tm_year + 1900, localTime.tm_hour, localTime.tm_min, localTime.tm_sec);

        RETURN filename;
    }

}
