/*
 * rgbw.h
 *
 *  Created on: Jun 14, 2017
 *
 * 	@author Artur Balanuta <Artur [dot] Balanuta [at] Gmail [dot] com>
 */

#ifndef APPS_RGBWSTRIPCOTROLLER_RGBW_H_
#define APPS_RGBWSTRIPCOTROLLER_RGBW_H_

#include <xdc/std.h>


#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Mailbox.h>

#include <xdc/runtime/System.h>

#include <ti/drivers/PWM.h>

#include <Board_LoRaBUG.h>

#include "utilities.h"


#define TIME_MS (1000/Clock_tickPeriod)
#define DELAY_MS(i)    Task_sleep(((i) * 1000) / Clock_tickPeriod)
#define DELAY_US(i)    Task_sleep(((i) * 1) / Clock_tickPeriod)

#define COMMAND_SIZE 5





typedef enum eColors
{
	COLOR_BLACK 	= 0b000,
	COLOR_RED 		= 0b100,
	COLOR_GREEN 	= 0b010,
	COLOR_BLUE 		= 0b001,
	COLOR_YELLOW	= 0b110,
	COLOR_CYAN		= 0b011,
	COLOR_PURPLE	= 0b101,
	COLOR_WHITE		= 0b111

}Color_t;

typedef struct PWMDevice_s {
	PWM_Handle handler;
	volatile uint8_t duty;		// duty cycle of the pwm device (0-255)
	bool inverted;				// used to negate the duty cycle
} PWMDevice;

struct ColorHandlers_s
{
	PWMDevice LedRed;
	PWMDevice LedGreen;
	PWMDevice LedBlue;
	PWMDevice MosfetRed;
	PWMDevice MosfetGreen;
	PWMDevice MosfetBlue;
	PWMDevice MosfetWhite;
};

struct Command_s
{
	uint8_t buff[COMMAND_SIZE];
} Command_t;

// Variables Accessible to the main thread
extern Mailbox_Handle mbox;

void PWM_CreateTask();
void PWM_Init_Devices();
void PWM_fadeTo(PWMDevice *pwm, uint8_t duty, uint16_t time);
void PWM_test();


#endif /* APPS_RGBWSTRIPCOTROLLER_RGBW_H_ */
