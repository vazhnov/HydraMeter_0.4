#ifndef SQUAREWAVE_H
#define SQUAREWAVE_H

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

#define swave_wrap_target 0
#define swave_wrap 1

/*static const uint16_t swave_program_instructions[] = {
            //     .wrap_target
    0xe001, //  0: set    pins, 1                    
    0xe002, //  1: set    pins, 2                    
            //     .wrap
};

static const struct pio_program swave_program = {
    .instructions = swave_program_instructions,
    .length = 2,
    .origin = -1,
};
*/
static inline pio_sm_config swave_program_get_default_config(uint offset);

//pio_sm_config pioConfigC;

static inline void swave_program_init(PIO pio, uint sm, uint offset, uint pin, float clk_div);

//PIO squareWavePIO;
//uint squareWaveOffset;
//uint squareWaveSm;
void createFreqOnPin(double frequency, int pin);
void changeFreqOnPin(double frequency);

#endif