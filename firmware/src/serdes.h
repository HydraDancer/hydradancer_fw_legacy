#ifndef SERDES_H
#define SERDES_H

#include <stdarg.h>
#include <stdint.h>

#include "CH56xSFR.h"
#include "CH56x_common.h"

/* macros */
#define SERDES_DMA_LEN  (512)

/* enums */
enum SerdesMagicNumber {
    SerdesMagicNumberLog     = 0x01111111,
    SerdesMagicNumberRetCode = 0x02222222,
    SerdesMagicNumberMask    = 0x0FFFFFFF,
};

/* variables */
extern uint8_t serdesDmaAddr[]; // Buffer for SerDes

/* functions declaration */

/*******************************************************************************
 * Function Name  : serdes_wait_for_tx
 * Description    : Wait the amount of time required to ensure the transmission
 *                  is completed and that we can safely send the next one
 *                  More details can be found in the implementation
 * Input          : Size of the buffer transmitted
 * Return         : None
 *******************************************************************************/
void serdes_wait_for_tx(uint16_t sizeTransmission);

/*******************************************************************************
 * Function Name  : serdes_log
 * Description    : Function used to log data to the top board via SerDes
 * Input          : Variadic function, same arguments as you would give to
 *                  printf()
 * Return         : None
 *******************************************************************************/
void serdes_log(const char *fmt, ...);

/*******************************************************************************
 * Function Name  : serdes_vlog
 * Description    : Function used to log data to the top board via SerDes
 * Input          : A format strind and the associated va_list
 * Return         : None
 *******************************************************************************/
void serdes_vlog(const char *fmt, va_list vargs);



#endif /* SERDES_H */
