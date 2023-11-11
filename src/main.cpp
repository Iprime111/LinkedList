#include <stdio.h>

#include "LinkedList.h"
#include "Logger.h"

int main () {
    PushLog (1);

    LinkedList::List list = {};
    LinkedList::InitList (&list);

    LinkedList::Node *newPointer = 0;
    LinkedList::InsertAfter (&list, list.head,  &newPointer, 5);
    LinkedList::InsertAfter (&list, newPointer, &newPointer, 6);
    LinkedList::InsertAfter (&list, newPointer, &newPointer, 7);

    DumpList (&list, ".");

    LinkedList::DeleteValue (&list, list.head->next);

    LinkedList::InsertAfter (&list, newPointer, &newPointer, 4);

    LinkedList::DumpList (&list, ".");

    LinkedList::DestroyList (&list);

    RETURN 0;
}
