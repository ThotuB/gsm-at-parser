#include <stdio.h>
#include <stdlib.h>

#include "../lib/Parser.h"

void parse(FILE* file) {
    while (1) {
        char current_char = fgetc(file);

        switch (parse_char(current_char)) {
            case STATE_MACHINE_READY: {
                print_data();
            }
            case STATE_MACHINE_ERROR: {
                printf("PARSER ERROR\n");
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
        return EXIT_FAILURE;
    }

    const char* filename = argv[1];
    FILE* file = fopen(filename, "r");

    if (file) {
        parse(file);
        fclose(file);
    }
    return 0;
}