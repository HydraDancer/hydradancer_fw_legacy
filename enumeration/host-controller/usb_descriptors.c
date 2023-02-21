#include <stddef.h>

#include "usb_descriptors.h"


/* macros */
/* USB descriptor type */
#define DEV_DESCR_DEVICE  0x01
#define DEV_DESCR_CONFIG  0x02
#define DEV_DESCR_STRING  0x03
#define DEV_DESCR_INTERF  0x04
#define DEV_DESCR_ENDP    0x05
#define DEV_DESCR_QUALIF  0x06
#define DEV_DESCR_SPEED   0x07
#define DEV_DESCR_OTG     0x09
#define DEV_DESCR_HID     0x21
#define DEV_DESCR_REPORT  0x22
#define DEV_DESCR_PHYSIC  0x23
#define DEV_DESCR_CS_INTF 0x24
#define DEV_DESCR_CS_ENDP 0x25
#define DEV_DESCR_HUB     0x29

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

struct Device_t g_deviceGeneric = { "Generic", _genericDescriptorDevice, _genericDescriptorConfig, NULL };


/*******************************************************************************
 * GENERIC DEVICES CONFIGURATIONS
 */
struct DeviceConfig_t g_genericDeviceConfig = { "Generic Config", 0x00, 0x00, 0x00 };


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

struct Device_t g_deviceAudio = { "Audio", _audioDescriptorDevice, _audioDescriptorConfig, NULL };


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

struct Device_t g_deviceCdc = { "CDC (Virtual COM Port)", _cdcDescriptorDevice, _cdcDescriptorConfig, NULL };


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

struct Device_t g_deviceKeyboard = { "Keyboard", _keyboardDescriptorDevice, _keyboardDescriptorConfig, _keyboardDescriptorHidReport };

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

struct Device_t g_deviceImage = { "Image", _imageDescriptorDevice, _imageDescriptorConfig, NULL };


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

struct Device_t g_devicePrinter = { "printer", _printerDescriptorDevice, _printerDescriptorConfig, NULL };


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

struct Device_t g_deviceMassStorage = { "MassStorage", _massStorageDescriptorDevice, _massStorageDescriptorConfig, NULL };


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

struct Device_t g_deviceSmartCard = { "SmartCard", _smartCardDescriptorDevice, _smartCardDescriptorConfig, NULL };


