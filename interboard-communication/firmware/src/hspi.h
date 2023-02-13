#ifndef HSPI_H
#define HSPI_H

#include <stdint.h>

#include "CH56xSFR.h"
#include "CH56x_common.h"

/* macros */
#define HSPI_DMA_LEN    (512)
#define HSPI_DMA_LEN0   HSPI_DMA_LEN
#define HSPI_DMA_LEN1   HSPI_DMA_LEN

/* variables */
extern uint8_t hspiDmaAddr0[]; // Buffer 0 for HSPI
extern uint8_t hspiDmaAddr1[]; // Buffer 1 for HSPI

/* functions declaration */

/*******************************************************************************
 * Function Name  : hspi_wait_for_tx
 * Description    : Wait the amount of time required to ensure the transmission
 *                  is completed and that we can safely send the next one
 *                  More details can be found in the implementation
 * Input          : Size of the buffer transmitted
 * Return         : None
 *******************************************************************************/
void hspi_wait_for_tx(uint16_t sizeTransmission);

/*******************************************************************************
 * Function Name  : hspi_get_rtx_status
 * Description    : Get the status of the transmission/reception of the
 *                  transaction
 * Input          : None
 * Return         : 0b0010 if CRC_ERR, 0b0100 if NUM_MIS, 0 else.
 *******************************************************************************/
uint8_t hspi_get_rtx_status(void);

/*******************************************************************************
 * Function Name  : hspi_get_buffer_next_tx
 * Description    : Get the buffer that will be used for the next transmission
 * Input          : None
 * Return         : The buffer that will be used for the next transmission
 *******************************************************************************/
uint8_t *hspi_get_buffer_next_tx(void);

/*******************************************************************************
 * Function Name  : hspi_get_buffer_tx
 * Description    : Get the buffer that was used for the previous transmission
 * Input          : None
 * Return         : The buffer that was used for the previous transmission
 *******************************************************************************/
uint8_t *hspi_get_buffer_tx(void);

/*******************************************************************************
 * Function Name  : hspi_get_buffer_next_rx
 * Description    : Get the buffer that will be used for the next reception
 * Input          : None
 * Return         : The buffer that will be used for the next reception
 *******************************************************************************/
uint8_t *hspi_get_buffer_next_rx(void);

/*******************************************************************************
 * Function Name  : hspi_get_buffer_rx
 * Description    : Get the buffer that was used for the previous reception
 * Input          : None
 * Return         : The buffer that was used for the previous reception
 *******************************************************************************/
uint8_t *hspi_get_buffer_rx(void);

#endif /* HSPI_H */

