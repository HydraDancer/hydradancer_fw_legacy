#ifndef BBIO_H
#define BBIO_H

/* macros */


/* enums */
enum BbioCommand {
    BbioSetDescrDevice      = 0b00010000,   // Only the device descriptor does not have multiple possible "values"
    BbioSetDescrConfig      = 0b00100000,   // The 4 lower bytes are dedicated to the index of the given descriptor
    BbioSetDescrInterface   = 0b00110000,   // The 4 lower bytes are dedicated to the index of the given descriptor
    BbioSetDescrEndpoint    = 0b01000000,   // The 4 lower bytes are dedicated to the index of the given descriptor
};

/* variables */


/* functions declaration */


#endif /* BBIO_H */

