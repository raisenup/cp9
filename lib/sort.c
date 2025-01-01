#include "validation.h"
#include "common.h"
#include "sort.h"

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compare_region(const void* a, const void* b) {
    const Record* record_a = (const Record*)a;
    const Record* record_b = (const Record*)b;
    return strcmp(record_a->region, record_b->region) > 0;
}

int sort_region_asc(const char* full_path) {
    FILE* file = fopen(full_path, "rb");
    if (!file) return 0;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* buffer = malloc(file_size);
    if (!buffer) {
        fclose(file);
        return 0;
    }
    fread(buffer, 1, file_size, file);
    fclose(file);

    size_t desc_len = strlen(DESCRIPTOR);
    char* records_start = buffer + desc_len + 2;
    size_t records_size = file_size - (desc_len + 2);
    int total_records = records_size / sizeof(Record);

    if (total_records <= 1) {
        free(buffer);
        return 1;
    }

    qsort(records_start, total_records, sizeof(Record), compare_region);

    file = fopen(full_path, "wb");
    if (!file) {
        free(buffer);
        return 0;
    }

    fwrite(buffer, 1, desc_len + 2, file);
    fwrite(records_start, sizeof(Record), total_records, file);

    free(buffer);
    fclose(file);
    return 1;
}

int sort_by(const char *full_path, const ushort is_asc) {
    char choice = 0;
    do {
        system("cls");
        print_sort_by_menu();
        printf("Enter the number of the field you want to sort by: ");
        choice = getch();
        system("cls");
        switch (choice) {
            case '1':
                if (is_asc) {
                    sort_region_asc(full_path);
                } else {

                }
            break;
            case '2':
                break;
            case '3':
                break;
            default:
                break;
        }
    } while (choice != '1' && choice != '2' && choice != '3');
    return 1;
}

void sort_by_wrapper(const char *full_path) {
    char is_asc = 0;
    do {
        system("cls");
        print_sort_order_menu();
        printf("Enter the sort order: ");
        is_asc = getch();
        system("cls");
        switch (is_asc) {
            case '0':
                sort_by(full_path, 0);
                break;
            case '1':
                sort_by(full_path, 1);
                break;
            default:
                break;
        }
    } while (is_asc != '0' && is_asc != '1');
}