/******************************************************************************

 @file  loraPingService.h

 @brief This file contains the LoRa Service Interface
        Created on: Jan 26, 2017

 @author: Artur Balanuta

 ******************************************************************************/

#ifndef SERVICES_LORAPINGSERVICE_H_
#define SERVICES_LORAPINGSERVICE_H_

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

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for the LoRa Service.
 */
extern void LoRaPingService_createTask(void);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SERVICES_LORAPINGSERVICE_H_ */
