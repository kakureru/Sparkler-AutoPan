#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class SparklerAutoPanAudioProcessor :   public juce::AudioProcessor
{
public:
    //==============================================================================
    SparklerAutoPanAudioProcessor();
    ~SparklerAutoPanAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;

    juce::LinearSmoothedValue<float> rmsLevel;
    juce::LinearSmoothedValue<float> rmsLevelLatency;
    bool trans = 0;

    int speedMultiplier = 1000;
    float sensitivityMultiplier = 0.2f;

    // parameters
    float sensitivity;
    float latency;
    int peakLength;
    int panModel = 0;
    float speed;
    int width;
    
    double currentPeakLength = 1100000;
    double sampleRateValue;
                     
    int mDetectionLength = 128;

    float position = juce::double_Pi / 4;
    float currentPosition = position;
    float radsPerSample = 0;
    bool positionChannel = 0;
    float panLowCompensation = 2 / (std::sin(juce::double_Pi / 4) + std::cos(juce::double_Pi / 4));

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();   

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SparklerAutoPanAudioProcessor)
};
