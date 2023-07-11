#ifdef CORE_CM7

#include <PDM.h>

static signed short sampleBuffer[2048]; // PDM sample buffer

/** Structure to manage audio capture */
typedef struct {
  uint8_t buf_ready;
  uint32_t buf_count;
  uint32_t n_samples;
} recording_t;

static recording_t recording = {0, 0, EI_CLASSIFIER_RAW_SAMPLE_COUNT};

void setup_m7() {
  PDM.onReceive(on_microphone_receive);
}

void loop_m7() {
  print_m4_messages();

  digitalWrite(LED_BUILTIN, HIGH);

  int read = Serial.read();
  if (read == 't') {
    Serial.println("ok");

    while (Serial.available() < 1) {}
    uint8_t num_button = Serial.read();
    Serial.print("Button "); Serial.println(num_button);

    while (Serial.available() < 1) {}
    bool only_forward = Serial.read() == 1;
    Serial.print("Only forward "); Serial.println(only_forward);

    byte ref[2];
    for (int i = 0; i < EI_CLASSIFIER_RAW_SAMPLE_COUNT; i++) {
      while (Serial.available() < 2) {}
      Serial.readBytes(ref, 2);
      shared_ptr->audio_input_buffer[i] = 0;
      shared_ptr->audio_input_buffer[i] = (ref[1] << 8) | ref[0];
    }
    Serial.print("Sample received for button ");
    Serial.println(num_button);
    delay(100);

    // This delay is required, otherwise the program will randomly crash at one of the RPC calls (https://github.com/arduino/ArduinoCore-mbed/issues/238).
    // The grater the delay, the more reliable. 100ms works "well".
    RPC.call("train", num_button, only_forward);
  } else if (read == 'r') {
    Serial.println("Started recording");
    record();
    Serial.println("Recording done, training");
    RPC.call("train", 1, false);
  }

  /*for (int i = 0; i < 100; i++) {
    delay(9000);
    record();
    delay(100);
    RPC.call("train", 1, false);
  } */
}

// Check the RPC buffer and print it to serial
void print_m4_messages() {
  String buffer = "";
  while (RPC.available()) {
    buffer += (char)RPC.read(); // Fill the buffer with characters
  }
  if (buffer.length() > 0) {
    Serial.print(buffer);
  }
}

void record() {
  bool m = microphone_record();
  if (!m) {
    Serial.println("ERR: Failed to record audio");
    return;
  }
  Serial.println("Recording done");
}

static bool microphone_record() {
  recording.buf_ready = 0;
  recording.buf_count = 0;
  digitalWrite(LEDR, LOW);
  if (!PDM.begin(1, EI_CLASSIFIER_FREQUENCY)) {
    Serial.println("Failed to start PDM!");
    return false;
  }
  while (recording.buf_ready == 0) {
    delay(10);
  }
  PDM.end();
  digitalWrite(LEDR, HIGH);
  return true;
}

static void on_microphone_receive() {
  int bytesAvailable = PDM.available();

  // read into the sample buffer
  int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);

  if (recording.buf_ready == 0) {
    for (int i = 0; i < bytesRead >> 1; i++) {
      shared_ptr->audio_input_buffer[recording.buf_count++] = sampleBuffer[i];

      if (recording.buf_count >= recording.n_samples) {
        recording.buf_count = 0;
        recording.buf_ready = 1;
        break;
      }
    }
  }
}

#endif
