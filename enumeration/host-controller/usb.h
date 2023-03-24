#ifndef USB_H
#define USB_H


/* macros */
#define ID_VENDOR  0x1337
#define ID_PRODUCT 0x1337

#define TRANSFER_SIZE       64 /* Currently doing USB HS */
#define USB20_EP1_MAX_SIZE  512

#define USB_INTERFACE 1
#define EP1OUT                  0x01
#define EP1IN                   0x81
#define EP_DEBUG_BOARD_TOP      0x86
#define EP_DEBUG_BOARD_BOTTOM   0x87


/* enums */


/* variables */
extern struct libusb_device_handle *g_deviceHandle;


/* functions declaration */


/*******************************************************************************
 * Function Name  : usb_init_verbose
 * Description    : Initialise the USB connection to the board
 * Input          : None
 * Return         : 0 if success, else an integer indicating the stage that failed
 *******************************************************************************/
int usb_init_verbose(void);

/*******************************************************************************
 * Function Name  : usb_close
 * Description    : Close the USB connection to the board
 * Input          : None
 * Return         : None
 *******************************************************************************/
void usb_close(void);


#endif /* USB_H */

