/*
 * rgbw.c
 *
 *  Created on: Jun 14, 2017
 *
 * 	@author Artur Balanuta <Artur [dot] Balanuta [at] Gmail [dot] com>
 */

#include "rgbw.h"


#define PWMTASKSTACKSIZE   	1024
#define UPDATE_INTERVAL		75	//ms
#define MAILBOX_SIZE 		3


Task_Struct PWMTaskStruct;
Char TaskPWMStack[PWMTASKSTACKSIZE];


struct ColorHandlers_s ColorHandlers;
Mailbox_Handle mbox;

void PWM_Task_Fxn_Loop();



void PWM_CreateTask(){

	/* Create pwm Task */
	Task_Params TaskParams;
	Task_Params_init(&TaskParams);

	TaskParams.stackSize = PWMTASKSTACKSIZE;
	TaskParams.stack = &TaskPWMStack;
	TaskParams.priority = 1;
	TaskParams.instance->name = "pwm_Task";
	Task_construct(&PWMTaskStruct, (Task_FuncPtr) PWM_Task_Fxn_Loop, &TaskParams,NULL);

	// Create Mailbox before BIOS_Start
	mbox = Mailbox_create(sizeof(Command_t), MAILBOX_SIZE, NULL, NULL);
	if (mbox == NULL)
	{
		System_abort("Failed to create mailbox");
	}

}

void PWM_fadeRainbow(uint32_t delay){

	PWM_fadeTo(&ColorHandlers.LedGreen, 255, delay);
	PWM_fadeTo(&ColorHandlers.LedRed, 0, delay);
	PWM_fadeTo(&ColorHandlers.LedBlue, 255, delay);
	PWM_fadeTo(&ColorHandlers.LedGreen, 0, delay);
	PWM_fadeTo(&ColorHandlers.LedRed, 255, delay);
	PWM_fadeTo(&ColorHandlers.LedBlue, 0, delay);
}


void PWM_Pulse(Color_t color, uint16_t delay){

	switch(color){

	case COLOR_RED:
		PWM_fadeTo(&ColorHandlers.LedRed, 255, delay);
		PWM_fadeTo(&ColorHandlers.LedRed, 0, delay);
		break;

	case COLOR_GREEN:
		PWM_fadeTo(&ColorHandlers.LedGreen, 255, delay);
		PWM_fadeTo(&ColorHandlers.LedGreen, 0, delay);
		break;

	case COLOR_BLUE:
		PWM_fadeTo(&ColorHandlers.LedBlue, 255, delay);
		PWM_fadeTo(&ColorHandlers.LedBlue, 0, delay);
		break;

	case COLOR_YELLOW:
		PWM_fadeTo(&ColorHandlers.LedRed, 255, delay/2);
		PWM_fadeTo(&ColorHandlers.LedGreen, 255, delay/2);
		PWM_fadeTo(&ColorHandlers.LedGreen, 0, delay/2);
		PWM_fadeTo(&ColorHandlers.LedRed, 0, delay/2);
		break;

	case COLOR_CYAN:
		PWM_fadeTo(&ColorHandlers.LedGreen, 255, delay/2);
		PWM_fadeTo(&ColorHandlers.LedBlue, 255, delay/2);
		PWM_fadeTo(&ColorHandlers.LedBlue, 0, delay/2);
		PWM_fadeTo(&ColorHandlers.LedGreen, 0, delay/2);
		break;

	case COLOR_PURPLE:
		PWM_fadeTo(&ColorHandlers.LedRed, 255, delay/2);
		PWM_fadeTo(&ColorHandlers.LedBlue, 255, delay/2);
		PWM_fadeTo(&ColorHandlers.LedBlue, 0, delay/2);
		PWM_fadeTo(&ColorHandlers.LedRed, 0, delay/2);
		break;

	case COLOR_WHITE:
		PWM_fadeTo(&ColorHandlers.LedRed, 255, delay/3);
		PWM_fadeTo(&ColorHandlers.LedGreen, 255, delay/3);
		PWM_fadeTo(&ColorHandlers.LedBlue, 255, delay/3);
		PWM_fadeTo(&ColorHandlers.LedBlue, 0, delay/3);
		PWM_fadeTo(&ColorHandlers.LedGreen, 0, delay/3);
		PWM_fadeTo(&ColorHandlers.LedRed, 0, delay/3);
		break;

	default:
		delay > UPDATE_INTERVAL ? DELAY_MS(delay) : DELAY_MS(UPDATE_INTERVAL);
	}
}

void PWM_Task_Fxn_Loop(){

    struct Command_s command;
    uint16_t delay;
    uint8_t color;

	// Initializes PWM devices
	PWM_Init_Devices();

    /* Demo PWM */
	PWM_fadeRainbow(500);
	PWM_fadeTo(&ColorHandlers.LedRed, 0, 500);


	while(1){

	    if(!Mailbox_pend(mbox, &command, BIOS_WAIT_FOREVER)) {
	    	System_abort("Failed to pend mailbox\n");
	    }

	    switch (command.buff[0])	// Select the operation mode
	    {
	    case 0:	// set Fixed Color (byte 1 to 4 contain rgbw intensity)
	    	// Colors Off
	    	PWM_fadeTo(&ColorHandlers.LedRed, 0, 200);
	    	PWM_fadeTo(&ColorHandlers.LedGreen, 0, 200);
	    	PWM_fadeTo(&ColorHandlers.LedBlue, 0, 200);

	    	PWM_fadeTo(&ColorHandlers.LedRed, command.buff[1], 200);
	    	PWM_fadeTo(&ColorHandlers.LedGreen, command.buff[2], 200);
	    	PWM_fadeTo(&ColorHandlers.LedBlue, command.buff[3], 200);

	    	break;

	    case 1:	// Random Color Mode

	    	// Colors Off
	    	PWM_fadeTo(&ColorHandlers.LedRed, 0, 200);
	    	PWM_fadeTo(&ColorHandlers.LedGreen, 0, 200);
	    	PWM_fadeTo(&ColorHandlers.LedBlue, 0, 200);

	    	PWM_fadeTo(&ColorHandlers.LedRed, (uint8_t)randr(0, 255), 200);
	    	PWM_fadeTo(&ColorHandlers.LedGreen, (uint8_t)randr(0, 255), 200);
	    	PWM_fadeTo(&ColorHandlers.LedBlue, (uint8_t)randr(0, 255), 200);

	    	break;

	    case 2: // Rainbow Transition Mode (byte 1 contains delay/50)

	    	delay = command.buff[1] * 50;

	    	while(Mailbox_getNumPendingMsgs(mbox) == 0){
	    		PWM_fadeRainbow(delay);
	    	}
	    	break;

	    case 3:	//Pulsating Color (byte 1 contains the color, byte 2 contains delay/50)

	    	color = command.buff[1];
	    	delay = command.buff[2] * 50;

	    	while(Mailbox_getNumPendingMsgs(mbox) == 0){
	    		PWM_Pulse((Color_t)color, delay);
	    	}
	    	break;

	    default:
	    	break;
	    }
	}

}

void PWM_fadeTo(PWMDevice *pwm, uint8_t duty, uint16_t time){

	int32_t range;
	int32_t increment;
	uint16_t divisions = (time < UPDATE_INTERVAL) ? 1 : time / UPDATE_INTERVAL;	// mimimum delay is UPDATE_INTERVAL milliseconds
	int32_t duty_now = pwm->duty;

	if(pwm->inverted){
		range = duty - (255 - pwm->duty);
		increment = -(range/divisions);
		duty = 255 - duty;	// inverts the value (duty, is the final value)
	}else{
		range = duty - pwm->duty;
		increment = (range/divisions);
	}

	if (range == 0)
		return;

	for (uint16_t i = 0; i < time; i += UPDATE_INTERVAL){

		duty_now += increment;

		if( duty_now >= 0 && duty_now <= 255){
			PWM_setDuty(pwm->handler, duty_now/(float)255 * PWM_DUTY_FRACTION_MAX);
			pwm->duty = duty_now;
		}else{
			// adjust for any fraction errors
			PWM_setDuty(pwm->handler, duty/(float)255 * PWM_DUTY_FRACTION_MAX);
			pwm->duty = duty;
		}
		DELAY_MS(UPDATE_INTERVAL);
	}

}


/*
 * Function that initiates the PWM devices, the LEDs are powered at LOW
 * this means their PWM value should be the inverted of the active.
 * i.e. 255 duty cycle means 0, the Mosfets are not inverted.
 */
void PWM_Init_Devices(){

	PWM_Params params;
	PWM_Params_init(&params);

	// Same as default
	params.idleLevel = PWM_IDLE_LOW;              		// Output low when PWM is not running
	params.periodUnits = PWM_PERIOD_HZ;					// Period is in Hz
	params.periodValue = 1e6;                      		// 1MHz
	params.dutyUnits = PWM_DUTY_FRACTION;   			// Duty is fraction of period
	params.dutyValue = 0 * PWM_DUTY_FRACTION_MAX; 		// % duty cycle

	ColorHandlers.LedRed.handler = 		PWM_open(Board_PWM1, &params);
    ColorHandlers.LedGreen.handler = 	PWM_open(Board_PWM2, &params);
    ColorHandlers.LedBlue.handler =  	PWM_open(Board_PWM3, &params);

    ColorHandlers.MosfetRed.handler = 	PWM_open(Board_PWM4, &params);
    ColorHandlers.MosfetGreen.handler = PWM_open(Board_PWM5, &params);
    ColorHandlers.MosfetBlue.handler = 	PWM_open(Board_PWM6, &params);
    ColorHandlers.MosfetWhite.handler = PWM_open(Board_PWM7, &params);

    if (ColorHandlers.LedRed.handler == NULL || ColorHandlers.LedGreen.handler == NULL ||
    		ColorHandlers.LedBlue.handler == NULL || ColorHandlers.MosfetRed.handler == NULL ||
			ColorHandlers.MosfetGreen.handler == NULL || ColorHandlers.MosfetBlue.handler == NULL ||
			ColorHandlers.MosfetWhite.handler == NULL){
    	System_abort("Failed to Init PWMs");
    }

    PWM_start(ColorHandlers.LedRed.handler);
    PWM_start(ColorHandlers.LedGreen.handler);
    PWM_start(ColorHandlers.LedBlue.handler);
    PWM_start(ColorHandlers.MosfetRed.handler);
    PWM_start(ColorHandlers.MosfetGreen.handler);
    PWM_start(ColorHandlers.MosfetBlue.handler);
    PWM_start(ColorHandlers.MosfetWhite.handler);


    // RGB LEDs LOW is at duty cycle 100 % (255)
    ColorHandlers.LedRed.inverted = true;
    ColorHandlers.LedRed.duty = 255;
	ColorHandlers.LedGreen.inverted = true;
	ColorHandlers.LedGreen.duty = 255;
	ColorHandlers.LedBlue.inverted = true;
	ColorHandlers.LedBlue.duty = 255;

    // Turn RGB LEDs off buy increasing duty cycle to 100%
    PWM_setDuty(ColorHandlers.LedRed.handler, 1 * PWM_DUTY_FRACTION_MAX);
    PWM_setDuty(ColorHandlers.LedGreen.handler, 1 * PWM_DUTY_FRACTION_MAX);
    PWM_setDuty(ColorHandlers.LedBlue.handler, 1 * PWM_DUTY_FRACTION_MAX);

}
