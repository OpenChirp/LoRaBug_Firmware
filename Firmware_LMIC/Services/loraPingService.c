/******************************************************************************

  @file  loraPingService.c

 @brief This file contains the Simple Ping sample application
         Created on: Jan 26, 2017

 @author: Artur Balanuta

 ******************************************************************************/

#ifndef SERVICES_LORAPINGSERVICE_C_
#define SERVICES_LORAPINGSERVICE_C_

/*********************************************************************
 * INCLUDES
 */
#include <stdio.h>
#include <string.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

/* lmic LoRa Lib */
#include <lmic.h>
#include <oslmic.h>
#include <hal.h>

/* Board Header files */
#include "Config/Board.h"

/* UART logs */
//#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>   // For Log_warning1("Warning number #%d", 4); things
#include <xdc/runtime/Diags.h> // For Log_print0(Diags_USER1, "hello"); things.

#include "loraCommon.h"

/*******************************************************************************
 * MACROS
 */

#define DELAY_MS(i)    Task_sleep(((i) * 1000) / Clock_tickPeriod)

/*******************************************************************************
 * CONSTANTS
 */
#define LORA_TASK_PRIORITY                     5

#ifndef BLINK_TASK_STACK_SIZE
#define LORA_TASK_STACK_SIZE                   1200
#endif

#define LED_PIN_RX      Board_GLED
#define LED_PIN_TX      Board_RLED
#define TX_DELAY        2000        //  Send ping every X ms

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
Task_Struct loraPingTask;
Char loraRawTaskStack[LORA_TASK_STACK_SIZE];

static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

static PIN_Config ledPinTable[] = {
    Board_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    Board_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

static uint8_t mydata[] = "C0FFEx2";

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void onEvent (ev_t ev) {} // Defined by LMIC



/*********************************************************************
 * @fn      LoraRaw_taskFxn
 * @return  None.
 */
static void LoraPing_taskFxn (UArg a0, UArg a1)
{
    Log_info0("Starting Ping Application...");

    /* Open LED pins */
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
    if(!ledPinHandle) {
        Log_error0("Error initializing board LED pins");
        System_abort("Error initializing board LED pins\n");
    }

    // Initialize LMIC
    setup();

    Log_info0("Started");

    // use inverted I/Q signal (Enable mote-to-mote communication)
    LMIC.noRXIQinversion = 1;

    DELAY_MS(10); // Used to push the logs to the UART

    while(true){

        // Send Beacon
        PIN_setOutputValue(ledPinHandle, LED_PIN_TX, true);
        tx(mydata, sizeof(mydata));
        PIN_setOutputValue(ledPinHandle, LED_PIN_TX, false);

        DELAY_MS(TX_DELAY); // Also used to push the logs to the UART
    }

}


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      LoRaLmicRawService_createTask
 *
 * @brief   Task creation function for the LoRa Application.
 *
 * @param   None.
 *
 * @return  None.
 */
void LoRaPingService_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = loraRawTaskStack;
  taskParams.stackSize = LORA_TASK_STACK_SIZE;
  taskParams.priority = LORA_TASK_PRIORITY;

  Task_construct(&loraPingTask, LoraPing_taskFxn, &taskParams, NULL);
}



#endif /* SERVICES_LORAPINGSERVICE_C_ */
