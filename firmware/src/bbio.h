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
 *
 */
extern USB_DEV_DESCR  *g_descriptorDevice;
extern USB_CFG_DESCR  *g_descriptorsConfiguration[];
extern USB_ITF_DESCR  *g_descriptorsInterface[];
extern USB_ENDP_DESCR *g_descriptorsEndpoint[];


/* functions declaration */


#endif /* BBIO_H */

