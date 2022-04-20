// This code is a modification of the code from http://robotics.hobbizine.com/arduinoann.html

#include <arduino.h>
#include "neural_network.h"
#include <math.h>

void NeuralNetwork::initialize(float LearningRate, float Momentum, int DropoutRate) {
    this->LearningRate = LearningRate;
    this->Momentum = Momentum;
    this->DropoutRate = DropoutRate;

    for (int i = 0; i < (InputNodes+1) * HiddenNodes; ++i) {
      HiddenWeights[i] = random(InitialWeightMin*100, InitialWeightMax*100);
    }

    for (int i = 0; i < (HiddenNodes+1) * OutputNodes; ++i) {
      OutputWeights[i] = random(InitialWeightMin*100, InitialWeightMax*100);
    }
}

float NeuralNetwork::forward(const float Input[], const float Target[]){
    float error = 0;

    /******************************************************************
    * Compute hidden layer activations
    ******************************************************************/
    for (int i = 0; i < HiddenNodes; i++) {
        float Accum = HiddenWeights[InputNodes*HiddenNodes + i] / 100.f;
        for (int j = 0; j < InputNodes; j++) {
            Accum += Input[j] * (HiddenWeights[j*HiddenNodes + i] / 100.f);
        }
        Hidden[i] = 1.0 / (1.0 + exp(-Accum));
    }

    /******************************************************************
    * Compute output layer activations and calculate errors
    ******************************************************************/
    for (int i = 0; i < OutputNodes; i++) {
        float Accum = OutputWeights[HiddenNodes*OutputNodes + i] / 100.f;
        for (int j = 0; j < HiddenNodes; j++) {
            Accum += Hidden[j] * OutputWeights[j*OutputNodes + i] / 100.f;
        }
        Output[i] = 1.0 / (1.0 + exp(-Accum));
        // OutputDelta[i] = (Target[i] - Output[i]) * Output[i] * (1.0 - Output[i]);
        error += 0.33333 * (Target[i] - Output[i]) * (Target[i] - Output[i]);
    }
    return error;
}

// Input will be changed!!
float NeuralNetwork::backward(float Input[], const float Target[]){
    float error = 0;

    for (int i = 0; i < InputNodes; i++) {
        if (rand() % 100 < this->DropoutRate) {
            Input[i] = 0;
        }
    }

    // Forward
    /******************************************************************
    * Compute hidden layer activations
    ******************************************************************/
    for (int i = 0; i < HiddenNodes; i++) {
        float Accum = HiddenWeights[InputNodes*HiddenNodes + i] / 100.f;
        for (int j = 0; j < InputNodes; j++) {
            Accum += Input[j] * (HiddenWeights[j*HiddenNodes + i] / 100.f);
        }
        Hidden[i] = 1.0 / (1.0 + exp(-Accum));
    }

    /******************************************************************
    * Compute output layer activations and calculate errors
    ******************************************************************/
    for (int i = 0; i < OutputNodes; i++) {
        float Accum = OutputWeights[HiddenNodes*OutputNodes + i] / 100.f;
        for (int j = 0; j < HiddenNodes; j++) {
            Accum += Hidden[j] * (OutputWeights[j*OutputNodes + i] / 100.f);
        }
        Output[i] = 1.0 / (1.0 + exp(-Accum)); // Sigmoid, from 0 to 1
        OutputDelta[i] = (Target[i] - Output[i]) * Output[i] * (1.0 - Output[i]);
        Serial.print("OutputDelta "); Serial.print(i); Serial.print(": "); Serial.println(OutputDelta[i]);
        Serial.print("OutputDelta "); Serial.print(i); Serial.print(": "); Serial.println(OutputDelta[i]);
        Serial.print("OutputAccoum "); Serial.print(i); Serial.print(": "); Serial.println(Accum);
        error += 1/OutputNodes * (Target[i] - Output[i]) * (Target[i] - Output[i]);
    }
    // End forward

    // Backward
    /******************************************************************
    * Backpropagate errors to hidden layer
    ******************************************************************/
    for(int i = 0 ; i < HiddenNodes ; i++ ) {    
        float Accum = 0.0 ;
        for(int j = 0 ; j < OutputNodes ; j++ ) {
            Accum += (OutputWeights[i*OutputNodes + j] / 100.f) * OutputDelta[j] ;
        }
        HiddenDelta[i] = Accum * Hidden[i] * (1.0 - Hidden[i]) ;
    }

    /******************************************************************
    * Update Inner-->Hidden Weights
    ******************************************************************/
    for(int i = 0 ; i < HiddenNodes ; i++ ) {     
        ChangeHiddenWeights[InputNodes*HiddenNodes + i] = (LearningRate * HiddenDelta[i] + Momentum * (ChangeHiddenWeights[InputNodes*HiddenNodes + i] / 100.f)) * 100.f ;
        HiddenWeights[InputNodes*HiddenNodes + i] += ChangeHiddenWeights[InputNodes*HiddenNodes + i];
        for(int j = 0 ; j < InputNodes ; j++ ) { 
            ChangeHiddenWeights[j*HiddenNodes + i] = (LearningRate * Input[j] * HiddenDelta[i] + Momentum * (ChangeHiddenWeights[j*HiddenNodes + i] / 100.f)) * 100.f;
            HiddenWeights[j*HiddenNodes + i] += ChangeHiddenWeights[j*HiddenNodes + i];
        }
    }

    /******************************************************************
    * Update Hidden-->Output Weights
    ******************************************************************/
    for(int i = 0 ; i < OutputNodes ; i ++ ) {    
        ChangeOutputWeights[HiddenNodes*OutputNodes + i] = (LearningRate * OutputDelta[i] + Momentum * (ChangeOutputWeights[HiddenNodes*OutputNodes + i] / 100.f)) * 100.f ;
        OutputWeights[HiddenNodes*OutputNodes + i] += ChangeOutputWeights[HiddenNodes*OutputNodes + i];
        for(int j = 0 ; j < HiddenNodes ; j++ ) {
            ChangeOutputWeights[j*OutputNodes + i] = (LearningRate * Hidden[j] * OutputDelta[i] + Momentum * (ChangeOutputWeights[j*OutputNodes + i] / 100.f)) * 100.f ;
            OutputWeights[j*OutputNodes + i] += ChangeOutputWeights[j*OutputNodes + i];
        }
    }

    return error;
}


float* NeuralNetwork::get_output(){
    return Output;
}

int* NeuralNetwork::get_HiddenWeights(){
    return HiddenWeights;
}

int* NeuralNetwork::get_OutputWeights(){
    return OutputWeights;
}

float NeuralNetwork::get_error(){
    return Error;
}
