
#ifndef CH56X_USB30_UTIL_H
#define CH56X_USB30_UTIL_H

#include "CH56x_common.h"



__attribute__((aligned(16))) uint8_t endp0RTbuff[512] __attribute__((section(".DMADATA"))); // Endpoint 0 data transceiver buffer




// link CFG
#define TERM_EN    (1<<1)
#define PIPE_RESET (1<<3)
#define LFPS_RX_PD (1<<5)
#define CFG_EQ_EN  (1<<6)
#define DEEMPH_CFG (1<<8)

#define POWER_MODE_0 ((uint32_t)0x00000000)
#define POWER_MODE_1 ((uint32_t)0x00000001)
#define POWER_MODE_2 ((uint32_t)0x00000002)
#define POWER_MODE_3 ((uint32_t)0x00000003)

#define LINK_PRESENT  (1<<0)
#define RX_WARM_RESET ((uint32_t)1<<1)

#define LINK_TXEQ   (1<<6)
#define GO_DISABLED (1<<4)
#define POLLING_EN  (1<<12)

#define TX_HOT_RESET ((uint32_t)1<<16)
#define RX_HOT_RESET ((uint32_t)1<<24)

#define TX_WARM_RESET ((uint32_t)1<<8)
#define TX_Ux_EXIT    ((uint32_t)1<<9)
// link int flag
#define LINK_RDY_FLAG       (1<<0)
#define LINK_RECOV_FLAG     (1<<1)
#define LINK_INACT_FLAG     (1<<2)
#define LINK_DISABLE_FLAG   (1<<3)
#define LINK_GO_U3_FLAG     (1<<4)
#define LINK_GO_U2_FLAG     (1<<5)
#define LINK_GO_U1_FLAG     (1<<6)
#define LINK_GO_U0_FLAG     (1<<7)
#define LINK_U3_WAKE_FLAG   (1<<8)
#define LINK_Ux_REJECT_FLAG (1<<9)
#define TERM_PRESENT_FLAG   (1<<10)
#define LINK_TXEQ_FLAG      (1<<11)
#define LINK_Ux_EXIT_FLAG   (1<<12)
#define WARM_RESET_FLAG     (1<<13)
#define U3_WAKEUP_FLAG      (1<<14)
#define HOT_RESET_FLAG      (1<<15)
#define LINK_RX_DET_FLAG    (1<<20)

#define EP0_R_EN (1<<0)
#define EP1_R_EN (1<<1)
#define EP2_R_EN (1<<2)
#define EP3_R_EN (1<<3)
#define EP4_R_EN (1<<4)
#define EP5_R_EN (1<<5)
#define EP6_R_EN (1<<6)
#define EP7_R_EN (1<<7)

#define EP0_T_EN (1<<8)
#define EP1_T_EN (1<<9)
#define EP2_T_EN (1<<10)
#define EP3_T_EN (1<<11)
#define EP4_T_EN (1<<12)
#define EP5_T_EN (1<<13)
#define EP6_T_EN (1<<14)
#define EP7_T_EN (1<<15)

#define USB_FORCE_RST (1<<2)
#define USB_ALL_CLR   (1<<1)
// LMP
#define LMP_HP            0
#define LMP_SUBTYPE_MASK (0xf<<5)
#define SET_LINK_FUNC    (0x1<<5)
#define U2_INACT_TOUT    (0x2<<5)
#define VENDOR_TEST      (0x3<<5)
#define PORT_CAP         (0x4<<5)
#define PORT_CFG         (0x5<<5)
#define PORT_CFG_RES     (0x6<<5)

#define LINK_SPEED       (1<<9)

#define NUM_HP_BUF       (4<<0)
#define DOWN_STREAM      (1<<16)
#define UP_STREAM        (2<<16)
#define TIE_BRK          (1<<20)



#define SS_RX_CONTRL(ep) (&USBSS->UEP0_RX_CTRL)[ep*4]
#define SS_TX_CONTRL(ep) (&USBSS->UEP0_TX_CTRL)[ep*4]



/*******************************************************************************
 * @fn     USB30_device_init
 *
 * @brief  USB3.0 device initialization helper function for USB30D_init
 *         Stolent from CH56x_usb30_devbulk_LIB.h
 *
 * @return None
 */
static int
USB30_device_init(void)
{
	USBSS->LINK_CFG = 0x140;
	USBSS->LINK_CTRL = 0x12;
	uint32_t t = 0x4c4b41;
	while(USBSS->LINK_STATUS&4)
	{
		t--;
		if(t == 0)
			return -1;
	}
	for(int i = 0; i < 8; i++)
	{
		SS_TX_CONTRL(i) = 0;
		SS_RX_CONTRL(i) = 0;
	}
	USBSS->USB_STATUS = 0x13;

	USBSS->USB_CONTROL = 0x30021;
	USBSS->UEP_CFG = 0;

	USBSS->LINK_CFG |= 2;

	USBSS->LINK_INT_CTRL = 0x10bc7d;

	USBSS->LINK_CTRL = 2;
	return 0;
}



/*******************************************************************************
 * @fn     USB30D_init
 *
 * @brief  USB3.0 device initialization
 *         Stolent from CH56x_usb30_devbulk_LIB.h
 *
 * @return None
 */
void USB30D_init_ep0only(FunctionalState sta)
{
	int s;
	if(sta)
	{
		// Enable USB
		s = USB30_device_init();
		if(s)
		{
			// cprintf("USB30_device_init err\n");
			while(1);
		}
		USBSS->UEP_CFG = EP0_R_EN | EP0_T_EN; // set end point rx/tx enable

		USBSS->UEP0_DMA = (uint32_t)(uint8_t *)endp0RTbuff;
	}
	else
	{
		// Disable USB
		USB30_switch_powermode(POWER_MODE_2);
		USBSS->LINK_CFG = PIPE_RESET | LFPS_RX_PD;
		USBSS->LINK_CTRL = GO_DISABLED | POWER_MODE_3;
		USBSS->LINK_INT_CTRL = 0;
		USBSS->USB_CONTROL = USB_FORCE_RST | USB_ALL_CLR;
	}
}

#endif /* CH56X_USB30_UTIL_H */

