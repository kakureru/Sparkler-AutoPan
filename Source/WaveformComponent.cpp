#include <JuceHeader.h>
#include "WaveformComponent.h"
#include "PluginProcessor.h"

//==============================================================================
WaveformComponent::WaveformComponent(SparklerAutoPanAudioProcessor& p): audioProcessor(p)
{
    setSize(550, 150);
}

WaveformComponent::~WaveformComponent()
{
}

void WaveformComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    // moving histogram
    for (int i = 0; i < pluginWidth - 1; i++)
    {
        ampArray[i] = ampArray[i + 1];
        ampArrayLatency[i] = ampArrayLatency[i + 1];
        transArray[i] = transArray[i + 1];
    }

    // new values
    amp = audioProcessor.rmsLevel.getCurrentValue() * ampMultiplier;
    ampLatency = audioProcessor.rmsLevelLatency.getCurrentValue() * ampMultiplier;
    ampArray[pluginWidth - 1] = amp;
    ampArrayLatency[pluginWidth - 1] = ampLatency;
    transArray[pluginWidth - 1] = audioProcessor.trans;

    for (int i = 0; i < pluginWidth; i++)
    {
        if (transArray[i] == 1)
            g.setColour(juce::Colour::fromRGB(255, 111, 0));    // transient        
        else
            g.setColour(juce::Colour::fromRGB(66, 66, 66));     // envelope follower

        g.fillRect(i, waveformXPos - ampArray[i], 1, ampArray[i] * 2);

        // envelope follower with latency
        g.setColour(juce::Colour::fromRGB(0, 188, 212));
        g.fillRect(i, waveformXPos - ampArrayLatency[i], 1, 2);
    }
}

void WaveformComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
