#ifndef USB_H
#define USB_H


/* macros */
#define ID_VENDOR  0x1337
#define ID_PRODUCT 0x1337

#define TRANSFER_SIZE       64 /* Currently doing USB HS */
#define USB20_EP1_MAX_SIZE  512

#define INTERFACE 1
#define EP1OUT                  0x01
#define EP1IN                   0x81
#define EP_DEBUG_BOARD_TOP      0x86
#define EP_DEBUG_BOARD_BOTTOM   0x87


/* enums */


/* variables */
extern struct libusb_device_handle *g_deviceHandle;


/* functions declaration */


#endif /* USB_H */

