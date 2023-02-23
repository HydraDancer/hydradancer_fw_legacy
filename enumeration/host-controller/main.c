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
enumerate_device(struct Device_t device, bool verbose)
{
    int retCode;
    int bbioRetCode;
    bool isDeviceSupported = false;

    char dummyPacket[] = "toto";
    int dummyPacketSize = sizeof(dummyPacket);

    unsigned char *descriptorDevice = device.descriptorDevice;
    unsigned char *descriptorConfig = device.descriptorConfig;
    unsigned char *descriptorHidReport = device.descriptorHidReport;
    int sz_descriptorDevice = descriptorDevice[0];
    int sz_descriptorConfig = (descriptorConfig[3] << 8) + descriptorConfig[2];         // From 2 char to short
    int sz_descriptorHidReport = (descriptorConfig[26] << 8) + descriptorConfig[25];    // From 2 char to short

    // Reset the board
    do {
        if (verbose) { printf("Resetting board\n"); }
        bbio_command_send(BbioDisconnect);
        bbioRetCode = bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode |= bbio_get_return_code();
    } while (bbioRetCode);

    // Reset descriptors
    do {
        if (verbose) { printf("Resetting descriptors\n"); }
        bbio_command_send(BbioResetDescr);
        bbioRetCode = bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode |= bbio_get_return_code();
    } while (bbioRetCode);

    // Send device descriptor
    do {
        if (verbose) { printf("Setting device descriptor\n"); }
        bbio_command_sub_send(BbioSetDescr, BbioSubSetDescrDevice, 0, sz_descriptorDevice);
        bbioRetCode = bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, descriptorDevice, sz_descriptorDevice, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode |= bbio_get_return_code();
    } while (bbioRetCode);

    // Send configuration descriptor
    do {
        if (verbose) { printf("Setting configuration descriptor\n"); }
        bbio_command_sub_send(BbioSetDescr, BbioSubSetDescrConfig, 0, sz_descriptorConfig);
        bbioRetCode = bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, descriptorConfig, sz_descriptorConfig, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode |= bbio_get_return_code();
    } while (bbioRetCode);

    // if it exists
    if (descriptorHidReport) {
        // Send HID report descriptor
        do {
            if (verbose) { printf("Setting configuration descriptor\n"); }
            bbio_command_sub_send(BbioSetDescr, BbioSubSetDescrHidReport, 0, sz_descriptorHidReport);
            bbioRetCode = bbio_get_return_code();
            retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, descriptorHidReport, sz_descriptorHidReport, NULL, 0);
            if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
            bbioRetCode |= bbio_get_return_code();
        } while (bbioRetCode);
    }

    // No necessity to enable endpoints according to the descriptor

    // Connect the device
    do {
        if (verbose) { printf("Connecting the device\n"); }
        bbio_command_send(BbioConnect);
        bbioRetCode = bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode |= bbio_get_return_code();
    } while (bbioRetCode);

    // Wait to see if our device is supported
    if (verbose) { printf("Querying results...\n"); }
    for (int i = 0; i < TIMEOUT; ++i) {
        bbio_command_send(BbioGetStatus);
        bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode = bbio_get_return_code();
        if (bbioRetCode == 1) {
            isDeviceSupported = true;
            break;
        }
        usleep(100000);
    }

    // Reset the board
    do {
        if (verbose) { printf("Resetting board\n"); }
        bbio_command_send(BbioDisconnect);
        bbioRetCode = bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode |= bbio_get_return_code();
    } while (bbioRetCode);

    // Reset descriptors
    do {
        if (verbose) { printf("Resetting descriptors\n"); }
        bbio_command_send(BbioResetDescr);
        bbioRetCode = bbio_get_return_code();
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, (void *)dummyPacket, dummyPacketSize, NULL, 0);
        if (retCode) { printf("[ERROR]\t usb_descriptor_set(): bulk transfer failed"); }
        bbioRetCode |= bbio_get_return_code();
    } while (bbioRetCode);

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
        // - Enumerate Audio
        case 3:
            enumerate_device(g_deviceAudio, true);
            break;
        // - Enumerate CDC
        case 4:
            enumerate_device(g_deviceCdc, true);
            break;
        // - Enumerate Keyboard
        case 5:
            enumerate_device(g_deviceKeyboard, true);
            break;
        // - Enumerate Image
        case 6:
            enumerate_device(g_deviceImage, true);
            break;
        // - Enumerate Image
        case 7:
            enumerate_device(g_devicePrinter, true);
            break;
        // - Enumerate Mass Storage
        case 8:
            enumerate_device(g_deviceMassStorage, true);
            break;
        // - Enumerate Smart Card
        case 9:
            enumerate_device(g_deviceSmartCard, true);
            break;
        // - Enumerate Personal Healthcare
        case 10:
            enumerate_device(g_devicePersonalHealthcare, true);
            break;
        // - Enumerate Video
        case 11:
            enumerate_device(g_deviceVideo, true);
            break;
        // - Enumerate DFU
        case 12:
            enumerate_device(g_deviceDFU, true);
            break;
        // - Enumerate FTDI
        case 13:
            enumerate_device(g_deviceFTDI, true);
            break;
        // - Disconnect Current Device 
        case 99:
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
