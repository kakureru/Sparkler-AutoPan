#pragma once
#include <JuceHeader.h>
#include <vector>

class Panner
{
public:
    Panner();
    ~Panner();

    void pan(juce::AudioBuffer<float>& buffer, std::vector<int> transVector, int width, int speed, int panModel);
    float position = juce::double_Pi / 4;
    float currentPosition = position;

private:    
    float radsPerSample = 0;
    bool positionChannel = 0;
    float panLowCompensation = 2 / (std::sin(juce::double_Pi / 4) + std::cos(juce::double_Pi / 4));

};
