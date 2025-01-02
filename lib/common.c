#include "common.h"
#include "validation.h"
#include "sort.h"

#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>

unsigned get_file_size(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return 0;
    }
    return path_stat.st_size;
}

ushort get_file_count(const char *folder_path) {
    struct dirent *entry;
    DIR *dir = opendir(folder_path);
    if (dir == NULL) {
        printf("Error opening directory.\n");
        wait_for_input();
        return -1;
    }

    int file_count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        file_count++;
    }
    closedir(dir);
    return file_count;
}

void view_files() {
    struct dirent *entry;
    char full_path[MAX_PATH_SIZE];

    DIR *dir = opendir(FOLDER_PATH);
    if (dir == NULL) {
        printf("Error opening directory.\n");
        wait_for_input();
        exit(0);
    }

    int count = 0;
    printf(BOLD);
    printf("\nDirectory : %s\n", FOLDER_PATH);

    printf("%-10s %-3s %-80s\n", "Size", "#", "Name");
    printf("-----------------------------------------------------------\n");
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        snprintf(full_path, MAX_PATH_SIZE, "%s%s", FOLDER_PATH, entry->d_name);
        printf("%-10d %-3d %-80s\n", get_file_size(full_path), ++count, entry->d_name);
    }
    printf("-----------------------------------------------------------\n");
    printf(RESET);

    closedir(dir);
}

int create_file(const char *filename) {
    char full_path[MAX_PATH_SIZE+strlen(FOLDER_PATH)+1];
    snprintf(full_path, sizeof(full_path), "%s%s%s", FOLDER_PATH, filename, FILE_EXTENSION);

    FILE *check_file = fopen(full_path, "r");
    if (check_file != NULL) {
        printf(RED"Error: File '%s' already exists.\n"RESET, filename);
        fclose(check_file);
        return -1;
    }

    FILE *file = fopen(full_path, "w");
    if (file == NULL) {
        printf(RED"Error creating file %s.\n"RESET, filename);
        return -1;
    }
    printf(GREEN"File '%s%s' created successfully!\n"RESET, filename, FILE_EXTENSION);
    fprintf(file, "%s\n", DESCRIPTOR);
    fclose(file);
    return 0;
}

void create_file_wrapper() {
    if (get_file_count(FOLDER_PATH) >= MAX_FILES) {
        printf("Error: maximum file limit of %d reached.\n", MAX_FILES);
        wait_for_input();
        return;
    }

    char filename[MAX_PATH_SIZE];
    do {
        get_input(TYPE_STRING, filename, "Enter name of the file (up to 80 characters, without file extension):\n", is_filename_valid,
            RED"Error: the file name is either empty or too long, or with file extension.\n"RESET);
    } while (create_file(filename) == -1);
    wait_for_input();
}

Record get_record_input() {
    Record record = {0, 0, 0};

    get_input(TYPE_STRING, &record.region, "Enter region name (less than 40 characters): ", is_region_valid, "Error: region must be under 40 characters and not empty.\n");
    get_input(TYPE_UNSIGNED, &record.area, "Enter area (up to 1e9): ", is_record_number_in_range, "Error: area not in range.\n");
    get_input(TYPE_UNSIGNED, &record.population, "Enter population (up to 1e9): ", is_record_number_in_range, "Error: population not in range.\n");
    return record;
}

void append_record(const char *full_path, Record record) {
    FILE *file = fopen(full_path, "ab");
    if (file == NULL) {
        printf("Error opening file for appending.");
        return;
    }

    if (fwrite(&record, sizeof(Record), 1, file) != 1) {
        printf("Error writing record to file.");
    } else {
        printf(GREEN"Record saved successfully.\n"RESET);
    }

    fclose(file);
}

int read_records(const char *full_path) {
    FILE *file = fopen(full_path, "rb");
    if (file == NULL) {
        perror("Error opening file for reading.\n");
        return 0;
    }

    int c;
    while ((c = fgetc(file)) != EOF && c != '\n') {}

    Record record;
    ushort count = 0;
    printf("\nAll Records:\n");
    printf("-------------------------------------------------------------------\n");
    printf("%-3s %-40s %-10s %-10s\n", "#", "Region", "Area", "Population");
    printf("-------------------------------------------------------------------\n");

    while (fread(&record, sizeof(Record), 1, file) == 1) {
        printf("%-3u %-40s %-10u %-10u\n", ++count, record.region, record.area, record.population);
    }

    printf("-------------------------------------------------------------------\n");
    fclose(file);
    return 1;
}

int get_record_count(const char* full_path) {
    FILE* file = fopen(full_path, "rb");
    if (!file) return -1;

    fseek(file, strlen(DESCRIPTOR), SEEK_SET);
    int c;
    while ((c = fgetc(file)) != EOF && (c == '\n' || c == '\r')){}
    if (c != EOF) ungetc(c, file);

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file) - strlen(DESCRIPTOR) - 1;
    fclose(file);

    return file_size / sizeof(Record);
}

int edit_record(const char* full_path, const int index, Record updated_record) {
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
    size_t records_size = file_size - (desc_len + 2);
    int total_records = records_size / sizeof(Record);

    if (index >= total_records || index < 0) {
        free(buffer);
        return 0;
    }

    file = fopen(full_path, "wb");
    if (!file) {
        free(buffer);
        return 0;
    }
    char* records_start = buffer + desc_len + 2;
    fwrite(buffer, 1, desc_len + 2, file);
    if (index > 0) {
        fwrite(records_start, sizeof(Record), index, file);
    }
    fwrite(&updated_record, sizeof(Record), 1, file);
    if (index < total_records - 1) {
        fwrite(records_start + ((index + 1) * sizeof(Record)),
               sizeof(Record),
               total_records - index - 1,
               file);
    }

    free(buffer);
    fclose(file);
    return 1;
}

void edit_record_wrapper(const char* full_path) {
    read_records(full_path);

    ushort count = get_record_count(full_path);
    ushort index = 0;
    do {
        printf("Enter the index of the record to edit: ");
        if (scanf("%hu", &index) != 1) {
            printf("Error: wrong input.\n");
        } else if (index == 0 || index > count) {
            printf("Error: index should be from 1 to %hu.\n", count);
        }
        clear_input_buffer();
    } while (index == 0 || index > count);

    if (!edit_record(full_path, index-1, get_record_input())) {
        printf("Failed to edit record.\n");
        return;
    }
    printf(GREEN"Record edited successfully.\n"RESET);
}

int insert_record(const char* full_path, const int index, Record new_record) {
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

    file = fopen(full_path, "wb");
    if (!file) {
        free(buffer);
        return 0;
    }

    size_t desc_len = strlen(DESCRIPTOR);
    fwrite(buffer, 1, desc_len + 2, file);

    char* records_start = buffer + desc_len + 2;
    size_t records_size = file_size - (desc_len + 2);
    int total_records = records_size / sizeof(Record);

    if (index > 0) {
        fwrite(records_start, sizeof(Record), index, file);
    }

    fwrite(&new_record, sizeof(Record), 1, file);

    if (index < total_records) {
        fwrite(records_start + (index * sizeof(Record)),
               sizeof(Record),
               total_records - index,
               file);
    }
    free(buffer);
    fclose(file);
    return 1;
}

void insert_record_wrapper(const char* full_path) {
    read_records(full_path);

    ushort count = get_record_count(full_path);
    ushort index = 0;
    do {
        printf("Enter the index to insert the record: ");
        if (scanf("%hu", &index) != 1) {
            printf("Error: wrong input.\n");
        } else if (index == 0 || index > count) {
            printf("Error: index should be from 1 to %hu.\n", count);
        }
        clear_input_buffer();
    } while (index == 0 || index > count);

    Record record = get_record_input();
    if (!insert_record(full_path, index - 1, record)) {
        printf("Failed to insert record.\n");
        return;
    }
    printf(GREEN"Record inserted successfully.\n"RESET);
}

int delete_record(const char* full_path, const int index) {
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

    if (index < 0 || index >= total_records) {
        free(buffer);
        return 0;
    }

    file = fopen(full_path, "wb");
    if (!file) {
        free(buffer);
        return 0;
    }

    fwrite(buffer, 1, desc_len + 2, file);

    if (index > 0) {
        fwrite(records_start, sizeof(Record), index, file);
    }

    if (index < total_records - 1) {
        fwrite(records_start + (index + 1) * sizeof(Record),
               sizeof(Record),
               total_records - index - 1,
               file);
    }

    free(buffer);
    fclose(file);
    return 1;
}

void delete_record_wrapper(const char* full_path) {
    read_records(full_path);

    ushort count = get_record_count(full_path);
    ushort index = 0;
    do {
        printf("Enter the index to delete the record: ");
        if (scanf("%hu", &index) != 1) {
            printf("Error: wrong input.\n");
        } else if (index == 0 || index > count) {
            printf("Error: index should be from 1 to %hu.\n", count);
        }
        clear_input_buffer();
    } while (index == 0 || index > count);

    if (!delete_record(full_path, index - 1)) {
        printf("Failed to delete record.\n");
        return;
    }
    printf(GREEN"Record deleted successfully.\n"RESET);
}

void open_file_wrapper(const char *full_path) {
    char choice;
    do {
        system("cls");

        if(!read_records(full_path)) {
            printf("Cannot proceed with invalid file.\n");
            wait_for_input();
            return;
        }

        print_record_menu();
        printf("Choose an option (0-5): ");
        choice = getch();
        system("cls");
        switch (choice) {
            case '0':
                break;
            case '1':
                append_record(full_path, get_record_input());
                wait_for_input();
                break;
            case '2':
                edit_record_wrapper(full_path);
                wait_for_input();
                break;
            case '3':
                sort_by_wrapper(full_path);
                wait_for_input();
                break;
            case '4':
                insert_record_wrapper(full_path);
                wait_for_input();
                break;
            case '5':
                delete_record_wrapper(full_path);
                wait_for_input();
                break;
            default:
                printf("Error: there is no such option.");
                wait_for_input();
                break;
        }
    } while (choice != '0');
}

int validate_file_descriptor(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file) return 0;

    char first_line[256];
    if (!fgets(first_line, sizeof(first_line), file)) {
        fclose(file);
        return 0;
    }

    ushort len = strlen(first_line);
    if (len > 0 && first_line[len-1] == '\n') {
        first_line[len-1] = '\0';
    }

    fclose(file);
    return strcmp(first_line, DESCRIPTOR) == 0;
}

void select_file(const Operation operation) {
    if (operation != OPEN && operation != DELETE) {
        printf("Error: invalid operation used in select_file().\n");
        wait_for_input();
        return;
    }

    struct dirent *entry;
    char full_path[MAX_PATH_SIZE];
    DIR *dir = opendir(FOLDER_PATH);

    if (dir == NULL) {
        printf("Error opening directory.\n");
        wait_for_input();
        exit(0);
    }

    ushort count = 0;
    char file_names[MAX_FILES][MAX_PATH_SIZE];
    printf(BOLD);
    printf("\nDirectory : %s\n", FOLDER_PATH);
    printf("Size\t #\tName\n");
    printf("-----------------------------------------------------------\n");
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        snprintf(full_path, MAX_PATH_SIZE, "%s/%s", FOLDER_PATH, entry->d_name);
        printf("%d \t %d. %s\n", get_file_size(full_path), ++count, entry->d_name);
        strncpy(file_names[count - 1], entry->d_name, MAX_PATH_SIZE);
    }
    printf("-----------------------------------------------------------\n");
    printf(RESET);

    if (count == 0) {
        printf("No files found in the directory.\n");
        wait_for_input();
        closedir(dir);
        return;
    }

    ushort index = 0;
    do {
        printf("Enter the file index to perform an operation: ");
        if (scanf("%hu", &index) != 1) {
            printf("Error: wrong input.\n");
        } else if (index == 0 || index > count) {
            printf("Error: index should be from 1 to %hu.\n", count);
        }
        clear_input_buffer();
    } while (index == 0 || index > count);

    snprintf(full_path, MAX_PATH_SIZE, "%s%s", FOLDER_PATH, file_names[index - 1]);

    if (!validate_file_descriptor(full_path)) {
        printf("Error: the file has either different or no descriptor.\n");
        wait_for_input();
        closedir(dir);
        return;
    }

    switch (operation) {
        case OPEN:
            open_file_wrapper(full_path);
            break;
        case DELETE:
            if (remove(full_path) == 0) {
                printf(GREEN"File '%s' deleted successfully.\n"RESET, file_names[index - 1]);
            } else {
                printf("Error deleting file '%s'.\n", file_names[index - 1]);
            }
            wait_for_input();
            break;
    }

    closedir(dir);
}
