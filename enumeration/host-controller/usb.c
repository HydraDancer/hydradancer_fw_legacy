#include <stdio.h>
#include <stddef.h>
#include "libusb.h"

#include "usb.h"


/* variables */
struct libusb_device_handle *g_deviceHandle = NULL;

/* functions implementation */

/*******************************************************************************
 * @fn      usb_init_verbose
 *
 * @brief   Initialise the USB connection to the board
 *
 * @warning This function output error messages on stdout
 *
 * @return  0 if success, else an integer indicating the stage that failed
 */
int
usb_init_verbose(void)
{
    int retCode;
    retCode = libusb_init(NULL);
    if ( retCode < 0) {
        printf("[ERROR]\tlibusb_init(NULL)");
        return 1;
    }

    g_deviceHandle = libusb_open_device_with_vid_pid(NULL, ID_VENDOR, ID_PRODUCT);
    if (g_deviceHandle == NULL) {
        printf("Error USB device not found\n");
        libusb_exit(NULL);
        return 2;
    }

    libusb_set_auto_detach_kernel_driver(g_deviceHandle, USB_INTERFACE);
    /*
	if(libusb_kernel_driver_active(g_deviceHandle, 0) == 1)
	{
		printf("Kernel Driver Active\n");
		if(libusb_detach_kernel_driver(g_deviceHandle, 0) == 0)
		{
			printf("Kernel Driver Detached!\n");
		}
		else
		{
			printf("libusb_opendev() error could not detach kernel driver!\n");
			return 4;
		}
	}	
	*/
    retCode = libusb_claim_interface(g_deviceHandle, 0);
    if ( retCode < 0) {
        printf("Error claiming interface: %s\n", libusb_error_name(retCode));
        if (g_deviceHandle) {
            libusb_close(g_deviceHandle);
        }

        libusb_exit(NULL);
        return 3;
	}

    return 0;
}

/*******************************************************************************
 * @fn      usb_close
 *
 * @brief   Close the USB connection to the board
 *
 * @return  None
 */
void
usb_close(void)
{
    libusb_release_interface(g_deviceHandle, USB_INTERFACE);
    //libusb_release_interface(g_deviceHandle, 0);
    libusb_close(g_deviceHandle);
    libusb_exit(NULL);
}
