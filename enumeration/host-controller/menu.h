#ifndef MENU_H
#define MENU_H


/* functions declaration */

/*******************************************************************************
 * Function Name  : menu_print
 * Description    : Print the main selection menu
 * Input          : None
 * Return         : None
 *******************************************************************************/
void menu_print(void);

/*******************************************************************************
 * Function Name  : menu_get_input
 * Description    : Gather the user input (from stdin) and returns it
 * Input          : None
 * Return         : An integer correspond to the user selection
 *******************************************************************************/
int menu_get_input(void);


#endif /* MENU_H */

