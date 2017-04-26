/******************************************************************************

  @file  loraLmicRawService.c

 @brief This file contains the Simple blink sample application
         Created on: Nov 29, 2016

 @author: Artur Balanuta

 ******************************************************************************/

#ifndef SERVICES_LORALMICRAWSERVICE_C_
#define SERVICES_LORALMICRAWSERVICE_C_

/*********************************************************************
 * INCLUDES
 */
#include <stdio.h>
#include <string.h>

#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/hal/Seconds.h>

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
#define LORA_TASK_STACK_SIZE                   1200
#endif

#define Freq1 867500000
#define Freq2 867700000
#define Freq3 867900000
#define Freq4 868100000
#define Freq5 868300000
#define Freq6 868500000

#define TX_POWER 2  //dBm goes from 2 to 30
// First 8 125kHz uplink channels
#define FreqUS0 (902300000+(200000*0))
#define FreqUS1 (902300000+(200000*1))
#define FreqUS2 (902300000+(200000*2))
#define FreqUS3 (902300000+(200000*3))
#define FreqUS4 (902300000+(200000*4))
#define FreqUS5 (902300000+(200000*5))
#define FreqUS6 (902300000+(200000*6))
#define FreqUS7 (902300000+(200000*7))
// ... FreqUS63

// First 500kHz uplink channel
#define FreqUS64 (903000000+(1600000*0))
// ... FreqUS71

#define TX_DELAY 4000  //This - LED_ON_DELAY
#define TX_FREQ FreqUS2
#define TX_DATARATE DR_SF7 //from 7 to 12
#define TX_MAX_PACKETS 1000

#define LED_PIN Board_GLED
#define LED_ON_DELAY 10 	//ms

//#define RX_FUNCTION 1



/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

// LoRaWAN NwkSKey, network session key
static const u1_t NWKSKEY[16] = { 0x00, 0x11, 0x22, 0x33,
                                          0x44, 0x55, 0x66, 0x77,
                                          0x88, 0x99, 0xaa, 0xbb,
                                          0xcc, 0xdd, 0xee, 0xff };

// LoRaWAN AppSKey, application session key
static const u1_t APPSKEY[16] = { 0x00, 0x11, 0x22, 0x33,
                                          0x44, 0x55, 0x66, 0x77,
                                          0x88, 0x99, 0xaa, 0xbb,
                                          0xcc, 0xdd, 0xee, 0xff };

// LoRaWAN end-device address (DevAddr)
//static const u4_t DEVADDR = 0x07931236 ; // <-- Change this address for every node!

static const u4_t DEVADDR = 0x06f0e61a ; // Viking
//static const u4_t DEVADDR = 0x0791cb49 ; // Romans
//static const u4_t DEVADDR = 0x??? ; // Persian
//static const u4_t DEVADDR = 0x06f42b2e ; // Aztecs

// Task configuration
Task_Struct loraRawTask;
Char loraRawTaskStack[LORA_TASK_STACK_SIZE];

static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

static PIN_Config ledPinTable[] = {
	Board_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};


static osjob_t txjob;

static uint8_t mydata[] = "1";

/*********************************************************************
 * LOCAL FUNCTIONS
 */

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
//void os_getArtEui (u1_t* buf) { }
//void os_getDevEui (u1_t* buf) { }
//void os_getDevKey (u1_t* buf) { }

// Aux functions for the TX messages to work
static void aes_cipher (xref2cu1_t key, u4_t devaddr, u4_t seqno, int dndir, xref2u1_t payload, int len);
static void aes_appendMic (xref2cu1_t key, u4_t devaddr, u4_t seqno, int dndir, xref2u1_t pdu, int len);
static void micB0 (u4_t devaddr, u4_t seqno, int dndir, int len);
static void tx_func (osjob_t* job);
static void txdone_func (osjob_t* job);
void buildTxFrame();
void tx(const char *str, osjobcb_t func);


// log text to USART and toggle LED
static void tx_func (osjob_t* job) {

  static uint32_t count = 0;
  int waiting = 0;

  // say hello
  PIN_setOutputValue(ledPinHandle, LED_PIN, true); // LED on
  tx(mydata, txdone_func);

  while(hal_io_get_dio0() == false){
    DELAY_US(1);
    waiting++;
  }
  PIN_setOutputValue(ledPinHandle, LED_PIN, false); // LED off, packet has been sent


  Log_info3("%i :    Powar: %i Waited %i us", count++, LMIC.txpow, waiting);

  if(count >= TX_MAX_PACKETS)
    while (1) {
      /* code */
    }

  // reschedule job every TX_INTERVAL (plus a bit of random to prevent
  // systematic collisions), unless packets are received, then rx_func
  // will reschedule at half this time.
  os_setTimedCallback(job, os_getTime() + ms2osticks(TX_DELAY), tx_func);
}


/*********************************************************************
 * @fn      LoraRaw_taskFxn
 * @brief   Application task entry point for the Simple BLE Peripheral.
 * @param   a0, a1 - not used.
 * @return  None.
 */
static void LoraRaw_taskFxn (UArg a0, UArg a1)
{
	Log_info0("Starting");

	/* Open LED pins */
	ledPinHandle = PIN_open(&ledPinState, ledPinTable);
	if(!ledPinHandle) {
		Log_error0("Error initializing board LED pins");
		System_abort("Error initializing board LED pins\n");
	}


	// initialize runtime env
	os_init();
	// Reset the MAC state. Session and pending data transfers will be discarded.
	LMIC_reset();

	// On AVR, these values are stored in flash and only copied to RAM
	// once. Copy them to a temporary buffer here, LMIC_setSession will
	// copy them into a buffer of its own again.
	LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);

	// Use a frequency in the g3 which allows 10% duty cycling.
	LMIC.freq = TX_FREQ;

	// Maximum TX power
	LMIC.txpow = TX_POWER;
	// Use a medium spread factor. This can be increased up to SF12 for
	// better range, but then the interval should be (significantly)
	// lowered to comply with duty cycle limits as well.
	LMIC.datarate = TX_DATARATE;
	// This sets CR 4/5, BW125 (except for DR_SF7B, which uses BW250)
	LMIC.rps = updr2rps(LMIC.datarate);

	Log_info0("Started");

	// setup initial job
	os_setCallback(&txjob, tx_func);

	os_runloop();
}


// Do nothing
void onEvent (ev_t ev) {}

// Transmit the given string and call the given function afterwards
void tx(const char *str, osjobcb_t func) {

  os_radio(RADIO_RST); // Stop RX first
  DELAY_MS(1); // Wait a bit, without this os_radio below asserts, apparently because the state hasn't changed yet

  LMIC.dataLen = 0;
  //while (*str)
  //  LMIC.pendTxData[LMIC.dataLen++] = *str++;

  if( mydata != (xref2u1_t)0 )
      os_copyMem(LMIC.pendTxData, mydata, sizeof(mydata));
  LMIC.pendTxConf = 0;
  LMIC.pendTxPort = 1;
  LMIC.pendTxLen  = sizeof(mydata);
  LMIC.txCnt = 0;

  //show_frame(0);

  //LMIC.pendTxLen = LMIC.dataLen;
  LMIC.opmode = OP_TXDATA;

  buildTxFrame();

  //show_frame(99);

  LMIC.osjob.func = func;
  os_radio(RADIO_TX);
}


void buildTxFrame(){
  bit_t txdata = ((LMIC.opmode & (OP_TXDATA|OP_POLL)) != OP_POLL);
  u1_t dlen = txdata ? LMIC.pendTxLen : 0;
  int  end = OFF_DAT_OPTS;
  ASSERT(end <= OFF_DAT_OPTS+16);
  u1_t flen = end + (txdata ? 5+dlen : 4);

  //show_frame(2);

  if( flen > MAX_LEN_FRAME ) {
      // Options and payload too big - delay payload
      txdata = 0;
      flen = end+4;
  }
  LMIC.frame[OFF_DAT_HDR] = HDR_FTYPE_DAUP | HDR_MAJOR_V1;
  LMIC.frame[OFF_DAT_FCT] = (LMIC.dnConf | LMIC.adrEnabled
                            | (LMIC.adrAckReq >= 0 ? FCT_ADRARQ : 0)
                            | (end-OFF_DAT_OPTS));
  //show_frame(3);
  os_wlsbf4(LMIC.frame+OFF_DAT_ADDR, LMIC.devaddr);
  //show_frame(4);

  if( LMIC.txCnt == 0 ) {
      LMIC.seqnoUp += 1;
      DO_DEVDB(LMIC.seqnoUp,seqnoUp);
  } else {
      EV(devCond, INFO, (e_.reason = EV::devCond_t::RE_TX,
                         e_.eui    = MAIN::CDEV->getEui(),
                         e_.info   = LMIC.seqnoUp-1,
                         e_.info2  = ((LMIC.txCnt+1) |
                                      (TABLE_GET_U1(DRADJUST, LMIC.txCnt+1) << 8) |
                                      ((LMIC.datarate|DR_PAGE)<<16))));
  }
  //show_frame(5);
  os_wlsbf2(LMIC.frame+OFF_DAT_SEQNO, LMIC.seqnoUp-1);
  //show_frame(6);
  // Clear pending DN confirmation
  LMIC.dnConf = 0;

  if( txdata ) {
      if( LMIC.pendTxConf ) {
          // Confirmed only makes sense if we have a payload (or at least a port)
          LMIC.frame[OFF_DAT_HDR] = HDR_FTYPE_DCUP | HDR_MAJOR_V1;
          if( LMIC.txCnt == 0 ) LMIC.txCnt = 1;
          //show_frame(7);
      }
      LMIC.frame[end] = LMIC.pendTxPort;
      //show_frame(8);
      os_copyMem(LMIC.frame+end+1, LMIC.pendTxData, dlen);
      //show_frame(9);
      aes_cipher(LMIC.pendTxPort==0 ? LMIC.nwkKey : LMIC.artKey,
                 LMIC.devaddr, LMIC.seqnoUp-1,
                 /*up*/0, LMIC.frame+end+1, dlen);
      //show_frame(10);
  }
  aes_appendMic(LMIC.nwkKey, LMIC.devaddr, LMIC.seqnoUp-1, /*up*/0, LMIC.frame, flen-4);
  //show_frame(11);

  EV(dfinfo, DEBUG, (e_.deveui  = MAIN::CDEV->getEui(),
                     e_.devaddr = LMIC.devaddr,
                     e_.seqno   = LMIC.seqnoUp-1,
                     e_.flags   = (LMIC.pendTxPort < 0 ? EV::dfinfo_t::NOPORT : EV::dfinfo_t::NOP),
                     e_.mic     = Base::lsbf4(&LMIC.frame[flen-4]),
                     e_.hdr     = LMIC.frame[LORA::OFF_DAT_HDR],
                     e_.fct     = LMIC.frame[LORA::OFF_DAT_FCT],
                     e_.port    = LMIC.pendTxPort,
                     e_.plen    = txdata ? dlen : 0,
                     e_.opts.length = end-LORA::OFF_DAT_OPTS,
                     memcpy(&e_.opts[0], LMIC.frame+LORA::OFF_DAT_OPTS, end-LORA::OFF_DAT_OPTS)));
  //show_frame(12);
  LMIC.dataLen = flen;
  //show_frame(13);

}

void aes_cipher (xref2cu1_t key, u4_t devaddr, u4_t seqno, int dndir, xref2u1_t payload, int len) {

	if( len <= 0 )
		return;

    os_clearMem(AESaux, 16);
    AESaux[0] = AESaux[15] = 1; // mode=cipher / dir=down / block counter=1
    AESaux[5] = dndir?1:0;
    os_wlsbf4(AESaux+ 6,devaddr);
    os_wlsbf4(AESaux+10,seqno);
    os_copyMem(AESkey,key,16);
    os_aes(AES_CTR, payload, len);
}

void aes_appendMic (xref2cu1_t key, u4_t devaddr, u4_t seqno, int dndir, xref2u1_t pdu, int len) {
    micB0(devaddr, seqno, dndir, len);
    os_copyMem(AESkey,key,16);
    // MSB because of internal structure of AES
    os_wmsbf4(pdu+len, os_aes(AES_MIC, pdu, len));
}

void micB0 (u4_t devaddr, u4_t seqno, int dndir, int len) {
    os_clearMem(AESaux,16);
    AESaux[0]  = 0x49;
    AESaux[5]  = dndir?1:0;
    AESaux[15] = len;
    os_wlsbf4(AESaux+ 6,devaddr);
    os_wlsbf4(AESaux+10,seqno);
}

static void txdone_func (osjob_t* job) {
  //rx(rx_func);
}


#ifdef RX_FUNCTION

// Enable rx mode and call func when a packet is received
void rx(osjobcb_t func) {
  LMIC.osjob.func = func;
  LMIC.rxtime = os_getTime(); // RX _now_
  // Enable "continuous" RX (e.g. without a timeout, still stops after
  // receiving a packet)
  os_radio(RADIO_RXON);
  Log_info0("RX");
}

static void rxtimeout_func(osjob_t *job) {
  PIN_setOutputValue(ledPinHandle, LED_PIN, false); //off
}

static void rx_func (osjob_t* job) {
	// Blink once to confirm reception and then keep the led on
	PIN_setOutputValue(ledPinHandle, LED_PIN, false); //off
	DELAY_MS(10);
	PIN_setOutputValue(ledPinHandle, LED_PIN, true); //on

  // Timeout RX (i.e. update led status) after 3 periods without RX
  os_setTimedCallback(&timeoutjob, os_getTime() + ms2osticks(3*TX_INTERVAL), rxtimeout_func);

  // Reschedule TX so that it should not collide with the other side's
  // next TX
  os_setTimedCallback(&txjob, os_getTime() + ms2osticks(TX_INTERVAL/2), tx_func);

  Log_info1("Got %i bytes", LMIC.dataLen);

  // Restart RX
  rx(rx_func);
}

#endif /* RX_FUNCTION */

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
void LoRaLmicRawService_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = loraRawTaskStack;
  taskParams.stackSize = LORA_TASK_STACK_SIZE;
  taskParams.priority = LORA_TASK_PRIORITY;
  taskParams.instance->name = "LoRa";

  Task_construct(&loraRawTask, LoraRaw_taskFxn, &taskParams, NULL);

}



#endif /* SERVICES_LORALMICRAWSERVICE_C_ */
