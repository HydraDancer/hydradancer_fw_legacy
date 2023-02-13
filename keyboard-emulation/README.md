# keyboard emulation

This firmware is the first firmware developed for the HydraUSB3 board.

The goal of this firmware was to understand how to build a USB2 device stack for this board. It was not meant to be clean and polished.

The interesting part about keyboard emulation can be found in th is function :
`ep1_transmit_keyboard()`


Here are additional ressources used during the development if this firmware :

- https://www.usb.org/sites/default/files/hid1_11.pdf
- https://wiki.osdev.org/USB_Human_Interface_Devices
- http://www.zilog.com/docs/AN0416.pdf
