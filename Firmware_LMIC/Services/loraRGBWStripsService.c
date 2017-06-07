/******************************************************************************

 @file  loraRGBWStripsService.c

 @brief This file contains the Simple blink sample application
         Created on: Sep 14, 2016

 @author: Artur Balanuta

 ******************************************************************************/

#ifndef SERVICES_LORARGB_C_
#define SERVICES_LORARGB_C_

/*********************************************************************
 * INCLUDES
 */
#include <stdio.h>
#include <string.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/hal/Seconds.h>

#include <driverlib/sys_ctrl.h> // SysCtrlSystemReset()


/* lmic LoRa Lib */
#include <lmic/src/lmic.h>

/* Board Header files */
#include "Config/Board.h"

/* UART logs */
#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>   // For Log_warning1("Warning number #%d", 4); things
#include <xdc/runtime/Diags.h> // For Log_print0(Diags_USER1, "hello"); things.


/*******************************************************************************
 * MACROS
 */

#define DELAY_MS(i)    Task_sleep(((i) * 1000) / Clock_tickPeriod)
#define msbf4_read(p)   (u4_t)((u4_t)(p)[0]<<24 | (u4_t)(p)[1]<<16 | (p)[2]<<8 | (p)[3])

/*******************************************************************************
 * CONSTANTS
 */
#define LORA_TASK_PRIORITY                     5

#ifndef BLINK_TASK_STACK_SIZE
#define LORA_TASK_STACK_SIZE                   1500
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
// LoRaWAN Application identifier (AppEUI)
static const u1_t APPEUI[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
//static const u1_t APPEUI[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10 };

// LoRaWAN DevEUI, unique device ID (LSBF)
//static const u1_t DEVEUI[8]  = { 0xAB, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA }; 	//rgb1
static const u1_t DEVEUI[8]  = { 0xAC, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };	//rgb2

// LoRaWAN NwkSKey, network session key
// Use this key for The Things Network
static const u1_t DEVKEY[16] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };

// LoRaWAN AppSKey, application session key
// Use this key to get your data decrypted by The Things Network
//static const u1_t ARTKEY[16] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };

// LoRaWAN end-device address (DevAddr)
// See http://thethingsnetwork.org/wiki/AddressSpace
//const u4_t DEVADDR = 0x08EA5799 ; // <-- Change this address for every node!
//static unsigned char DevAddr[4] = { 0x08, 0x08, 0x08, 0x08 };

// Task configuration
Task_Struct loraTask;
Char loraTaskStack[LORA_TASK_STACK_SIZE];

static osjob_t bogusjob; //this is a job in order to keep the lmic_os sleep once in a while
static osjob_t sendjob;

static char send_buff[MAX_LEN_PAYLOAD] = "Empty String\0";

static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

static PIN_Config ledPinTable[] = {
	Board_BTN1 | PIN_INPUT_EN | PIN_NOPULL | PIN_IRQ_NEGEDGE,
	Board_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	Board_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,

	Board_LED_R | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	Board_LED_G | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	Board_LED_B | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,

	Board_MOSFET_R | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	Board_MOSFET_G | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	Board_MOSFET_B | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	Board_MOSFET_W | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_DRVSTR_MAX,

	Board_BTN_PREV 		| PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
	Board_BTN_SELECT 	| PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
	Board_BTN_NEXT 		| PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,

    PIN_TERMINATE
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void os_getDevEui (u1_t* buf);
void os_getDevKey (u1_t* buf);
void os_getArtEui (u1_t* buf);
static void Lora_taskFxn (UArg a0, UArg a1);
void send_periodic( osjob_t* j);
void do_bogus_pediodic( osjob_t* j);
void onEvent (ev_t ev);


/*********************************************************************
 * @fn      os_getDevEui
 *
 * @brief   provide device ID (8 bytes, LSBF)
 *
 * @param   buf - buffer where the Dev EUI will be copied
 *
 * @return  None.
 */
void os_getDevEui (u1_t* buf) {
	memcpy(buf, DEVEUI, 8);
}

/*********************************************************************
 * @fn      os_getDevKey
 *
 * @brief   provide device key (16 bytes)
 *
 * @param   buf - buffer where the Dev Key will be copied
 *
 * @return  None.
 */
void os_getDevKey (u1_t* buf) {
    memcpy(buf, DEVKEY, 16);
}

/*********************************************************************
 * @fn      os_getArtEui
 *
 * @brief   provide application router ID (8 bytes, LSBF)
 *
 * @param   buf - buffer where the App EUI will be copied
 *
 * @return  None.
 */
void os_getArtEui (u1_t* buf) {
    memcpy(buf, APPEUI, 8);
}



/**
 *
 */
static void btnIntCallback(PIN_Handle handle, PIN_Id pinId) {

	/* Debounce logic, only toggle if the button is still pushed (low) */
	CPUdelay(8000*50);

	// Do a system hard reset when triggered
	if (pinId == Board_BTN1){
		SysCtrlSystemReset();
	}

	// Change red color
	else if (pinId == Board_BTN_PREV){
		int r = PIN_getOutputValue(Board_MOSFET_R);
		PIN_setOutputValue(ledPinHandle, Board_MOSFET_R, !r);
		PIN_setOutputValue(ledPinHandle, Board_LED_R, r);
	}

	// Change green color
	else if (pinId == Board_BTN_SELECT){
		int g = PIN_getOutputValue(Board_MOSFET_G);
		PIN_setOutputValue(ledPinHandle, Board_MOSFET_G, !g);
		PIN_setOutputValue(ledPinHandle, Board_LED_G, g);
	}

	// Change blue color
	else if (pinId == Board_BTN_NEXT){
		int b = PIN_getOutputValue(Board_MOSFET_B);
		PIN_setOutputValue(ledPinHandle, Board_MOSFET_B, !b);
		PIN_setOutputValue(ledPinHandle, Board_LED_B, b);
	}

}


/*********************************************************************
 * @fn      Lora_taskFxn
 *
 * @brief   Application task entry point for the Simple BLE Peripheral.
 *
 * @param   a0, a1 - not used.
 *
 * @return  None.
 */
static void Lora_taskFxn (UArg a0, UArg a1)
{

	/* Open LED pins */
	ledPinHandle = PIN_open(&ledPinState, ledPinTable);
	if(!ledPinHandle) {
		//System_abort("Error initializing board LED pins\n");
		Log_error0("Error initializing board LED pins");
	}

	// Setup interupt
	if (PIN_registerIntCb(ledPinHandle, btnIntCallback) != PIN_SUCCESS) {
		System_abort("Failed to register btn int callback\n");
	}


	// LMIC init
	Log_print0(Diags_USER5, "@Init LMIC ...");
	os_init();

	// Reset the MAC state. Session and pending data transfers will be discarded.
	Log_print0(Diags_USER5, "@Reset LMIC ...");
	LMIC_reset();

	// Link check validation
	LMIC_setLinkCheckMode(1);

	// Data rate adaptation
	LMIC_setAdrMode(1);

	// Set data rate and transmit power (note: txpow seems to be ignored by the library)
	//LMIC_setDrTxpow(DR_SF7,32);

	// Disable beacon tracking
	//LMIC_disableTracking();

	// Stop listening for downstream data (periodical reception)
	//LMIC_stopPingable();

	// start joining
	LMIC_startJoining(); // init done - onEvent() callback will be invoked...
	// This also defines the default Channels

	#if defined(CFG_us915)
    // NA-US channels 0-71 are configured automatically
    // but only one group of 8 should (a subband) should be active
    // TTN recommends the second sub band, 1 in a zero based count.
    // https://github.com/TheThingsNetwork/gateway-conf/blob/master/US-global_conf.json
    LMIC_selectSubBand(0);
    //LMIC_disableSubBand(0);
    //LMIC_enableChannel(64);
    #endif

	// Enable link check validation
	//LMIC_setLinkCheckMode(1);

	LMIC_setClockError(20);

	//LMIC.datarate = DR_SF10;


	// TTN uses SF9 for its RX2 window.
	//LMIC.dn2Dr = DR_SF10;
	//LMIC.dndr = DR_SF12;


	//LMIC_setupChannel(0, 916800000, DR_RANGE_MAP(DR_SF12CR, DR_SF7), 0);

	// Set static session parameters. Instead of dynamically establishing a session
	// by joining the network, precomputed session parameters are be provided.
	//LMIC_setSession (0x00010204, msbf4_read(DevAddr), (uint8_t*)DEVKEY, (uint8_t*)ARTKEY);


	Log_print0(Diags_USER5, "@Init os_loop ...");
	DELAY_MS(250);

	//printu("@ Init blink Function ...\r\n\0");
	//os_setTimedCallback(&blinkjob, os_getTime()+ms2osticks(500), blinkfunc);
	//blinkfunc(&blinkjob);
	//redLedBlinkFunc(&redLedBlinkjob);
	//printu("@ Init blink Function Done\r\n\0");

	do_bogus_pediodic(&bogusjob);

	os_runloop();
}

void send_periodic( osjob_t* j){

	Log_print0(Diags_USER5,"send...");
	DELAY_MS(50);

	//static uint8_t mydata[] = "Artur Rullezzzzz !!!";
	static uint32_t cnt = 0;
	uint8_t len = 0;

	len = sprintf(send_buff, "!", cnt++);

	if ( !(LMIC.opmode & OP_LINKDEAD) ) {
		//LMIC_clrTxData();
		LMIC_setTxData2(2, (xref2u1_t)send_buff, len, true); //send and acknowledged message
		//LMIC_sendAlive();
	}
	//os_setTimedCallback(j, os_getTime()+ms2osticks(10000), send_periodic);
}

void do_bogus_pediodic( osjob_t* j){
	os_setTimedCallback(j, os_getTime()+ms2osticks(10000), do_bogus_pediodic);
}


static osjob_t joinblinkjob;
static bool ledstate = 0;

static void joinblinkfunc (osjob_t* j) {
    // toggle LED
    ledstate = !ledstate;
    PIN_setOutputValue(ledPinHandle, Board_GLED, ledstate);
    // reschedule blink job
    os_setTimedCallback(j, os_getTime()+ms2osticks(200), joinblinkfunc);
}

/*********************************************************************
 * @fn      onEvent
 *
 * @brief   Callback called by the LoRa Library when it changes state.
 *
 * @param   ev - Type of interrupt event that occurred
 * 				NOTE: execute the minimum amount of code or program a
 * 					Timed callback.
 * 				WARNING: do NOT try to use System_printf for debugging
 * 					purposes, use the UART instead.
 *
 * @return  None.
 */
void onEvent (ev_t ev) {

	switch(ev) {

		// scheduled data sent (optionally data received)
		// note: this includes the receive window!
		case EV_TXCOMPLETE:

			DELAY_MS(10);

			if (LMIC.txrxFlags & TXRX_ACK){
				Log_print0(Diags_USER5,"EV_TXCOMPLETE + Ack");
				//LMIC_clrTxData();
			}else
				Log_print0(Diags_USER5,"EV_TXCOMPLETE");

			LMIC_clrTxData();

			if (LMIC.dataLen) {

				Log_print1(Diags_USER5,"Received  %d bytes of payload", LMIC.dataLen);

				if(LMIC.frame[OFF_DAT_OPTS+1] == 'R'){
					int r = PIN_getOutputValue(Board_MOSFET_R);
					PIN_setOutputValue(ledPinHandle, Board_MOSFET_R, !r);
					PIN_setOutputValue(ledPinHandle, Board_LED_R, r);
				}

				else if(LMIC.frame[OFF_DAT_OPTS+1] == 'G'){
					int g = PIN_getOutputValue(Board_MOSFET_G);
					PIN_setOutputValue(ledPinHandle, Board_MOSFET_G, !g);
					PIN_setOutputValue(ledPinHandle, Board_LED_G, g);
				}

				else if(LMIC.frame[OFF_DAT_OPTS+1] == 'B'){
					int b = PIN_getOutputValue(Board_MOSFET_B);
					PIN_setOutputValue(ledPinHandle, Board_MOSFET_B, !b);
					PIN_setOutputValue(ledPinHandle, Board_LED_B, b);
				}

				else if(LMIC.frame[OFF_DAT_OPTS+1] == 'W'){
					int w = PIN_getOutputValue(Board_MOSFET_W);
					PIN_setOutputValue(ledPinHandle, Board_MOSFET_W, !w);
				}

				PIN_setOutputValue(ledPinHandle, Board_RLED, true);
				DELAY_MS(30);
				PIN_setOutputValue(ledPinHandle, Board_RLED, false);
			}

			PIN_setOutputValue(ledPinHandle, Board_GLED, true);
			DELAY_MS(20);
			PIN_setOutputValue(ledPinHandle, Board_GLED, false);

			os_setTimedCallback(&sendjob, os_getTime()+ms2osticks(2500 + os_getTime()%2500), send_periodic);
			break;

		case EV_RXCOMPLETE:
			Log_print0(Diags_USER5,"EV_RXCOMPLETE");
			break;

		case EV_JOINING:
			Log_print0(Diags_USER5,"EV_JOINING");
			joinblinkfunc(&joinblinkjob); // led starts blinking
			break;

		case EV_JOINED:
			Log_print0(Diags_USER5,"EV_JOINED");
			// cancel blink job
			os_clearCallback(&joinblinkjob);
			// switch on LED
			PIN_setOutputValue(ledPinHandle, Board_GLED, true);
			//start sending data
			os_setTimedCallback(&sendjob, os_getTime()+ms2osticks(1000), send_periodic);
			break;

		case EV_SCAN_TIMEOUT:
			Log_print0(Diags_USER5,"EV_SCAN_TIMEOUT");
			//System_printf( " EV_SCAN_TIMEOUT\r\n");
			break;

		case EV_BEACON_FOUND:
			Log_print0(Diags_USER5,"EV_BEACON_FOUND");
			//System_printf( " EV_BEACON_FOUND\r\n");
			break;

		case EV_BEACON_MISSED:
			Log_print0(Diags_USER5,"EV_BEACON_MISSED");
			//System_printf( " EV_BEACON_MISSED\r\n");
			break;

		case EV_BEACON_TRACKED:
			Log_print0(Diags_USER5,"EV_BEACON_TRACKED");
			//System_printf( " EV_BEACON_TRACKED\r\n");
			break;

		case EV_RFU1:
			Log_print0(Diags_USER5,"EV_RFU1");
			//*System_printf( " EV_RFU1\r\n");
			break;

		case EV_JOIN_FAILED:
			Log_print0(Diags_USER5,"EV_JOIN_FAILED");
			DELAY_MS(250);
			LMIC_reset();
			LMIC_setLinkCheckMode(1);
			LMIC_setAdrMode(0);
		    LMIC_selectSubBand(0);
		    //LMIC_disableSubBand(0);
		    //LMIC_enableChannel(64);
			LMIC_setClockError(20);
			break;

		case EV_REJOIN_FAILED:
			Log_print0(Diags_USER5,"EV_REJOIN_FAILED");
			LMIC_reset();
			LMIC_setLinkCheckMode(1);
			LMIC_setAdrMode(0);
		    LMIC_selectSubBand(0);
		    //LMIC_disableSubBand(0);
		    //LMIC_enableChannel(64);
			LMIC_setClockError(20);
			break;

		case EV_LOST_TSYNC:
			Log_print0(Diags_USER5,"EV_LOST_TSYNC");
			LMIC_reset();
			LMIC_setLinkCheckMode(1);
			LMIC_setAdrMode(0);
		    LMIC_selectSubBand(0);
		    //LMIC_disableSubBand(0);
		    //LMIC_enableChannel(64);
			LMIC_setClockError(20);
			break;

		case EV_RESET:
			Log_print0(Diags_USER5,"EV_RESET");
			//LMIC_reset();
			break;

		case EV_LINK_DEAD:
			Log_print0(Diags_USER5,"EV_LINK_DEAD");
			os_clearCallback(&sendjob);
			DELAY_MS(250);
			LMIC_reset();
			LMIC_setLinkCheckMode(1);
			LMIC_setAdrMode(0);
		    LMIC_selectSubBand(0);
		    //LMIC_disableSubBand(0);
		    //LMIC_enableChannel(64);
			LMIC_setClockError(20);
			break;

		case EV_LINK_ALIVE:
			Log_print0(Diags_USER5,"EV_LINK_ALIVE");
			break;

		default:
			Log_print0(Diags_USER5,"EV_UNKNOWN");
			break;
	}
}



/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      LoRa_createTask
 *
 * @brief   Task creation function for the LoRa Application.
 *
 * @param   None.
 *
 * @return  None.
 */
void LoRa_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = loraTaskStack;
  taskParams.stackSize = LORA_TASK_STACK_SIZE;
  taskParams.priority = LORA_TASK_PRIORITY;
  taskParams.instance->name = "LoRa";

  Task_construct(&loraTask, Lora_taskFxn, &taskParams, NULL);

}



#endif /* SERVICES_LORARGB_C_ */
