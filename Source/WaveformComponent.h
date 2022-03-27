#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class WaveformComponent  : public juce::Component
{
public:
    WaveformComponent(SparklerAutoPanAudioProcessor&);
    ~WaveformComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;   

private:
    static int const pluginHeight = 375, pluginWidth = 550;
    int waveformXPos = 43 + 150 / 2;
    int ampMultiplier = 150;

    int ampArray[pluginWidth] = { 0 };
    int ampArrayLatency[pluginWidth] = { 0 };
    bool transArray[pluginWidth] = { 0 };
    int amp = 0;
    int ampLatency = 0;

    SparklerAutoPanAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformComponent)
};
