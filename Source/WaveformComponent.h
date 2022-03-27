#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class WaveformComponent  : public juce::Component
{
public:
    WaveformComponent(SparklerAutoPanAudioProcessor& p);
    ~WaveformComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;   

private:
    SparklerAutoPanAudioProcessor& audioProcessor;

    static int const height = 150, width = 550;
    int waveformXPos = height / 2;
    int ampMultiplier = 150;

    int ampArray[width] = { 0 };
    int ampArrayLatency[width] = { 0 };
    bool transArray[width] = { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformComponent)
};
