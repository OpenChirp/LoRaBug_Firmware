
// Uncomment this in case you are using the
// CC2650 Launchpad + mbed SX1276 boards

//#define _DEV_BOARD_


#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/drivers/Power.h>


/* These #defines allow us to reuse TI-RTOS across other device families */
#define     Board_LED0              Board_RLED
#define     Board_LED1              Board_GLED
#define     Board_LED2              Board_LED0

#define     Board_BUTTON0           Board_BTN1
#define     Board_BUTTON1           Board_BTN2

#define     Board_UART0             Board_UART
#define     Board_AES0              Board_AES
#define     Board_WATCHDOG0         CC2650_LAUNCHXL_WATCHDOG0

#define     Board_ADC0              CC2650_LAUNCHXL_ADCVSS
#define     Board_ADC1              CC2650_LAUNCHXL_ADCVDDS

#define     Board_ADCBuf0           CC2650_LAUNCHXL_ADCBuf0
#define     Board_ADCBufChannel0    (0)
#define     Board_ADCBufChannel1    (1)

#define     Board_initGeneral() { \
    Power_init(); \
    if (PIN_init(BoardGpioInitTable) != PIN_SUCCESS) \
        {System_abort("Error with PIN_init\n"); \
    } \
}

#define     Board_initGPIO()
#define     Board_initPWM()        PWM_init()
#define     Board_initSPI()         SPI_init()
#define     Board_initUART()        UART_init()
#define     Board_initWatchdog()    Watchdog_init()
#define     Board_initADCBuf()      ADCBuf_init()
#define     Board_initADC()         ADC_init()
#define     GPIO_toggle(n)
#define     GPIO_write(n,m)


#ifdef _DEV_BOARD_
#include "Pins/CC2650_LAUNCHXL.h"
#else
#include "Pins/CC2650_LORABUG_HW_V3.1.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
