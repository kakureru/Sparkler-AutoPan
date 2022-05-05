#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveformComponent.h"
#include "PanComponent.h"

class SparklerAutoPanAudioProcessorEditor  : 
    public juce::AudioProcessorEditor,
    public juce::Timer,
    public juce::Slider::Listener
{
public:
    SparklerAutoPanAudioProcessorEditor (SparklerAutoPanAudioProcessor&);
    ~SparklerAutoPanAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;

private:
    SparklerAutoPanAudioProcessor& audioProcessor;

    WaveformComponent waveformComponent;
    PanComponent panComponent;

    juce::TextButton bypassButton{ "bypass" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassButtonAttachment;

    juce::Slider sensitivitySlider, sharpnessSlider, peakLengthSlider, panModelSlider, speedSlider, widthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sensitivitySliderAttachment,
        sharpnessSliderAttachment, peakLengthSliderAttachment, panModelSliderAttachment, speedSliderAttachment, 
        widthSliderAttachment;

    static int const pluginHeight = 370, pluginWidth = 550;
    int topSize = 40, knobSize = 80, margin = 10;

    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SparklerAutoPanAudioProcessorEditor)
};
