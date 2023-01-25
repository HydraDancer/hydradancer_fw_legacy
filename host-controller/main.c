#include <libusb-1.0/libusb.h>

#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
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
int usb_init_verbose(void);
void usb_close(void);
void menu_print(void);
int menu_get_input(void);
void usb_log_print(unsigned char *buffer, int capBuffer);
void usb_bulk_rot13(unsigned char *buffer, int capBuffer);


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
 * @fn      menu_print
 *
 * @brief   Print the main selection menu
 *
 * @return  None
 */
void
menu_print(void)
{
    printf("HydraDancer host controller\n");
    printf("Select your action:\n");
    printf("1)Log once\n");
    printf("2)Log infinite loop\n");
    printf("3)ROT13\n");
    printf("\n");
    printf("9)Exit\n");
    printf(">");
}
/*******************************************************************************
 * @fn      menu_get_input
 *
 * @brief   Getthe user input and returns it
 *
 * @return  A number corresponding to the user input
 */
int
menu_get_input(void)
{
    int userChoice = 0;
    scanf("%d", &userChoice);

    while (!getchar());

    return userChoice;
}

/*******************************************************************************
 * @fn      usb_log_print
 *
 * @brief   Query the endpoint dedicated to log and print received log
 *
 * @return  None
 */
void
usb_log_print(unsigned char *buffer, int capBuffer)
{
    int retCode;

    retCode = libusb_bulk_transfer(g_deviceHandle, EP_DEBUG, buffer, capBuffer, NULL, 0);
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

/*******************************************************************************
 * @fn      usb_bulk_rot13
 *
 * @brief   Send the message to cypher to the board and print the received
 *          cyphered message
 *
 * @return  None
 */
void
usb_bulk_rot13(unsigned char *buffer, int capBuffer)
{
    int retCode;

    // Send the message
    retCode = libusb_bulk_transfer(g_deviceHandle, EP1OUT, (unsigned char *)buffer, capBuffer, NULL, 0);
    buffer[capBuffer-1] = 0;
    if (retCode) {
        printf("[ERROR]\tData NOT transmitted successfully: %s\n", libusb_strerror(retCode));
    }

    memset(buffer, 0, capBuffer);

    // Wait for the message to come back
    while (buffer[0] == 0) {
        retCode = libusb_bulk_transfer(g_deviceHandle, EP1IN, buffer, capBuffer, NULL, 0);
        buffer[capBuffer-1] = 0; // Force null terminating the string
        if (retCode) {
            printf("[ERROR]\tData NOT received successfully: %s\n", libusb_strerror(retCode));
        }
    }

    printf("%s\n", buffer);
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
            usb_log_print(buffer, capBuffer);
            usb_log_print(buffer, capBuffer);
            break;
        // - get log infinite loop
        case 2:
            while (1) {
                usb_log_print(buffer, capBuffer);
                usleep(10000);
            }
            break;
        // - send input + read input
        case 3:
            printf("Message to cypher: ");
            fgets(buffer, 512, stdin);
            usb_bulk_rot13(buffer, 512);
            break;
        // - exit
        case 9:
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
