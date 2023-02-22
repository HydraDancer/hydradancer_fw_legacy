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
    printf("1)  Log once\n");
    printf("2)  Log infinite loop\n");
    printf("3)  Enumerate Audio\n");
    printf("4)  Enumerate CDC\n");
    printf("5)  Enumerate HID\n");
    printf("6)  Enumerate Image\n");
    printf("7)  Enumerate Printer\n");
    printf("8)  Enumerate Mass Storage\n");
    printf("9)  Enumerate Smart Card\n");
    printf("10) Enumerate Personal Healthcare\n");
    printf("11) Enumerate Video\n");
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

