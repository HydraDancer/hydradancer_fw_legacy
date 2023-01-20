#include "serdes.h"

/* variables */
uint32_t serdesCustomNumber = 0x05555555;
__attribute__((aligned(16))) uint8_t serdesDmaAddr[4096] __attribute__((section(".DMADATA"))); // Buffer for SerDes

/* functions implementation */

/* @fn      serdes_wait_for_tx
 *
 * @brief   Wait the amount of time required to ensure the transmission is
 *          completed and that we can safely send the next one.
 *
 * @warning This function assumes the transfer speed is 1.2Gbps.
 *
 * @return  Nothing.
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
    // (sizeTransmission*20) / 1200 = delay in us.
    // Additionally we add a margin of 20us.
    bsp_wait_us_delay((sizeTransmission*20)/1200 + 20);
}

