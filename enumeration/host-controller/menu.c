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
    printf("3) Send descriptor device\n");
    printf("4) Send descriptor configuration\n");
    printf("5) Set endpoints\n");
    printf("6) Connect\n");
    printf("7) Get status\n");
    printf("8) Disconnect\n");
    printf("9) Reset descriptors\n");
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

