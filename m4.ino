#ifdef CORE_CM4

#include <PDM.h>

static signed short sampleBuffer[2048]; // PDM sample buffer

// Change for shared_ptr->audio_input_buffer
/** Audio buffers, pointers and selectors */
typedef struct {
  uint8_t buf_ready;
  uint32_t buf_count;
  uint32_t n_samples;
} inference_t;

static inference_t inference;

void setup_m4() {
  RPC.println("Setup M4");
  if (microphone_inference_start(EI_CLASSIFIER_RAW_SAMPLE_COUNT) == false) {
    ei_printf("ERR: Failed to setup audio sampling\r\n");
    return;
  }
}

void loop_m4() {
  RPC.println("In loop_m4");
  delay(3000);

  digitalWrite(LED_BUILTIN, HIGH);

  RPC.println("Recording...");
  bool m = microphone_inference_record();
  if (!m) {
    RPC.println("ERR: Failed to record audio...");
    return;
  }
  RPC.println("Recording done");
  digitalWrite(LED_BUILTIN, LOW);

  RPC.println("Value set, asking M7");
  //RPC.call("train", 1, false);
}


static bool microphone_inference_start(uint32_t n_samples) {
  inference.buf_count  = 0;
  inference.n_samples  = n_samples;
  inference.buf_ready  = 0;

  // configure the data receive callback
  PDM.onReceive(&pdm_data_ready_inference_callback);

  // optionally set the gain, defaults to 20
  PDM.setGain(80);
  PDM.setBufferSize(4096);

  // initialize PDM with one channel (mono mode) and 16 kHz sample rate
  if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY)) {
    ei_printf("Failed to start PDM!");
    PDM.end();
    return false;
  }

  return true;
}

static void pdm_data_ready_inference_callback(void) {
  int bytesAvailable = PDM.available();

  // read into the sample buffer
  int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);

  if (inference.buf_ready == 0) {
    for (int i = 0; i < bytesRead >> 1; i++) {
      shared_ptr->audio_input_buffer[inference.buf_count++] = sampleBuffer[i];

      if (inference.buf_count >= inference.n_samples) {
        inference.buf_count = 0;
        inference.buf_ready = 1;
        break;
      }
    }
  }
}

static bool microphone_inference_record(void) {
  inference.buf_ready = 0;
  inference.buf_count = 0;
  while (inference.buf_ready == 0) {
    RPC.print("[M4] Waiting for buffer to fill...");
    delay(10);
  }
  return true;
}


#endif
