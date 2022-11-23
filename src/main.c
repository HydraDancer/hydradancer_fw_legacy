#include "CH56x_common.h"
// #include "CH56x_debug_log.h"

#include "CH56x_usb30_util.h"
// #include "CH56x_usb30_devbulk.h"
// #include "CH56x_usb20_devbulk.h"
// #include "CH56x_usb30_devbulk_LIB.h"
// #include "CH56x_usb_devbulk_desc_cmd.h"

// #include "usb_cmd.h"

// debug_log_buf_t log_buf;




/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int
main(void)
{
    // usb_descriptor_serial_number_t serial_number = { .sn_64b = 0xefbeaddeefbeadde };
    // usb_descriptor_usb_vid_pid_t st_vid_pid = {
    //     .vid = 0x1337,
    //     .pid = 0x1337,
    // };

    
    bsp_init(FREQ_SYS);
    // log_init(&log_buf);
      
    /* Code copied from USB#_force(void) (CH56x_usb30_devbulk.c) */
	// USB2 & USB3 Init
	// USB2 & USB3 are managed in LINK_IRQHandler/TMR0_IRQHandler/USBHS_IRQHandler
	R32_USB_CONTROL = 0;
	PFIC_EnableIRQ(USBSS_IRQn);
	PFIC_EnableIRQ(LINK_IRQn);

	PFIC_EnableIRQ(TMR0_IRQn);
	R8_TMR0_INTER_EN = RB_TMR_IE_CYC_END;
	TMR0_TimerInit(67000000); // USB3.0 connection failure timeout about 0.56 seconds

    // usb_descriptor_set_string_serial_number(&serial_number);
    // usb_descriptor_set_usb_vid_pid(&st_vid_pid);

	USB30D_init_ep0only(ENABLE); // USB3.0 initialization, make sure that the two USB3.0 interrupts are enabled before initialization

    while (1) { }
}
