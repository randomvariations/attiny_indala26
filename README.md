# ATtiny85 Indala 26 RFID tag

Allows an ATtiny85 to emulate the phase-shift keying
modulation required by Indala format RFID devices.

Use the Flipper Zero to read the RFID device.  This will 
result in a four byte code that represents the encoded
RFID value.

Generate from assembly source:

Edit FLIPPER_CODE with the value from from the flipper zero

avr-gcc -mmcu=attiny85 indala.S -o indala.elf
avr-objcopy  -O binary indala.elf indala.bin

Generate from C code:

Edit FLIPPER_CODE with the value from from the flipper zero

gcc indala.c -o indala
./indala

Credits:

https://scanlime.org/2008/09/using-an-avr-as-an-rfid-tag/
https://github.com/rgerganov/ggtag

Tested with a GGtag:

https://www.crowdsupply.com/eurolan-ltd/ggtag
https://www.mouser.com/ProductDetail/Eurolan/GGTAG-01
