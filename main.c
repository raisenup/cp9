#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

#include "lib/validation.h"
#include "lib/common.h"

int main(void) {
    print_app_description();
    char choice = 0;
    do {
        system("cls");

        view_files();

        print_file_menu();
        printf("Choose an option (0-3): ");
        choice = getch();
        system("cls");
        switch (choice) {
            case '0':
                break;
            case '1':
                fflush(stdin);
                create_file_wrapper();
                break;
            case '2':
                select_file(OPEN);
                break;
            case '3':
                // check for descriptor
                select_file(DELETE);
                break;
            default:
                printf("Error: there is no such option.");
                wait_for_input();
                break;
        }
    } while (choice != '0');

    return 0;
}
