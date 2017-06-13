/**@file io.c
 *
 * @author Craig Hesling <craig@hesling.com>
 * @date Apr 21, 2017
 */

#include <Board_LoRaBUG.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
// #include <ti/drivers/I2C.h>
#include <ti/drivers/PIN.h>
//#include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
//#include <ti/drivers/Watchdog.h>

/* BIOS Header files */
#include <ti/sysbios/gates/GateMutexPri.h>

#include <driverlib/sys_ctrl.h> // SysCtrlSystemReset()

#define UART_PRINTF_BUFFER_SIZE 128 // SHould be at least 5+17 for uarthexdump
#define UART_RECV_LINE_LENGTH 768
#define HEXDUMP_STR_PREFIX "# " // Must be defined, but can be ""

/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State ledPinState;
static PIN_Handle hdrPinHandle;
static PIN_State hdrPinState;

static PIN_Handle btnPinHandle;
static PIN_State btnPinState;

/* UART driver handle */
static UART_Handle uartHandle;

static GateMutexPri_Struct uartMutexStruct;

// Buffer for snprintf and uartwrite
static char uartsbuf[UART_PRINTF_BUFFER_SIZE];
static char uartlinebuf[UART_RECV_LINE_LENGTH+1]; // +1 for '\0'

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config ledPinTable[] = {
    Board_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

PIN_Config hdrPinTable[] = {
     Board_HDR_HDIO0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
     Board_HDR_HDIO1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
     Board_HDR_HDIO2 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
     Board_HDR_ADIO0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL,
     Board_HDR_ADIO1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL,
     Board_HDR_ADIO2 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL,
     Board_HDR_ADIO3 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL,
     Board_HDR_ADIO4 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL,
     Board_HDR_ADIO5 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL,
     Board_HDR_ADIO6 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL,
     Board_HDR_ADIO7 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL,
     PIN_TERMINATE
};

PIN_Config btnPinTable[] = {
    Board_BTN | PIN_INPUT_EN | PIN_NOPULL | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};

/**
 * Do a system hard reset when triggered
 */
static void btnIntCallback(PIN_Handle handle, PIN_Id pinId)
{
    SysCtrlSystemReset();
}

void setuppins()
{
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
    if (ledPinHandle == NULL)
    {
        System_abort("Failed to open board LED pins\n");
    }
    hdrPinHandle = PIN_open(&hdrPinState, hdrPinTable);
    if (hdrPinHandle == NULL)
    {
        System_abort("Failed to open board header pins\n");
    }
    btnPinHandle = PIN_open(&btnPinState, btnPinTable);
    if (btnPinHandle == NULL)
    {
        System_abort("Failed to open board BTN pin\n");
    }
    if (PIN_registerIntCb(btnPinHandle, btnIntCallback) != PIN_SUCCESS)
    {
        System_abort("Failed to register btn int callback\n");
    }
}

void setupuart()
{
    GateMutexPri_construct(&uartMutexStruct, NULL);

    UART_Params uartParams;
    UART_Params_init(&uartParams);
    uartParams.baudRate = 115200; // 3000000
    uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.writeMode = UART_MODE_BLOCKING;
    // UARTCC26xx does not implement any of these parameters
//    uartParams.readDataMode = UART_DATA_TEXT;
//    uartParams.writeDataMode = UART_DATA_TEXT;
//    uartParams.readReturnMode = UART_RETURN_NEWLINE;
//    uartParams.readEcho = UART_ECHO_ON;
    uartHandle = UART_open(Board_UART, &uartParams);
    if (!uartHandle)
    {
        System_abort("Failed to open UART\n");
    }

//    // Initialize the logger output
//    UartLog_init(hUart);
//    UartPrintf_init()
}

void uartwrite(const char *str, size_t size)
{
    IArg key = GateMutexPri_enter(GateMutexPri_handle(&uartMutexStruct));
    if (UART_write(uartHandle, str, size) == UART_ERROR)
    {
        System_abort("Failed to write str to uart\n");
    }
    GateMutexPri_leave(GateMutexPri_handle(&uartMutexStruct), key);
}

/**
 * Print the given C string to UART followed by carriage return and new line.
 *
 * We define this little function, since we want the primary
 * logging to be over cJTAG, not UART.
 * @param str The C string to print
 */
void uartputs(const char *str)
{
    IArg key = GateMutexPri_enter(GateMutexPri_handle(&uartMutexStruct));
    if (strlen(str) && UART_write(uartHandle, str, strlen(str)) == UART_ERROR)
    {
        System_abort("Failed to write str to uart\n");
    }
    if (UART_write(uartHandle, "\r\n", 2) == UART_ERROR)
    {
        System_abort("Failed to write CRLR to uart\n");
    }
    GateMutexPri_leave(GateMutexPri_handle(&uartMutexStruct), key);
}

void uartprintf(const char *format, ...)
{
    IArg key = GateMutexPri_enter(GateMutexPri_handle(&uartMutexStruct));
    va_list args;
    va_start (args, format);
    vsnprintf(uartsbuf, sizeof(uartsbuf), format, args);
    va_end(args);

    if (UART_write(uartHandle, uartsbuf, strlen(uartsbuf)) == UART_ERROR)
    {
        System_abort("Failed to write formatted buffer to uart\n");
    }
    GateMutexPri_leave(GateMutexPri_handle(&uartMutexStruct), key);
}

/**
 * Fetch a line from UART.
 * @return The null terminated line of at most \a UART_RECV_LINE_LENGTH string length.
 */
char *uartreadline() {
	size_t index;
	for (index = 0; index < (sizeof(uartlinebuf) - 1); index++) {
		int count = UART_read(uartHandle, uartlinebuf + index, 1);
		if (count == UART_ERROR) {
			System_abort("Failed to read line from uart\n");
		}
		if (uartlinebuf[index] == '\n') {
			break;
		}
	}
	uartlinebuf[index] = '\0';
	return uartlinebuf;
}

void setPin(PIN_Id pin, uint_t value)
{
    if (PIN_setOutputValue(hdrPinHandle, pin, value) != PIN_SUCCESS)
    {
        System_abort("Failed to set pin value\n");
    }
}

void togglePin(PIN_Id pin)
{
    if (PIN_setOutputValue(hdrPinHandle, pin, !PIN_getOutputValue(pin))
            != PIN_SUCCESS)
    {
        System_abort("Failed to toggle pin value\n");
    }
}

void setLed(PIN_Id pin, uint_t value)
{
    if (PIN_setOutputValue(ledPinHandle, pin, value) != PIN_SUCCESS)
    {
        System_abort("Failed to set pin value\n");
    }
}

void toggleLed(PIN_Id pin)
{
    if (PIN_setOutputValue(ledPinHandle, pin, !PIN_getOutputValue(pin))
            != PIN_SUCCESS)
    {
        System_abort("Failed to toggle pin value\n");
    }
}

/* Utilities */

void hexdump(uint8_t *data, size_t size) {
    // Stolen from https://gist.github.com/ccbrown/9722406
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        printf("%02X ", ((unsigned char*)data)[i]);
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i+1) % 8 == 0 || i+1 == size) {
            printf(" ");
            if ((i+1) % 16 == 0) {
                printf("|  %s \n", ascii);
            } else if (i+1 == size) {
                ascii[(i+1) % 16] = '\0';
                if ((i+1) % 16 <= 8) {
                    printf(" ");
                }
                for (j = (i+1) % 16; j < 16; ++j) {
                    printf("   ");
                }
                printf("|  %s \n", ascii);
            }
        }
    }
}

void uarthexdump(uint8_t *data, size_t size) {
    // Stolen from https://gist.github.com/ccbrown/9722406
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';

    for (i = 0; i < size; ++i) {
    	if ((i % 16) == 0) uartprintf(HEXDUMP_STR_PREFIX);
        uartprintf("%02X ", ((unsigned char*)data)[i]);
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i+1) % 8 == 0 || i+1 == size) {
        	uartprintf(" ");
            if ((i+1) % 16 == 0) {
            	// done with row
            	uartprintf("|  %s \n", ascii);
            } else if (i+1 == size) {
            	// must be done with stream
                ascii[(i+1) % 16] = '\0';
                if ((i+1) % 16 <= 8) {
                	uartprintf(" ");
                }
                for (j = (i+1) % 16; j < 16; ++j) {
                	uartprintf("   ");
                }
                uartprintf("|  %s \n", ascii);
            }
        }
    }
}
