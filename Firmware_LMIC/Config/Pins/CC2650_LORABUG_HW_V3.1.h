
/** ============================================================================
 *  @file       CC2650_LORABUG_HW_V3.1.h
 *
 *  @brief      CC2650 Lorabug HW v3.1 Board Specific header file.
 *
 *
 *  ============================================================================
 */
#ifndef __CC2650_LORABUG_HW_V3_1_BOARD_H__
#define __CC2650_LORABUG_HW_V3_1_BOARD_H__

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
#define Board_RLED                  IOID_8
#define Board_GLED                  IOID_9
#define Board_LED_ON                1
#define Board_LED_OFF               0

/* Discrete inputs */
#define Board_BTN1                  IOID_22
#define Board_BTN2                  PIN_UNASSIGNED

/* UART Board */
#define Board_UART_RX               IOID_2          /* FTDI RXD  */
#define Board_UART_TX               IOID_3          /* FTDI TXD  */
#define Board_UART_CTS              PIN_UNASSIGNED  /* CTS  */
#define Board_UART_RTS              PIN_UNASSIGNED  /* RTS  */
#define Board_UART_HDR_TX1          IOID_23         /* Header MCU TX  */
#define Board_UART_HDR_RX1          IOID_24         /* Header MCU RX  */

/* SPI Board */
#define Board_SPI0_MISO             IOID_14
#define Board_SPI0_MOSI             IOID_13
#define Board_SPI0_CLK              IOID_15
#define Board_SPI0_CSN              PIN_UNASSIGNED    /* Assign Manually */
#define Board_SPI0_CSN_SX           IOID_12           /* SX1276 Chip*/
#define Board_SPI0_CSN_HDR          IOID_1            /* Header */

#define Board_SPI1_MISO             PIN_UNASSIGNED
#define Board_SPI1_MOSI             PIN_UNASSIGNED
#define Board_SPI1_CLK              PIN_UNASSIGNED
#define Board_SPI1_CSN              PIN_UNASSIGNED

/* I2C */
#define Board_I2C0_SCL0             IOID_6
#define Board_I2C0_SDA0             IOID_7
#define Board_I2C_HDR_SCL           Board_I2C0_SCL0        /* Header I2C SCL */
#define Board_I2C_HDR_SDA           Board_I2C0_SDA0        /* Header I2C SDA */

/* SPI */
#define Board_SPI_FLASH_CS          PIN_UNASSIGNED
#define Board_FLASH_CS_ON           0
#define Board_FLASH_CS_OFF          1

/* LoRa Pins */
#define Board_SX_RF_CTRL1           IOID_0           /* Controls RF switches complement pair positive */
#define Board_SX_RF_CTRL2           IOID_10          /* Controls RF switches complement pair negative */
#define Board_SX_RESET              IOID_11
#define Board_SX_DIO0               IOID_21
#define Board_SX_DIO1               IOID_20
#define Board_SX_DIO2               IOID_19
#define Board_SX_DIO3               IOID_18
#define Board_SX_DIO4               IOID_17
#define Board_SX_DIO5               IOID_16

/* Header Pins */
#define Board_HDR_PB7               IOID_5
#define Board_HDR_PB6               IOID_25
#define Board_HDR_PB3               IOID_26
#define Board_HDR_GP0               IOID_28
#define Board_HDR_PORTF1            IOID_27          /* Header Analog 1*/
#define Board_HDR_PORTF3            IOID_29          /* Header Analog 3*/
#define Board_HDR_PORTF6            IOID_30          /* Header Analog 4*/

/* Header Pins2 */
#define Board_HDR_HDIO0             IOID_5
#define Board_HDR_HDIO1             IOID_6
#define Board_HDR_HDIO2             IOID_7
#define Board_HDR_ADIO0             IOID_23
#define Board_HDR_ADIO1             IOID_24
#define Board_HDR_ADIO2             IOID_25
#define Board_HDR_ADIO3             IOID_26
#define Board_HDR_ADIO4             IOID_27
#define Board_HDR_ADIO5             IOID_28
#define Board_HDR_ADIO6             IOID_29
#define Board_HDR_ADIO7             IOID_30

/* RGBW Board LED */
#define Board_LED_R					IOID_5
#define Board_LED_G					IOID_6
#define Board_LED_B					IOID_7

/* RGBW Board MOSFETS */
#define Board_MOSFET_R				IOID_23
#define Board_MOSFET_G				IOID_24
#define Board_MOSFET_B				IOID_25
#define Board_MOSFET_W				IOID_26

/* RGBW Board BUTTONS */
#define Board_BTN_PREV				IOID_30
#define Board_BTN_SELECT			IOID_28
#define Board_BTN_NEXT				IOID_29



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

/* PWM outputs */
#define Board_PWMPIN0                       Board_RLED
#define Board_PWMPIN1                       Board_GLED
#define Board_PWMPIN2                       PIN_UNASSIGNED
#define Board_PWMPIN3                       PIN_UNASSIGNED
#define Board_PWMPIN4                       PIN_UNASSIGNED
#define Board_PWMPIN5                       PIN_UNASSIGNED
#define Board_PWMPIN6                       PIN_UNASSIGNED
#define Board_PWMPIN7                       PIN_UNASSIGNED

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
/* Generic Crypto instance identifiers */
#define Board_CRYPTO                CC2650_LAUNCHXL_CRYPTO0
/* Generic GPTimer instance identifiers */
#define Board_GPTIMER0A             CC2650_LAUNCHXL_GPTIMER0A
#define Board_GPTIMER0B             CC2650_LAUNCHXL_GPTIMER0B
#define Board_GPTIMER1A             CC2650_LAUNCHXL_GPTIMER1A
#define Board_GPTIMER1B             CC2650_LAUNCHXL_GPTIMER1B
#define Board_GPTIMER2A             CC2650_LAUNCHXL_GPTIMER2A
#define Board_GPTIMER2B             CC2650_LAUNCHXL_GPTIMER2B
#define Board_GPTIMER3A             CC2650_LAUNCHXL_GPTIMER3A
#define Board_GPTIMER3B             CC2650_LAUNCHXL_GPTIMER3B
/* Generic PWM instance identifiers */
#define Board_PWM0                  CC2650_LAUNCHXL_PWM0
#define Board_PWM1                  CC2650_LAUNCHXL_PWM1
#define Board_PWM2                  CC2650_LAUNCHXL_PWM2
#define Board_PWM3                  CC2650_LAUNCHXL_PWM3
#define Board_PWM4                  CC2650_LAUNCHXL_PWM4
#define Board_PWM5                  CC2650_LAUNCHXL_PWM5
#define Board_PWM6                  CC2650_LAUNCHXL_PWM6
#define Board_PWM7                  CC2650_LAUNCHXL_PWM7

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
 *  @def    CC2650_LAUNCHXL_GPTimerName
 *  @brief  Enum of GPTimer parts
 */
typedef enum CC2650_LAUNCHXL_GPTimerName
{
    CC2650_LAUNCHXL_GPTIMER0A = 0,
    CC2650_LAUNCHXL_GPTIMER0B,
    CC2650_LAUNCHXL_GPTIMER1A,
    CC2650_LAUNCHXL_GPTIMER1B,
    CC2650_LAUNCHXL_GPTIMER2A,
    CC2650_LAUNCHXL_GPTIMER2B,
    CC2650_LAUNCHXL_GPTIMER3A,
    CC2650_LAUNCHXL_GPTIMER3B,
    CC2650_LAUNCHXL_GPTIMERPARTSCOUNT
} CC2650_LAUNCHXL_GPTimerName;

/*!
 *  @def    CC2650_LAUNCHXL_GPTimers
 *  @brief  Enum of GPTimers
 */
typedef enum CC2650_LAUNCHXL_GPTimers
{
    CC2650_LAUNCHXL_GPTIMER0 = 0,
    CC2650_LAUNCHXL_GPTIMER1,
    CC2650_LAUNCHXL_GPTIMER2,
    CC2650_LAUNCHXL_GPTIMER3,
    CC2650_LAUNCHXL_GPTIMERCOUNT
} CC2650_LAUNCHXL_GPTimers;

/*!
 *  @def    CC2650_LAUNCHXL_PWM
 *  @brief  Enum of PWM outputs on the board
 */
typedef enum CC2650_LAUNCHXL_PWM
{
    CC2650_LAUNCHXL_PWM0 = 0,
    CC2650_LAUNCHXL_PWM1,
    CC2650_LAUNCHXL_PWM2,
    CC2650_LAUNCHXL_PWM3,
    CC2650_LAUNCHXL_PWM4,
    CC2650_LAUNCHXL_PWM5,
    CC2650_LAUNCHXL_PWM6,
    CC2650_LAUNCHXL_PWM7,
    CC2650_LAUNCHXL_PWMCOUNT
} CC2650_LAUNCHXL_PWM;

/*!
 *  @def    CC2650_LAUNCHXL_ADCBufName
 *  @brief  Enum of ADCs
 */
typedef enum CC2650_LAUNCHXL_ADCBufName {
    CC2650_LAUNCHXL_ADCBuf0 = 0,
    CC2650_LAUNCHXL_ADCBufCOUNT
} CC2650_LAUNCHXL_ADCBufName;


/*!
 *  @def    CC2650_LAUNCHXL_ADCName
 *  @brief  Enum of ADCs
 */
typedef enum CC2650_LAUNCHXL_ADCName {
    CC2650_LAUNCHXL_ADC0 = 0,
    CC2650_LAUNCHXL_ADC1,
    CC2650_LAUNCHXL_ADC2,
    CC2650_LAUNCHXL_ADC3,
    CC2650_LAUNCHXL_ADC4,
    CC2650_LAUNCHXL_ADC5,
    CC2650_LAUNCHXL_ADC6,
    CC2650_LAUNCHXL_ADC7,
    CC2650_LAUNCHXL_ADCDCOUPL,
    CC2650_LAUNCHXL_ADCVSS,
    CC2650_LAUNCHXL_ADCVDDS,
    CC2650_LAUNCHXL_ADCCOUNT
} CC2650_LAUNCHXL_ADCName;

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

#endif /* __CC2650_LORABUG_HW_V3_1_BOARD_H__ */
