#include <stddef.h>

#include "usb_descriptors.h"


/* macros */
/* USB descriptor type */
#define DEV_DESCR_DEVICE    0x01
#define DEV_DESCR_CONFIG    0x02
#define DEV_DESCR_STRING    0x03
#define DEV_DESCR_INTERF    0x04
#define DEV_DESCR_ENDP      0x05
#define DEV_DESCR_QUALIF    0x06
#define DEV_DESCR_SPEED     0x07
#define DEV_DESCR_OTG       0x09
#define DEV_DESCR_IF_ASSOC  0x0B
#define DEV_DESCR_HID       0x21
#define DEV_DESCR_REPORT    0x22
#define DEV_DESCR_PHYSIC    0x23
#define DEV_DESCR_CS_INTF   0x24
#define DEV_DESCR_CS_ENDP   0x25
#define DEV_DESCR_HUB       0x29

/* USB device class */
#define DEV_CLASS_RESERVED      0x00
#define DEV_CLASS_AUDIO         0x01
#define DEV_CLASS_CDC_CTRL      0x02
#define DEV_CLASS_HID           0x03
#define DEV_CLASS_PHYSIC_IF     0x05
#define DEV_CLASS_IMAGE         0x06
#define DEV_CLASS_PRINTER       0x07
#define DEV_CLASS_STORAGE       0x08
#define DEV_CLASS_HUB           0x09
#define DEV_CLASS_CDC_DATA      0x0A
#define DEV_CLASS_SMART_CARD    0x0B
#define DEV_CLASS_VIDEO         0x0E
#define DEV_CLASS_PHDC          0x0F
#define DEV_CLASS_APP_SPEC      0xFE
#define DEV_CLASS_VEN_SPEC      0xFF


/* variables */

/*******************************************************************************
 * DEVICE GENERIC
 */
unsigned char _genericDescriptorDevice[] = {
    0x12,   // bLength
    DEV_DESCR_DEVICE,   // bDescriptorType
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
    0x01,   // bNumConfigurations
};

unsigned char _genericDescriptorConfig[] = {
    //  Descriptor Config
	0x09, // bLength
	DEV_DESCR_CONFIG, // bDescriptorType
	0x19, // wTotalLengthL
	0x00, // wTotalLengthH
	0x01, // bNumInterfaces
	0x01, // bConfigurationValue
	0x00, // iConfiguration
	0x80, // bmAttributes
	0x64, // MaxPower
    //  Descriptor Interface
	0x09, // bLength
	DEV_DESCR_INTERF, // bDescriptorType
	0x00, // bInterfaceNumber
	0x00, // bAlternateSetting
	0x01, // bNumEndpoint
	0x00, // bInterfaceClass
	0x00, // bInterfaceSubClass
	0x00, // bInterfaceProtocol
	0x00, // iInterface
    //  Descriptor Endpoint
	0x07, // bLength
	DEV_DESCR_ENDP, // bDescriptorType
	0x01, // bEndpointAddress (OUT)
	0x02, // bmAttributes
	0x00, // wMaxPacketSizeL
	0x02, // wMaxPacketSizeH
	0x00, // bInterval
};

struct Device_t g_deviceGeneric = { "Generic", _genericDescriptorDevice, _genericDescriptorConfig, NULL, NULL };


/*******************************************************************************
 * DEVICE AUDIO
 */

// Based on https://www.usb.org/sites/default/files/audio10.pdf
unsigned char _audioDescriptorDevice[] = {
    // Table B-1: USB Microphone Device Descriptor
    0x12, // bLength                Size of this descriptor, in bytes.
    DEV_DESCR_DEVICE, // bDescriptorType        DEVICE descriptor.
    0x00, // bcdUSBL                1.00 - current revision of USB specification.
    0x02, // bcdUSBH                1.00 - current revision of USB specification.
    0x00, // bDeviceClass           Device defined at Interface level.
    0x00, // bDeviceSubClass        Unused.
    0x00, // bDeviceProtocol        Unused.
    0x40, // bMaxPacketSize0        8 bytes.
    0x34, // idVendorL              Vendor ID.
    0x12, // idVendorH              Vendor ID.
    0xCD, // idProductL             Product ID.
    0xAB, // idProductH             Product ID.
    0x00, // bcdDeviceL             Device Release Code.
    0x42, // bcdDeviceH             Device Release Code.
    0x00, // iManufacturer          Index to string descriptor that contains the string <Your Name> in Unicode.
    0x00, // iProduct               Index to string descriptor that contains the string <Your Product Name> in Unicode.
    0x00, // iSerialNumber          Unused.
    0x01, // bNumConfigurations     One configuration.
};

unsigned char _audioDescriptorConfig[] = {
    // Table B-2: USB Microphone Configuration Descriptor
    0x09, //  bLength                 Size of this descriptor, in bytes.
    DEV_DESCR_CONFIG, //  bDescriptorType         CONFIGURATION descriptor.
    0x64, //  wTotalLengthL           Length of the total configuration block, including this descriptor, in bytes.
    0x00, //  wTotalLengthH           Length of the total configuration block, including this descriptor, in bytes.
    0x02, //  bNumInterfaces          Two interfaces.
    0x01, //  bConfigurationValue     ID of this configuration.
    0x00, //  iConfiguration          Unused.
    0x80, //  bmAttributes            Bus Powered device, not Self Powered, no Remote wakeup capability.
    0x0A, //  MaxPower                20 mA Max. power consumption.

    // Table B-3: USB Microphone Standard AC Interface Descriptor
    0x09, //  bLength             Size of this descriptor, in bytes.
    DEV_DESCR_INTERF, //  bDescriptorType     INTERFACE descriptor.
    0x00, //  bInterfaceNumber    Index of this interface.
    0x00, //  bAlternateSetting   Index of this setting.
    0x00, //  bNumEndpoints       0 endpoints.
    DEV_CLASS_AUDIO, //  bInterfaceClass     AUDIO.
    0x01, //  bInterfaceSubclass  AUDIO_CONTROL.
    0x00, //  bInterfaceProtocol  Unused.
    0x00, //  iInterface          Unused.

    // Table B-4: USB Microphone Class-specific AC Interface Descriptor
    0x09, //  bLength             Size of this descriptor, in bytes.
    DEV_DESCR_CS_INTF, //  bDescriptorType     CS_INTERFACE.
    0x01, //  bDescriptorSubtype  HEADER subtype.
    0x00, //  bcdADCL             Revision of class specification - 1.0
    0x01, //  bcdADCH             Revision of class specification - 1.0
    0x1E, //  wTotalLengthL       Total size of class specific descriptors.
    0x00, //  wTotalLengthH       Total size of class specific descriptors.
    0x01, //  bInCollection       Number of streaming interfaces.
    0x01, //  baInterfaceNr(1)    AudioStreaming interface 1 belongs to this AudioControl interface.

    // Table B-5: USB Microphone Input Terminal Descriptor
    0x0C, //  bLength             Size of this descriptor, in bytes.
    DEV_DESCR_CS_INTF, //  bDescriptorType     CS_INTERFACE.
    0x02, //  bDescriptorSubtype  INPUT_TERMINAL subtype.
    0x01, //  bTerminalID         ID of this Input Terminal.
    0x01, //  wTerminalTypeL      Terminal is Microphone.
    0x02, //  wTerminalTypeH      Terminal is Microphone.
    0x00, //  bAssocTerminal      No association.
    0x01, //  bNrChannels         One channel.
    0x00, //  wChannelConfigL     Mono sets no position bits.
    0x00, //  wChannelConfigH     Mono sets no position bits.
    0x00, //  iChannelNames       Unused.
    0x00, //  iTerminal           Unused.

    // Table B-6: USB Microphone Output Terminal Descriptor
    0x09, //  bLength             Size of this descriptor, in bytes.
    DEV_DESCR_CS_INTF, //  bDescriptorType     CS_INTERFACE.
    0x03, //  bDescriptorSubtype  OUTPUT_TERMINAL subtype.
    0x02, //  bTerminalID         ID of this Output Terminal.
    0x01, //  wTerminalTypeL      USB Streaming.
    0x01, //  wTerminalTypeH      USB Streaming.
    0x00, //  bAssocTerminal      Unused.
    0x01, //  bSourceID           From Input Terminal.
    0x00, //  iTerminal           Unused.

    // Table B-7: USB Microphone Standard AS Interface Descriptor (Alt. Set. 0)
    0x09, //  bLength             Size of this descriptor, in bytes.
    DEV_DESCR_INTERF, //  bDescriptorType     INTERFACE descriptor.
    0x01, //  bInterfaceNumber    Index of this interface.
    0x00, //  bAlternateSetting   Index of this alternate setting.
    0x00, //  bNumEndpoints       0 endpoints.
    DEV_CLASS_AUDIO, //  bInterfaceClass     AUDIO.
    0x02, //  bInterfaceSubclass  AUDIO_STREAMING.
    0x00, //  bInterfaceProtocol  Unused.
    0x00, //  iInterface          Unused.

    // Table B-8: USB Microphone Standard AS Interface Descriptor
    0x09, //  bLength             Size of this descriptor, in bytes.
    DEV_DESCR_INTERF, //  bDescriptorType     INTERFACE descriptor.
    0x01, //  bInterfaceNumber    Index of this interface.
    0x01, //  bAlternateSetting   Index of this alternate setting.
    0x01, //  bNumEndpoints       One endpoint.
    DEV_CLASS_AUDIO, //  bInterfaceClass     AUDIO.
    0x02, //  bInterfaceSubclass  AUDIO_STREAMING.
    0x00, //  bInterfaceProtocol  Unused.
    0x00, //  iInterface          Unused.

    // Table B-9: USB Microphone Class-specific AS General Interface Descriptor
    0x07, //  bLength             Size of this descriptor, in bytes.
    DEV_DESCR_CS_INTF, //  bDescriptorType     CS_INTERFACE descriptor.
    0x01, //  bDescriptorSubtype  GENERAL subtype.
    0x02, //  bTerminalLink       Unit ID of the Output Terminal.
    0x01, //  bDelay              Interface delay.
    0x01, //  wFormatTagL         PCM Format.
    0x00, //  wFormatTagH         PCM Format.

    // Table B-10: USB Microphone Type I Format Type Descriptor
    0x0B, //  bLength             Size of this descriptor, in bytes.
    DEV_DESCR_CS_INTF, //  bDescriptorType     CS_INTERFACE descriptor.
    0x02, //  bDescriptorSubtype  FORMAT_TYPE subtype.
    0x01, //  bFormatType         FORMAT_TYPE_I.
    0x01, //  bNrChannels         One channel.
    0x02, //  bSubFrameSize       Two bytes per audio subframe.
    0x10, //  bBitResolution      16 bits per sample.
    0x01, //  bSamFreqType        One frequency supported.
    0x40, //  tSamFreqL           8000Hz.
    0x1F, //  tSamFreqM           8000Hz.
    0x00, //  tSamFreqH           8000Hz.

    // Table B-11: USB Microphone Standard Endpoint Descriptor
    0x09, //  bLength           Size of this descriptor, in bytes.
    DEV_DESCR_ENDP, //  bDescriptorType   ENDPOINT descriptor.
    0x81, //  bEndpointAddress  IN Endpoint 1.
    0x01, //  bmAttributes      Isochronous, not shared.
    0x10, //  wMaxPacketSizeL   16 bytes per packet.
    0x00, //  wMaxPacketSizeH   16 bytes per packet.
    0x01, //  bInterval         One packet per frame.
    0x00, //  bRefresh          Unused.
    0x00, //  bSynchAddress     Unused.

    // Table B-12: USB Microphone Class-specific Isoc. Audio Data Endpoint Descriptor
    0x07, //  bLength             Size of this descriptor, in bytes.
    DEV_DESCR_CS_ENDP, //  bDescriptorType     CS_ENDPOINT descriptor
    0x01, //  bDescriptorSubtype  GENERAL subtype.
    0x00, //  bmAttributes        No sampling frequency control, no pitch control, no packet padding.
    0x00, //  bLockDelayUnits     Unused.
    0x00, //  wLockDelayL         Unused
    0x00, //  wLockDelayH         Unused
};

struct Device_t g_deviceAudio = { "Audio", _audioDescriptorDevice, _audioDescriptorConfig, NULL, NULL };


/* CDC descriptors is based on
 * https://gist.github.com/tai/acd59b125a007ad47767
 */
/*******************************************************************************
 * DEVICE CDC
 */
unsigned char _cdcDescriptorDevice[] = {
    18,     // bLength
    DEV_DESCR_DEVICE,      // bDescriptorType
    0x00,   // bcdUSB (low)
    0x02,   // bcdUSB (high)
    DEV_CLASS_CDC_CTRL,   // bDeviceClass
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

unsigned char _cdcDescriptorConfig[] = {
    //  Descriptor Config
    0x09, // bLength
    DEV_DESCR_CONFIG, // bDescriptorType
    0x43, // wTotalLengthL
    0x00, // wTotalLengthH
    0x02, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0x80, // bmAttributes
    0x64, // MaxPower
    //  Descriptor Interface 0 (Communication Class)
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x01, // bNumEndpoint
    DEV_CLASS_CDC_CTRL, // bInterfaceClass        (Communication Interface Class)
    0x02, // bInterfaceSubClass     (Abstract Control Model)
    0x01, // bInterfaceProtocol     (AT Commands V.250)
    0x00, // iInterface
        //  Descriptors Functional
        //  Descriptor Functional Header
        0x05, // bLength
        DEV_DESCR_CS_INTF, // bDescriptorType
        0x00, // bDescriptorSubtype (Header FD)
        0x10, // bcdCDCL
        0x01, // bcdCDCH
        //  Descriptor Functional Abstract Control Model
        0x04, // bLength
        DEV_DESCR_CS_INTF, // bDescriptorType
        0x02, // bDescriptorSubtype (ACM-FD)
        0x02, // bmCapabilities
        //  Descriptor Functional Union
        0x05, // bLength
        DEV_DESCR_CS_INTF, // bDescriptorType
        0x06, // bDescriptorSubtype (Union FD)
        0x00, // bControlInterface      (I/F# of Communication Class Interface)
        0x01, // bSubordinateInterface0 (I/F# of Data Class Interface)
        //  Descriptor Functional Call Management
        0x05, // bLength
        DEV_DESCR_CS_INTF, // bDescriptorType
        0x01, // bDescriptorSubtype (CM-FD)
        0x00, // bmCapabilities
        0x01, // bDataInterface
        //  Descriptor Endpoint 1
        0x07, // bLength
        DEV_DESCR_ENDP, // bDescriptorType
        0x81, // bEndpointAddress   (ep1 IN)
        0x03, // bmAttributes
        0x10, // wMaxPacketSizeL
        0x00, // wMaxPacketSizeH
        0x02, // bInterval
    //  Descriptor Interface 1 (Data Class)
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x01, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x02, // bNumEndpoint
    DEV_CLASS_CDC_DATA, // bInterfaceClass        (CDC-Data)
    0x00, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x00, // iInterface
        //  Descriptor Endpoint 1
        0x07, // bLength
        DEV_DESCR_ENDP, // bDescriptorType
        0x01, // bEndpointAddress   (ep1 OUT)
        0x02, // bmAttributes
        0x40, // wMaxPacketSizeL
        0x00, // wMaxPacketSizeH
        0x00, // bInterval
        //  Descriptor Endpoint 8
        0x07, // bLength
        DEV_DESCR_ENDP, // bDescriptorType
        0x88, // bEndpointAddress   (ep8 IN)
        0x02, // bmAttributes
        0x00, // wMaxPacketSizeL
        0x02, // wMaxPacketSizeH
        0x00, // bInterval
};

struct Device_t g_deviceCdc = { "CDC (Virtual COM Port)", _cdcDescriptorDevice, _cdcDescriptorConfig, NULL, NULL };


/*******************************************************************************
 * DEVICE KEYBOARD
 */
unsigned char _keyboardDescriptorDevice[] = {
    18,     // bLength
    DEV_DESCR_DEVICE,      // bDescriptorType
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
    DEV_DESCR_CONFIG, // bDescriptorType
    0x22, // wTotalLengthL
    0x00, // wTotalLengthH
    0x01, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0x80, // bmAttributes
    0x64, // MaxPower
    //  Descriptor Interface
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x01, // bNumEndpoint
    DEV_CLASS_HID, // bInterfaceClass
    0x00, // bInterfaceSubClass
    0x01, // bInterfaceProtocol
    0x00, // iInterface
    //  Descriptor HID
    0x09, // bLength
    DEV_DESCR_HID, // bDescriptorType
    0x11, // bcdHIDL = 0x11,
    0x01, // bcdHIDH = 0x01,
    0x08, // bCountryCode
    0x01, // bNumDescriptors
    0x22, // bDescriptorTypeX
    0x17, // wDescriptorLengthL
    0x00, // wDescriptorLengthH
    //  Descriptor Endpoint
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
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

struct Device_t g_deviceKeyboard = { "Keyboard", _keyboardDescriptorDevice, _keyboardDescriptorConfig, _keyboardDescriptorHidReport, NULL };

/* Image descriptor is based on
 * https://www.xmos.ai/download/AN00132:-USB-Image-Device-Class(2.0.2rc1).pdf
 */
/*******************************************************************************
 * DEVICE IMAGE
 */
unsigned char _imageDescriptorDevice[] = {
    18,     // bLength
    DEV_DESCR_DEVICE,      // bDescriptorType
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

unsigned char _imageDescriptorConfig[] = {
    //  Descriptor Config
    0x09, // bLength
    DEV_DESCR_CONFIG, // bDescriptorType
    0x27, // wTotalLengthL
    0x00, // wTotalLengthH
    0x01, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0x80, // bmAttributes
    0x64, // MaxPower
    //  Descriptor Interface
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x03, // bNumEndpoint
    DEV_CLASS_IMAGE, // bInterfaceClass
    0x01, // bInterfaceSubClass     (Still Image Capture Device)
    0x01, // bInterfaceProtocol     (PIMA 15740 compliant)
    0x00, // iInterface
    //  Descriptor Endpoint (1 OUT)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x01, // bEndpointAddress (OUT)
    0x02, // bmAttributes
    0x40, // wMaxPacketSizeL
    0x00, // wMaxPacketSizeH
    0x00, // bInterval
    //  Descriptor Endpoint (1 IN)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x81, // bEndpointAddress (IN)
    0x02, // bmAttributes
    0x40, // wMaxPacketSizeL
    0x00, // wMaxPacketSizeH
    0x00, // bInterval
    //  Descriptor Endpoint (7 OUT)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x07, // bEndpointAddress (OUT)
    0x03, // bmAttributes
    0x40, // wMaxPacketSizeL
    0x00, // wMaxPacketSizeH
    0x01, // bInterval
};

struct Device_t g_deviceImage = { "Image", _imageDescriptorDevice, _imageDescriptorConfig, NULL, NULL };


/* Printer descriptor is based on
 * https://www.xmos.ai/download/AN00126:-USB-Printer-Device-Class(2.0.0rc3).pdf
 */
/*******************************************************************************
 * DEVICE PRINTER
 */
unsigned char _printerDescriptorDevice[] = {
    18,     // bLength
    DEV_DESCR_DEVICE,      // bDescriptorType
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

unsigned char _printerDescriptorConfig[] = {
    //  Descriptor Config
    0x09, // bLength
    DEV_DESCR_CONFIG, // bDescriptorType
    0x19, // wTotalLengthL
    0x00, // wTotalLengthH
    0x01, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0x80, // bmAttributes
    0x64, // MaxPower
    //  Descriptor Interface
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x01, // bNumEndpoint
    DEV_CLASS_PRINTER, // bInterfaceClass
    0x01, // bInterfaceSubClass
    0x01, // bInterfaceProtocol
    0x00, // iInterface
    //  Descriptor Endpoint (1 OUT)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x01, // bEndpointAddress (OUT)
    0x02, // bmAttributes
    0x40, // wMaxPacketSizeL
    0x00, // wMaxPacketSizeH
    0x01, // bInterval
};

struct Device_t g_devicePrinter = { "printer", _printerDescriptorDevice, _printerDescriptorConfig, NULL, NULL };


/* Mass Storage descriptor is based on
 * https://www.xmos.ai/download/AN00125:-USB-Mass-Storage-Device-Class%282.0.0rc3%29.pdf
 */
/*******************************************************************************
 * DEVICE MASS STORAGE
 */
unsigned char _massStorageDescriptorDevice[] = {
    18,     // bLength
    DEV_DESCR_DEVICE,      // bDescriptorType
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

unsigned char _massStorageDescriptorConfig[] = {
    //  Descriptor Config
    0x09, // bLength
    DEV_DESCR_CONFIG, // bDescriptorType
    0x20, // wTotalLengthL
    0x00, // wTotalLengthH
    0x01, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0x80, // bmAttributes
    0x64, // MaxPower
    //  Descriptor Interface
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x02, // bNumEndpoint
    DEV_CLASS_STORAGE, // bInterfaceClass
    0x06, // bInterfaceSubClass
    0x50, // bInterfaceProtocol
    0x00, // iInterface
    //  Descriptor Endpoint (1 OUT)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x01, // bEndpointAddress (OUT)
    0x02, // bmAttributes
    0x00, // wMaxPacketSizeL
    0x02, // wMaxPacketSizeH
    0x00, // bInterval
    //  Descriptor Endpoint (1 IN)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x81, // bEndpointAddress (IN)
    0x02, // bmAttributes
    0x00, // wMaxPacketSizeL
    0x02, // wMaxPacketSizeH
    0x00, // bInterval
};

struct Device_t g_deviceMassStorage = { "MassStorage", _massStorageDescriptorDevice, _massStorageDescriptorConfig, NULL, NULL };


/*******************************************************************************
 * DEVICE SMART CARD
 */
unsigned char _smartCardDescriptorDevice[] = {
    18,     // bLength
    DEV_DESCR_DEVICE,      // bDescriptorType
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

unsigned char _smartCardDescriptorConfig[] = {
    //  Descriptor Config
    0x09, // bLength
    DEV_DESCR_CONFIG, // bDescriptorType
    0x56, // wTotalLengthL
    0x00, // wTotalLengthH
    0x01, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0x80, // bmAttributes
    0x64, // MaxPower
    //  Descriptor Interface
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x02, // bNumEndpoint
    DEV_CLASS_SMART_CARD, // bInterfaceClass
    0x00, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x00, // iInterface
    //  Descriptor Smart Card Device Class
    0x36, // bLength
    0x21, // bDescriptorType    (Same as DEV_DESCR_HID ?)
    0x10, // bcdCCIDL
    0x01, // bcdCCIDH
    0x00, // bMaxSlotIndex      (Here only one)
    0x04, // bVoltageSupport
    0x03, // dwProtocols0   (ProtocolL)
    0x00, // dwProtocols1   (ProtocolH)
    0x00, // dwProtocols2   (ReservedL)
    0x00, // dwProtocols3   (ReservedH)
    0xFC, // dwDefaultClock0    (3.58 MHz)
    0x0D, // dwDefaultClock1
    0x00, // dwDefaultClock2
    0x00, // dwDefaultClock3
    0xF0, // dwMaximumClock0    (14.32 MHz)
    0x37, // dwMaximumClock1
    0x00, // dwMaximumClock2
    0x00, // dwMaximumClock3
    0x00, // bNumClockSupported
    0x80, // dwDataRate0        (9600 bauds)
    0x25, // dwDataRate1
    0x00, // dwDataRate2
    0x00, // dwDataRate3
    0x00, // dwMaxDataRate0     (115200 bauds)
    0xC2, // dwMaxDataRate1
    0x01, // dwMaxDataRate2
    0x00, // dwMaxDataRate3
    0x00, // bNumDataRatesSupported
    0xFE, // dwMaxIFSD0
    0x00, // dwMaxIFSD1
    0x00, // dwMaxIFSD2
    0x00, // dwMaxIFSD3
    0x04, // dwSynchProtocols0
    0x00, // dwSynchProtocols1
    0x00, // dwSynchProtocols2
    0x00, // dwSynchProtocols3
    0x00, // dwMechanical0
    0x00, // dwMechanical1
    0x00, // dwMechanical2
    0x00, // dwMechanical3
    0x00, // dwFeatures0
    0x00, // dwFeatures1
    0x00, // dwFeatures2
    0x00, // dwFeatures3
    0x00, // dwMaxCCIDMessageLength0    (Careful! The minimum value is the wMaxPacketSize of endpoint Bulk OUT)
    0x02, // dwMaxCCIDMessageLength1
    0x00, // dwMaxCCIDMessageLength2
    0x00, // dwMaxCCIDMessageLength3
    0xFF, // bClassGetResponse
    0xFF, // bClassEnvelope
    0x00, // wLcdLayoutL
    0x00, // wLcdLayoutH
    0x00, // bPINSupport
    0x01, // bMaxCCIDBusySlots
    //  Descriptor Endpoint (1 OUT)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x01, // bEndpointAddress (OUT)
    0x02, // bmAttributes
    0x00, // wMaxPacketSizeL
    0x02, // wMaxPacketSizeH
    0x00, // bInterval
    //  Descriptor Endpoint (1 IN)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x81, // bEndpointAddress (IN)
    0x02, // bmAttributes
    0x00, // wMaxPacketSizeL
    0x02, // wMaxPacketSizeH
    0x00, // bInterval
};

struct Device_t g_deviceSmartCard = { "SmartCard", _smartCardDescriptorDevice, _smartCardDescriptorConfig, NULL, NULL };


/*******************************************************************************
 * DEVICE PERSONAL HEALTHCARE
 */
unsigned char _personalHealthcareDescriptorDevice[] = {
    18,     // bLength
    DEV_DESCR_DEVICE,      // bDescriptorType
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

unsigned char _personalHealthcareDescriptorConfig[] = {
    //  Descriptor Config
    0x09, // bLength
    DEV_DESCR_CONFIG, // bDescriptorType
    0x2F, // wTotalLengthL
    0x00, // wTotalLengthH
    0x01, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0x80, // bmAttributes
    0x64, // MaxPower
    //  Descriptor Interface
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x03, // bNumEndpoint
    DEV_CLASS_PHDC, // bInterfaceClass
    0x00, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x00, // iInterface
    //  Descriptor PHDC Class Function
    0x04, // bLength
    0x20, // bDescriptorType
    0x02, // bPHDCDataCode
    0x00, // bmCapability
    //  Descriptor PHDC QoS
    0x04, // bLength
    0x21, // bDescriptorType
    0x01, // bQoSEncodingVersion
    0x10, // bmLatencyReliability
    //  Descriptor Endpoint (1 Bulk OUT)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x01, // bEndpointAddress (OUT)
    0x02, // bmAttributes
    0x00, // wMaxPacketSizeL
    0x02, // wMaxPacketSizeH
    0x00, // bInterval
    //  Descriptor Endpoint (1 Bulk IN)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x81, // bEndpointAddress (IN)
    0x02, // bmAttributes
    0x00, // wMaxPacketSizeL
    0x02, // wMaxPacketSizeH
    0x00, // bInterval
    //  Descriptor Endpoint (2 Interrupt IN)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x82, // bEndpointAddress (IN)
    0x03, // bmAttributes
    0x00, // wMaxPacketSizeL
    0x02, // wMaxPacketSizeH
    0x01, // bInterval
};

struct Device_t g_devicePersonalHealthcare = { "PersonalHealthcare", _personalHealthcareDescriptorDevice, _personalHealthcareDescriptorConfig, NULL, NULL };


/*******************************************************************************
 * DEVICE VIDEO
 */

/* Video descriptor is based on
 * https://www.xmos.ai/download/AN00127:-USB-Video-Class-Device(2.0.1rc1).pdf
 */
unsigned char _videoDescriptorDevice[] = {
    18,     // bLength
    DEV_DESCR_DEVICE,      // bDescriptorType
    0x00,   // bcdUSB (low)
    0x02,   // bcdUSB (high)
    0xEF,   // bDeviceClass
    0x02,   // bDeviceSubClass
    0x01,   // bDeviceProtocol
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

unsigned char _videoDescriptorConfig[] = {
    //  Descriptor Config
    0x09, // bLength
    DEV_DESCR_CONFIG, // bDescriptorType
    0xAE, // wTotalLengthL
    0x00, // wTotalLengthH
    0x02, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0x80, // bmAttributes
    0x64, // MaxPower
    //  Descriptor Interface Association
    0x08, // bLength
    DEV_DESCR_IF_ASSOC, // bDescriptorType
    0x00, // bFirstInterface
    0x02, // bInterfaceCount
    0x0E, // bFunctionClass
    0x03, // bFunctionSubClass
    0x00, // bFunctionProtocol
    0x00, // iFunction
    //  Descriptor Video Control (VC) Interface
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x01, // bNumEndpoint
    DEV_CLASS_VIDEO, // bInterfaceClass
    0x01, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x00, // iInterface
    //  Descriptor Class-Specific VC Interface Header
    0x0D, // bLength
    DEV_DESCR_CS_INTF, // bDescriptorType
    0x01, // bDescriptorSubtype
    0x10, // bcdUVCL
    0x01, // bcdUVCH
    0x28, // wTotalLengthL
    0x00, // wTotalLengthH
    0x00, // dwClockFrequency0
    0xe1, // dwClockFrequency1
    0xf5, // dwClockFrequency2
    0x05, // dwClockFrequency3
    0x01, // bInCollection
    0x01, // baInterfaceNr
    //  Descriptor Input Terminal (Here a Camera)
    0x12, // bLength
    DEV_DESCR_CS_INTF, // bDescriptorType
    0x02, // bDescriptorSubtype
    0x01, // bTerminalID
    0x01, // wTerminalTypeL
    0x02, // wTerminalTypeH
    0x00, // bAssocTerminal
    0x00, // iTerminal
    0x00, // wObjectiveFocalLengthMinL
    0x00, // wObjectiveFocalLengthMinH
    0x00, // wObjectiveFocalLengthMaxL
    0x00, // wObjectiveFocalLengthMaxH
    0x00, // wOcularFocalLengthL
    0x00, // wOcularFocalLengthH
    0x03, // bControlSize
    0x00, // bmControlsL
    0x00, // bmControlsM
    0x00, // bmControlsH
    //  Descriptor Output Terminal
    0x09, // bLength
    DEV_DESCR_CS_INTF, // bDescriptorType
    0x03, // bDescriptorSubtype
    0x02, // bTerminalID
    0x01, // wTerminalTypeL
    0x01, // wTerminalTypeH
    0x00, // bAssocTerminal
    0x01, // bSourceID
    0x00, // iTerminal
    //  Descriptor Endpoint (Interrupt IN)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x81, // bEndpointAddress
    0x03, // bmAttributes
    0x40, // wTotalLengthL
    0x00, // wTotalLengthH
    0x09, // bInterval
    //  Descriptor Class-Specific (CS) Endpoint
    0x05, // bLength
    DEV_DESCR_CS_ENDP, // bDescriptorType
    0x03, // bDescriptorSubtype
    0x40, // wTotalLengthL
    0x00, // wTotalLengthH
    //  Descriptor Video Streaming (VS) Interface
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x01, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x00, // bNumEndpoint
    DEV_CLASS_VIDEO, // bInterfaceClass
    0x02, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x00, // iInterface
    //  Descriptor Class-Specific VS Interface Header
    0x0E, // bLength
    DEV_DESCR_CS_INTF, // bDescriptorType
    0x01, // bDescriptorSubtype
    0x01, // bNumFormats
    0x47, // wTotalLengthL
    0x00, // wTotalLengthH
    0x82, // bEndpointAddress
    0x00, // bmInfo
    0x02, // bTerminalLink
    0x01, // bStillCaptureMethod
    0x00, // bTriggerSupport
    0x00, // bTriggerUsage
    0x01, // bControlSize
    0x00, // bmaControls
    //  Descriptor Class-Specific VS Format
    0x1B, // bLength
    DEV_DESCR_CS_INTF, // bDescriptorType
    0x04, // bDescriptorSubtype
    0x01, // bFormatIndex
    0x01, // bFrameDescriptors
    0x59, // guidFormat0 (YUY2 Video Format)
    0x55, // guidFormat1
    0x59, // guidFormat2
    0x32, // guidFormat3
    0x00, // guidFormat4
    0x00, // guidFormat5
    0x10, // guidFormat6
    0x00, // guidFormat7
    0x80, // guidFormat8
    0x00, // guidFormat9
    0x00, // guidFormat10
    0xAA, // guidFormat11
    0x00, // guidFormat12
    0x38, // guidFormat13
    0x9B, // guidFormat14
    0x71, // guidFormat15
    0x10, // bBitsPerPixel
    0x01, // bDefaultFrameIndex
    0x00, // bAspectRatioX
    0x00, // bAspectRatioY
    0x00, // bmInterlaceFlags
    0x00, // bCopyProtect
    //  Descriptor Class-Specific VS Frame
    0x1E, // bLength
    DEV_DESCR_CS_INTF, // bDescriptorType
    0x05, // bDescriptorSubtype
    0x01, // bFrameIndex
    0x01, // bmCapabilities
    0xe0, // wWidthL
    0x01, // wWidthH
    0x40, // wHeightL
    0x01, // wHeightH
    0x00, // dwMinBitRate0
    0x00, // dwMinBitRate1
    0x65, // dwMinBitRate2
    0x04, // dwMinBitRate3
    0x00, // dwMaxBitRate0
    0x00, // dwMaxBitRate1
    0x65, // dwMaxBitRate2
    0x04, // dwMaxBitRate3
    0x00, // dwMaxVideoFrameBufSize0
    0xB0, // dwMaxVideoFrameBufSize1
    0x04, // dwMaxVideoFrameBufSize2
    0x00, // dwMaxVideoFrameBufSize3
    0x15, // dwDefaultFrameInterval0
    0x16, // dwDefaultFrameInterval1
    0x05, // dwDefaultFrameInterval2
    0x00, // dwDefaultFrameInterval3
    0x01, // bFrameIntervalType
    0x15, // dwFrameInterval0
    0x16, // dwFrameInterval1
    0x05, // dwFrameInterval2
    0x00, // dwFrameInterval3
    //  Descriptor Video Stream Interface
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x01, // bInterfaceNumber
    0x01, // bAlternateSetting
    0x01, // bNumEndpoint
    DEV_CLASS_VIDEO, // bInterfaceClass
    0x02, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x00, // iInterface
    //  Descriptor Endpoint (Isochronous IN)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x82, // bEndpointAddress
    0x05, // bmAttributes
    0x00, // wMaxPacketSizeL
    0x04, // wMaxPacketSizeH
    0x01, // bInterval
};

struct Device_t g_deviceVideo = { "Video", _videoDescriptorDevice, _videoDescriptorConfig, NULL, NULL };


/*******************************************************************************
 * DEVICE DEVICE FIRMWARE UPDATE (DFU)
 *
 * According to the specification this one is special
 * - There is no endpoints other than ep0 (control)
 * - Enumeration is done in 2 phases
 *      - First enumeration the Run-Time DFU Descriptor is given
 *      - Second enumeration the DFU Mode Descriptor is given
 * Here we only enumerate for the first one, the Run-Time DFU Descriptor. If an
 * anwser is given we assume DFU is supported
 * Note that doing the whole process would require a firmware modification
 */
unsigned char _dfuDescriptorDevice[] = {
    18,     // bLength
    DEV_DESCR_DEVICE,      // bDescriptorType
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

unsigned char _dfuDescriptorConfig[] = {
    //  Descriptor Config
    0x09, // bLength
    DEV_DESCR_CONFIG, // bDescriptorType
    0x1B, // wTotalLengthL
    0x00, // wTotalLengthH
    0x01, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0x80, // bmAttributes
    0x64, // MaxPower
    //  Descriptor Interface
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x00, // bNumEndpoint
    DEV_CLASS_APP_SPEC, // bInterfaceClass
    0x01, // bInterfaceSubClass
    0x01, // bInterfaceProtocol
    0x00, // iInterface
    //  Descriptor DFU Functional
    0x09, // bLength
    0x21, // bDescriptorType
    0x03, // bmAttributes
    0x64, // wDetachTimeOutL
    0x00, // wDetachTimeOutH
    0x80, // wTransferSizeL
    0x00, // wTransferSizeH
    0x37, // bcdDFUVersionL
    0x13, // bcdDFUVersionH
};

struct Device_t g_deviceDFU = { "DFU", _dfuDescriptorDevice, _dfuDescriptorConfig, NULL, NULL };


/*******************************************************************************
 * DEVICE FTDI (VEndor Specific)
 */
unsigned char _ftdiDescriptorDevice[] = {
    18,     // bLength
    DEV_DESCR_DEVICE,      // bDescriptorType
    0x00,   // bcdUSB (low)
    0x02,   // bcdUSB (high)
    0x00,   // bDeviceClass (Defined in the interface descriptor)
    0x00,   // bDeviceSubClass
    0x00,   // bDeviceProtocol
    64,     // bMaxPacketSize0
    0x03,   // idVendor (low)
    0x04,   // idVendor (high)
    0x01,   // idProduct (low)
    0x60,   // idProduct (high)
    0x01,   // bcdDevice (low)
    0x00,   // bcdDevice (high)
    0x00,   // iManufacturer
    0x00,   // iProduct
    0x00,   // iSerialNumber
    1,      // bNumConfigurations
};

unsigned char _ftdiDescriptorConfig[] = {
    //  Descriptor Config
    0x09, // bLength
    DEV_DESCR_CONFIG, // bDescriptorType
    0x20, // wTotalLengthL
    0x00, // wTotalLengthH
    0x01, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0x80, // bmAttributes
    0x64, // MaxPower
    //  Descriptor Interface
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x02, // bNumEndpoint
    DEV_CLASS_VEN_SPEC, // bInterfaceClass
    0xFF, // bInterfaceSubClass
    0xFF, // bInterfaceProtocol
    0x00, // iInterface
    //  Descriptor Endpoint (Bulk OUT)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x01, // bEndpointAddress
    0x00, // bmAttributes
    0x00, // wMaxPacketSizeL
    0x40, // wMaxPacketSizeH
    0x00, // bInterval
    //  Descriptor Endpoint (Bulk IN)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x83, // bEndpointAddress
    0x00, // bmAttributes
    0x00, // wMaxPacketSizeL
    0x40, // wMaxPacketSizeH
    0x00, // bInterval
};

struct Device_t g_deviceFTDI = { "FTDI", _ftdiDescriptorDevice, _ftdiDescriptorConfig, NULL, NULL };

/*******************************************************************************
 * DEVICE HUB
 */

/* Hub device is based on
 * https://docplayer.net/25762520-Genesys-logic-inc-gl850-usb-port-hub-controller.html
 */

unsigned char _hubDescriptorDevice[] = {
    18,     // bLength
    DEV_DESCR_DEVICE,      // bDescriptorType
    0x00,   // bcdUSB (low)
    0x02,   // bcdUSB (high)
    0x09,   // bDeviceClass (Defined in the interface descriptor)
    0x00,   // bDeviceSubClass
    0x01,   // bDeviceProtocol
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

unsigned char _hubDescriptorConfig[] = {
    //  Descriptor Config
    0x09, // bLength
    DEV_DESCR_CONFIG, // bDescriptorType
    0x19, // wTotalLengthL
    0x00, // wTotalLengthH
    0x01, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0x80, // bmAttributes
    0x64, // MaxPower
    //  Descriptor Interface
    0x09, // bLength
    DEV_DESCR_INTERF, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    0x01, // bNumEndpoint
    DEV_CLASS_VEN_SPEC, // bInterfaceClass
    0x09, // bInterfaceSubClass
    0x00, // bInterfaceProtocol
    0x01, // iInterface
    //  Descriptor Endpoint (Interrupt IN)
    0x07, // bLength
    DEV_DESCR_ENDP, // bDescriptorType
    0x81, // bEndpointAddress
    0x03, // bmAttributes
    0x01, // wMaxPacketSizeL
    0x00, // wMaxPacketSizeH
    0x0C, // bInterval
};

unsigned char _hubDescriptorReport[] = {
    0x09, // bLength
    0x29, // bDescriptorType
    0x04, // bNbrPorts
    0x89, // wHubCharacteristicsL
    0x80, // wHubCharacteristicsH
    0x32, // bPwrOn2PwrGood
    0x64, // bHubContrCurrent
    0x00, // bDeviceRemoveable
    0xFF, // bPortPwrCtrlMask
};

struct Device_t g_deviceHub = { "Hub", _hubDescriptorDevice, _hubDescriptorConfig, NULL, _hubDescriptorReport };


/*******************************************************************************
 * AUTOMODE ARRAY
 */
struct Device_t *g_devices[] = {
    &g_deviceGeneric,
    &g_deviceAudio,
    &g_deviceCdc,
    &g_deviceKeyboard,
    &g_deviceImage,
    &g_devicePrinter,
    &g_deviceMassStorage,
    &g_deviceSmartCard,
    &g_devicePersonalHealthcare,
    &g_deviceVideo,
    &g_deviceDFU,
    &g_deviceFTDI,
    &g_deviceHub,
    NULL,   // Careful, must be null terminated
};

