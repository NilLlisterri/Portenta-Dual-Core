#ifndef NEURAL_NETWORK
#define NEURAL_NETWORK


/******************************************************************
 * Network Configuration - customized per network 
 ******************************************************************/

static const int PatternCount = 3;
static const int InputNodes = 650;
static const int HiddenNodes = 25;
static const int OutputNodes = 3;
static const float InitialWeightMax = 0.5;

class NeuralNetwork {
    public:

        void initialize(float LearningRate, float Momentum, int DropoutRate);
        // ~NeuralNetwork();

        // void initWeights();
        float forward(const float Input[], const float Target[]);
        float backward(float Input[], const float Target[]); // Input will be changed!!

        float* get_output();

        float* get_HiddenWeights();
        float* get_OutputWeights();

        float get_error();
        // float asd[500] = {};
        
    private:

        float Hidden[HiddenNodes] = {};
        float Output[OutputNodes] = {};
        float HiddenWeights[(InputNodes+1) * HiddenNodes] = {};
        float OutputWeights[(HiddenNodes+1) * OutputNodes] = {};
        float HiddenDelta[HiddenNodes] = {};
        float OutputDelta[OutputNodes] = {};
        float ChangeHiddenWeights[(InputNodes+1) * HiddenNodes] = {};
        float ChangeOutputWeights[(HiddenNodes+1) * OutputNodes] = {};

        float Error;
        float LearningRate = 0.3;
        float Momentum = 0.9;
        // From 0 to 100, the percentage of input features that will be set to 0
        int DropoutRate = 0;
};


#endif
