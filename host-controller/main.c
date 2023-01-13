#include <libusb-1.0/libusb.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


// TODOO: Capture C-c signal for clean exit.


#define ID_VENDOR  0x1337
#define ID_PRODUCT 0x1337

#define TRANSFER_SIZE 8 /* Currently doing USB LS. */

#define INTERFACE 1

#define EP1OUT 0x01
#define EP1IN  0x81




int
main(int argc, char *argv[])
{
    struct libusb_device_handle *device_handle = NULL;
    int retCode;
    //char ping[] = "ping!";
    char ping[] = "debug";
    char buffer[1024];
    int szBuffer = 0;


    /* Prolog. */
    retCode = libusb_init(NULL);
    if ( retCode < 0) {
        printf("[ERROR]\tlibusb(NULL)");
        return -1;
    }

    device_handle = libusb_open_device_with_vid_pid(NULL, ID_VENDOR, ID_PRODUCT);
    if (device_handle == NULL) {
        printf("Error finding USB device\n");
        libusb_exit(NULL);
        return -2;
    }

    libusb_set_auto_detach_kernel_driver(device_handle, INTERFACE);

    retCode = libusb_claim_interface(device_handle, 0);
    if ( retCode < 0) {
        printf("Error claiming interface: %s\n", libusb_error_name(retCode));
        if (device_handle)
        {
            libusb_close(device_handle);
        }

        libusb_exit(NULL);
        return -3;
	}
    /* end of prolog. */

    szBuffer = strnlen(ping, 1023) + 1; /* Do not forget the null terminator. */
    memcpy(buffer, ping, szBuffer);
    retCode = libusb_bulk_transfer(device_handle, EP1OUT, (unsigned char *)buffer, szBuffer, NULL, 0);
    if (retCode == 0) {
        printf("[INFO]\tData send successfully\n");
    } else {
        printf("[ERROR]\tData NOT send successfully\n");
    }

    memset(buffer, 0x30, 1023);

    // printf("Sleeping a bit before receiving data...\n");
    // usleep(100000);
    // printf("Waking up!\n");

    retCode = libusb_bulk_transfer(device_handle, EP1IN, (unsigned char *)buffer, 1024, NULL, 0);
    buffer[1023] = 0;
    if (retCode == 0) {
        printf("[INFO]\tData received successfully: %s\n", buffer);
    } else {
        printf("[ERROR]\tData NOT received successfully: %s\n", libusb_strerror(retCode));
    }

    for (int i = 0; i < 128; ++i) {
        if (isprint(buffer[i])) {
            putchar(buffer[i]);
        } else {
            putchar('.');
        }

        if ((i%8) == 7) {
            putchar('\n');
        }
    }

    /* Epilog. */
    libusb_release_interface(device_handle, INTERFACE);
    libusb_close(device_handle);
    libusb_exit(NULL);

    return 0;
}
