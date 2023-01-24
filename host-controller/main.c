#include <libusb-1.0/libusb.h>

#include <ctype.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/* macros */
#define ID_VENDOR  0x1337
#define ID_PRODUCT 0x1337

#define TRANSFER_SIZE 64 /* Currently doing USB HS */

#define INTERFACE 1
#define EP1OUT      0x01
#define EP1IN       0x81
#define EP_DEBUG    0x87

/* variables */
struct libusb_device_handle *g_deviceHandle = NULL;


/* functions declaration */
void handler_sigint(int sig);
uint8_t usb_init_verbose(void);
void usb_close(void);


/* functions implementation */

/*******************************************************************************
 * @fn      handler_sigint
 *
 * @brief   Function that will be used to cleanly exit when receiving C-c
 *
 * @return  None
 */
void
handler_sigint(int sig)
{
    usb_close();

    printf("Exiting\n");
    exit(0);
}

/*******************************************************************************
 * @fn      usb_init_verbose
 *
 * @brief   Initialise the USB connection to the board
 *
 * @warning This function output error messages on stdout
 *
 * @return  0 if success, else an integer indicating the stage that failed
 */
uint8_t
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
        printf("Error finding USB device\n");
        libusb_exit(NULL);
        return 2;
    }

    libusb_set_auto_detach_kernel_driver(g_deviceHandle, INTERFACE);

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
    libusb_release_interface(g_deviceHandle, INTERFACE);
    libusb_close(g_deviceHandle);
    libusb_exit(NULL);
}

/*******************************************************************************
 * @fn      usb_bulk_transfer_verbose
 *
 * @brief   Wrapper for libusb_bulk_transfer
 *
 * @warning This function output error messages on stdout
 *
 * @return  0 if success, else an integer indicating the stage that failed
 */
uint8_t
usb_bulk_transfer_verbose(void)
{
    // business logic

    return 0;
}


/*******************************************************************************
 * @fn      main
 *
 * @brief   main
 *
 * @return  None
 */
int
main(int argc, char *argv[])
{
    int retCode;
    uint8_t ping[] = "Hello, World!\r\n";
    uint8_t buffer[4096];
    const uint16_t capBuffer = 4096;
    uint16_t sizeBuffer = 0;

    signal(SIGINT, handler_sigint);

    usb_init_verbose();


    sizeBuffer = strnlen(ping, 1023) + 1; /* Do not forget the null terminator. */
    memcpy(buffer, ping, sizeBuffer);
    retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, (unsigned char *)buffer, sizeBuffer, NULL, 0);
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
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1IN, (unsigned char *)buffer, capBuffer, NULL, 0);
        buffer[capBuffer-1] = 0;
        if (retCode == 0) {
            printf("%s", buffer);
        } else {
            printf("[ERROR]\tData NOT received successfully: %s\n", libusb_strerror(retCode));
        }
        memset(buffer, 0, capBuffer);
        usleep(10000);

        retCode = libusb_bulk_transfer(g_deviceHandle, EP_DEBUG, (unsigned char *)buffer, capBuffer, NULL, 0);
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

    usb_close();

    return 0;
}
