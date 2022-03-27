#include <JuceHeader.h>
#include "WaveformComponent.h"
#include "PluginProcessor.h"

//==============================================================================
WaveformComponent::WaveformComponent(SparklerAutoPanAudioProcessor& p): audioProcessor(p)
{
    setSize(width, height);
}

WaveformComponent::~WaveformComponent()
{
}

void WaveformComponent::paint (juce::Graphics& g)
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

void WaveformComponent::resized()
{
}
