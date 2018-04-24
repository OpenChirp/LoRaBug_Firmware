# (Discontinued) 
 Use LoRaBUG_DemoClassA instead.


# How to use
```
git clone git@github.com:OpenChirp/LoRaBug_Firmware.git
cd LoRaBug_Firmware/Firmware_LoRaMAC/LoRaMac-node
git submodule update --init --recursive
git checkout lorabug
```


# Structure
* ```Firmware_LMIC``` contains th LMIC Firmware implementation adapted from [link](https://github.com/matthijskooijman/arduino-lmic)
* ```Firmware_LoRaMCA``` contains the LoRA_MAC Firmware implementation adapted from [Semtech](https://github.com/Lora-net/LoRaMac-node)

# Based on TI-RTOS
# Libraries Versions for TI-RTOS + LoRa
The project should be compiled with the this set of tools:
* ~~CCS v6.2.0.00048 or higher [link](http://www.ti.com/tool/ccstudio) (Check **CC26xx devices**)~~
* CCS v7 [link](http://processors.wiki.ti.com/index.php/Download_CCS) (Check **CC13xx and CC26xx devices during Install**)
* XDCtools version: [3.32.1.22_core](http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/index.html)
* TI-RTOS for CC13XX and CC26XX [v2.18.0.03](http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/tirtos/index.html)
* Compiler version: TI ARM v5.2.8 (in CCS Software search for Arm Compiler Tools v5.2.8)

# Libraries Versions for BLE
* [TI BLE SDK v2.x](http://www.ti.com/tool/ble-stack?DCMP=wbu-blestack&HQS=ble-stack)
* For BLE Dev: [TI-BDS-PLUGIN 1.0.7 beta](http://software-dl.ti.com/lprf/bds/ti-bds-plugin.html)
* [Additional Link](http://software-dl.ti.com/lprf/simplelink_academy/overview.html#introduction)
* [Problems?](http://processors.wiki.ti.com/index.php/CC2640_Porting_Projects#Porting_BLEv2.2.0_Projects_to_BLEv2.2.1)

## Space Optimization
* The logging takes up ```~23kB``` of Flash memory in this application, because it uses ```System_printf``` functionality, as well as the UART driver and a Text table from TI-RTOS. In addition there is some overhead per statement, as well as the strings themselves. To remove the impact Log has on the image size, add a global compiler define ```xdc_runtime_Log_DISABLE_ALL``` as well as changing the TI-RTOS config file to read ```Text.isLoaded = false;``` instead of true.
* For ignoring the logs from a specific file,  add ```#define xdc_runtime_Log_DISABLE_ALL 1``` to the top of the file
* Predefined Option for shorter filepaths: xdc_FILE="\"${InputFileName}\""
* Others in Section 10.4.1  of the BLE Manual

## Version Control Rules
* [.gitignore](http://processors.wiki.ti.com/index.php/Source_control_with_CCS)

# Notes

* UART
  * Rate: 3M baud
  * 8 Data and 1 Stop bits
  * No Parity
  * Flow control XON/XOFF

# Changelog for LMIC
* v0.3 (29-Nov-2016)
  * Changed the maximum TX Power from 15 to 30, possible values are now from 2 to 30 
  * Introducing the RAW TX Service (does not use the LMIC os, bypass of the rx function) 
* v0.2 (17-Nov-2016)
  * Basic BLE App that writes APP EUI, DEV EUI and DEV KEY to the Flash
* v0.1 (2-Nov-2016)
  * Faster Join Process
  * Changed the default EU868 DR_DNW2 to 869525000 Hz and SF12
  * Using 2 attempts of joining before throwing a Join_Fail Event
* v0.0 (1-Nov-2016)
  * Initial release based on lmic v1.6

# BLE
* ```devinfo Service``` can be remove to gain additional space (796 Flash and 480 SRAM Less)
