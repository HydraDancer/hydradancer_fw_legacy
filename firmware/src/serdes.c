#include <stdarg.h>

#include "serdes.h"

/* variables */
__attribute__((aligned(16))) uint8_t serdesDmaAddr[4096] __attribute__((section(".DMADATA"))); // Buffer for SerDes

/* functions implementation */

/* @fn      serdes_wait_for_tx
 *
 * @brief   Wait the amount of time required to ensure the transmission is
 *          completed and that we can safely send the next one
 *
 * @warning This function assumes the transfer speed is 1.2Gbps
 *
 * @return  Nothing
 */
void
serdes_wait_for_tx(uint16_t sizeTransmission)
{
    // NOTE: A delay is required to ensure SerDes transmission completed and not
    // sending an other one too quickly. Here (2023-01-29)
    // https://github.com/hydrausb3/hydrausb3_fw/blob/main/HydraUSB3_DualBoard_SerDes/User/Main.c#L360
    // we can see the following line :
    // @code
    // bsp_wait_us_delay(100); /* Wait 100us (about 80us to transmit
    // 2x*4096bytes @1.2Gbps) */
    // @endcode
    // Thus we get the following formula for our transmission delay (assuming 2x
    // refers to e_sds_pll_freq):
    // (sizeTransmission*20) / 1200 = delay in us
    // Additionally we add a margin of 20us
    bsp_wait_us_delay((sizeTransmission*20)/1200 + 20);
}

/* @fn      serdes_log
 *
 * @brief   Function used to log data to the top board via SerDes
 *
 * @return  None
 */
void
serdes_log(const char *fmt, ...)
{
    // Critical section, if we print something (outside of an interrrupt) and an
    // interrupt is called and do a print, then the first print is partially
    // overwritten
    va_list ap;
    // bsp_disable_interrupt();
    va_start(ap, fmt);
    SerDes_DMA_Tx_CFG((uint32_t)serdesDmaAddr, SERDES_DMA_LEN, SerdesMagicNumberLog);

    vsnprintf((char *)serdesDmaAddr, SERDES_DMA_LEN, fmt, ap);

    SerDes_DMA_Tx();
    SerDes_Wait_Txdone();
    // serdes_wait_for_tx(SDS_PLL_FREQ_1_20G); // TODO: Check if can be removed
    // bsp_enable_interrupt();
}

/* @fn      serdes_vlog
 *
 * @brief   Function used to log data to the top board via SerDes, takes a
 *          va_list as the second argument
 *
 * @return  None
 */
void
serdes_vlog(const char *fmt, va_list vargs)
{
    // Critical section, if we print something (outside of an interrrupt) and an
    // interrupt is called and do a print, then the first print is partially
    // overwritten
    // bsp_disable_interrupt();
    SerDes_DMA_Tx_CFG((uint32_t)serdesDmaAddr, SERDES_DMA_LEN, SerdesMagicNumberLog);

    vsnprintf((char *)serdesDmaAddr, SERDES_DMA_LEN, fmt, vargs);

    SerDes_DMA_Tx();
    SerDes_Wait_Txdone();
    // serdes_wait_for_tx(SDS_PLL_FREQ_1_20G); // TODO: Check if can be removed
    // bsp_enable_interrupt();
}

