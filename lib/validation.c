#include "validation.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF){}
}

void wait_for_input() {
    printf("\nPress any key on keyboard to continue...");
    getch();
}

bool is_filename_valid(void *value) {
    const ushort length = strlen(value);
    return length != 0 && length <= 80 && strchr(value, '.') == NULL;
}

bool is_region_valid(void *value) {
    const ushort length = strlen(value);
    return length != 0 && length < 40;
}

bool is_positive(void *value) {
    return value > 0;
}

void get_input(const DataType type, void *variable, const char *print_msg, bool (*condition)(void*), const char *error_msg) {
    const char *format = NULL;
    switch (type) {
        case TYPE_USHORT:
            format = "%hu";
            break;
        case TYPE_UNSIGNED:
            format = "%u";
            break;
        case TYPE_FLOAT:
            format = "%f";
            break;
        case TYPE_STRING:
            do {
                printf("%s", print_msg);
                if (fgets(variable, MAX_PATH_SIZE, stdin) != NULL) {
                    ((char*)variable)[strcspn(variable, "\n")] = 0;
                } else {
                    printf("Error: wrong input.");
                }
                if (!condition(variable)) {
                    printf("%s", error_msg);
                }
            } while (!condition(variable));
            return;
        default:
            printf("Error: unsupported data type used in get_input().\n");
            wait_for_input();
            exit(0);
    }

    ushort is_input_valid = 0;
    do {
        printf("%s", print_msg);
        is_input_valid = scanf(format, variable);
        if (is_input_valid != 1) {
            printf("Error: wrong input.\n");
        } else if (!condition(variable)) {
            printf("%s", error_msg);
        }
        clear_input_buffer();
    } while (!condition(variable) || is_input_valid != 1);
}

void print_app_description() {
    printf(
        BOLD
        "-----------------------------------------------------------\n"
        "This application allows users to manage records within files,\n"
        "offering multiple functions such as:\n"
        "- Creating records\n"
        "- Editing records\n"
        "- Deleting records\n"
        "- Sorting records\n"
        "- Inserting records\n"
        "- Viewing and managing files\n"
        "-----------------------------------------------------------\n"
        RESET);
    wait_for_input();
}

void print_file_menu() {
    printf(
        BOLD YELLOW
        "-----------------------------\n"
        ITALIC
        "\tFile Menu\n"
        "-----------------------------\n"
        RESET
        BOLD
        "\t1. Create File\n"
        "\t2. Open File\n"
        "\t3. Delete File\n"
        "\t0. Exit\n"
        YELLOW
        "-----------------------------\n"
        RESET);
}

void print_record_menu() {
    printf(
        BOLD MAGENTA
        "-----------------------------\n"
        ITALIC
        "\tRecord Menu\n"
        "-----------------------------\n"
        RESET
        BOLD
        "\t1. Create Record\n"
        "\t2. Edit Record\n"
        "\t3. Sort Records\n"
        "\t4. Insert Record\n"
        "\t5. Delete Record\n"
        "\t0. Exit\n"
        MAGENTA
        "-----------------------------\n"
        RESET);
}

void print_sort_order_menu() {
    printf(
        BOLD
        "-----------------------------\n"
        ITALIC
        "\tSort Order\n"
        "-----------------------------\n"
        RESET
        BOLD
        "\t0. Descension\n"
        "\t1. Ascension\n"
        "-----------------------------\n"
        RESET);
}

void print_sort_by_menu() {
    printf(
        BOLD
        "-----------------------------\n"
        ITALIC
        "\tSort By\n"
        "-----------------------------\n"
        RESET
        BOLD
        "\t1. Region\n"
        "\t2. Area\n"
        "\t3. Population\n"
        "-----------------------------\n"
        RESET);
}
