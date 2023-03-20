# enumeration

This firmware aims to offer the enumeration capability of umap (with the facedancer).


## DISCLAIMER !

Ensuring a device is supported by the ToE can be done in 2 ways :
- During the enumeration phase (enumeration as in the spec.), a device can be
  considered supported when the ToE sends a `setConfiguration()` to the device
  (the last command issued during the enumeration phase).
- Waiting to receive a packet on a given endpoint (other than endpoint 0).

Here we choose the first option, note that it might have false positives, as it
is the case on linux hosts (linux hosts always sends the `setConfiguration()`
even if no driver were loaded).

To be more exhaustive both mode should be implemented to let the choice of the
method to the final user.


## Quick Start


Prerequisites :
- MRS toolchains (more infos here:
[Linux](https://github.com/hydrausb3/hydrausb3_fw/wiki/how-to-build-flash-and-use-examples-on-linux),
[Windows](https://github.com/hydrausb3/hydrausb3_fw/wiki/how-to-build-flash-and-use-examples-on-windows)
)
- pkg-config
- libusb (1.0.X developer)


```shell
# Get the MRS toolchain
cd ~
wget http://file.mounriver.com/tools/MRS_Toolchain_Linux_x64_V1.50.tar.xz
mkdir -p MRS_Toolchain_Linux_x64_V1.50
tar xvf MRS_Toolchain_Linux_x64_V1.50.tar.xz --directory=MRS_Toolchain_Linux_x64_V1.50
PATH=$PATH:~/MRS_Toolchain_Linux_x64_V1.50/RISC-V\ Embedded\ GCC/bin

# Get and build the flashing tool
cd ~
git clone --recursive git@github.com:hydrausb3/wch-ch56x-isp.git
cd ./wch-ch56x-isp
make

# Get the HydraDancer sources
cd ~
git clone --recursive git@github.com:hydrausb3/HydraDancer.git
cd ./HydraDancer/firmware

# Building and flashing firmware
cd ./firmware
make clean all
# Put the jumper on P3 (Flash Mode)
# Warning ! You have 10 seconds to flash the board !
sudo ~/wch-ch56x-isp/1ch-ch56x-isp -v flash ./build/hydrausb3-enumeration.bin # Flash first board
sudo ~/wch-ch56x-isp/1ch-ch56x-isp -v flash ./build/hydrausb3-enumeration.bin # Flash second board

# Build and run host controller software
cd ../host-controller
make clean all
sudo ./build/host-controller
```

## How To Use

Build and flash the firmware on both board.

Build and run host-controller. Everything is done through that software.


### Build and Flash the firmware on both boards

Note that when connecting the board (in flash mode) you have 10 seconds to
flash it, after that delay it swaps back to the firmware already present on the
board.

```
$ cd ./firmware

$ make clean all
$ /path/to/wch-ch56x-isp -v flash /path/to/build/hydrausb3-enumeration.bin
```
More explanations about how to flash can be found here :
* [How to Flash: Linux  ](https://github.com/hydrausb3/hydrausb3_fw/wiki/how-to-build-flash-and-use-examples-on-linux)
* [How to Flash: Windows](https://github.com/hydrausb3/hydrausb3_fw/wiki/how-to-build-flash-and-use-examples-on-windows)

### Build and run `host-controller`
```
$ cd ./host-controller

$ make clean all
$ ./build/host-controller
```
Note: root privileges may be required.

The enumeration is done through `host-controller`, you can either enumerate one
by one manually or use _automode_ to automatically enumerate every device
already implemented.


## Global overview

To know if a device is recognized by the host ToE (Target of Evaluation) we
behave as a USB device until the host ToE sends us a setConfiguration().

The top board is connected to the Evaluator (the computer running the
evaluation) and the bottom board to the ToE.

the `host-controller` is the software running on the Evaluator. It controls the
enumeration process.

The enumeration process works as follow :
```mermaid
sequenceDiagram
    Evaluator->>+HydraUSB3: Configuration of the USB device to emulate
    Evaluator->>+HydraUSB3: Connect Command
    HydraUSB3->>+TargetofEvaluation:  
    TargetofEvaluation->>+HydraUSB3: Enumeration phase
    Evaluator->>+HydraUSB3: Query results
    HydraUSB3->>+Evaluator: Results
```
<!---![architecture-diagram](architecture-diagram.png)--->

### Limitations

- Currently only the enumeration phase is done, there is no communication with
  the driver once the enumeration is done.

- The configuration (descriptors) of the device has to be uploaded before
  "connecting" (with BBIO command, not physically). It can not be sent "on the
  fly", while the enumeration is ongoing.


Addtional informations can be found here :
[BBIO_CMD_HydraDancer](https://github.com/hydrausb3/HydraDancer/blob/main/docs/BBIO_CMD_HydraDancer.md)


## Adding devices to enumeration

Adding devices to enumerations is done in `host-controller`.

To add a device to enumeration (automode) you need to :
- add the descriptors in `usb_descriptors.c`
    - add an array for the device descriptor
    - add an array for the configuration descriptor (The whole tree !)
    - add those arrays in a `struct Device_t`
- add this struct in `g_devices` (At the bottom of `usb_descriptors.c`)

The following generic device can be copy-pasted and adapted to create a new device :
```C
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
```


## Common issues

- Make sure to have gcc toolchain for RISC-V, instructions can be found here :
    * [How to flash: Linux  ](https://github.com/hydrausb3/hydrausb3_fw/wiki/how-to-build-flash-and-use-examples-on-linux)
    * [How to flash: Windows](https://github.com/hydrausb3/hydrausb3_fw/wiki/how-to-build-flash-and-use-examples-on-windows)

- Do not forget to clone the submodules
```
$ git submodule init
$ git submodule update
```

- As mentionned in _How to flash_, when connecting the board (in flash mode)
  you have 10 seconds to flash it, after that delay it swaps back to the
  firmware already present on the board.

- "Error USB device not found", ensure device is plugged and not in flash mode, additionally it can be a permissions issue, try in root.




