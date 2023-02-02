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
extern uint8_t *g_descriptorDevice;
extern uint8_t *g_descriptorConfiguration;
extern uint8_t *g_descriptorsString[];

extern uint16_t g_descriptorDeviceSize;
extern uint16_t g_descriptorConfiguratioSize;
extern uint16_t g_descriptorsStringSizes[];


/* functions declaration */


#endif /* BBIO_H */

