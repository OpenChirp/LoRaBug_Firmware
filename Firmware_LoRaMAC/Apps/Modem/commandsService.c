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
#include "Board_LoRaBUG.h"

/* UART logs */
//#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>   // For Log_warning1("Warning number #%d", 4); things
#include <xdc/runtime/Diags.h> // For Log_print0(Diags_USER1, "hello"); things.

#include <driverlib/sys_ctrl.h> // SysCtrlSystemReset()

#include "commandsService.h"

/*******************************************************************************
 * MACROS
 */

#define DELAY_MS(i)    Task_sleep(((i) * 1000) / Clock_tickPeriod)

/*******************************************************************************
 * CONSTANTS
 */
#define COMMANDS_TASK_PRIORITY                     1

#ifndef COMMANDS_TASK_STACK_SIZE
#define COMMANDS_TASK_STACK_SIZE                   1000
#endif

// Main Menu
const char SYS[] = {"sys"};
const char MAC[] = {"mac"};
const char RADIO[] = {"radio"};

const char STATUS_OK[] = {"ok"};
const char STATUS_INVALID_PARAM[] = {"invalid_param"};
const char STATUS_KEYS_NOT_INIT[] = {"keys_not_init"};
const char STATUS_NOT_IMPLEMENTED[] = {"not_implemented"};

// Others
const char HELP[] = {"help"};
const char SLEEP[] = {"sleep"};
const char RESET[] = {"reset"};
const char GET[] = {"get"};
const char SET[] = {"set"};
const char TX[] = {"tx"};
const char JOIN[] = {"join"};
const char SAVE[] = {"save"};
const char PAUSE[] = {"pause"};
const char RESUME[] = {"resume"};

//enum commands { sys="sys", mac="mac", radio="radio" };

//enum status { ok="ok", invalid="invalid_param", keys_not_init="keys_not_init",
//    not_implemented="not_implemented"};
//
//enum commands_sys { sleep="sleep", reset="reset", get="get", set="set" };
//enum commands_mac { reset="reset", tx="tx", join="join", save="save",
//    pause="pause", resume="resume", get="get", set="set" };
//enum commands_radio { sleep="sleep", reset="reset" };
//
//
//enum set_mac_commands_mac_set { devaddr="devaddr", deveui="deveui",
//    appeui="appeui", nwkskey="nwkskey", appskey="appskey", appkey="appkey",
//    pwridx="pwridx", dr="dr", adr="adr", bat="bat", retx="retx",
//    linkchk="linkchk", rxdelay1="rxdelay1", ar="ar", rx2="rx2", sync="sync",
//    upctr="upctr", dnctr="dnctr", ch="ch"};


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
void Process_buffer_help();
void Process_buffer_sys(char* token);
void Process_buffer_mac(char* token);
void Process_buffer_radio(char* token);

void Aux_delete_terminal_char();


const char clearScreen[] = {"\f"};
const char newline[] = {"\r\n"};
const char backspace[] = {"\b"};
const char zero[] = {"0"};
const char echoPrompt[] = {"LoRaBUG:~ $ "};
const char delim[] = " ";

unsigned char tempBuf;
char buffer[100];        // Transmit buffer
char prevBuffer[100];    // last executed command
volatile uint8_t buffIndex = 0;

void Process_buffer(){

    buffer[buffIndex++] = '\0'; // add a string terminator

    char *token = strtok(buffer, delim);

    while(token != NULL) {

        if (!strcmp(token, HELP)) {
            Process_buffer_help();
            break;
        }
        else if (!strcmp(token, SYS)){
            Process_buffer_sys(token);
            break;
        }
        else if (!strcmp(token, MAC)){
            Process_buffer_mac(token);
            break;
        }
        else if (!strcmp(token, RADIO)){
            Process_buffer_radio(token);
            break;
        }else{
            UART_write(uart, STATUS_INVALID_PARAM, strlen(STATUS_INVALID_PARAM));
            UART_write(uart, newline, sizeof(newline));
            break;
        }
     }

    // Clean buffer
    buffIndex = 0;
    memset(buffer, '\0', 100);
}


void Process_buffer_help(){
    UART_write(uart, SYS, sizeof(SYS));
    UART_write(uart, newline, sizeof(newline));

    UART_write(uart, MAC, sizeof(MAC));
    UART_write(uart, newline, sizeof(newline));

    UART_write(uart, RADIO, strlen(RADIO));
    UART_write(uart, newline, strlen(newline));
}

void Process_buffer_sys(char* token){

    token = strtok(NULL, " ");

    // display options
    if (token == NULL){
        UART_write(uart, SLEEP, strlen(SLEEP));
        UART_write(uart, newline, sizeof(newline));

        UART_write(uart, RESET, strlen(RESET));
        UART_write(uart, newline, sizeof(newline));

        UART_write(uart, SET, strlen(SET));
        UART_write(uart, newline, sizeof(newline));

        UART_write(uart, GET, strlen(GET));
        UART_write(uart, newline, sizeof(newline));
     }
    // process options
    else{
        if (!strcmp(token, SLEEP)) {
            //TODO call the real function
            UART_write(uart, "Going to Sleep ...", strlen("Going to Sleep ..."));
            UART_write(uart, newline, sizeof(newline));
            DELAY_MS(20000);    // Sleep for 20 seconds
        }
        else if (!strcmp(token, RESET)) {
           char t = '5';
           for(uint8_t i = 5; i > 0; i--, t--){
               UART_write(uart, &t, 1);
               UART_write(uart, newline, sizeof(newline));
               DELAY_MS(1000);    // Sleep for 1 second
           }
            SysCtrlSystemReset();
        }
        else if (!strcmp(token, SET)) {
            //TODO implement
            UART_write(uart, STATUS_NOT_IMPLEMENTED, strlen(STATUS_NOT_IMPLEMENTED));
            UART_write(uart, newline, sizeof(newline));
        }
        else if (!strcmp(token, GET)) {
            //TODO implement
            UART_write(uart, STATUS_NOT_IMPLEMENTED, strlen(STATUS_NOT_IMPLEMENTED));
            UART_write(uart, newline, sizeof(newline));
        }
        else {
            UART_write(uart, STATUS_INVALID_PARAM, strlen(STATUS_INVALID_PARAM));
            UART_write(uart, newline, sizeof(newline));
        }
    }
}

void Process_buffer_mac(char* token){

    token = strtok(NULL, " ");

    // display options
    if (token == NULL){
        UART_write(uart, RESET, strlen(RESET));
        UART_write(uart, newline, sizeof(newline));

        UART_write(uart, TX, strlen(TX));
        UART_write(uart, newline, sizeof(newline));

        UART_write(uart, JOIN, strlen(JOIN));
        UART_write(uart, newline, sizeof(newline));

        UART_write(uart, SAVE, strlen(SAVE));
        UART_write(uart, newline, sizeof(newline));

        UART_write(uart, PAUSE, strlen(PAUSE));
        UART_write(uart, newline, sizeof(newline));

        UART_write(uart, RESUME, strlen(RESUME));
        UART_write(uart, newline, sizeof(newline));

        UART_write(uart, GET, strlen(GET));
        UART_write(uart, newline, sizeof(newline));

        UART_write(uart, SET, strlen(SET));
        UART_write(uart, newline, sizeof(newline));
     }
    // process options
    else{
        if (!strcmp(token, RESET)) {
            //TODO implement
            UART_write(uart, STATUS_NOT_IMPLEMENTED, strlen(STATUS_NOT_IMPLEMENTED));
            UART_write(uart, newline, sizeof(newline));
        }
        else if (!strcmp(token, TX)) {
            //TODO implement
            UART_write(uart, STATUS_NOT_IMPLEMENTED, strlen(STATUS_NOT_IMPLEMENTED));
            UART_write(uart, newline, sizeof(newline));
        }
        else if (!strcmp(token, JOIN)) {
            //TODO implement
            UART_write(uart, STATUS_NOT_IMPLEMENTED, strlen(STATUS_NOT_IMPLEMENTED));
            UART_write(uart, newline, sizeof(newline));
        }
        else if (!strcmp(token, SAVE)) {
            //TODO implement
            UART_write(uart, STATUS_NOT_IMPLEMENTED, strlen(STATUS_NOT_IMPLEMENTED));
            UART_write(uart, newline, sizeof(newline));
        }
        else if (!strcmp(token, PAUSE)) {
            //TODO implement
            UART_write(uart, STATUS_NOT_IMPLEMENTED, strlen(STATUS_NOT_IMPLEMENTED));
            UART_write(uart, newline, sizeof(newline));
        }
        else if (!strcmp(token, RESUME)) {
            //TODO implement
            UART_write(uart, STATUS_NOT_IMPLEMENTED, strlen(STATUS_NOT_IMPLEMENTED));
            UART_write(uart, newline, sizeof(newline));
        }
        else if (!strcmp(token, GET)) {
            //TODO implement
            UART_write(uart, STATUS_NOT_IMPLEMENTED, strlen(STATUS_NOT_IMPLEMENTED));
            UART_write(uart, newline, sizeof(newline));
        }
        else if (!strcmp(token, SET)) {
            //TODO implement
            UART_write(uart, STATUS_NOT_IMPLEMENTED, strlen(STATUS_NOT_IMPLEMENTED));
            UART_write(uart, newline, sizeof(newline));
        }
        else {
            UART_write(uart, STATUS_INVALID_PARAM, strlen(STATUS_INVALID_PARAM));
            UART_write(uart, newline, sizeof(newline));
        }
    }
}

void Process_buffer_radio(char* token){

}


void Aux_delete_terminal_char(){
    UART_write(uart, backspace, sizeof(backspace));     // backspace
    UART_write(uart, delim, sizeof(delim));             // space
    UART_write(uart, backspace, sizeof(backspace));     // backspace
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
    uartParams.baudRate = 115200;
    uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.writeMode = UART_MODE_BLOCKING;
//    UARTCC26xx does not implement any of these parameters
//    uartParams.readDataMode = UART_DATA_TEXT;
//    uartParams.writeDataMode = UART_DATA_TEXT;
//    uartParams.readReturnMode = UART_RETURN_NEWLINE;
//    uartParams.readEcho = UART_ECHO_ON;


    uart = UART_open(Board_UART, &uartParams);

    if (uart == NULL) {
        System_abort("Error opening the UART");
        return;
    }

    UART_write(uart, clearScreen, sizeof(clearScreen));
    UART_write(uart, echoPrompt, sizeof(echoPrompt));

    memset(buffer, '\0', sizeof(buffer));
    memset(prevBuffer, '\0', sizeof(prevBuffer));

    while(1) {
        UART_read(uart, &tempBuf, sizeof(tempBuf));

        switch (tempBuf) {

            case 8:  // backspace key pressed, delete last char
                if (buffIndex > 1){
                    buffer[--buffIndex] = '\0'; //clean last char
                    Aux_delete_terminal_char();
                }
                break;

            case 13:  // enter key pressed
                if(buffIndex > 0)
                    memcpy(prevBuffer, buffer, sizeof(prevBuffer)); // save the command
                UART_write(uart, newline, sizeof(newline));
                Process_buffer();
                UART_write(uart, echoPrompt, sizeof(echoPrompt));
                break;

            case 27:  // keypad pressed, show last command
                UART_read(uart, &tempBuf, sizeof(tempBuf)); // ignore special composed character
                UART_read(uart, &tempBuf, sizeof(tempBuf)); // tempBuf contains the direction

                if (tempBuf == 'A' && strcmp(buffer, prevBuffer)){    // up key pressed
                    memcpy(buffer, prevBuffer, sizeof(buffer));
                    buffIndex = strlen(prevBuffer) + 1;
                    UART_write(uart, buffer, strlen(buffer));
                }
                else if (tempBuf == 'B' && buffIndex > 0){ // down key pressed
                    memset(buffer, '\0', sizeof(buffer));
                    for(uint8_t i = buffIndex - 1; i > 0; i--){
                        Aux_delete_terminal_char();
                    }
                    buffIndex = 0;
                }
                break;

            default :  //add to buffer
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
