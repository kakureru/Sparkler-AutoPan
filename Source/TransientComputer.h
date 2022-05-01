#pragma once

#include <JuceHeader.h>

class TransientComputer
{
public:
    TransientComputer();
    ~TransientComputer();

    void prepare(int numChannels, int sampleRate, int detectionBlockSize);

    void reset();

    void detectTransients(juce::AudioBuffer<float>& buffer, float sensitivity, float latency, int peakLength);

    juce::LinearSmoothedValue<float> rmsLevel;
    juce::LinearSmoothedValue<float> rmsLevelLatency;
    double currentPeakLength;
    std::vector<int> transVector;
    int trans = 0;

private:
    int rmsBlock;
    int tempPosition;
    juce::AudioBuffer<float> rmsBuffer;    
};
