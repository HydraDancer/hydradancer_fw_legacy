#include <stddef.h>

#include "usb_descriptors.h"

/*******************************************************************************
 * DEVICE GENERIC
 */
unsigned char _genericDescriptorDevice[] = {
    18,     // bLength
    1,      // bDescriptorType
    0x00,   // bcdUSB (low)
    0x02,   // bcdUSB (high)
    0x00,   // bDeviceClass (Defined in the interface descriptor)
    0x00,   // bDeviceSubClass
    0x00,   // bDeviceProtocol
    64,     // bMaxPacketSize0
    0x34,   // idVendor (low)
    0x12,   // idVendor (high)
    0xCD,   // idProduct (low)
    0xAB,   // idProduct (high)
    0x00,   // bcdDevice (low)
    0x42,   // bcdDevice (high)
    0x00,   // iManufacturer
    0x00,   // iProduct
    0x00,   // iSerialNumber
    1,      // bNumConfigurations
};

unsigned char _genericDescriptorConfig[] = {
    //  Descriptor Config
	0x09, // bLength
	0x02, // bDescriptorType
	0x19, // wTotalLengthL
	0x00, // wTotalLengthH
	0x01, // bNumInterfaces
	0x01, // bConfigurationValue
	0x00, // iConfiguration
	0x80, // bmAttributes
	0x64, // MaxPower
    //  Descriptor Interace
	0x09, // bLength
	0x04, // bDescriptorType
	0x00, // bInterfaceNumber
	0x00, // bAlternateSetting
	0x01, // bNumEndpoint
	0x00, // bInterfaceClass
	0x00, // bInterfaceSubClass
	0x00, // bInterfaceProtocol
	0x00, // iInterface
    //  Descriptor Endpoint
	0x07, // bLength
	0x05, // bDescriptorType
	0x01, // bEndpointAddress (OUT)
	0x02, // bmAttributes
	0x00, // wMaxPacketSizeL
	0x02, // wMaxPacketSizeH
	0x00, // bInterval
};

struct Device_t g_deviceGeneric = { "Generic", _genericDescriptorDevice, _genericDescriptorConfig, NULL };

/*******************************************************************************
 * GENERIC DEVICES CONFIGURATIONS
 */
struct DeviceConfig_t g_audioDeviceConfig = { "Audio", 0x02, 0x00, 0x00 };


/*******************************************************************************
 * DEVICE KEYBOARD
 */
unsigned char _keyboardDescriptorDevice[] = {
    18,     // bLength
    1,      // bDescriptorType
    0x00,   // bcdUSB (low)
    0x02,   // bcdUSB (high)
    0x00,   // bDeviceClass (Defined in the interface descriptor)
    0x00,   // bDeviceSubClass
    0x00,   // bDeviceProtocol
    64,     // bMaxPacketSize0
    0x34,   // idVendor (low)
    0x12,   // idVendor (high)
    0xCD,   // idProduct (low)
    0xAB,   // idProduct (high)
    0x00,   // bcdDevice (low)
    0x42,   // bcdDevice (high)
    0x00,   // iManufacturer
    0x00,   // iProduct
    0x00,   // iSerialNumber
    1,      // bNumConfigurations
};

unsigned char _keyboardDescriptorConfig[] = {
    //  Descriptor Config
	0x09, // bLength
	0x02, // bDescriptorType
	0x22, // wTotalLengthL
	0x00, // wTotalLengthH
	0x01, // bNumInterfaces
	0x01, // bConfigurationValue
	0x00, // iConfiguration
	0x80, // bmAttributes
	0x64, // MaxPower
    //  Descriptor Interace
	0x09, // bLength
	0x04, // bDescriptorType
	0x00, // bInterfaceNumber
	0x00, // bAlternateSetting
	0x01, // bNumEndpoint
	0x03, // bInterfaceClass
	0x00, // bInterfaceSubClass
	0x01, // bInterfaceProtocol
	0x00, // iInterface
    //  Descriptor HID
    0x09, // bLength
    0x21, // bDescriptorType
    0x11, // bcdHIDL = 0x11,
    0x01, // bcdHIDH = 0x01,
    0x08, // bCountryCode
    0x01, // bNumDescriptors
    0x22, // bDescriptorTypeX
    0x17, // wDescriptorLengthL
    0x00, // wDescriptorLengthH
    //  Descriptor Endpoint
	0x07, // bLength
	0x05, // bDescriptorType
	0x81, // bEndpointAddress (IN)
	0x03, // bmAttributes
	0x04, // wMaxPacketSizeL
	0x00, // wMaxPacketSizeH
	0x01, // bInterval
};

unsigned char _keyboardDescriptorHidReport[] = {
	0x05, 0x01,		// Usage Page (Generic Desktop)
	0x09, 0x06,		// Usage (Keyboard)
	0xA1, 0x01,		// Collection (Application)
	0x95, 0x04,		// Report Count (4)
	0x75, 0x08,		// Report Size (8)
	0x15, 0x00,		// Logical Minimum (0)
	0x25, 0x65,		// Logical Maximum(101)
	0x05, 0x07,		// Usage Page (Key Codes)
	0x19, 0x00,		// Usage Minimum (0)
	0x29, 0x65,		// Usage Maximum (101)
	0x81, 0x00,		// Input (Data, Array)   ; Key arrays (4 bytes)
	0xC0,		    // End Collection
};

struct Device_t g_deviceKeyboard = { "Keyboard", _keyboardDescriptorDevice, _keyboardDescriptorConfig, _keyboardDescriptorHidReport };


