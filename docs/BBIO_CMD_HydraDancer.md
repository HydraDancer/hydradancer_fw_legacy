# BBIO commands proposal for HydraDancer (HydraUSB3 fuzzing mode USB Device/Host) 25 Jan 2022 Draft v0.1

# 1 HydraDancer Introduction
TODO introduction of HydraDancer ...

## 1.1 HydraDancer Hardware
The HydraDance hardware use 2x HydraUSB3 boards connected together
* SerDes shall be connected between 2 boards Board1 GXM<=>Board2 GXM & Board1 GXP<=>Board2 GXP

## 1.2 HydraDancer Firmware
DualBoard Firmware (same binary firmware for both boards)
- Firmware HydraUSB3 Board1(Top) PC Host
  - PB24 Jumper Present (to detect it is the part for Host communication to be executed)
  - All BBIO commands to be detected from USB are directly restransmitted to Target Board over HSPI
  - Check regularly(Under IRQ or polling to be checked) if there is data on SerDes(coming from Target)
    - If data are present simply transfer them to PC Host over USB

- Firmware HydraUSB3 Board2(bottom) PC Host Target
  - PB24 Jumper NOT Present (To detect is it the part for Target)
  - Protocol using BBIO

## 1.3 HydraUSB3 USB Device fuzzing mode
In this mode HydraUSB3 emulate an USB device for fuzzing
|    Computer     | HydraUSB3 Board1 Top  | Inter-board | HydraUSB3 Board2 Bottom |     Host         |
|:---------------:|:---------------------:|:-----------:|:------------------------:|:---------------:|
| PC Host Attacker| <= USB3 SS Device =>  |  HSPI =>    |<= USB Device Emulation =>| PC Host Target  |
|                 |                       |  <= SerDes  |                          |                 |

Actual mode which will be implemented in priority

## 1.4 HydraUSB3 USB Host fuzzing mode
In this mode HydraUSB3 emulate a USB host for fuzzing
|    Computer     | HydraUSB3 Board1 Top  | Inter-board | HydraUSB3 Board2 Bottom |     Device    |
|:---------------:|:---------------------:|:-----------:|:-----------------------:|:-------------:|
| PC Host Attacker| <= USB3 SS Device =>  |  HSPI =>    | <= USB Host Emulation =>| Device Target |
|                 |                       |  <= SerDes  |                         |               |

This use case is not implemented and not planned today (to be seen in future)

TODO add other details ...

# 2. BBIO protocol format

- ## 2.1 Main BBIO protocol format
  - 8 bits Main mode
  - 8 bits Commands
  - 8 bits Sub Commands and/or N data depending on commands

- ## 2.2 BBIO Main mode (8 bits in binary)
  - `0b00000000` Reset binary mode. Returns BBIO1 
  - `0b01000000` HydraUSB3 USB Device fuzzing mode (HydraUSB3 emulate an USB device for fuzzing)
  - `0b01000001` HydraUSB3 USB Host fuzzing mode (HydraUSB3 emulate a USB host for fuzzing)
  - `0b0100001x` HydraUSB3 debug log (x=1 retrieve debug from Board2, x=0 retrieve debug from Board1)

Note: HydraUSB3 fuzzing mode USB Host is not supported and requires some reverse engineering for USB3 part to support USB Host mode without blob.

  - ## 2.2.1 HydraUSB3 USB Device fuzzing mode
    - BBIO Commands (8 bits in binary)
    - `0b00000000` Return to main mode. Returns BBIO1 
    - `0b00000001` Mode identification. Returns USBD
    - `0b00000010` Set USB device descriptors configurations see BBIO Sub Commands (8 bits in binary)

    - ### 2.2.1.1 HydraUSB3 USB Device fuzzing mode USB device descriptors configurations tree

      ![USB_DeviceDescriptor_Tree V1 board](USB_DeviceDescriptor_Tree.jpg)
	  - Reference https://www.beyondlogic.org/usbnutshell/usb5.shtml

    - ### 2.2.1.2 BBIO Sub Commands (8 bits in binary)
      - #### 2.2.1.2.1 Set Device Descriptor
        - `0b10000000`
        - Followed by 16bits (Big Endian) size in bytes and N data bytes (max len 4096)

      - #### 2.2.1.2.2 Set Configuration Descriptors (max len TBD see XRAM limitation, use allocator ?)
        - `0b00010xxxx` with xxxx from 0 to 15 for up to 16 Configuration Descriptors
        - Followed by 16bits (Big Endian) size in bytes and N data bytes

      - #### 2.2.1.2.3 Set Interface Descriptors for each Configuration Descriptor (max len TBD see XRAM limitation, use allocator ?)
        - `0b00100xxxx` with xxxx from 0 to 15 for up to 16 Interface Descriptors
        - Followed by 16bits (Big Endian) size in bytes and N data bytes

      - #### 2.2.1.2.4 Set Endpoint Descriptors for each Interface descriptors (max len TBD see XRAM limitation, use allocator ?)
        - `0b01000xxxx` with xxxx from 0 to 15 for up to 16 Interface Descriptors
        - Followed by 16bits (Big Endian) size in bytes and N data bytes

      - #### 2.2.1.2.5 Set String Descriptors (max len TBD see XRAM limitation, use allocator ?)
        - `0b10000xxxx` with xxxx from 0 to 15 for up to 16 String Descriptors
        - Followed by 16bits (Big Endian) size in bytes and N data bytes

Warning the device is limited in memory to be checked what is possible with remaining XRAM (as lot of KB are reserved for different devices HSPI, SerDes, USB2/USB3)
It will probably requires a basic memory allocator for that to optimize memory as much as possible

## Future
An other HydraUSB3 USB Device fuzzing passthrough mode shall be studied(with potentially MITM features to be added/configured)
This mode could be used for lowlevel USB enumeration/pipe fuzzing with invalid descriptors including invalid index ... (worst case everything is wrong to test the Target robustness...)

=> PC Host Target packets will be transparently transceived from/to PC Host Attacker which will analyze the data and reply as fast as possible.
