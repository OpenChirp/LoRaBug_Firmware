/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== ccfg.c ========
 *  Customer Configuration for CC26xx and CC13xx devices.  This file is used to
 *  configure Boot ROM, start-up code, and SW radio behaviour.
 *
 *  By default, driverlib startup_files/ccfg.c settings are used.  However, if
 *  changes are required there are two means to do so:
 *
 *    1.  Remove this file and copy driverlib's startup_files/ccfg.c file in
 *        its place.  Make all changes to the file.  Changes made are local to
 *        the project and will not affect other projects.
 *
 *    2.  Perform changes to driverlib startup_files/ccfg.c file.  Changes
 *        made to this file will be applied to all projects.  This file must
 *        remain unmodified.
 */

//#####################################
// Bootloader settings
//#####################################

#include "Board.h"

#ifndef SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE
//#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE            0x00       // Disable ROM boot loader
#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE            0xC5       // Enable ROM boot loader
#endif

#ifndef SET_CCFG_BL_CONFIG_BL_LEVEL
#define SET_CCFG_BL_CONFIG_BL_LEVEL                     0x0        // Active low to open boot loader backdoor
//#define SET_CCFG_BL_CONFIG_BL_LEVEL                     0x1        // Active high to open boot loader backdoor
#endif

#ifndef SET_CCFG_BL_CONFIG_BL_PIN_NUMBER
#define SET_CCFG_BL_CONFIG_BL_PIN_NUMBER                Board_BTN1       // DIO number for boot loader backdoor
#endif

#ifndef SET_CCFG_BL_CONFIG_BL_ENABLE
#define SET_CCFG_BL_CONFIG_BL_ENABLE                    0xC5       // Enabled boot loader backdoor
//#define SET_CCFG_BL_CONFIG_BL_ENABLE                    0xFF       // Disabled boot loader backdoor
#endif

//#####################################
// Alternative IEEE 802.15.4 MAC address
//#####################################
#ifndef SET_CCFG_IEEE_MAC_0
#define SET_CCFG_IEEE_MAC_0                             0xFFFFFFFF // Bits [31:0]
#endif

#ifndef SET_CCFG_IEEE_MAC_1
#define SET_CCFG_IEEE_MAC_1                             0xFFFFFFFF // Bits [63:32]
#endif

//#####################################
// Alternative BLE address
//#####################################
#ifndef SET_CCFG_IEEE_BLE_0
#define SET_CCFG_IEEE_BLE_0                             0xFFFFFFFF // Bits [31:0]
#endif

#ifndef SET_CCFG_IEEE_BLE_1
#define SET_CCFG_IEEE_BLE_1                             0xFFFFFFFF // Bits [63:32]
#endif


#include <startup_files/ccfg.c>
