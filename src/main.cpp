#include <stdio.h>

#include "LinkedList.h"
#include "Logger.h"

int main () {
    PushLog (1);

    LinkedList::List list = {};
    LinkedList::InitList (&list, 10);
    LinkedList::ClearHtmlFile ();

    ssize_t newIndex = 0;
    LinkedList::InsertAfter (&list, 0,        &newIndex, 5);
    LinkedList::InsertAfter (&list, newIndex, &newIndex, 6);
    LinkedList::InsertAfter (&list, newIndex, &newIndex, 7);

    LinkedList::DeleteValue (&list, 1);

    LinkedList::InsertAfter (&list, newIndex, &newIndex, 4);

    LinkedList::DumpList (&list, ".");

    LinkedList::DestroyList (&list);

    RETURN 0;
}
