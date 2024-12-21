#ifndef PGA_CTRL_H
#define PGA_CTRL_H

#include "pins.h"
#include <SPI.h>
#include <Arduino.h>

#define PGA_SPI_RATE 8000000
#define PGA_CHCHANGE_DELAY 500 //any time PGA channel is changed, wait this long to settle
#define PGA_CMD_NOP         0x00
#define PGA_CMD_WRITE_GAIN  0x40
#define PGA_CMD_WRITE_CH    0x41
#define PGA_CMD_SHUTDOWN    0x20

#define PGA_CHANNEL_VOLTS 0
#define PGA_CHANNEL_MA    1
#define PGA_CHANNEL_UA    2
#define PGA_CHANNEL_AMPS  3
#define PGA_CHANNEL_OHMS  4
#define PGA_CHANNEL_MOD0  5
#define PGA_CHANNEL_MOD1  6
#define PGA_CHANNEL_MOD23 7

typedef enum pgaGain_type{
	PGA_GAIN_1=0,
	PGA_GAIN_2=1,
	PGA_GAIN_4=2,
	PGA_GAIN_5=3,
	PGA_GAIN_8=4,
	PGA_GAIN_10=5,
	PGA_GAIN_16=6,
	PGA_GAIN_32=7,
}pgaGain_t;

extern double currentPGAGain;
extern int activePGAChannel;
void pgaSetGain(uint8_t in);
void pgaSetChannel(uint8_t newCh);


#endif