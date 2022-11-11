#include "CH56x_common.h"




/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */

int main(void)
{
    bsp_gpio_init();
    bsp_init(FREQ_SYS);
    bsp_uled_on();
    while (1) { }
}
