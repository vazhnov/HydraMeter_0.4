# HydraMeter_0.4

Open source [multimeter](https://en.wikipedia.org/wiki/Multimeter)

## Intro

I don't have detailed assembly documents yet, and this isn't really well made for easy assembly in any case, it's a proof of concept at the moment.

There are some more details here: https://hackaday.io/project/176607-hydrameter but it's not much. Mainly a few entries down there's a tour
of how the analog stuff works: https://hackaday.io/project/176607-hydrameter/log/223270-schematic-and-operation

## Repository structure

* Directory [MMTR_AFE_00_04_TORELEASE](./MMTR_AFE_00_04_TORELEASE) contains all schematics and board layouts.
* Directory [MMTR_V00_03_Case](./MMTR_V00_03_Case) contains all the CAD, source type is solidedge (they have a free community edition).
* Directory [Code -> HackBoard_V02_10_shbrd_cal](./Code/HackBoard_V02_10_shbrd_cal/) contains the code for the analog frontend.
* Directory [Code -> Display_VP_10_WirelessV2_SC](./Code/Display_VP_10_WirelessV2_SC/) contains code for the display section.

## Notes

I would recommend 24 to 28 AWG stranded wire for connecting to switches, except the mA/uA switch, for which I'd use 20AWG.
The switches should pull inputs to the 74165s to ground, for numbering you'll have to check the code (or adjust it), when assembled I just
wired up all of them then recorded which switch went where, I don't have a list or diagram. On the board these are labeled A-H, one set
is near the middle of the board, the other next to the module connector.

Hardwired connections are: the two ohms jumpers (J7 and J8) are wired to the second pole of the lower-right switch (right from the front),
and the mA/uA switch goes to J5 and J6.

There's a hardwired connection from J30 to J34, which should be made with at least 18AWG wire, as it carries the full current for the 10A range.

## Tweaks

Note that this version of the PCB has some tweaks from the last manufactured version,
these have not been verified to work.


The as-made PCB is under version 0.3,
however, this version requires significant manual rework including:
* pullup resistors for all switches to D3V3
* cutting jumpers to the Module outputs (J96-99, to configure them as inputs), and wiring switches to them
* additional capacitors on D3V3 to D0V, to stabilize the rail when using a radio module
* DNP U5, and bridge pin 3 to pin 4
* DNP: R74, D5, C72, R76, R77, U26, R78, Q20, Q21, R80, R79
* Replace R33 with a ~16K resistor, or two 33K in parallel
* Manually cut the trace going to pin 10 on U12, U13 and U16 (pulling it to D0V), and wire that pin on the chip to D3V3
* U18 has the wrong footprint (SOIC-16 used on the board, the actual chip is a SOIC-14).
* Angle U18 such that only pins 14-9 make contact with the board (so pins 1-7 are above the surface), and solder in place.
* Then manually wire pins 2-7 to the appropriate pads with short wires.
* If using a battery, a protected cell must be used, there is no circuitry onboard to protect from over or under voltage.
* Positive should be soldered to J10, negative to J11.

To charge, two diodes can be placed in series from VBUS on the
PI pico (MCU_VBUS) to battery positive (charging the battery when USB is plugged in). Bypassing one of the diodes with
a 50 or 100 ohm resistor can improve charge time. This however WILL overcharge the battery if proper cell protection
is not in place! This is a terrible hack solution I used to get something working in like an hour. Use a charger IC
or board or something.

## License

This project uses the CERN Open Hardware Licence Version 2 - Strongly Reciprocal.
Any derived works must fall under the same license and must be open source themselves.
