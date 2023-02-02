#ifndef BBIO_H
#define BBIO_H

#include "CH56xSFR.h"
#include "CH56x_common.h"

#include "usb20.h"

/* macros */


/* enums */
enum BbioCommand {
    BbioMainMode      = 0b00000000,
    BbioIdentifMode   = 0b00000001,
    BbioSetDescr      = 0b00000010,
    BbioSetEndp       = 0b00000011,
    BbioConnect       = 0b00000100,
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
void bbio_command_decode(uint8_t *command);


// Return 0 if success, 1 else
uint8_t bbio_command_handle(uint8_t *bufferData);

void bbio_sub_command_handle(uint8_t *bufferData);

// This function will enable endpoints with the right mode
// (isochronous/bulk/interrupt)
// The data is encoded in 1 byte :
// 0b00yy Xxxx
// yy correspond to the mode :
// 01: isochronous
// 10: bulk
// 11: interrupt
//
// Xxxx correspond to the endpoint number
// X: 0 for OUT, 1 for IN
// xxx: the endpoint number (from 1 to 7)
void bbio_command_endpoints_handle(uint8_t *bufferEndpoints);

#endif /* BBIO_H */

