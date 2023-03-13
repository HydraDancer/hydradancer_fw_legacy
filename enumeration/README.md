# enumeration

This firmware aims to offer the enumeration capability of umap (with the facedancer).


## Global overview

To know if a device is recognized by the host ToE (Target of Evaluation) we
behave as a USB device until the host ToE sends us a setConfiguration().

The top board is connected to the Evalutor (the computer running the
evaluation) and the bottom board to the Toe.

the `host-controller` is the software running on the Evalutor. It controls the
enumeration process.

The enumeration process works as follow :
![architecture-diagram](architecture-diagram.png)


Addtional informations can be found here :
[BBIO_CMD_HydraDancer](https://github.com/hydrausb3/HydraDancer/blob/main/docs/BBIO_CMD_HydraDancer.md)
