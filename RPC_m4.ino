#include "Arduino.h"
#include "RPC.h"


#define SERIAL_BR 115200
#define EIDSP_QUANTIZE_FILTERBANK   0
#include <training_kws_inference.h>

// This variable will hold the recorded audio.
// Ideally this would only hold the features extracted in this core, but we have to support serial training on the M7, and that would require that bot cores knew how to extract the features
struct sharedMemory {
  // int var; // Required when a struct contains a flexible array member
  int16_t audio_input_buffer[EI_CLASSIFIER_RAW_SAMPLE_COUNT];
};
/* Datasheet: https://www.st.com/resource/en/reference_manual/dm00176879-stm32h745-755-and-stm32h747-757-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf
   In this region (SRAM3) we can use 32767 bytes (32kB)
   "AHB SRAM3 is mapped at address 0x3004 0000 and accessible by all system
    masters except BDMA through D2 domain AHB matrix. AHB SRAM3 can be used
    as buffers to store peripheral input/output data for Ethernet and USB, or as shared
    memory between the two cores."
*/
volatile struct sharedMemory * const shared_ptr = (struct sharedMemory *)0x30040000;

const uint8_t button_1 = 2;
const uint8_t button_2 = 3;
const uint8_t button_3 = 4;
const uint8_t button_4 = 5;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(button_1, INPUT);
  pinMode(button_2, INPUT);
  pinMode(button_3, INPUT);
  pinMode(button_4, INPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);

  RPC.begin();
  Serial.begin(SERIAL_BR);
#ifdef CORE_CM7
  setup_m7();
#else
  Serial.println("Calling setup_m4");
  setup_m4();
#endif
}

void loop() {
#ifdef CORE_CM4
  loop_m4();
#else
  loop_m7();
#endif
}
