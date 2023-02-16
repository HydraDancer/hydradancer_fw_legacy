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
#define TIMEOUT 1000

/* enums */


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

// TODOO: Use struct rather than multiple arguments tied together
// TODO: Header doc
bool
enumerate_device(struct Device_t device)
{
    int retCode;
    int bbioRetCode;
    bool isDeviceSupported = false;

    char dummyPacket[] = "toto";
    int dummyPacketSize = sizeof(dummyPacket);

    unsigned char *descriptorDevice = device.descriptorDevice;
    unsigned char *descriptorConfig = device.descriptorConfig;
    int sz_descriptorDevice = descriptorDevice[0];
    int sz_descriptorConfig = (descriptorConfig[3] << 8) + descriptorConfig[2]; // From 2 char to short

    // Reset the board
    do {
        printf("Resetting board\n");
        bbio_command_send(BbioDisconnect);
        bbioRetCode = bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode |= bbio_get_return_code();
    } while (bbioRetCode);

    // Reset descriptors
    do {
        printf("Resetting descriptors\n");
        bbio_command_send(BbioResetDescr);
        bbioRetCode = bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode |= bbio_get_return_code();
    } while (bbioRetCode);

    // Send device descriptor
    do {
        printf("Setting device descriptor\n");
        bbio_command_sub_send(BbioSetDescr, BbioSubSetDescrDevice, 0, sz_descriptorDevice);
        bbioRetCode = bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, descriptorDevice, sz_descriptorDevice, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode |= bbio_get_return_code();
    } while (bbioRetCode);

    // Send configuration descriptor
    do {
        printf("Setting configuration descriptor\n");
        bbio_command_sub_send(BbioSetDescr, BbioSubSetDescrConfig, 0, sz_descriptorConfig);
        bbioRetCode = bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, descriptorConfig, sz_descriptorConfig, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode |= bbio_get_return_code();
    } while (bbioRetCode);

    // No necessity to enable endpoints according to the descriptor

    // Connect the device
    do {
        printf("Connecting the device\n");
        bbio_command_send(BbioConnect);
        bbioRetCode = bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode |= bbio_get_return_code();
    } while (bbioRetCode);

    // Wait to see if our device is supported
    for (int i = 0; i < TIMEOUT; ++i) {
        bbio_command_send(BbioGetStatus);
        bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode = bbio_get_return_code();
        // printf("%d, 0x%X\n", i, bbioRetCode);    // TODO: Remove, used for debug only
        if (bbioRetCode == 1) {
            isDeviceSupported = true;
            break;
        }
        usleep(10000);
    }

    // Print the result
    if (isDeviceSupported) {
        printf("Class 0x%02X 0x%02X (%s) is supported\n", descriptorDevice[4], descriptorConfig[14], device.s_name);
        return true;
    } else {
        printf("Class 0x%02X 0x%02X (%s) is NOT supported\n", descriptorDevice[4], descriptorConfig[14], device.s_name);
        return false;
    }
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
    int bbioRetCode;
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

            // Send BBIO command with sub command and underlying fields
            bbio_command_sub_send(BbioSetDescr, BbioSubSetDescrDevice, 0, sizeof(_vendorDescriptorDevice));
            bbio_get_return_code();

            // Send descriptor
            retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, _vendorDescriptorDevice, sizeof(_vendorDescriptorDevice), NULL, 0);
            if (retCode) {
                printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed");
            }
            bbio_get_return_code();
            break;
        // - Send Config Descriptor
        case 4:
            // Fill Config Descriptor of the ToE board

            // Send BBIO command with sub command and underlying fields
            bbio_command_sub_send(BbioSetDescr, BbioSubSetDescrConfig, 0, sizeof(_vendorDescriptorConfig));
            bbio_get_return_code();

            // Send descriptor
            retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, _vendorDescriptorConfig, sizeof(_vendorDescriptorConfig), NULL, 0);
            if (retCode) {
                printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed");
            }
            bbio_get_return_code();
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
        // Auto mode
        case 10:
            enumerate_device(g_deviceVendor);
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
