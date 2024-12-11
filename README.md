# HydraMeter_0.4
Open source multimeter

This project uses the CERN Open Hardware Licence Version 2 - Strongly Reciprocal
Any derived works must fall under the same license and must be open source themselves






Note that this version of the PCB has some tweaks from the last manufactured version,
these have not been verified to work.  The as-made PCB is under version 0.3, 
however, this version requires significant manual rework including:
pullup resistors for all switches to D3V3
cutting jumpers to the Module outputs (J96-99, to configure them as inputs), and wiring switches to them
additional capacitors on D3V3 to D0V, to stabilize the rail when using a radio module
DNP U5, and bridge pin 3 to pin 4
DNP: R74, D5, C72, R76, R77, U26, R78, Q20, Q21, R80, R79
Replace R33 with a ~16K resistor, or two 33K in parallel
Manually cut the trace going to pin 10 on U12, U13 and U16 (pulling it to D0V), and wire that pin on the chip to D3V3
U18 has the wrong footprint (SOIC-16 used on the board, the actual chip is a SOIC-14).
Angle U18 such that only pins 14-9 make contact with the board (so pins 1-7 are above the surface), and solder in place.
Then manually wire pins 2-7 to the appropriate pads with short wires. 
If using a battery, a protected cell must be used, there is no circuitry onboard to protect from over or under voltage. 
Positive should be soldered to J10, negative to J11.  To charge, two diodes can be placed in series from VBUS on the
pi pico (MCU_VBUS) to battery positive (charging the battery when USB is plugged in).  Bypassing one of the diodes with
a 50 or 100 ohm resistor can improve charge time.  This however WILL overcharge the battery if proper cell protection
is not in place!  This is a terrible hack solution I used to get something working in like an hour.  Use a charger IC
or board or something. 

