#include "Arduino.h"
#include "RPC.h"

struct sharedMemory {
  int var; // Required when a struct contains a flexible array member
  float buf[];
};

/* Datasheet: https://www.st.com/resource/en/reference_manual/dm00176879-stm32h745-755-and-stm32h747-757-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf
   In this region (SRAM3) we can use 32767 bytes (32kB)
   "AHB SRAM3 is mapped at address 0x3004 0000 and accessible by all system
    masters except BDMA through D2 domain AHB matrix. AHB SRAM3 can be used
    as buffers to store peripheral input/output data for Ethernet and USB, or as shared
    memory between the two cores."
*/
volatile struct sharedMemory * const shared_ptr = (struct sharedMemory *)0x30040000;

void setup() {
  RPC.begin();
  Serial.begin(115200);
  #ifdef CORE_CM7
    setup_m7();
  #else
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
