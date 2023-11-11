#include <bits/types/FILE.h>
#include <cstddef>
#include <cstdio>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "GraphVizDump.h"
#include "Buffer.h"
#include "CustomAssert.h"
#include "LinkedList.h"
#include "Logger.h"

namespace LinkedList {

    static ListErrorCode DumpNode            (List *list, ssize_t nodeIndex, Buffer <char> *graphvizBuffer);
    static ListErrorCode DumpNodeConnections (List *list, ssize_t nodeIndex, Buffer <char> *graphvizBuffer);
    static ListErrorCode DumpConnection      (List *list, Buffer <char> *graphvizBuffer, const char *from, const char *to, const char *color);
    static ListErrorCode WriteDumpHeader     (List *list, Buffer <char> *graphvizBuffer, CallingFileData *callData);
    static ListErrorCode WriteCallData       (List *list, CallingFileData *callData, Buffer <char> *graphvizBuffer);
    static char         *GetLogFilename      (char *logFolder);
    static ListErrorCode WriteToHtml         (char *dotFilename);

    #define CheckWriteErrors(buffer, data)                                                          \
                do  {                                                                               \
                    if (WriteStringToBuffer (buffer, data) != BufferErrorCode::NO_BUFFER_ERRORS) {  \
                        DestroyBuffer (buffer);                                                     \
                        RETURN GRAPHVIZ_BUFFER_ERROR;                                               \
                    }                                                                               \
                } while (0)

    #define WriteIndexToDump(dumpBuffer, indexBuffer, index)            \
                snprintf (indexBuffer, MAX_INDEX_LENGTH, "%ld", index); \
                CheckWriteErrors (dumpBuffer, indexBuffer)

    ListErrorCode DumpList_ (List *list, char *logFolder, CallingFileData callData) {
        PushLog (3);

        ListErrorCode verificationResult = VerifyList (list);

        if (verificationResult & (LIST_NULL_POINTER | DATA_NULL_POINTER | PREV_NULL_POINTER | NEXT_NULL_POINTER)) {
            RETURN verificationResult;
        }

        Buffer <char> graphvizBuffer = {};
        if (InitBuffer (&graphvizBuffer, 1) != BufferErrorCode::NO_BUFFER_ERRORS) {
            RETURN GRAPHVIZ_BUFFER_ERROR;
        }

        WriteDumpHeader (list, &graphvizBuffer, &callData);
        WriteCallData (list, &callData, &graphvizBuffer);

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
        if (!logFile) {
            free (filename);
            RETURN LOG_FILE_ERROR;
        }

        fwrite (graphvizBuffer.data, graphvizBuffer.currentIndex, sizeof (char), logFile);
        fclose (logFile);

        WriteToHtml (filename);

        free (filename);
        DestroyBuffer (&graphvizBuffer);

        RETURN NO_LIST_ERRORS;
    }

    static ListErrorCode WriteToHtml (char *dotFilename) {
        PushLog (4);

        char htmlFileCommand [FILENAME_MAX * 2] = "";
        char imageFilename   [FILENAME_MAX]     = "";

        snprintf (imageFilename,   FILENAME_MAX,     "%s.svg",            dotFilename);
        snprintf (htmlFileCommand, FILENAME_MAX * 2, "dot -Tsvg %s > %s", dotFilename, imageFilename);

        system (htmlFileCommand);

        FILE *htmlFile  = fopen (HTML_FILENAME, "a");
        if (!htmlFile) {
            RETURN LOG_FILE_ERROR;
        }

        FILE *imageFile = fopen (imageFilename, "r");
        if (!imageFile) {
            fclose (htmlFile);
            RETURN LOG_FILE_ERROR;
        }

        fprintf (htmlFile, "This dump has been created from file %s. List graph:", dotFilename);

        int byte = fgetc (imageFile);

        while (byte != EOF) {
            fputc (byte, htmlFile);
            byte = fgetc (imageFile);
        }

        fclose (htmlFile);
        fclose (imageFile);

        RETURN NO_LIST_ERRORS;
    }

    ListErrorCode ClearHtmlFile () {
        FILE *htmlFile= fopen (HTML_FILENAME, "w");
        if (!htmlFile)
            return NO_LIST_ERRORS;

        fclose (htmlFile);
        return NO_LIST_ERRORS;
    }

    static ListErrorCode DumpNode (List *list, ssize_t nodeIndex, Buffer <char> *graphvizBuffer) {
        PushLog (3);

        custom_assert (list,           pointer_is_null, LIST_NULL_POINTER);
        custom_assert (graphvizBuffer, pointer_is_null, GRAPHVIZ_BUFFER_ERROR);

        char indexBuffer [MAX_INDEX_LENGTH] = "";

        CheckWriteErrors (graphvizBuffer, "\t");
        WriteIndexToDump (graphvizBuffer, indexBuffer, nodeIndex);
        CheckWriteErrors (graphvizBuffer, " [style=\"filled, rounded\" fillcolor=\"" DUMP_NODE_COLOR "\" shape=\"Mrecord\" color=\"");

        if (nodeIndex) {
            if (list->prev [nodeIndex] < 0) {
                CheckWriteErrors (graphvizBuffer, DUMP_FREE_NODE_OUTLINE_COLOR);
            } else {
                CheckWriteErrors (graphvizBuffer, DUMP_NODE_OUTLINE_COLOR);
            }
        } else {
            CheckWriteErrors (graphvizBuffer, DUMP_HEADER_NODE_COLOR);
        }

        CheckWriteErrors (graphvizBuffer, "\" label=\"");

        char nodeDataBuffer [MAX_NODE_DATA_LENGTH] = "";

        snprintf (nodeDataBuffer, MAX_NODE_DATA_LENGTH, "<prev> prev: %ld | {<index> index: %ld | <data> data: %lf} | <next> next: %ld\"",
                    list->prev [nodeIndex], nodeIndex, list->data [nodeIndex], list->next [nodeIndex]);

        CheckWriteErrors (graphvizBuffer, nodeDataBuffer);

        CheckWriteErrors (graphvizBuffer, "];\n");

        RETURN NO_LIST_ERRORS;
    }

    static ListErrorCode DumpNodeConnections (List *list, ssize_t nodeIndex, Buffer <char> *graphvizBuffer) {
        PushLog (3);

        custom_assert (list,           pointer_is_null, LIST_NULL_POINTER);
        custom_assert (graphvizBuffer, pointer_is_null, GRAPHVIZ_BUFFER_ERROR);

        if (nodeIndex == 0) {
            RETURN NO_LIST_ERRORS;
        }

        char fromNodeDataBuffer [MAX_NODE_DATA_LENGTH] = "";
        char toNodeDataBuffer   [MAX_NODE_DATA_LENGTH] = "";

        // Dump next connection

        if (list->next [nodeIndex] > 0) {
            snprintf (fromNodeDataBuffer, MAX_NODE_DATA_LENGTH, "%ld:next", nodeIndex);
            snprintf (toNodeDataBuffer,   MAX_NODE_DATA_LENGTH, "%ld", list->next [nodeIndex]);

            DumpConnection (list, graphvizBuffer, fromNodeDataBuffer, toNodeDataBuffer, DUMP_NEXT_CONNECTION_COLOR);
        }

        // Dump prev connection

        if (list->prev [nodeIndex] <= 0) {
            RETURN NO_LIST_ERRORS;
        }

        snprintf (fromNodeDataBuffer, MAX_NODE_DATA_LENGTH, "%ld:prev", nodeIndex);
        snprintf (toNodeDataBuffer,   MAX_NODE_DATA_LENGTH, "%ld", list->prev [nodeIndex]);

        DumpConnection (list, graphvizBuffer, fromNodeDataBuffer, toNodeDataBuffer, DUMP_PREV_CONNECTION_COLOR);

        RETURN NO_LIST_ERRORS;
    }

    static ListErrorCode DumpConnection (List *list, Buffer <char> *graphvizBuffer, const char *from, const char *to, const char *color) {
        PushLog (3);

        CheckWriteErrors (graphvizBuffer, "\t");
        CheckWriteErrors (graphvizBuffer, from);
        CheckWriteErrors (graphvizBuffer, "->");
        CheckWriteErrors (graphvizBuffer, to);
        CheckWriteErrors (graphvizBuffer, " [color=\"");
        CheckWriteErrors (graphvizBuffer, color);
        CheckWriteErrors (graphvizBuffer, "\"];\n");

        RETURN NO_LIST_ERRORS;
    }

    static ListErrorCode WriteDumpHeader (List *list, Buffer <char> *graphvizBuffer, CallingFileData *callData) {
        PushLog (4);

        custom_assert (graphvizBuffer, pointer_is_null, GRAPHVIZ_BUFFER_ERROR);

        CheckWriteErrors (graphvizBuffer, "digraph {\n\tbgcolor=\"" DUMP_BACKGROUND_COLOR "\";\n\tsplines=ortho\n\t");

        WriteCallData (list, callData, graphvizBuffer);

        char indexBuffer [MAX_INDEX_LENGTH] = "";

        for (ssize_t nodeIndex = list->next [0]; nodeIndex > 0 && nodeIndex < list->capacity; nodeIndex = list->next [nodeIndex]) {
            WriteIndexToDump (graphvizBuffer, indexBuffer, nodeIndex);

            CheckWriteErrors (graphvizBuffer, " -> ");
        }

        CheckWriteErrors (graphvizBuffer, " 0 -> ");

        for (ssize_t nodeIndex = list->freeElem; nodeIndex > 0 && nodeIndex < list->capacity; nodeIndex = list->next [nodeIndex]) {
            WriteIndexToDump (graphvizBuffer, indexBuffer, nodeIndex);

            CheckWriteErrors (graphvizBuffer, " -> ");
        }

        WriteIndexToDump (graphvizBuffer, indexBuffer, list->prev [0]);
        CheckWriteErrors (graphvizBuffer, " [weight=999999 color=\"" DUMP_BACKGROUND_COLOR "\"; style=invis];\n");

        CheckWriteErrors (graphvizBuffer, "\t{rank=same; ");

        for (ssize_t nodeIndex = 0; nodeIndex < list->capacity; nodeIndex++) {
            WriteIndexToDump (graphvizBuffer, indexBuffer, nodeIndex);
            CheckWriteErrors (graphvizBuffer, " ");
        }

        CheckWriteErrors (graphvizBuffer, "}\n");

        const char *HeaderFieldStyle = "[style=\"filled, rounded\" fillcolor=\"" DUMP_NODE_COLOR"\" shape=\"rectangle\" color = \"" DUMP_HEADER_NODE_COLOR "\"];\n";

        CheckWriteErrors (graphvizBuffer, "\tHead");
        CheckWriteErrors (graphvizBuffer, HeaderFieldStyle);

        CheckWriteErrors (graphvizBuffer, "\tTail");
        CheckWriteErrors (graphvizBuffer, HeaderFieldStyle);

        CheckWriteErrors (graphvizBuffer, "\tFree");
        CheckWriteErrors (graphvizBuffer, HeaderFieldStyle);

        const char *HeaderConnectionStyle = "[color=\"" DUMP_HEADER_NODE_COLOR "\"];\n";

        CheckWriteErrors (graphvizBuffer, "\tHead -> ");
        WriteIndexToDump (graphvizBuffer, indexBuffer, list->next [0]);
        CheckWriteErrors (graphvizBuffer, HeaderConnectionStyle);


        CheckWriteErrors (graphvizBuffer, "\tTail -> ");
        WriteIndexToDump (graphvizBuffer, indexBuffer, list->prev [0]);
        CheckWriteErrors (graphvizBuffer, HeaderConnectionStyle);

        CheckWriteErrors (graphvizBuffer, "\tFree -> ");
        WriteIndexToDump (graphvizBuffer, indexBuffer, list->freeElem);
        CheckWriteErrors (graphvizBuffer, HeaderConnectionStyle);

        RETURN NO_LIST_ERRORS;
    }

    static ListErrorCode WriteCallData (List *list, CallingFileData *callData, Buffer <char> *graphvizBuffer) {
        PushLog (4);

        char callDataBuffer [FILENAME_MAX] = "";

        CheckWriteErrors (graphvizBuffer, "\tCreation [shape=rectangle style=filled fillcolor=\"" DUMP_NODE_COLOR "\" rank=max label=\"Was created in ");
        snprintf (callDataBuffer, FILENAME_MAX, "%s (%s:%d)",
                    list->creationData.function, list->creationData.file, list->creationData.line);
        CheckWriteErrors (graphvizBuffer, callDataBuffer);
        CheckWriteErrors (graphvizBuffer, "\"]\n");

        CheckWriteErrors (graphvizBuffer, "\tCall [shape=rectangle style=filled fillcolor=\"" DUMP_NODE_COLOR "\" rank=max label=\"Was called in ");
        snprintf (callDataBuffer, FILENAME_MAX, "%s (%s:%d)",
                    callData->function, callData->file, callData->line);
        CheckWriteErrors (graphvizBuffer, callDataBuffer);
        CheckWriteErrors (graphvizBuffer, "\"]\n");

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

        int versionCounter = 0;

        do {
            snprintf (filename, FILENAME_MAX, "%s/%.2d-%.2d-%.4d_%.2d:%.2d:%.2d_%d.dot", logFolder, localTime.tm_mday, localTime.tm_mon,
                        localTime.tm_year + 1900, localTime.tm_hour, localTime.tm_min, localTime.tm_sec, versionCounter);

            versionCounter++;
        }  while (!access (filename, F_OK));

        RETURN filename;
    }

}
