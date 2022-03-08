#ifdef CORE_CM4

#include "neural_network.h"

static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal

static NeuralNetwork myNetwork;

uint16_t num_epochs = 0;

void setup_m4() {
  RPC.bind("train", train);
  // init_network_model();
}

void loop_m4() {
  uint8_t num_button = 0;
  bool only_forward = false;

  int read = Serial.read();
  if (read == '>') { // s -> FEDERATED LEARNING
    Serial.write('<');
    digitalWrite(LED_BUILTIN, LOW);    // ON
    // delay(1000);
    if (Serial.read() == 's') {
      RPC.println("start");
      RPC.println(num_epochs);
      num_epochs = 0;

      // Sending hidden layer
      char* myHiddenWeights = (char*) myNetwork.get_HiddenWeights();
      for (uint16_t i = 0; i < (InputNodes + 1) * HiddenNodes; ++i) {
        Serial.write(myHiddenWeights + i * sizeof(float), sizeof(float));
      }
      // Sending output layer
      char* myOutputWeights = (char*) myNetwork.get_OutputWeights();
      for (uint16_t i = 0; i < (HiddenNodes + 1) * OutputNodes; ++i) {
        Serial.write(myOutputWeights + i * sizeof(float), sizeof(float));
      }

      // Receiving hidden layer
      for (uint16_t i = 0; i < (InputNodes + 1) * HiddenNodes; ++i) {
        //Serial.write('n');
        while (Serial.available() < 4) {}
        for (int n = 0; n < 4; n++) {
          myHiddenWeights[i * 4 + n] = Serial.read();
        }
      }
      // Receiving output layer
      for (uint16_t i = 0; i < (HiddenNodes + 1) * OutputNodes; ++i) {
        //Serial.write('n');
        while (Serial.available() < 4) {}
        for (int n = 0; n < 4; n++) {
          myOutputWeights[i * 4 + n] = Serial.read();
        }
      }
    }

    digitalWrite(LED_BUILTIN, HIGH);    // OFF
  }
}

/*
void init_network_model() {
  char startChar;
  do {
    startChar = Serial.read();
    RPC.println("Waiting for new model...");
    delay(100);
  } while (startChar != 's'); // s -> START

  RPC.println("start");
  float learningRate = readFloat();
  float momentum = readFloat();

  while (Serial.available() < 1) {}
  int dropoutRate = Serial.read();

  float seed = readFloat();
  srand(seed);

  myNetwork.initialize(learningRate, momentum, dropoutRate);

  char* myHiddenWeights = (char*) myNetwork.get_HiddenWeights();
  for (uint16_t i = 0; i < (InputNodes + 1) * HiddenNodes; ++i) {
    //Serial.write('n');
    while (Serial.available() < 4) {}
    for (int n = 0; n < 4; n++) {
      myHiddenWeights[i * 4] = Serial.read();
    }
  }

  char* myOutputWeights = (char*) myNetwork.get_OutputWeights();
  for (uint16_t i = 0; i < (HiddenNodes + 1) * OutputNodes; ++i) {
    //Serial.write('n');
    while (Serial.available() < 4) {}
    for (int n = 0; n < 4; n++) {
      myOutputWeights[i * 4 + n] = Serial.read();
    }
  }

  RPC.println("Received new model.");
}

float readFloat() {
  byte res[4];
  while (Serial.available() < 4) {}
  for (int n = 0; n < 4; n++) {
    res[n] = Serial.read();
  }
  return *(float *)&res;
}*/

/**
   Obtains features from the audio stored in the shared_ptr->audio_input_buffer (see get_input_data)
*/
void train(int nb, bool only_forward) {
  digitalWrite(LEDB, LOW);
  signal_t signal;
  signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
  signal.get_data = &get_input_data;
  ei::matrix_t features_matrix(1, EI_CLASSIFIER_NN_INPUT_FRAME_SIZE);

  EI_IMPULSE_ERROR r = get_one_second_features(&signal, &features_matrix, debug_nn);
  if (r != EI_IMPULSE_OK) {
    RPC.print("ERR: Failed to get features: ");
    RPC.println(r);
    return;
  }

  float myTarget[3] = {0};
  myTarget[nb - 1] = 1.f; // button 1 -> {1,0,0};  button 2 -> {0,1,0};  button 3 -> {0,0,1}

  // FORWARD
  float forward_error = myNetwork.forward(features_matrix.buffer, myTarget);

  float backward_error = 0;
  if (!only_forward) {
    // BACKWARD
    backward_error = myNetwork.backward(features_matrix.buffer, myTarget);
    ++num_epochs;
  }

  // Info to plot & graph
  RPC.println("graph");
  for (int i = 0; i < OutputNodes; i++) { // Print outputs
    RPC.print(myNetwork.get_output()[i]);
    RPC.print("|");
  }
  RPC.println();
  RPC.println(forward_error);
  RPC.println(num_epochs, DEC);
  RPC.println(nb, DEC);

  digitalWrite(LEDB, HIGH);
}


void ei_printf(const char *format, ...) {
  static char print_buf[1024] = { 0 };

  va_list args;
  va_start(args, format);
  int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
  va_end(args);

  if (r > 0) {
    Serial.write(print_buf);
  }
}

// I don't like this cast to non-volatile...
static int get_input_data(size_t offset, size_t length, float *out_ptr) {
  numpy::int16_to_float((const int16_t*)&shared_ptr->audio_input_buffer[offset], out_ptr, length);
  return 0;
}


#endif
