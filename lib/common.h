#pragma once

#define FOLDER_PATH "../files/"
#define FILE_EXTENSION ".dat"
#define DESCRIPTOR "C@UsE1GN0R/\\NCE1SBL1ss"
#define MAX_FILES 100

typedef struct Record {
    char region[40];
    unsigned area;
    unsigned population;
} Record;

typedef enum {
    OPEN,
    DELETE,
} Operation;

void view_files();

void create_file_wrapper();

void select_file(const Operation operation);
