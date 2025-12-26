#include <stdio.h>
#include "print.h"

int main(void) {
    orderbook* ob = orderbook_create(1024);

    /* add some orders here */

    char choice;
    while (1) {
        printf("Select view:\n");
        printf("1 → L1 (Top of Book)\n");
        printf("2 → L2 (Market Depth)\n");
        printf("q → Quit\n");
        printf("> ");

        scanf(" %c", &choice);

        if (choice == '1')
            print_orderbook(ob, VIEW_L1);
        else if (choice == '2')
            print_orderbook(ob, VIEW_L2);
        else if (choice == 'q')
            break;
    }

    orderbook_destroy(ob);
    return 0;
}
