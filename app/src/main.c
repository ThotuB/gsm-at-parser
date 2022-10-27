#include <stdio.h>
#include <stdlib.h>

#include "../lib/Parser.h"

void parse(FILE* file) {
    char current_char;
    while ((current_char = fgetc(file)) != EOF) {
        switch (parse_char(current_char)) {
            case STATE_MACHINE_READY: {
                print_data();
                break;
            }
            case STATE_MACHINE_ERROR: {
                return;
            }
            case STATE_MACHINE_NOT_READY: break;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("error: wrong number of arguments\n");
        printf("usage: ./main.exe sample.txt\n");
        exit(EXIT_FAILURE);
    }

    const char* filename = argv[1];
    FILE* file = fopen(filename, "r");

    if (file) {
        parse(file);
        fclose(file);
    }
    return 0;
}