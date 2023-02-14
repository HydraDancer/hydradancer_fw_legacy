#ifndef BBIO_H
#define BBIO_H


/* enums */
enum BbioCommand {
    BbioMainMode      = 0b00000001,
    BbioIdentifMode   = 0b00000010,
    BbioSetDescr      = 0b00000011,
    BbioSetEndp       = 0b00000100,
    BbioConnect       = 0b00000101,
    BbioGetStatus     = 0b00000110,
    BbioDisconnect    = 0b00000111,
    BbioResetDescr    = 0b00001000,
};

enum BbioSubCommand {
    BbioSubSetDescrDevice      = 0b00000001,
    BbioSubSetDescrConfig      = 0b00000010,
    BbioSubSetDescrInterface   = 0b00000011,
    BbioSubSetDescrEndpoint    = 0b00000100,
    BbioSubSetDescrString      = 0b00000101,
};

/* variables */


/* functions declaration */


#endif /* BBIO_H */

