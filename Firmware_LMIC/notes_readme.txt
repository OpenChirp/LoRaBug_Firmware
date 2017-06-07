Example Summary
----------------
This example is intended to be a starting point for new development where
a fuller set of kernel features and debug capabilities are enabled.

Peripherals Exercised
---------------------
Board_LED0  Toggled by heartBeat task.
Board_LED1  Indicates that the board was initialized within main()

Example Usage
-------------
The example lights Board_LED1 as part of the initialization in main().
Then a heartBeat task toggles LED0 at a rate determined by the arg0 parameter
for the constructed Task instance in the .c file.

Application Design Details
--------------------------
This examples is the same as the "Empty (Minimal)" example except many
development and debug features are enabled. For example:
    - Kernel Idle task
    - Stack overflow checking
    - Default kernel heap is present

Please refer to the "Memory Footprint Reduction" section in the TI-RTOS User
Guide (spruhd4.pdf) for a complete and detailed list of the differences
between the empty minimal and empty projects.

Please note that the ROM is being used in this example. This is controlled
by the following lines in the .cfg file:
    var ROM = xdc.useModule('ti.sysbios.rom.ROM');
    if (Program.cpu.deviceName.match(/CC26/)) {
        ROM.romName = ROM.CC2650;
    }
    else if (Program.cpu.deviceName.match(/CC13/)) {
        ROM.romName = ROM.CC1350;
    }

Since the kernel in the ROM is being used, there is no logging or assert
checking done by the kernel.

Note: For IAR users using any SensorTag(STK) Board, the XDS110 debugger must be
selected with the 4-wire JTAG connection within your projects' debugger
configuration.

For GNU and IAR users, please read the following website for details about semi-hosting
http://processors.wiki.ti.com/index.php/TI-RTOS_Examples_SemiHosting.


## Very Low Power Mode Notes

- In order to achive low power mode an the board has an FTDI chip,
make sure that the "Pull Down IO Pins in USB Suspend" options is
used in the EEPRom of the FTDI chip. You can verify this by using the 
FTDI FT_PROG Utility.
 
- Make sure that the UART Data Pins are in Input mode:
	Board_UART_RX   | PIN_INPUT_EN | PIN_PULLDOWN,
	Board_UART_TX   | PIN_INPUT_EN | PIN_PULLDOWN,
	
	
## LOG TO UART
https://github.com/ti-simplelink/ble_examples/tree/ble_examples-2.1/Components/uart_log
