#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveformComponent.h"

class SparklerAutoPanAudioProcessorEditor  : 
    public juce::AudioProcessorEditor,
    public juce::HighResolutionTimer,
    public juce::Slider::Listener
{
public:
    SparklerAutoPanAudioProcessorEditor (SparklerAutoPanAudioProcessor&);
    ~SparklerAutoPanAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    static int const pluginHeight = 370, pluginWidth = 550;

    juce::TextButton bypassButton { "bypass" };

    juce::Slider sensitivitySlider;
    juce::Slider sharpnessSlider;
    juce::Slider peakLengthSlider;
    juce::Slider panModelSlider;
    juce::Slider speedSlider;
    juce::Slider widthSlider;

    juce::Label sensitivityLabel;
    juce::Label sharpnessLabel;
    juce::Label peakLengthLabel;
    juce::Label pingPongLabel;
    juce::Label randomLabel;
    juce::Label speedLabel;
    juce::Label widthLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sensitivitySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sharpnessSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peakLengthSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panModelSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speedSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthSliderAttachment;

    int waveformXPos = 43 + 150 / 2;
    int ampMultiplier = 120;    

    int ampArray[pluginWidth] = { 0 };
    int ampArrayLatency[pluginWidth] = { 0 };
    bool transArray[pluginWidth] = { 0 };
    int amp = 0;
    int ampLatency = 0;    

    void sliderValueChanged(juce::Slider* slider) override;
    void hiResTimerCallback() override;
    void paintHistogram(juce::Graphics&);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SparklerAutoPanAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SparklerAutoPanAudioProcessorEditor)
};
