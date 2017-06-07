/******************************************************************************

 @file  CommandsService.h

 @brief This file contains the LoRa Service Interface
        Created on: Feb 24, 2017

 @author: Artur Balanuta

 ******************************************************************************/

#ifndef SERVICES_COMMANDSSERVICE_H_
#define SERVICES_COMMANDSSERVICE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */

//enum commands { sys="sys", mac="mac", radio="radio" };
//enum status { ok="ok", invalid="invalid_param", keys_not_init="keys_not_init",
//    not_implemented="not_implemented"};
//
//enum commands_sys { sleep="sleep", reset="reset", get="get", set="set" };
//enum commands_mac { reset="reset", tx="tx", join="join", save="save",
//    pause="pause", resume="resume", get="get", set="set" };
//enum commands_radio { sleep="sleep", reset="reset" };
//
//
//enum set_mac_commands_mac_set { devaddr="devaddr", deveui="deveui",
//    appeui="appeui", nwkskey="nwkskey", appskey="appskey", appkey="appkey",
//    pwridx="pwridx", dr="dr", adr="adr", bat="bat", retx="retx",
//    linkchk="linkchk", rxdelay1="rxdelay1", ar="ar", rx2="rx2", sync="sync",
//    upctr="upctr", dnctr="dnctr", ch="ch"};

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for the LoRa Service.
 */
extern void LoRaCommands_createTask(void);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SERVICES_COMMANDSSERVICE_H_ */
