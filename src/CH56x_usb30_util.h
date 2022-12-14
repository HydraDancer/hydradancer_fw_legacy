
#ifndef CH56X_USB30_UTIL_H
#define CH56X_USB30_UTIL_H

#include "CH56x_common.h"



__attribute__((aligned(16))) uint8_t endp0RTbuff[512] __attribute__((section(".DMADATA"))); // Endpoint 0 data transceiver buffer
__attribute__((aligned(16))) uint8_t endp1Rbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 0 data transceiver buffer
__attribute__((aligned(16))) uint8_t endp1Tbuff[4096] __attribute__((section(".DMADATA"))); // Endpoint 0 data transceiver buffer


#ifndef USB_CFG_DESCR_KEYBOARD
typedef struct __PACKED _USB_CONFIG_DESCR_KEYBOARD
{
	USB_CFG_DESCR  cfg_descr;
	USB_ITF_DESCR  itf_descr;
	USB_HID_DESCR  hid_descr;
	USB_ENDP_DESCR endp_descr;
} USB_CFG_DESCR_KEYBOARD, *PUSB_CFG_DESCR_KEYBOARD;
#endif


/*********************/
typedef union
{
	uint16_t w;
	struct BW
	{
		uint8_t bb1; // low byte
		uint8_t bb0;
	}
	bw;
} UINT16_UINT8;
/**********standard request command***********/
typedef struct __PACKED
{
	uint8_t       bRequestType;
	uint8_t       bRequest;
	UINT16_UINT8 wValue;
	UINT16_UINT8 wIndex;
	uint16_t       wLength;
} *PUSB_SETUP;

#define UsbSetupBuf ((PUSB_SETUP)endp0RTbuff)// endpoint 0
#define ENDP0_MAXPACK 512

// status response
#define NRDY    0
#define ACK     0x01
#define STALL   0x02
#define INVALID 0x03





#endif /* CH56X_USB30_UTIL_H */

