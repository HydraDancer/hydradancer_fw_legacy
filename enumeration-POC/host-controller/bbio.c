#include <libusb-1.0/libusb.h>

#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include "usb.h"

#include "bbio.h"

/* functions implementation */

/*******************************************************************************
 * @fn      bbio_command_send
 *
 * @brief   Send the given BBIO command to the ToE board
 *
 * @return  None
 */
void
bbio_command_send(enum BbioCommand bbioCommand)
{
    int retCode;
    unsigned char bbioBuffer[1];

    bbioBuffer[0] = bbioCommand;

    retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, bbioBuffer, 1, NULL, 0);
    if (retCode) {
        printf("[ERROR]\t bbio_command_send(): bulk transfer failed");
    }
}

/*******************************************************************************
 * @fn      bbio_command_sub_send
 *
 * @brief   Send the given BBIO command, sub command, index and size to the ToE
 *          board
 *
 * @return  None
 */
void
bbio_command_sub_send(enum BbioCommand bbioCommand, enum BbioSubCommand bbioSubCommand, int indexDescriptor, int sizeDescriptor)
{
    /* Safeguards */
    assert(indexDescriptor <= 16 && "bbio_command_send() index > 16");
    assert(sizeDescriptor <= UINT16_MAX && "Desciptor size > UINT16_MAX\n");
    assert(sizeDescriptor <= USB20_EP1_MAX_SIZE && "bbio_command_sub_send(): Descriptor is too big for the buffer\n");
    int retCode;
    unsigned char bbioBuffer[5];

    bbioBuffer[0] = bbioCommand;
    bbioBuffer[1] = bbioSubCommand;
    bbioBuffer[2] = indexDescriptor;
    bbioBuffer[3] = sizeDescriptor % 256;   // Lower byte
    bbioBuffer[4] = sizeDescriptor / 256;   // Higher Byte

    retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, bbioBuffer, 5, NULL, 0);
    if (retCode) {
        printf("[ERROR]\t bbio_command_sub_send(): bulk transfer failed");
    }
}


/*******************************************************************************
 * @fn      bbio_get_return_code
 *
 * @brief   Query the ToE board for the return code of the previous request
 *
 * @return  Depending on the previous request :
 *          - BbioGetStatus: 1 if device is support by ToE USB's stack, 0 else
 *          - Everything else: The return code from the associated function in
 *            firmware/src/bbio.c
 */
unsigned char
bbio_get_return_code(void)
{
    int retCode;
    unsigned char bbioRetCode;

    retCode = libusb_bulk_transfer(g_deviceHandle, EP1IN, &bbioRetCode, 1, NULL, 0);
    if (retCode) {
        printf("[ERROR]\t bbio_command_sub_send(): bulk transfer failed");
    }

    return bbioRetCode;
}


