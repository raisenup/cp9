#pragma once
#include <stdbool.h>

#define RESET       "\x1b[0m"
#define BOLD        "\x1b[1m"
#define ITALIC      "\x1b[3m"

#define BLACK       "\x1b[30m"
#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[34m"
#define MAGENTA     "\x1b[35m"
#define CYAN        "\x1b[36m"
#define WHITE       "\x1b[37m"

#define MAX_PATH_SIZE 255

typedef unsigned short ushort;

typedef enum {
    TYPE_USHORT,
    TYPE_UNSIGNED,
    TYPE_FLOAT,
    TYPE_STRING,
} DataType;

void clear_input_buffer();

void wait_for_input();

bool is_filename_valid(void *value);

bool is_region_valid(void *value);

bool is_positive(void *value);

void get_input(const DataType type, void *variable, const char *print_msg, bool (*condition)(void*), const char *error_msg);

void print_app_description();

void print_file_menu();

void print_record_menu();

void print_sort_order_menu();

void print_sort_by_menu();
