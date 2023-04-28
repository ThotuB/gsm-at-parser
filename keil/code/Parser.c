#include "Parser.h"
#include <stdlib.h>
#include <stdbool.h>

AT_COMMAND_DATA data;
uint8_t current_col = 0;

static const AT_COMMAND_DATA emptyData;

STATE_MACHINE_RETURN_VALUE parser_error(uint16_t state, char* expected, char got) {
    printf("ERROR (%d):\n\tEXPECTED: %s\n\tGOT: %c (%d)\n", state, expected, got, got);
    return STATE_MACHINE_ERROR;
}

void data_push_char(char c) {
    if (current_col >= AT_COMMAND_MAX_LINE_SIZE) return;
    if (data.line_count >= AT_COMMAND_MAX_LINES) return;
    data.data[data.line_count][current_col++] = c;
}

void data_push_line() {
    if (data.line_count >= AT_COMMAND_MAX_LINES) return;
    data.data[data.line_count][current_col] = '\0';
    data.line_count++;
    current_col = 0;
}

STATE_MACHINE_RETURN_VALUE parse_char_(uint8_t current_char, uint16_t *state, bool *flag) {
    switch (*state) {
        case 0: { // starting state
            data = emptyData;
            if (current_char == '\r') {
                *state = 1;
                break;
            }
            return parser_error(*state, "\\r", current_char);
        }
        case 1: { // <CR>
            if (current_char == '\n') {
                *state = 2;
                break;
            }
            return parser_error(*state, "\\n", current_char);
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
            else if (' ' <= current_char && current_char <= '~'){
                data_push_char(current_char);
                *state = 16;
                *flag = true;
                break;
            }
            return parser_error(*state, "O, E, + or ASCII", current_char);
        }
        case 3: { // "O"
            if (current_char == 'K') {
                data.ok = 1;
                *state = 8;
                break;
            }
            return parser_error(*state, "K", current_char);
        }
        case 4: { // "E"
            if (current_char == 'R') {
                *state = 5;
                break;
            }
            return parser_error(*state, "R", current_char);
        }
        case 5: { // "ER"
            if (current_char == 'R') {
                *state = 6;
                break;
            }
            return parser_error(*state, "R", current_char);
        }
        case 6: { // "ERR"
            if (current_char == 'O') {
                *state = 7;
                break;
            }
            return parser_error(*state, "O", current_char);
        }
        case 7: { // "ERRO"
            if (current_char == 'R') {
                data.ok = 0;
                *state = 8;
                break;
            }
            return parser_error(*state, "R", current_char);
        }
        case 8: { // "OK" or "ERROR"
           if (current_char == '\r') {
                *state = 9;
                break;
            }
            return parser_error(*state, "\\r", current_char);            
        }
        case 9: { // <CR>
            if (current_char == '\n') {
                return STATE_MACHINE_READY;
            }
            return parser_error(*state, "\\n", current_char);
        }
        case 10: { // "+"
            if (' ' <= current_char && current_char <= '~') {
                data_push_char(current_char);
                *state = 11;
                break;
            }
            return parser_error(*state, "ASCII", current_char);
        }
        case 11: { // "+[chars]"
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
            return parser_error(*state, "ASCII or \\r", current_char);
        }
        case 12: {  // "+[chars]<CR>"
            if (current_char == '\n') {
                *state = 13;
                break;
            }
            return parser_error(*state, "\\n", current_char);
        }
        case 13: { // "+[chars]<CR><LF>"
            if (current_char == '\r') {
                data.data[data.line_count][current_col] = '\0';
                *state = 14;
                break;
            }
            if (!*flag) {
                if (current_char == '+') {
                    *state = 10;
                    break;
                }
                return parser_error(*state, "+ or \\r", current_char);
            }
            else {
                return parser_error(*state, "\\r", current_char);
            }
        }
        case 14: { // "+[chars]<CR><LF><CR>"
            if (current_char == '\n') {
                *state = 15;
                break;
            }
            return parser_error(*state, "\\n", current_char);
        }
        case 15: { // "+[chars]<CR><LF><CR><LF>"
            if (current_char == 'O') {
                *state = 3;
                break;
            }
            else if (current_char == 'E') {
                *state = 4;
                break;
            }
            return parser_error(*state, "O or E", current_char);
        }
        case 16: { // [chars]+
            if (' ' <= current_char && current_char <= '~') {
                data_push_char(current_char);
                *state = 16;
                break;
            }
            else if (current_char == '\r') {
                data_push_line();
                *state = 12;
                break;
            }
            return parser_error(*state, "ASCII or \\r", current_char);
        }
    }
    return STATE_MACHINE_NOT_READY;
}

STATE_MACHINE_RETURN_VALUE parse_char(uint8_t current_char) {
    static uint16_t state = 0;
    static bool flag = false;

    STATE_MACHINE_RETURN_VALUE ret = parse_char_(current_char, &state, &flag);

    if (ret == STATE_MACHINE_READY) {
        state = 0;
        flag = false;
    }
    
    return ret;
}

void print_data(void) {
		uint32_t i;
    printf("Status: %s\n", data.ok ? "OK" : "ERROR");
    for (i = 0; i < data.line_count && i < AT_COMMAND_MAX_LINES; i++) {
        printf("%3u: %s\n", i + 1, data.data[i]);
    }
}

AT_COMMAND_DATA get_data(void) {
	return data;
}
