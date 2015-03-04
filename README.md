This message is here to annoy the maintainer. _Isn't this annoying?_

You'll need the following to work with this code base:
- A Linux machine (Maybe. Probably. :)
- BASH (for running the build and test scripts)
- GCC cross-compiling for the msp430-elf target (I'm using GCC version 4.9.2)
- GCC compiling for your own machine (also 4.9.2 in my case)
- The Boost libraries installed (I'm using 1.56.0)
- mspdebug (if you want to run the tests, or flash the firmware to actual chips; I'm using version 0.22)
- Misc text tools (dos2unix, cat, sed, awk) if you want to generate the mspdebug scripts from the NIST test vectors (you will also need to download the appropriate test vectors from http://csrc.nist.gov/groups/STM/cavp/)

You'll probably also want a sense of humor and at least some understanding of the MSP430 ISA. An understanding of how AES and Galois Counter Mode work is probably useful as well, but not strictly necessary. Ditto for SPI.

Firmware should nominally have a simple pin-based schematic at the start of config.h (or possibly isr.S). All firmware is currently coded against the MSP430G2412 chip specifically, but there should be an indicator of this with the firmware's schematic.

#Woah. Hold up. What is this codebase?
This is some prelim code for a project called "affordable home automation," or AHA. No, it's not complete/working yet; there's some interesting groundwork in place though.
