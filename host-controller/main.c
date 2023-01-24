#include <libusb-1.0/libusb.h>
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


// TODOO: Capture C-c signal for clean exit.


#define ID_VENDOR  0x1337
#define ID_PRODUCT 0x1337

#define TRANSFER_SIZE 64 /* Currently doing USB LS. */

#define INTERFACE 1

#define EP1OUT 0x01
#define EP1IN  0x81
#define EP_DEBUG_IN  0x87


struct libusb_device_handle *gDeviceHandle = NULL;


void
handler_sigint(int sig)
{
    libusb_release_interface(gDeviceHandle, INTERFACE);
    libusb_close(gDeviceHandle);
    libusb_exit(NULL);

    printf("Exiting\n");
    exit(0);
}


int
main(int argc, char *argv[])
{
    int retCode;
    //char ping[] = "ping!";
    char ping[] = "Hello, World!\r\n";
    char buffer[4096];
    const int capBuffer = 4096;
    int szBuffer = 0;

    signal(SIGINT, handler_sigint);

    /* Prolog. */
    retCode = libusb_init(NULL);
    if ( retCode < 0) {
        printf("[ERROR]\tlibusb(NULL)");
        return -1;
    }

    gDeviceHandle = libusb_open_device_with_vid_pid(NULL, ID_VENDOR, ID_PRODUCT);
    if (gDeviceHandle == NULL) {
        printf("Error finding USB device\n");
        libusb_exit(NULL);
        return -2;
    }

    libusb_set_auto_detach_kernel_driver(gDeviceHandle, INTERFACE);

    retCode = libusb_claim_interface(gDeviceHandle, 0);
    if ( retCode < 0) {
        printf("Error claiming interface: %s\n", libusb_error_name(retCode));
        if (gDeviceHandle)
        {
            libusb_close(gDeviceHandle);
        }

        libusb_exit(NULL);
        return -3;
	}
    /* end of prolog. */

    szBuffer = strnlen(ping, 1023) + 1; /* Do not forget the null terminator. */
    memcpy(buffer, ping, szBuffer);
    retCode = libusb_bulk_transfer(gDeviceHandle, EP1OUT, (unsigned char *)buffer, szBuffer, NULL, 0);
    if (retCode == 0) {
        printf("[INFO]\tData send successfully\n");
    } else {
        printf("[ERROR]\tData NOT send successfully\n");
    }

    memset(buffer, 0, capBuffer-1);

    // printf("Sleeping a bit before receiving data...\n");
    // usleep(100000);
    // printf("Waking up!\n");

    while (1) {
        retCode = libusb_bulk_transfer(gDeviceHandle, EP1IN, (unsigned char *)buffer, capBuffer, NULL, 0);
        buffer[capBuffer-1] = 0;
        if (retCode == 0) {
            printf("%s", buffer);
        } else {
            printf("[ERROR]\tData NOT received successfully: %s\n", libusb_strerror(retCode));
        }
        memset(buffer, 0, capBuffer);
        usleep(10000);

        retCode = libusb_bulk_transfer(gDeviceHandle, EP_DEBUG_IN, (unsigned char *)buffer, capBuffer, NULL, 0);
        buffer[capBuffer-1] = 0;
        if (retCode == 0) {
            // If we received something
            if (buffer[0] != 0) {
                printf("[DEBUG]\t%s", buffer);
            }
        } else {
            printf("[ERROR]\tData NOT received successfully: %s\n", libusb_strerror(retCode));
        }
        memset(buffer, 0, capBuffer);
        usleep(10000);
    }

    for (int i = 0; i < capBuffer; ++i) {
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
    libusb_release_interface(gDeviceHandle, INTERFACE);
    libusb_close(gDeviceHandle);
    libusb_exit(NULL);

    return 0;
}
