#include <stdio.h>

#include "LinkedList.h"
#include "Logger.h"

int main () {
    PushLog (1);

    LinkedList::List list = {};
    LinkedList::InitList_ (&list, 10);

    ssize_t newIndex = 0;
    LinkedList::InsertAfter_ (&list, 0,        &newIndex, 5);
    LinkedList::InsertAfter_ (&list, newIndex, &newIndex, 6);
    LinkedList::InsertAfter_ (&list, newIndex, &newIndex, 7);

    LinkedList::DeleteValue_ (&list, 1);

    LinkedList::InsertAfter_ (&list, newIndex, &newIndex, 4);

    LinkedList::DumpList_ (&list, ".");

    LinkedList::DestroyList (&list);

    RETURN 0;
}
