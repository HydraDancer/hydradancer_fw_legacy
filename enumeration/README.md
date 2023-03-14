# enumeration

This firmware aims to offer the enumeration capability of umap (with the facedancer).


## How To Use

### Build and Flash the firmware on both boards
```
# From ./firmware

$ make clean all
$ /path/to/wch-ch56x-isp -v flash /path/to/build/hydrausb3-enumeration.bin
```
More explanations about how to flash can be found here :
* [Linux  ](https://github.com/hydrausb3/hydrausb3_fw/wiki/how-to-build-flash-and-use-examples-on-linux)
* [Windows](https://github.com/hydrausb3/hydrausb3_fw/wiki/how-to-build-flash-and-use-examples-on-windows)

### Build and run `host-controller`
```
# From ./host-controller

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
![architecture-diagram](architecture-diagram.png)


Addtional informations can be found here :
[BBIO_CMD_HydraDancer](https://github.com/hydrausb3/HydraDancer/blob/main/docs/BBIO_CMD_HydraDancer.md)


## Adding devices to enumeration

To add a device to enumeration (automode) you need to :
- add the descriptors in `usb_descriptors.c`
    - add an array for the device descriptor
    - add an array for the configuration descriptor (The whole tree !)
    - add those arrays in a `struct Device_t`
- add this struct in `g_devices` (At the bottom of `usb_descriptors.c`)

