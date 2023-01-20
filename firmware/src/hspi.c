#include "CH56xSFR.h"
#include "CH56x_common.h"

#include "hspi.h"

/* variables */
__attribute__((aligned(16))) uint8_t hspiDmaAddr0[4096] __attribute__((section(".DMADATA"))); // Buffer 0 for HSPI
__attribute__((aligned(16))) uint8_t hspiDmaAddr1[4096] __attribute__((section(".DMADATA"))); // Buffer 1 for HSPI

/* functions implementation */

/* @fn      hspi_wait_for_tx
 *
 * @brief   Wait the amount of time required to ensure the transmission is
 *          completed and that we can safely send the next one.
 *
 * @warning This function assumes the transfer speed is 1.2Gbps.
 *
 * @return  Nothing.
 */
void
hspi_wait_for_tx(uint16_t sizeTransmission)
{
    // Same as serdes_wait_for_tx(), we need to wait a bit, sending 2
    // transmission "back to back" would make the second transaction not being
    // received. Here it is an arbitrary number.
    bsp_wait_us_delay(5);
}


/* @fn      hspi_get_rtx_status
 *
 * @brief   Get the status of the transmission/reception of the HSPI Transaction.
 *
 * @return  Return 0b0010 if CRC_ERR, 0b0100 if NUM_MIS, 0 else.
 */
uint8_t
hspi_get_rtx_status(void)
{
    return R8_HSPI_RTX_STATUS & (RB_HSPI_CRC_ERR | RB_HSPI_NUM_MIS);
}

/* @fn      hspi_get_buffer_next_tx
 *
 * @brief   Get the buffer that will be used for the next transmission over
 *          HSPI.
 *
 * @return  Return the buffer that will be used for the next transmission over
 *          HSPI.
 */
uint8_t *
hspi_get_buffer_next_tx(void)
{
    uint8_t *bufferTx = hspiDmaAddr0;
    if (R8_HSPI_TX_SC & RB_HSPI_TX_TOG) {
        bufferTx = hspiDmaAddr1;
    }

    return bufferTx;
}

/* @fn      hspi_get_buffer_tx
 *
 * @brief   Get the buffer that was used for the previous transmission over
 *          HSPI.
 *
 * @return  Return the buffer that was used for the previous transmission over
 *          HSPI. */
uint8_t *
hspi_get_buffer_tx(void)
{
    // R8_HSPI_TX_SC stores the buffer that will be used for the next
    // transmission, thus we need to inverse the buffers.
    uint8_t *bufferTx = hspiDmaAddr1;
    if (R8_HSPI_TX_SC & RB_HSPI_TX_TOG) {
        bufferTx = hspiDmaAddr0;
    }

    return bufferTx;
}

/* @fn      hspi_get_buffer_next_rx
 *
 * @brief   Get the buffer that will be used for the next reception over HSPI.
 *
 * @return  Return the buffer that will be used for the next reception over
 *          HSPI.
 */
uint8_t *
hspi_get_buffer_next_rx(void)
{
    uint8_t *bufferRx = hspiDmaAddr0;
    if (R8_HSPI_RX_SC & RB_HSPI_RX_TOG) {
        bufferRx = hspiDmaAddr1;
    }

    return bufferRx;
}

/* @fn      hspi_get_buffer_rx
 *
 * @brief   Get the buffer that was used for the previous reception over HSPI.
 *
 * @return  Return the buffer that was used for the previous reception over
 *          HSPI. */
uint8_t *
hspi_get_buffer_rx(void)
{
    // R8_HSPI_RX_SC stores the buffer that will be used for the next
    // reception, thus we need to inverse the buffers.
    uint8_t *bufferRx = hspiDmaAddr1;
    if (R8_HSPI_RX_SC & RB_HSPI_RX_TOG) {
        bufferRx = hspiDmaAddr0;
    }

    return bufferRx;
}

