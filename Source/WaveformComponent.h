#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class WaveformComponent  : public juce::Component
{
public:
    WaveformComponent(SparklerAutoPanAudioProcessor& p): audioProcessor(p)
    {
        setSize(550, 150);
    }
    ~WaveformComponent() override
    {
    }

    void paint(juce::Graphics& g) override
    {
         g.fillAll(juce::Colour::fromRGB(22, 22, 22));
         // moving histogram
         for (int i = 0; i < width - 1; i++)
         {
             ampArray[i] = ampArray[i + 1];
             ampArrayLatency[i] = ampArrayLatency[i + 1];
             transArray[i] = transArray[i + 1];
         }

         // new values
         ampArray[width - 1] = audioProcessor.rmsLevel.getCurrentValue() * ampMultiplier;
         ampArrayLatency[width - 1] = audioProcessor.rmsLevelLatency.getCurrentValue() * ampMultiplier;
         transArray[width - 1] = audioProcessor.trans;

         for (int i = 0; i < width; i++)
         {
             // envelope follower
             g.setColour(juce::Colour::fromRGB(0, 188, 212));
             g.fillRect(i, waveformXPos - ampArray[i], 1, ampArray[i] * 2);

             // envelope follower with latency
             g.setColour(juce::Colour::fromRGB(66, 66, 66));
             g.fillRect(i, waveformXPos - ampArrayLatency[i], 1, ampArrayLatency[i] * 2);

             // transients
             if (transArray[i] == 1)
             {
                 g.setColour(juce::Colour::fromRGB(255, 111, 0));
                 g.fillRect(i, waveformXPos - ampArray[i], 1, ampArray[i] * 2);
             }
         }
    }
    void resized() override
    {
    }

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
