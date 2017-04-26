/** ============================================================================
 *  @file       CC2650_LAUNCHXL.h
 *
 *  @brief      CC2650 LaunchPad Board Specific header file.
 *
 *  NB! This is the board file for CC2650 LaunchPad PCB version 1.1
 *
 *  ============================================================================
 */
#ifndef __CC2650_LAUNCHXL_BOARD_H__
#define __CC2650_LAUNCHXL_BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

/** ============================================================================
 *  Includes
 *  ==========================================================================*/
#include <ti/drivers/PIN.h>
#include <driverlib/ioc.h>

/** ============================================================================
 *  Externs
 *  ==========================================================================*/
extern const PIN_Config BoardGpioInitTable[];

/** ============================================================================
 *  Defines
 *  ==========================================================================*/

/* Same RF Configuration as 7x7 EM */
#define CC2650EM_7ID

/* Mapping of pins to board signals using general board aliases
 *      <board signal alias>        <pin mapping>
 */

/* Discrete outputs */
#define Board_RLED                  IOID_6
#define Board_GLED                  IOID_7
#define Board_LED_ON                1
#define Board_LED_OFF               0

/* Discrete inputs */
#define Board_BTN1                  IOID_13
#define Board_BTN2                  IOID_14

#define Board_UART_RX               IOID_2         			/* RXD  */
#define Board_UART_TX               IOID_3          		/* TXD  */
#define Board_UART_CTS              PIN_UNASSIGNED         /* CTS  */
#define Board_UART_RTS              PIN_UNASSIGNED         /* RTS */

/* SPI Board */
#define Board_SPI0_MISO             IOID_8          /* RF1.20 */
#define Board_SPI0_MOSI             IOID_9          /* RF1.18 */
#define Board_SPI0_CLK              IOID_10         /* RF1.16 */
#define Board_SPI0_CSN              PIN_UNASSIGNED
#define Board_SPI0_CSN_SX           IOID_11
#define Board_SPI0_CSN_HDR          PIN_UNASSIGNED
#define Board_SPI1_MISO             PIN_UNASSIGNED
#define Board_SPI1_MOSI             PIN_UNASSIGNED
#define Board_SPI1_CLK              PIN_UNASSIGNED
#define Board_SPI1_CSN              PIN_UNASSIGNED

/* LoRa Pins */
#define Board_SX_RF_CTRL1           PIN_UNASSIGNED           /* SX1276 Antenna Control 1*/
#define Board_SX_RF_RXTX_SW         IOID_30          /* SX1276 Antenna RXTX Switch*/
#define Board_SX_RESET              IOID_18
#define Board_SX_DIO0               IOID_24
#define Board_SX_DIO1               IOID_22
#define Board_SX_DIO2               IOID_23
#define Board_SX_DIO3               PIN_UNASSIGNED
#define Board_SX_DIO4               PIN_UNASSIGNED
#define Board_SX_DIO5               PIN_UNASSIGNED

/* I2C */
#define Board_I2C0_SCL0             PIN_UNASSIGNED
#define Board_I2C0_SDA0             PIN_UNASSIGNED

/* SPI */
#define Board_SPI_FLASH_CS          IOID_20
#define Board_FLASH_CS_ON           0
#define Board_FLASH_CS_OFF          1

/* Booster pack generic */
#define Board_DIO0                  PIN_UNASSIGNED
#define Board_DIO1_RFSW             PIN_UNASSIGNED
#define Board_DIO12                 PIN_UNASSIGNED
#define Board_DIO15                 PIN_UNASSIGNED
#define Board_DIO16_TDO             PIN_UNASSIGNED
#define Board_DIO17_TDI             PIN_UNASSIGNED
#define Board_DIO21                 PIN_UNASSIGNED
#define Board_DIO22                 PIN_UNASSIGNED

#define Board_DIO23_ANALOG          PIN_UNASSIGNED
#define Board_DIO24_ANALOG          PIN_UNASSIGNED
#define Board_DIO25_ANALOG          PIN_UNASSIGNED
#define Board_DIO26_ANALOG          PIN_UNASSIGNED
#define Board_DIO27_ANALOG          PIN_UNASSIGNED
#define Board_DIO28_ANALOG          PIN_UNASSIGNED
#define Board_DIO29_ANALOG          PIN_UNASSIGNED
#define Board_DIO30_ANALOG          PIN_UNASSIGNED

/* Booster pack LCD (430BOOST - Sharp96 Rev 1.1) */
#define Board_LCD_CS                PIN_UNASSIGNED // SPI chip select
#define Board_LCD_EXTCOMIN          PIN_UNASSIGNED // External COM inversion
#define Board_LCD_ENABLE            PIN_UNASSIGNED // LCD enable
#define Board_LCD_POWER             PIN_UNASSIGNED // LCD power control
#define Board_LCD_CS_ON             1
#define Board_LCD_CS_OFF            0

/** ============================================================================
 *  Instance identifiers
 *  ==========================================================================*/
/* Generic I2C instance identifiers */
#define Board_I2C                   CC2650_LAUNCHXL_I2C0
/* Generic SPI instance identifiers */
#define Board_SPI0                  CC2650_LAUNCHXL_SPI0
#define Board_SPI1                  CC2650_LAUNCHXL_SPI1
/* Generic UART instance identifiers */
#define Board_UART                  CC2650_LAUNCHXL_UART0

#define	Board_WATCHDOG         		CC2650_LAUNCHXL_WATCHDOG0


/** ============================================================================
 *  Number of peripherals and their names
 *  ==========================================================================*/

/*!
 *  @def    CC2650_LAUNCHXL_I2CName
 *  @brief  Enum of I2C names on the CC2650 dev board
 */
typedef enum CC2650_LAUNCHXL_I2CName {
    CC2650_LAUNCHXL_I2C0 = 0,

    CC2650_LAUNCHXL_I2CCOUNT
} CC2650_LAUNCHXL_I2CName;

/*!
 *  @def    CC2650_LAUNCHXL_CryptoName
 *  @brief  Enum of Crypto names on the CC2650 dev board
 */
typedef enum CC2650_LAUNCHXL_CryptoName {
    CC2650_LAUNCHXL_CRYPTO0 = 0,

    CC2650_LAUNCHXL_CRYPTOCOUNT
} CC2650_LAUNCHXL_CryptoName;


/*!
 *  @def    CC2650_LAUNCHXL_SPIName
 *  @brief  Enum of SPI names on the CC2650 dev board
 */
typedef enum CC2650_LAUNCHXL_SPIName {
    CC2650_LAUNCHXL_SPI0 = 0,
    CC2650_LAUNCHXL_SPI1,

    CC2650_LAUNCHXL_SPICOUNT
} CC2650_LAUNCHXL_SPIName;

/*!
 *  @def    CC2650_LAUNCHXL_UARTName
 *  @brief  Enum of UARTs on the CC2650 dev board
 */
typedef enum CC2650_LAUNCHXL_UARTName {
    CC2650_LAUNCHXL_UART0 = 0,

    CC2650_LAUNCHXL_UARTCOUNT
} CC2650_LAUNCHXL_UARTName;

/*!
 *  @def    CC2650_LAUNCHXL_UdmaName
 *  @brief  Enum of DMA buffers
 */
typedef enum CC2650_LAUNCHXL_UdmaName {
    CC2650_LAUNCHXL_UDMA0 = 0,

    CC2650_LAUNCHXL_UDMACOUNT
} CC2650_LAUNCHXL_UdmaName;


/*!
 *  @def    CC2650_LAUNCHXL_WatchdogName
 *  @brief  Enum of Watchdogs on the CC2650_LAUNCHXL dev board
 */
typedef enum CC2650_LAUNCHXL_WatchdogName {
    CC2650_LAUNCHXL_WATCHDOG0 = 0,

    CC2650_LAUNCHXL_WATCHDOGCOUNT
} CC2650_LAUNCHXL_WatchdogName;

#ifdef __cplusplus
}
#endif

#endif /* __CC2650_LAUNCHXL_BOARD_H__ */
