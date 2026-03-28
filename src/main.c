#include "business.h"
#include "menu.h"

#include <stdio.h>

int main(void)
{
    int choice = -1;

    do {
        outputMenu();

        if (readMenuChoice(&choice) != 0) {
            showMenuInputFormatError();
            continue;
        }

        dispatchMenuChoice(choice);
        printf("\n");
    } while (choice != 0);

    bizShutdown();
    return 0;
}
