/******************************************************************************

 @file  blink.c

 @brief This file contains the Simple blink sample application
 Created on: Sep 8, 2016

 @author: Artur Balanuta

 ******************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>

//#include <ti/drivers/Watchdog.h>
//#include <WatchdogCC26XX.h>

#include "Config/Board.h"

/*******************************************************************************
 * MACROS
 */

#define DELAY_MS(i)    Task_sleep(((i) * 1000) / Clock_tickPeriod)

/*******************************************************************************
 * CONSTANTS
 */
#define BLINK_TASK_PRIORITY                     1

#ifndef BLINK_TASK_STACK_SIZE
#define BLINK_TASK_STACK_SIZE                   256
#endif

//#define WATCHDOG_EN
#define WATCHDOG_TIMEOUT_MS						30000

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
Task_Struct blinkTask;
Char blinkTaskStack[BLINK_TASK_STACK_SIZE];

#ifdef WATCHDOG_EN
Watchdog_Params params;
Watchdog_Handle watchdog;
#endif

static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

static PIN_Config ledPinTable[] = {
	Board_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */


/*********************************************************************
 * @fn      Blink_taskFxn
 *
 * @brief   Application task entry point for the Simple Blink function.
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void Blink_taskFxn(UArg a0, UArg a1)
{
	/* Open LED pins */
	//ledPinHandle = PIN_open(&ledPinState, ledPinTable);
	//if(!ledPinHandle) {
	//	System_abort("Error initializing board LED pins\n");
	//}

	//PIN_setOutputValue(ledPinHandle, Board_RLED, true);

	PWM_Handle pwm1;
	PWM_Params params;
	float dutyNow = 0.90;
	bool increasing = true;


	PWM_Params_init(&params);
	params.idleLevel = PWM_IDLE_LOW;                	// Output low when PWM is not running
	params.periodUnits = PWM_PERIOD_HZ;					// Period is in Hz
	params.periodValue = 1e6;                      		// 1MHz
	params.dutyUnits = PWM_DUTY_FRACTION;   			// Duty is fraction of period
	params.dutyValue = dutyNow * PWM_DUTY_FRACTION_MAX;   	// % duty cycle

	pwm1 = PWM_open(Board_PWM0, &params);

	if (pwm1 == NULL) {
		System_printf("Board_PWM0 did not open");
		System_flush();
	}

	PWM_start(pwm1);

	/* Loop forever incrementing the PWM duty */
	while (1) {

		if (increasing){
			dutyNow += 0.1;
			if (dutyNow >= 0.8)
				increasing = false;
		}

		else{
			dutyNow -= 0.05;
			if (dutyNow <= 0){
				increasing = true;
				dutyNow = 0;
			}
		}

		PWM_setDuty(pwm1, dutyNow * PWM_DUTY_FRACTION_MAX);
		DELAY_MS(75);
	}
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleBlink_createTask
 *
 * @brief   Task creation function for the Simple Blink Application.
 *
 * @param   None.
 *
 * @return  None.
 */
void SimpleBlink_createTask(void) {
	Task_Params taskParams;

	// Configure task
	Task_Params_init(&taskParams);
	taskParams.stack = blinkTaskStack;
	taskParams.stackSize = BLINK_TASK_STACK_SIZE;
	taskParams.priority = BLINK_TASK_PRIORITY;
	taskParams.instance->name = "heartBeat";

	Task_construct(&blinkTask, Blink_taskFxn, &taskParams, NULL);

#ifdef WATCHDOG_EN
	/* Create and enable a Watchdog with resets enabled */
	Watchdog_Params_init(&params);
	params.resetMode = Watchdog_RESET_ON;
	//params.callbackFxn = watchdog_callback;
	watchdog = Watchdog_open(Board_WATCHDOG0, &params);

	if (watchdog == NULL) {
		/* Error opening watchdog */
		System_printf("Error opening watchdog!\n");
		System_flush();
	}

	// set timeout period to 30 s
	Watchdog_setReload(watchdog, WatchdogCC26XX_convertMsToTicks(WATCHDOG_TIMEOUT_MS));
#endif

}
