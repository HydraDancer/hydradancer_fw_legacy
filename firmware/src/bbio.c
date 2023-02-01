

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

USB_DEV_DESCR *g_descriptorDevice;
USB_CFG_DESCR *g_descriptorConfiguration;
uint8_t *g_descriptorsString[10];




/* functions implementation */
bbio_decode_command(uint8_t *bufferCommand)
{
    if (bufferCommand[0] == BbioMainMode) {
        // Handle command BbioMainMode
    } else if (bufferCommand[0] == BbioIdentifMode) {
        // Handle command BbioMainMode
    } else if (bufferCommand[0] == BbioSetDescr) {
        // Handle command BbioMainMode
    } else {
        log_to_evaluator("ERROR: bbio_decode_command() unknown command\r\n");
    }
}



