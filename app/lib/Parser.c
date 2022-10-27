#include "Parser.h"

AT_COMMAND_DATA data;
uint8_t current_col = 0;

void print_error(uint16_t state, char* expected, char got) {
    printf("ERROR (%d):\n\tEXPECTED: %s\n\tGOT: %c (%d)\n", state, expected, got, got);
}

void data_push_char(char c) {
    if (current_col >= AT_COMMAND_MAX_LINE_SIZE ) return;
    if (data.line_count >= AT_COMMAND_MAX_LINES) return;
    data.data[data.line_count][current_col++] = c;
}

void data_push_line() {
    data.data[data.line_count][current_col] = '\0';
    if (data.line_count >= AT_COMMAND_MAX_LINES) return;
    data.line_count++;
    current_col = 0;
}

STATE_MACHINE_RETURN_VALUE parse_char_(uint8_t current_char, uint16_t *state) {
    switch (*state) {
        case 0: { // starting state
            data = (AT_COMMAND_DATA) {0};
            if (current_char == '\r') {
                *state = 1;
                break;
            }
            print_error(*state, "\\r", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 1: { // <CR>
            if (current_char == '\n') {
                *state = 2;
                break;
            }
            print_error(*state, "\\n", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 2: { // <CR><LF>
            if (current_char == 'O') {
                *state = 3;
                break;
            }
            else if (current_char == 'E') {
                *state = 4;
                break;
            }
            else if (current_char == '+') {
                *state = 10;
                break;
            }
            print_error(*state, "O, E or +", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 3: { // "O"
            if (current_char == 'K') {
                data.ok = 1;
                *state = 8;
                break;
            }
            print_error(*state, "K", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 4: { // "E"
            if (current_char == 'R') {
                *state = 5;
                break;
            }
            print_error(*state, "R", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 5: { // "ER"
            if (current_char == 'R') {
                *state = 6;
                break;
            }
            print_error(*state, "R", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 6: { // "ERR"
            if (current_char == 'O') {
                *state = 7;
                break;
            }
            print_error(*state, "O", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 7: { // "ERRO"
            if (current_char == 'R') {
                data.ok = 0;
                *state = 8;
                break;
            }
            print_error(*state, "R", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 8: { // "OK" or "ERROR"
           if (current_char == '\r') {
                *state = 9;
                break;
            }
            print_error(*state, "\\r", current_char);
            return STATE_MACHINE_ERROR;            
        }
        case 9: { // <CR>
            if (current_char == '\n') {
                return STATE_MACHINE_READY;
            }
            print_error(*state, "\\n", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 10: { // "+"
            if (' ' <= current_char && current_char <= '~') {
                data_push_char(current_char);
                *state = 11;
                break;
            }
            print_error(*state, "any printable character", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 11: { // "+"
            if (' ' <= current_char && current_char <= '~') {
                data_push_char(current_char);
                *state = 11;
                break;
            }
            else if (current_char == '\r') {
                data_push_line();
                *state = 12;
                break;
            }
            print_error(*state, "any printable character or \\r", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 12: {
            if (current_char == '\n') {
                *state = 13;
                break;
            }
            print_error(*state, "\\n", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 13: {
            if (current_char == '+') {
                *state = 10;
                break;
            }
            else if (current_char == '\r') {
                data.data[data.line_count][current_col] = '\0';
                *state = 14;
                break;
            }
            print_error(*state, "+ or \\r", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 14: {
            if (current_char == '\n') {
                *state = 15;
                break;
            }
            print_error(*state, "\\n", current_char);
            return STATE_MACHINE_ERROR;
        }
        case 15: {
            if (current_char == 'O') {
                *state = 3;
                break;
            }
            else if (current_char == 'E') {
                *state = 4;
                break;
            }
            print_error(*state, "O or E", current_char);
            return STATE_MACHINE_ERROR;
        }
    }
    return STATE_MACHINE_NOT_READY;
}

STATE_MACHINE_RETURN_VALUE parse_char(uint8_t current_char) {
    static uint16_t state = 0;

    STATE_MACHINE_RETURN_VALUE ret = parse_char_(current_char, &state);

    if (ret == STATE_MACHINE_READY || ret == STATE_MACHINE_ERROR) {
        state = 0;
    }
    
    return ret;
}

void print_data() {
    printf("Status: %s\n", data.ok ? "OK" : "ERROR");
    for (uint32_t i = 0; i < data.line_count && i < AT_COMMAND_MAX_LINES; i++) {
        printf("%s\n", data.data[i]);
    }
}