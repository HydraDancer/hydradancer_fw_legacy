

#include "usb20.h"

#include "bbio.h"

/* variables */

/* _descriptorsStore is our "free store", it is a memory pool dedicated to
 * descriptors the user will load.
 * In the future we will have a dedicated memory allocator to manage this free
 * store.
 * This free store is always hidden to the user, hence the underscore as a
 * prefix.
 */
static uint8_t _descriptorsStore[4096];

USB_DEV_DESCR  *g_descriptorDevice;
USB_CFG_DESCR  *g_descriptorsConfiguration[10];
USB_ITF_DESCR  *g_descriptorsInterface[10];
USB_ENDP_DESCR *g_descriptorsEndpoint[10];


/* functions implementation */




