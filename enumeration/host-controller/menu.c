#include <stdio.h>

#include "menu.h"


/* functions implementation */

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
    printf("1)  Set verbosity ON\n");
    printf("2)  Set verbosity OFF (default)\n");
    printf("3)  Auto mode\n");
    printf("4)  Enumerate Audio\n");
    printf("5)  Enumerate CDC\n");
    printf("6)  Enumerate HID\n");
    printf("7)  Enumerate Image\n");
    printf("8)  Enumerate Printer\n");
    printf("9)  Enumerate Mass Storage\n");
    printf("10) Enumerate Smart Card\n");
    printf("11) Enumerate Personal Healthcare\n");
    printf("12) Enumerate Video\n");
    printf("13) Enumerate DFU\n");
    printf("14) Enumerate FTDI\n");
    printf("98) Print logs\n");
    printf("99) Disconnect Current Device\n");
    printf("\n");
    printf("0) Exit\n");
    printf("> ");
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

