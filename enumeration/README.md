# enumeration

This firmware aims to offer the enumeration capability of umap (with the facedancer).


## How To Use

- Flash the firmware on both board
- Build and run `host-controller`

The enumeration is done through `host-controller`, you can either enumerate one
by one manually or use _automode_ to automatically enumerate every device
already implemented.


## Global overview

To know if a device is recognized by the host ToE (Target of Evaluation) we
behave as a USB device until the host ToE sends us a setConfiguration().

The top board is connected to the Evalutor (the computer running the
evaluation) and the bottom board to the ToE.

the `host-controller` is the software running on the Evalutor. It controls the
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

