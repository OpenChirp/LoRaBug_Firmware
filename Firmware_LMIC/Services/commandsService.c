/******************************************************************************
 *  Copyright (C) 2015, WiSE Lab, Carnegie Mellon University
 *  All rights reserved.
 *
 *  This software is the property of Carnegie Mellon University. Source may
 *  be modified, but this license does not allow distribution.  Binaries built
 *  for research purposes may be freely distributed, but must acknowledge
 *  Carnegie Mellon University.  No other use or distribution can be made
 *  without the written permission of the authors and Carnegie Mellon
 *  University.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  Contributing Author(s):
 *  Artur Balanuta
 *
 * @file  commandsService.c
 *
 * @brief This file contains the LoRa Service Interface
 *        Created on: Feb 24, 2017
 *******************************************************************************/

#ifndef SERVICES_COMMANDSSERVICE_C_
#define SERVICES_COMMANDSSERVICE_C_

/*********************************************************************
 * INCLUDES
 */
#include <stdio.h>
#include <string.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/UART.h>

/* Board Header files */
#include "Config/Board.h"

/* lmic LoRa Lib */
#include <lmic.h>
#include <oslmic.h>
#include <hal.h>

/* UART logs */
//#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>   // For Log_warning1("Warning number #%d", 4); things
#include <xdc/runtime/Diags.h> // For Log_print0(Diags_USER1, "hello"); things.

#include "commandsService.h"


/*******************************************************************************
 * MACROS
 */

#define DELAY_MS(i)    Task_sleep(((i) * 1000) / Clock_tickPeriod)

/*******************************************************************************
 * CONSTANTS
 */
#define COMMANDS_TASK_PRIORITY                     5

#ifndef COMMANDS_TASK_STACK_SIZE
#define COMMANDS_TASK_STACK_SIZE                   2000
#endif

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

// Task configuration
Task_Struct commandsTask;
Char commandsTaskStack[COMMANDS_TASK_STACK_SIZE];

UART_Handle uart = NULL;

/*********************************************************************
 * LOCAL FUNCTIONS
 */


const char clearScreen[] = {"\f"};
const char newline[] = {"\r\n"};
const char echoPrompt[] = {"LoRaBUG:~ $ "};

unsigned char tempBuf;
char buffer[100];        // Transmit buffer
volatile uint8_t buffIndex = 0;

void Process_buffer(UART_Handle uart){

    buffer[buffIndex++] = '\0'; // add a string terminator

    const char delim[2] = " ";

    char *token = strtok(buffer, delim);

    while(token != NULL) {
        //puts(token);
        UART_write(uart, token, strlen(token));
        UART_write(uart, newline, sizeof(newline));
        token = strtok(NULL, " ");
     }

    buffIndex = 0;
    memset(buffer, '\0', 100);
}

/*********************************************************************
 * @fn      LoraRaw_taskFxn
 * @return  None.
 */
static void Commands_taskFxn (UArg a0, UArg a1)
{

    Board_initUART();

    UART_Params uartParams;
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    //uartParams.readCallback  = readCallback;
    //uartParams.readMode      = UART_MODE_CALLBACK;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;

    UART_Handle uart = UART_open(Board_UART, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
        return;
    }

    UART_write(uart, clearScreen, sizeof(clearScreen));
    UART_write(uart, echoPrompt, sizeof(echoPrompt));

    memset(buffer, '\0', 100);

    while(1) {
        UART_read(uart, &tempBuf, sizeof(tempBuf));

        if(tempBuf == 13){
            UART_write(uart, newline, sizeof(newline));
            Process_buffer(uart);
            UART_write(uart, echoPrompt, sizeof(echoPrompt));
        }
        else{
            buffer[buffIndex++] = tempBuf;
            UART_write(uart, &tempBuf, 1);
        }

    }

}


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

void Commands_createTask()
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = commandsTaskStack;
  taskParams.stackSize = COMMANDS_TASK_STACK_SIZE;
  taskParams.priority = COMMANDS_TASK_PRIORITY;

  Task_construct(&commandsTask, Commands_taskFxn, &taskParams, NULL);
}

#endif /* SERVICES_COMMANDSSERVICE_C_ */
