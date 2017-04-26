/******************************************************************************

  @file  loraCommon.c

 @brief This file contains the Simple Ping Pong Common Functions
         Created on: Jan 27, 2017

 @author: Artur Balanuta

 ******************************************************************************/

#ifndef SERVICES_LORACOMMON_C_
#define SERVICES_LORACOMMON_C_

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

/* UART logs */
//#define xdc_runtime_Log_DISABLE_ALL 1  // Add to disable logs from this file
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>   // For Log_warning1("Warning number #%d", 4); things
#include <xdc/runtime/Diags.h> // For Log_print0(Diags_USER1, "hello"); things.


/*******************************************************************************
 * MACROS
 */

#define DELAY_MS(i) Task_sleep(((i) * 1000) / Clock_tickPeriod)

#define IRQ_LORA_RXTOUT_MASK                        0x80
#define IRQ_LORA_RXDONE_MASK                        0x40
#define IRQ_LORA_TXDONE_MASK                        0x08

#define OPMODE_SLEEP                                0x00
#define OPMODE_MASK                                 0x07

#define RegFifo                                     0x00 // common
#define RegOpMode                                   0x01 // common
#define LORARegIrqFlagsMask                         0x11
#define LORARegIrqFlags                             0x12
#define LORARegFifoAddrPtr                          0x0D
#define LORARegPktSnrValue                          0x19
#define LORARegFifoRxCurrentAddr                    0x10
#define LORARegModemConfig1                         0x1D
#define LORARegPayloadLength                        0x22
#define LORARegRxNbBytes                            0x13
#define LORARegPktRssiValue                         0x1A

#define SX1272_MC1_IMPLICIT_HEADER_MODE_ON          0x04 // required for receive

/*******************************************************************************
 * CONSTANTS
 */
#define TX_FREQ         904400000   //  Operational Frequency
#define TX_POWER        2           //  dBm goes from 2 to 16
#define TX_SF           SF10        //  from 7 to 12
#define TX_BANDWIDTH    BW500       // BW125, BW250, BW500
#define TX_CODINGRATE   CR_4_5      //C_4_5, CR_4_6, CR_4_7, CR_4_8

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void writeReg (u1_t addr, u1_t data ) {
    hal_pin_nss(0);
    hal_spi(addr | 0x80);
    hal_spi(data);
    hal_pin_nss(1);
}

static u1_t readReg (u1_t addr) {
    hal_pin_nss(0);
    hal_spi(addr & 0x7F);
    u1_t val = hal_spi(0x00);
    hal_pin_nss(1);
    return val;
}

static void readBuf (u1_t addr, xref2u1_t buf, u1_t len) {
    hal_pin_nss(0);
    hal_spi(addr & 0x7F);
    u1_t i;
    for (i=0; i<len; i++) {
        buf[i] = hal_spi(0x00);
    }
    hal_pin_nss(1);
}

static void opmode (u1_t mode) {
    writeReg(RegOpMode, (readReg(RegOpMode) & ~OPMODE_MASK) | mode);
}

void read_rx_status (){

    u1_t flags = readReg(LORARegIrqFlags);

    if( flags & IRQ_LORA_RXDONE_MASK ) {
        Log_info0("IRQ RX DONE");

        // read the PDU and inform the MAC that we received something
        LMIC.dataLen = (readReg(LORARegModemConfig1) & SX1272_MC1_IMPLICIT_HEADER_MODE_ON) ? readReg(LORARegPayloadLength) : readReg(LORARegRxNbBytes);
        // set FIFO read address pointer
        writeReg(LORARegFifoAddrPtr, readReg(LORARegFifoRxCurrentAddr));
        // now read the FIFO
        readBuf(RegFifo, LMIC.frame, LMIC.dataLen);

        // read rx quality parameters
        LMIC.snr  = readReg(LORARegPktSnrValue); // SNR [dB] * 4
        LMIC.rssi = readReg(LORARegPktRssiValue) - 125 + 64; // RSSI [dBm] (-196...+63)
    }

    else if( flags & IRQ_LORA_RXTOUT_MASK ) {
        Log_info0("IRQ RX TIMEOUT");
        // indicate timeout
        LMIC.dataLen = 0;
    }

    // mask all radio IRQs
    writeReg(LORARegIrqFlagsMask, 0xFF);
    // clear radio IRQ flags
    writeReg(LORARegIrqFlags, 0xFF);

    // go from stanby to sleep
    opmode(OPMODE_SLEEP);
}


/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void setup(){

    // initialize runtime env
    os_init();

    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    // Set configuration parameters
    LMIC.freq = TX_FREQ;
    LMIC.txpow = TX_POWER;
    //LMIC.datarate = TX_DATARATE;
    // This sets CR 4/5, BW125 (except for DR_SF7B, which uses BW250)
    //LMIC.rps = updr2rps(LMIC.datarate);
    LMIC.rps = makeRps(TX_SF, TX_BANDWIDTH, TX_CODINGRATE,0,0);

}

void tx(uint8_t* data, uint8_t data_len) {

  if( data != (xref2u1_t)0 )
      os_copyMem(LMIC.frame, data, data_len);
  LMIC.dataLen  = data_len;
  LMIC.opmode = OP_TXDATA;

  os_radio(RADIO_TX);

  // Wait for TX to be done
  while(hal_io_get_dio0() == false){}

  Log_info2("TX Powar: %i Len: %i", LMIC.txpow, LMIC.dataLen);
}

uint8_t rx() {

  // Enable "continuous" RX (e.g. without a timeout, still stops after receiving a packet)
  os_radio(RADIO_RXON);

  // Wait for RX to complete
  while(hal_io_get_dio0() == false && hal_io_get_dio1() == false){}

  // Reads reception status and packet info
  read_rx_status();

  Log_info5("RX Got %s has %i bytes RSSI: %i dBm SNR: %i,%i dB",
            (xdc_IArg)LMIC.frame, LMIC.dataLen, LMIC.rssi, LMIC.snr>>2, (LMIC.snr-((LMIC.snr/4)*4))*25 );

  return LMIC.dataLen;
}

#endif /* SERVICES_LORACOMMON_C_ */
