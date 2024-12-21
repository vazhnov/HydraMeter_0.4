#include "squarewave.h"
#include "Arduino.h"

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

//#define swave_wrap_target 0
//#define swave_wrap 1

static const uint16_t swave_program_instructions[] = {
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

static inline pio_sm_config swave_program_get_default_config(uint offset) {
  pio_sm_config c = pio_get_default_sm_config();
  sm_config_set_wrap(&c, offset + swave_wrap_target, offset + swave_wrap);
  return c;
}
pio_sm_config pioConfigC;
static inline void swave_program_init(PIO pio, uint sm, uint offset, uint pin, float clk_div){ //Program to initialize the PIO
  pio_sm_config pioConfigC = swave_program_get_default_config(offset); //Get default configurations for the PIO state machine
  sm_config_set_set_pins(&pioConfigC, pin, 1); //Set the state machine configurations on the given pin
  sm_config_set_clkdiv(&pioConfigC, clk_div); //Set the state machine clock divider
  pio_gpio_init(pio, pin); //Setup the function select for a gpio to use output from the given PIO instance
  pio_sm_set_consecutive_pindirs(pio, sm, pin,1, true); //Use a state machine to set the same pin direction for multiple consecutive pins for the PIO instance
  pio_sm_init(pio, sm, offset, &pioConfigC); //Resets the state machine to a consistent state, and configures it
  pio_sm_set_enabled(pio, sm, true); //Enable or disable a PIO state machine
}

PIO squareWavePIO;
uint squareWaveOffset;
uint squareWaveSm;
void createFreqOnPin(double frequency, int pin){
  squareWavePIO = pio0;
  frequency *= 2;
  float div = (float)(rp2040.f_cpu() / frequency); 
  squareWaveOffset = pio_add_program(squareWavePIO,&swave_program);
  squareWaveSm = pio_claim_unused_sm(squareWavePIO,true);
  swave_program_init(squareWavePIO,squareWaveSm,squareWaveOffset,3,div);
}

void changeFreqOnPin(double frequency){
	frequency *= 2;
	float div = (float)(rp2040.f_cpu() / frequency); 
	pio_sm_set_enabled(squareWavePIO,squareWaveSm, false); 
	swave_program_init(squareWavePIO,squareWaveSm,squareWaveOffset,3,div);
	//sm_config_set_clkdiv(&pioConfigC, div);
	//pio_sm_init(squareWavePIO, squareWaveSm, squareWaveOffset, &pioConfigC);
	//pio_sm_set_enabled(squareWavePIO, squareWaveSm, true);
	//Serial.printf("Freq set to %d/%f\n",rp2040.f_cpu(),div);
}
