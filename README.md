# CBM-PET-IEEE
File system for the CBM pet

This project implements a very simple IEEE interface between a CBM PET and an Arduino MEGA2560
and works as a simple disk emulator allowing BASIC SAVE and LOAD commands to read/write programs to an SD card.

The interface used no components and just connects the IEEE-488 spec connector to the Arduino pins and the SD card
interface to the SPI port.


Set as device 9 and updated to work on the bus with a genuine PET disk drive
