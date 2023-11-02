#ifndef GRAPHVIZ_DUMP_H_
#define GRAPHVIZ_DUMP_H_

#include "LinkedList.h"

namespace LinkedList {
    const size_t MAX_INDEX_LENGTH     = 32;
    const size_t MAX_NODE_DATA_LENGTH = 128;

    #define DUMP_NODE_COLOR                 "#5e69db"
    #define DUMP_NODE_OUTLINE_COLOR         "#000000"
    #define DUMP_FREE_NODE_OUTLINE_COLOR    "#10c929"
    #define DUMP_NEXT_CONNECTION_COLOR      "#10c94b"
    #define DUMP_PREV_CONNECTION_COLOR      "#c95410"
    #define DUMP_NULL_CONNECTION_COLOR      "#002fff"
    #define DUMP_BACKGROUND_COLOR           "#393f87"
    #define DUMP_HEADER_NODE_COLOR          "#dbd802"
}
#endif
