#include <libusb-1.0/libusb.h>

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bbio.h"
#include "menu.h"
#include "usb_descriptors.h"
#include "usb.h"


/* macros */


/* variables */


/* functions declaration */
void handler_sigint(int sig);
int usb_init_verbose(void);
void usb_close(void);
void usb_log_print(char endpoint, unsigned char *buffer, int capBuffer);


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
 * @fn      usb_log_print
 *
 * @brief   Query the given endpoint for logs and print received logs
 *
 * @return  None
 */
void
usb_log_print(char endpoint, unsigned char *buffer, int capBuffer)
{
    int retCode;

    retCode = libusb_bulk_transfer(g_deviceHandle, endpoint, buffer, capBuffer, NULL, 0);
    buffer[capBuffer-1] = 0; // Force null terminating the string
    if (retCode == 0) {
        // If we received something
        if (buffer[0] != 0) {
            printf("%s", buffer);
        }
    } else {
        printf("[ERROR]\tData NOT received successfully: %s\n", libusb_strerror(retCode));
    }
    memset(buffer, 0, capBuffer);
}



void
usb_descriptor_set(enum BbioSubCommand bbioSubCommand, int indexDescriptor, unsigned char *descriptor, int sizeDescriptor)
{
    int retCode;

    // Send BBIO command
    bbio_command_sub_send(BbioSetDescr, bbioSubCommand, indexDescriptor, sizeDescriptor);
    bbio_get_return_code();

    // Send descriptor
    retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, descriptor, sizeDescriptor, NULL, 0);
    if (retCode) {
        printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed");
    }
    bbio_get_return_code();
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
    bool exit = false;
    int retCode;
    int userChoice;

    unsigned char buffer[4096];
    const int capBuffer = 4096;

    char dummyPacket[] = "toto";
    int dummyPacketSize = sizeof(dummyPacket);

    signal(SIGINT, handler_sigint);

    usb_init_verbose();


    while (!exit) {
        // Print menu
        menu_print();
        userChoice = menu_get_input();
        
        // Handle selected action :
        switch (userChoice) {
        // - get log once
        case 1:
            // TODOO: Fix bug where the first IN bulk transfer is empty (even
            // when there is data to transmit)
            memset(buffer, 0, capBuffer);
            printf("Top Board:\n");
            usb_log_print(EP_DEBUG_BOARD_TOP, buffer, capBuffer);
            usb_log_print(EP_DEBUG_BOARD_TOP, buffer, capBuffer);
            printf("Bottom Board:\n");
            memset(buffer, 0, capBuffer);
            usb_log_print(EP_DEBUG_BOARD_BOTTOM, buffer, capBuffer);
            usb_log_print(EP_DEBUG_BOARD_BOTTOM, buffer, capBuffer);
            break;
        // - get log infinite loop
        case 2:
            while (1) {
                usb_log_print(EP_DEBUG_BOARD_TOP, buffer, capBuffer);
                usb_log_print(EP_DEBUG_BOARD_BOTTOM, buffer, capBuffer);
                usleep(100000);
            }
            break;
        // - Send Device Descriptor
        case 3:
            // Fill Device Descriptor of the ToE board
            usb_descriptor_set(BbioSubSetDescrDevice, 0, g_descriptorDevice, sizeof(g_descriptorDevice));
            break;
        // - Send Config Descriptor
        case 4:
            // Fill Config Descriptor of the ToE board
            usb_descriptor_set(BbioSubSetDescrConfig, 0, g_descriptorConfig, sizeof(g_descriptorConfig));
            break;
        // - Set endpoints
        case 5:
            // Connect to the target
            bbio_command_send(BbioSetEndp);
            bbio_get_return_code();

            // endpoint 1 Out
            char buffEndpoints[] = {0x01, 0x00};
            libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)buffEndpoints, 2, NULL, 0);
            bbio_get_return_code();
            break;
        // - Connect to the target
        case 6:
            // Connect to the target
            bbio_command_send(BbioConnect);
            bbio_get_return_code();

            // We need to send a packet to trigger the second step, no matter
            // the content of the packet
            libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
            bbio_get_return_code();
            break;
        // - Get status
        case 7:
            // Is the current device supported by the ToE ?
            bbio_command_send(BbioGetStatus);
            bbio_get_return_code();

            // We need to send a packet to trigger the second step, no matter
            // the content of the packet
            libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
            retCode = bbio_get_return_code();
            if (retCode) {
                printf("Device is supported!\n");
            }
            break;
        // - Disconnect
        case 8:
            // Disconnect to the target
            bbio_command_send(BbioDisconnect);
            bbio_get_return_code();

            // We need to send a packet to trigger the second step, no matter
            // the content of the packet
            libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
            bbio_get_return_code();
            break;
        // - Reset descriptors
        case 9:
            // Reset descriptors on TOE board
            bbio_command_send(BbioResetDescr);
            bbio_get_return_code();

            // We need to send a packet to trigger the second step, no matter
            // the content of the packet
            libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
            bbio_get_return_code();
            break;
        // - exit
        case 0:
            exit = true;
            break;
        default:
            break;
        }
    }

    // Unused
    // Print like a grid of printable and non printable char
    // for (int i = 0; i < capBuffer; ++i) {
    //     if (isprint(buffer[i])) {
    //         putchar(buffer[i]);
    //     } else {
    //         putchar('.');
    //     }
    //     if ((i%8) == 7) {
    //         putchar('\n');
    //     }
    // }


    usb_close();

    return 0;
}
