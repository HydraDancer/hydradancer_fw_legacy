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
    printf("1) Log once\n");
    printf("2) Log infinite loop\n");
    printf("3) Enumerate Audio\n");
    printf("4) Enumerate HID\n");
    printf("5) NOT WORKING!  Enumerate CDC\n");
    printf("6) NOT WORKING!  Enumerate physical\n");
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

