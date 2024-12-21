#ifndef PINS_H
#define PINS_H

#define ADC_CS 17
#define PGA_CS 20
#define ADC_MCLK 3

#define SR_RCK 13
#define SPI1_SCK 10
#define SPI1_MOSI 11
#define SPI1_MISO 12
#define RADIO_CS 14
#define RADIO_DO0 15
#define INMOD_CS 7
#define ONEWIRE 6
#define ONEWIRE_BRD 2

#define SDA_PIN 8
#define SCL_PIN 9

#define U0TX 0
#define U0RX 1
#define MTX 4
#define MRX 5

#define BATTERY_EN 22
#define D5V_ISO_EN 21

#define D5V_ISO_SENSE_ANIN 26
#define D5V_ISO_SENSE_CAL (((double)840)/330)
#define HV_SENSE_ANIN 27
#define HV_SENSE_ANIN_CAL (((double)133)/33)
#define VBAT_SENSE_ANIN 28 
#define VBAT_SENSE_ANIN_CAL (((double)840)/330)

#define ATTEN_147_SRPIN 8
#define ATTEN_11_SRPIN 9

#define OHM_PATH_DIS_SRPIN 0
#define OHM_BOOST_EN_SRPIN 1
#define OHMS_510_SRPIN 3
#define OHMS_10K_SRPIN 5
#define OHMS_100K_SRPIN 2
#define OHMS_1M_SRPIN 4
#define OHMS_LOWV_EN_SRPIN 6
#define OHMS_ELOWV_EN_SRPIN 19

#define MODULE_IN23SW_SRPIN 20

#define SWITCH_SRPIN_OHMS     0x0008
#define SWITCH_SRPIN_MA       0x0080

#define SWITCH_SRPIN_SW0      0x4000
#define SWITCH_SRPIN_SW1_UP   0x0800
#define SWITCH_SRPIN_SW1_DN   0x1000
#define SWITCH_SRPIN_SW2_UP   0x0200
#define SWITCH_SRPIN_SW2_DN   0x0400
#define SWITCH_SRPIN_SW3_UP   0x0010
#define SWITCH_SRPIN_SW3_DN   0x0100
#define SWITCH_SRPIN_SW4_UP   0x0040
#define SWITCH_SRPIN_SW4_DN   0x0020
#define SWITCH_SRPIN_SW6_UP   0x0001
#define SWITCH_SRPIN_SW6_DN   0x2000
#define SWITCH_SRPIN_SW7      0x0002
#define SWITCH_SRPIN_SW8      0x0004
#endif