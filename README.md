# CBM-PET-IEEE
File system for the CBM pet

This project implements an IEEE interface between a CBM PET and an Arduino MEGA2560
and works as a simple disk emulator allowing BASIC SAVE and LOAD commands to read/write programs to an SD card.

As the arduino ports are tri-state, the interface requires no buffer components and just connects the IEEE-488 spec connector directly to the Arduino pins and the SD card via the SPI port. This arrangement has been tested with a genuine 8050 sharing the same bus and works correctly.

Currently only supports the LOAD"filename",9 command, the SAVE"filename",9 command and the LOAD"$",9 commands, however each also can interpret drive numbers (ie LOAD"1:filename",9 or LOAD"$4",9). Files are stores in standard FAT 8.3 format as PET .PRG files (easily found online and downloaded) but the filename must be 8 characters or less (ie TANKS.PRG) and stored in a folder with names 0 to 9 to simulate 10 drives.

It is intended to expand the operation to cover sequential files and an command/error channel.
