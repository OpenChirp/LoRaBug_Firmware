/* BIOS Header files */
#include <ti/sysbios/knl/Task.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* UART logs */
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>   // For Log_warning1("Warning number #%d", 4);
#include <xdc/runtime/Diags.h> // For Log_print0(Diags_USER1, "hello");

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

//#include <ti/drivers/GPIO.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h> // speed-optimized for the <ti/drivers/PIN.h> drivers

/* SPI Header Files */
// All of them should be Included
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>
#include <ti/drivers/dma/UDMACC26XX.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "hal.h"
#include "../lmic/config.h"
#include <Config/Board.h>

static uint8_t txBuf;            // Transmit buffers
static uint8_t rxBuf;            // Receive buffer

static UInt32 last32BitClock = 0;
static ostime_t last64BitClock = 0;
static uint_t intPinMask = 0;
static uint_t prevIntPinMask = 0;
static uint_t Intkey;

static bool dioStates[3] = { 0 };

static SPI_Handle spiHandle;
static SPI_Params spiParams;
static SPI_Transaction spiTransaction;

/* Pin driver handle */
static PIN_Handle pinHandle;
static PIN_Handle interuptionHandle;
static PIN_State pinState;
static PIN_State interuptionState;

// PIN radio config
static const PIN_Config radioPinTable[] = {
Board_SPI0_CSN_SX | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL,
Board_SX_RF_CTRL1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL,
Board_SX_RF_CTRL2 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL,
Board_SX_RESET | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL,
PIN_TERMINATE };

// PIN radio interupts config
static const PIN_Config intruptPinTable[] = {
Board_SX_DIO0 | PIN_INPUT_EN | PIN_PULLDOWN,
Board_SX_DIO1 | PIN_INPUT_EN | PIN_PULLDOWN,
Board_SX_DIO2 | PIN_INPUT_EN | PIN_PULLDOWN,
PIN_TERMINATE };

// -----------------------------------------------------------------------------
// SPI

void hal_spi_init() {

	// SPI Init
	SPI_Params_init(&spiParams);
	spiParams.bitRate = 4000000;
	spiParams.frameFormat = SPI_POL0_PHA0;
	spiParams.mode = SPI_MASTER;

	// Configure the transaction
	spiTransaction.count = sizeof(txBuf);
	spiTransaction.txBuf = &txBuf;
	spiTransaction.rxBuf = &rxBuf;

	// Open the SPI and perform the transfer
	spiHandle = SPI_open(Board_SPI0, &spiParams);
	if (!spiHandle) {
		Log_warning0("SPI did not open");
	}
}

/*
 * drive radio NSS pin (0=low, 1=high).
 */
void hal_pin_nss(u1_t val) {
	PIN_setOutputValue(pinHandle, Board_SPI0_CSN_SX, val);
}

/*
 * drive radio RX/TX pins (0=rx, 1=tx).
 */
void hal_pin_rxtx(u1_t val) {
	if (val == 0 || val == 1) {
		PIN_setOutputValue(pinHandle, Board_SX_RF_CTRL1, val);
		PIN_setOutputValue(pinHandle, Board_SX_RF_CTRL2, !val);
	} else if (val == 2) {
		PIN_setOutputValue(pinHandle, Board_SX_RF_CTRL1, 0);
		PIN_setOutputValue(pinHandle, Board_SX_RF_CTRL2, 0);
	} else {
		//printu( "hal_pin_rxtx val UNKNOWN: \r\n");
		hal_failed();
	}
}

/*
 * control radio RST pin (0=low, 1=high, 2=floating)
 */
void hal_pin_rst(u1_t val) {
	if (val == 0 || val == 1) { // drive pin
		PIN_setOutputEnable(pinHandle, Board_SX_RESET, true); // Enable Output
		PIN_setOutputValue(pinHandle, Board_SX_RESET, val);
	} else { // keep pin floating
		PIN_setOutputEnable(pinHandle, Board_SX_RESET, false);
	}
}

/*
 * perform 8-bit SPI transaction with radio.
 *   - write given byte 'outval'
 *   - read byte and return value
 */
u1_t hal_spi(u1_t outval) {

	// Configure the transaction
	txBuf = outval;
	SPI_transfer(spiHandle, &spiTransaction);
	return rxBuf;
}

///**
// * Do a system hard reset when triggered
// */
//static void dioCallback(PIN_Handle handle, PIN_Id pinId) {
//	switch (pinId) {
//	case Board_SX_DIO0:
//		radio_irq_handler(0);
//		break;
//	case Board_SX_DIO1:
//		radio_irq_handler(1);
//		break;
//	case Board_SX_DIO2:
//		radio_irq_handler(2);
//		break;
//	default:
//		break;
//	}
//	Log_error0("dioCallback");
//	Task_sleep(1);
//}

static void hal_io_check() {

	//Speed up the verification process
	intPinMask = PIN_getPortInputValue(interuptionHandle);
	if ( prevIntPinMask == intPinMask )
		return;

	prevIntPinMask = intPinMask;

	if (dioStates[0] != PIN_getInputValue(Board_SX_DIO0)) {
		dioStates[0] = !dioStates[0];
		if (dioStates[0])
			radio_irq_handler(0);
	}

	if (dioStates[1] != PIN_getInputValue(Board_SX_DIO1)) {
		dioStates[1] = !dioStates[1];
		if (dioStates[1])
			radio_irq_handler(1);
	}

	if (dioStates[2] != PIN_getInputValue(Board_SX_DIO2)) {
		dioStates[2] = !dioStates[2];
		if (dioStates[2])
			radio_irq_handler(2);
	}
}

bool hal_io_get_dio0() {
	return PIN_getInputValue(Board_SX_DIO0);
}

bool hal_io_get_dio1() {
	return PIN_getInputValue(Board_SX_DIO1);
}

// -----------------------------------------------------------------------------
// TIME

void hal_time_init() {
	last32BitClock = Clock_getTicks();
	last64BitClock = (ostime_t) last32BitClock;
}

/*
 * return 64-bit system time in ticks.
 */
ostime_t hal_ticks() {

	UInt32 now = Clock_getTicks();

	// Check for Overflow and correct
	if (now < last32BitClock) {
		last64BitClock += (ostime_t) ((ostime_t) (UINT32_MAX - last32BitClock) + (ostime_t) now);
	} else { //add the diference and return
		last64BitClock += (ostime_t) (now - last32BitClock);
	}

	last32BitClock = now;
	return last64BitClock;
}

/*
 * busy-wait until specified timestamp (in ticks) is reached.
 */
void hal_waitUntil(ostime_t time) {

	// busy-wait is used for the last ticks
	while (hal_ticks() <= time);

}

ostime_t local_targettime = 0;

/*
 * check and rewind timer for target time.
 *   - return 1 if target time is close
 *   - otherwise rewind timer for target time or full period and return 0
 */
u1_t hal_checkTimer(ostime_t targettime) {

	//UInt64 now = hal_ticks();
	local_targettime = targettime;
	//ostime_t now =

	if (targettime > (hal_ticks() - US_PER_OSTICK)) {
		return 0;
	} else {
		return 1;
	}
}

/*
 * put system and CPU in low-power mode, sleep until interrupt.
 */
void hal_sleep() {

	hal_io_check();

//	if((local_targettime-hal_ticks()) < US_PER_OSTICK*600 ) // Increse the *value in case of many Join_Fails
//		return;
//	else
//	    Task_sleep(1);
//		Task_sleep( (local_targettime-hal_ticks()) - US_PER_OSTICK*350); // *value-50
}

// -----------------------------------------------------------------------------
// IRQ

/*
 * disable all CPU interrupts.
 *   - might be invoked nested
 *   - will be followed by matching call to hal_enableIRQs()
 */
void hal_disableIRQs() {

	// Instead of using proper interrupts (which are a bit tricky
	// and/or not available on all pins on AVR), just poll the pin
	// values. Since os_runloop disables and re-enables interrupts,
	// putting this here makes sure we check at least once every
	// loop.

	hal_io_check();

	Intkey = Hwi_disable();
}

/*
 * enable CPU interrupts.
 */
void hal_enableIRQs() {

	// Instead of using proper interrupts (which are a bit tricky
	// and/or not available on all pins on AVR), just poll the pin
	// values. Since os_runloop disables and re-enables interrupts,
	// putting this here makes sure we check at least once every
	// loop.

	Hwi_restore(Intkey);

	hal_io_check();
}

/*
 * initialize hardware (IO, SPI, TIMER, IRQ).
 */
void hal_init() {

	// configure radio I/O and interrupt handler
	hal_io_init();

	// configure radio SPI
	hal_spi_init();

	// configure timer
	hal_time_init();

}

/*
 * perform fatal failure action.
 *   - called by assertions
 *   - action could be HALT or reboot
 */
void hal_failed() {
	Log_error0("| Fatal HAL Error !!! Call an Engineer |");
	Task_sleep(ms2osticks(500));
	Hwi_disable();
	while (1);
}

// -----------------------------------------------------------------------------
// IO

void hal_io_init() {

	// Open PIN Handler
	pinHandle = PIN_open(&pinState, radioPinTable);
	if (!pinHandle) {
		hal_failed();
	}

	// Set Chip Select of SX1276 to High (Disable)
	if (PIN_setOutputValue(pinHandle, Board_SPI0_CSN_SX, true) != PIN_SUCCESS) {
		hal_failed();
	}

	// RF switch open
	if (PIN_setOutputValue(pinHandle, Board_SX_RF_CTRL1, 0) != PIN_SUCCESS) {
		hal_failed();
	}

	// RF switch open
	if (PIN_setOutputValue(pinHandle, Board_SX_RF_CTRL2, 0) != PIN_SUCCESS) {
		hal_failed();
	}

	// Open Interrupt Handler
	interuptionHandle = PIN_open(&interuptionState, intruptPinTable);
	if (!interuptionHandle) {
		hal_failed();
	}

//	// Setup interupt
//	if (PIN_registerIntCb(interuptionHandle, dioCallback) != PIN_SUCCESS) {
//		System_abort("Failed to register pin0 int callback\n");
//	}
}

