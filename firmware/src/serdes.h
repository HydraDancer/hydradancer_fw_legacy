#ifndef SERDES_H
#define SERDES_H

#include <stdint.h>

#include "CH56xSFR.h"
#include "CH56x_common.h"

/* macros */
#define SERDES_DMA_LEN  (512)

/* variables */
extern uint32_t serdesCustomNumber;
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

#endif /* SERDES_H */
