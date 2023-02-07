#ifndef BBIO_H
#define BBIO_H

#include "CH56xSFR.h"
#include "CH56x_common.h"

#include "usb20.h"

/* macros */


/* enums */
enum BbioCommand {
    BbioMainMode      = 0b00000001,
    BbioIdentifMode   = 0b00000010,
    BbioSetDescr      = 0b00000011,
    BbioSetEndp       = 0b00000100,
    BbioConnect       = 0b00000101,
    BbioDisconnect    = 0b00000110,
};

enum BbioSubCommand {
    BbioSubSetDescrDevice      = 0b00000001,
    BbioSubSetDescrConfig      = 0b00000010,
    BbioSubSetDescrInterface   = 0b00000011,
    BbioSubSetDescrEndpoint    = 0b00000100,
    BbioSubSetDescrString      = 0b00000101,
};

/* variables */

/* The following global variables are pointers to structures loaded by the user.
 */
extern uint8_t *g_bbioDescriptorDevice;
extern uint8_t *g_bbioDescriptorConfiguration;
extern uint8_t *g_bbioDescriptorsString[];

extern uint16_t g_bbioDescriptorDeviceSize;
extern uint16_t g_descriptorConfiguratioSize;
extern uint16_t g_bbioDescriptorsStringSizes[];


/* functions declaration */

/*******************************************************************************
 * Function Name  : bbio_command_decode
 * Description    : Store the commands for the next call to bbio_command_handle
 * Input          : Array containing the commands to decode, see bbio specs for
 *                  more details
 * Return         : 0 if success, else an error code
 *******************************************************************************/
uint8_t bbio_command_decode(uint8_t *command);

/*******************************************************************************
 * Function Name  : bbio_command_handle
 * Description    : Treat the command received at the previous call of
 *                  bbio_command_decode
 * Input          : Array containing the datas associated to the previous
 *                  command
 * Return         : 0 if success, else an error code
 *******************************************************************************/
uint8_t bbio_command_handle(uint8_t *bufferData);

/*******************************************************************************
 * Function Name  : bbio_command_set_descr_handle
 * Description    : Set the descriptor requested by the previous command
 * Input          : Array containing the descriptor to set
 * Return         : 0 if success, else an error code
 *******************************************************************************/
uint8_t bbio_command_set_descriptor_handle(uint8_t *bufferData);

/*******************************************************************************
 * Function Name  : bbio_command_set_endpoints_handle
 * Description    : Set the endpoints requested by the previous command
 * Input          : Array containing the endpoints to set
 * Return         : 0 if success, else an error code
 *******************************************************************************/
uint8_t bbio_command_set_endpoints_handle(uint8_t *bufferEndpoints);


#endif /* BBIO_H */

