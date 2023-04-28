#ifndef __PARSER_AT__
#define __PARSER_AT__

#define AT_COMMAND_MAX_LINES 100
#define AT_COMMAND_MAX_LINE_SIZE 128

#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t ok;
    uint8_t data[AT_COMMAND_MAX_LINES][AT_COMMAND_MAX_LINE_SIZE + 1];
    uint32_t line_count;
} AT_COMMAND_DATA;

typedef enum {
    STATE_MACHINE_NOT_READY,
    STATE_MACHINE_READY,
    STATE_MACHINE_ERROR
} STATE_MACHINE_RETURN_VALUE;

STATE_MACHINE_RETURN_VALUE parse_char(uint8_t current_char);

void print_data();

#endif
