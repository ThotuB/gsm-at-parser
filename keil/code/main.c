#include <DRV\drv_lcd.h>
#include <DRV\drv_led.h>
#include <DRV\drv_sdram.h>
#include <DRV\drv_touchscreen.h>
#include <DRV\drv_uart.h>
#include <assert.h>
#include <retarget.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utils\timer_software.h>
#include <utils\timer_software_init.h>

// #include "LPC177x_8x.h"
#include "Parser.h"
#include "system_LPC177x_8x.h"

void CreateSection_Sizes(uint32_t yi, uint32_t xi, uint32_t h, uint32_t w, LCD_PIXEL color) {
    uint32_t x, y;

    for (y = yi; y < yi + h; y++) {
        for (x = xi; x < xi + w; x++) {
            DRV_LCD_PutPixel(x, y, color.red, color.green, color.blue);
        }
    }
}

void CreateSection_Points(uint32_t y1, uint32_t x1, uint32_t y2, uint32_t x2, LCD_PIXEL color) {
    uint32_t x, y;

    for (y = y1; y < y2; y++) {
        for (x = x1; x < x2; x++) {
            DRV_LCD_PutPixel(x, y, color.red, color.green, color.blue);
        }
    }
}

void CreateUI() {
    uint32_t x, y;
    LCD_PIXEL white = {255, 255, 255};
    LCD_PIXEL dark = {54, 57, 63};
    LCD_PIXEL darker = {47, 49, 54};
    LCD_PIXEL darkest = {32, 34, 37};

    // create background
    CreateSection_Points(0, 0, LCD_HEIGHT, LCD_WIDTH, darker);

    // create status info section
    CreateSection_Points(LCD_HEIGHT / 3 - 4, LCD_WIDTH / 3 - 4, LCD_HEIGHT / 2 + 4, LCD_WIDTH / 2 + 4, darkest);
    CreateSection_Points(LCD_HEIGHT / 3, LCD_WIDTH / 3, LCD_HEIGHT / 2, LCD_WIDTH / 2, darker);

    // create status command section
    CreateSection_Sizes(2 * LCD_HEIGHT / 3, 90, 50, LCD_WIDTH - 70, darker);
    DRV_LCD_Puts("Hello", 2 * LCD_HEIGHT / 3 + 10, 100, white, darker, TRUE);

    // create buttons
    CreateSection_Sizes(2 * LCD_HEIGHT / 3, 20, 50, 50, darker);
    CreateSection_Sizes(2 * LCD_HEIGHT / 3, LCD_WIDTH - 70, 50, 50, darker);
}

void SendCommand(const char *command) {
    DRV_UART_FlushRX(UART_3);
    DRV_UART_FlushTX(UART_3);
    DRV_UART_Write(UART_3, (uint8_t *)command, strlen(command));
}

void GetCommandResponse(timer_software_handler_t handler) {
    uint8_t ch;
    BOOLEAN ready = FALSE;

    TIMER_SOFTWARE_reset_timer(handler);
    TIMER_SOFTWARE_start_timer(handler);

    while ((!TIMER_SOFTWARE_interrupt_pending(handler)) && (ready == FALSE)) {
        while (DRV_UART_BytesAvailable(UART_3) > 0) {
            DRV_UART_ReadByte(UART_3, &ch);
            if (parse_char(ch) != STATE_MACHINE_NOT_READY) {
                ready = TRUE;
            }
        }
    }
}

void ExecuteCommand(const char *command, timer_software_handler_t handler) {
    SendCommand(command);
    GetCommandResponse(handler);
}

BOOLEAN CommandResponseValid(timer_software_handler_t handler) {
    if (TIMER_SOFTWARE_interrupt_pending(handler)) {
        return FALSE;
    }
    if (!data.ok) {
        return FALSE;
    }
    return TRUE;
}

void BoardInit() {
    TIMER_SOFTWARE_init_system();
    DRV_SDRAM_Init();

    initRetargetDebugSystem();
    DRV_LCD_Init();
    DRV_LCD_ClrScr();
    DRV_LCD_PowerOn();

    DRV_TOUCHSCREEN_Init();
    DRV_TOUCHSCREEN_SetTouchCallback();
}

void handleCSQ() {
    uint32_t rssi_value_asu = atoi((const char *)data.data[0] + 5);
    uint32_t rssi_value_dbmw = 2 * rssi_value_asu - 113;

    printf("GSM Modem Signal: %d ASU -> %d dBmW\n", rssi_value_asu, rssi_value_dbmw);
}

void handleCREG() {
    uint8_t n, stat;

    uint8_t index;
    uint8_t size = 0;
    char *values[4];

    char *token = strtok((char *)data.data[0], " ");
    token = strtok(NULL, ",");

    while (token != NULL) {
        values[size] = token;
        token = strtok(NULL, ",");
        size++;
    }

    printf("State of Registration: ");

    n = atoi(values[0]);
    switch (n) {
        case 0: {
            printf("Unknown\n");
            return;
        }
        case 1:
        case 2:
            break;
    }

    stat = atoi(values[1]);
    switch (stat) {
        case 0: {
            printf("Not Registered, MT - not searching\n");
            break;
        }
        case 1: {
            printf("Home Network\n");
            break;
        }
        case 2: {
            printf("Not Registered, MT - searching\n");
            break;
        }
        case 3: {
            printf("Registration Denied\n");
            break;
        }
        case 4: {
            printf("Unknown\n");
            break;
        }
        case 5: {
            printf("Roaming");
            break;
        }
    }
}

void handleCOPS() {
    uint8_t n, stat;

    uint8_t index;
    uint8_t size = 0;
    char *values[4];

    char *token = strtok((char *)data.data[0], " ");
    token = strtok(NULL, ",");

    while (token != NULL) {
        values[size] = token;
        token = strtok(NULL, ",");
        size++;
    }

    printf("Operator Name: ");

    if (size < 3) {
        printf("Unknown\n");
        return;
    }

    printf("%s\n", values[2]);
}

void handleGSN()  // IMEI
{
    printf("Modem IMEI: %s\n", data.data[0]);
}

void handleGMI()  // Manufacturer
{
    printf("Modem Manufacturer: %s\n", data.data[0]);
}

void handleGMR()  // Revision
{
    printf("Modem Software Version: %s\n", data.data[0] + 9);
}

void handleNotImplemented() {
    printf("???\n");
}

int main(void) {
    timer_software_handler_t error_handler;
    timer_software_handler_t timer_handler;

    const char *commands[] = {
        "AT+CSQ\r\n",
        "AT+CREG?\r\n",
        "AT+COPS?\r\n",
        "AT+GSN\r\n",
        "AT+GMI\r\n",
        "AT+GMR\r\n"};

    void (*handlers[])(void) = {
        handleCSQ,
        handleCREG,
        handleCOPS,
        handleGSN,
        handleGMI,
        handleGMR};

    uint8_t commandIdx;

    BoardInit();
    CreateUI();

    DRV_UART_Configure(UART_3, UART_CHARACTER_LENGTH_8, 115200, UART_PARITY_NO_PARITY, 1, TRUE, 3);
    DRV_UART_Configure(UART_2, UART_CHARACTER_LENGTH_8, 115200, UART_PARITY_NO_PARITY, 1, TRUE, 3);

    DRV_UART_Write(UART_3, (uint8_t *)commands[0], strlen(commands[0]));
    TIMER_SOFTWARE_Wait(1000);
    DRV_UART_Write(UART_3, (uint8_t *)commands[0], strlen(commands[0]));
    TIMER_SOFTWARE_Wait(1000);
    DRV_UART_Write(UART_3, (uint8_t *)commands[0], strlen(commands[0]));
    TIMER_SOFTWARE_Wait(1000);

    timer_handler = TIMER_SOFTWARE_request_timer();
    TIMER_SOFTWARE_configure_timer(timer_handler, MODE_1, 3000, 1);
    TIMER_SOFTWARE_start_timer(timer_handler);

    error_handler = TIMER_SOFTWARE_request_timer();
    TIMER_SOFTWARE_configure_timer(error_handler, MODE_1, 5000, 1);

    while (1) {
        if (TIMER_SOFTWARE_interrupt_pending(timer_handler)) {
            for (commandIdx = 0; commandIdx < 6; commandIdx++) {
                ExecuteCommand(commands[commandIdx], error_handler);
                if (CommandResponseValid(error_handler)) {
                    // print_data();
                    (*handlers[commandIdx])();
                } else {
                    printf("ERROR: Could not get data, timer timed out\n");
                }
                TIMER_SOFTWARE_clear_interrupt(timer_handler);
            }
            printf("\n");
        }
    }

    return 0;
}
