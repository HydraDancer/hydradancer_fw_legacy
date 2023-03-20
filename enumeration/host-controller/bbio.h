#ifndef BBIO_H
#define BBIO_H


/* enums */
enum BbioCommand {
    BbioMainMode      = 0x01, // 0b00000001
    BbioIdentifMode   = 0x02, // 0b00000010
    BbioSetDescr      = 0x03, // 0b00000011
    BbioSetEndp       = 0x04, // 0b00000100
    BbioConnect       = 0x05, // 0b00000101
    BbioGetStatus     = 0x06, // 0b00000110
    BbioDisconnect    = 0x07, // 0b00000111
    BbioResetDescr    = 0x08, // 0b00001000
};

enum BbioSubCommand {
    BbioSubSetDescrDevice      = 0x01, // 0b00000001
    BbioSubSetDescrConfig      = 0x02, // 0b00000010
    BbioSubSetDescrInterface   = 0x03, // 0b00000011
    BbioSubSetDescrHidReport   = 0x04, // 0b00000100
    BbioSubSetDescrHubReport   = 0x05, // 0b00000101
    BbioSubSetDescrEndpoint    = 0x06, // 0b00000110
    BbioSubSetDescrString      = 0x07, // 0b00000111
};

/* variables */


/* functions declaration */

/*******************************************************************************
 * Function Name  : bbio_command_send
 * Description    : Send the given BBIO command to the ToE board
 * Input          : The BBIO command to send
 * Return         : None
 *******************************************************************************/
void bbio_command_send(enum BbioCommand bbioCommand);

/*******************************************************************************
 * Function Name  : bbio_command_sub_send
 * Description    : Send the given BBIO command, sub command, index and size to
 *                  the ToE board
 * Input          : - bbioCommand: The BBIO command to send
 *                  - bbioSubCommand: The BBIO sub command to send
 *                  - indexDescriptor: The index of the descriptor that will be
 *                    sent
 *                  - sizeDescriptor: The size of descriptor that will be sent
 * Note           : Curently only the command BbioSetDescr require a sub command
 *                  and underlying fields
 * Return         : None
 *******************************************************************************/
void bbio_command_sub_send(enum BbioCommand bbioCommand, enum BbioSubCommand bbioSubCommand, int indexDescriptor, int sizeDescriptor);

/*******************************************************************************
 * Function Name  : bbio_get_return_code
 * Description    : Query the ToE board for the return code of the previous
 *                  request
 * Input          : None
 * Return         : None
 * Return         : Depending on the previous request :
 *                  - BbioGetStatus: 1 if device is support by ToE USB's stack,
 *                    0 else
 *                  - Everything else: The return code from the associated
 *                    function in firmware/src/bbio.c
 *******************************************************************************/
unsigned char bbio_get_return_code(void);


#endif /* BBIO_H */

