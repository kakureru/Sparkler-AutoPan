#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <random>

//==============================================================================
SparklerAutoPanAudioProcessor::SparklerAutoPanAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{    
}

SparklerAutoPanAudioProcessor::~SparklerAutoPanAudioProcessor()
{
}

//==============================================================================
const juce::String SparklerAutoPanAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SparklerAutoPanAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SparklerAutoPanAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SparklerAutoPanAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SparklerAutoPanAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SparklerAutoPanAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SparklerAutoPanAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SparklerAutoPanAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SparklerAutoPanAudioProcessor::getProgramName (int index)
{
    return {};
}

void SparklerAutoPanAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SparklerAutoPanAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    rmsLevel.reset(sampleRate, 0.1);
    rmsLevel.setCurrentAndTargetValue(-100.f);

    sampleRateValue = sampleRate / 1000;

    sensitivity = std::abs(apvts.getRawParameterValue("SENSITIVITY")->load() - 1.f) * sensitivityMultiplier;
    latency     = apvts.getRawParameterValue("SHARPNESS")->load();
    peakLength  = apvts.getRawParameterValue("PEAK LENGTH")->load() * sampleRateValue;
    panModel = apvts.getRawParameterValue("PAN MODEL")->load();
    speed       = apvts.getRawParameterValue("SPEED")->load() * speedMultiplier;
    width       = apvts.getRawParameterValue("WIDTH")->load();
}

void SparklerAutoPanAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SparklerAutoPanAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SparklerAutoPanAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = buffer.getWritePointer(1);

    rmsLevel.skip(mDetectionLength);

    // rms follower
    const auto value = (buffer.getRMSLevel(0, 0, mDetectionLength) + buffer.getRMSLevel(1, 0, mDetectionLength)) / 2;
    if (value < rmsLevel.getCurrentValue())
    {
        rmsLevel.setTargetValue(value);       
    }        
    else
    {
        rmsLevel.setCurrentAndTargetValue(value);       
    }   

    // rms follower with latency
    const auto valueLatency = (1 - latency) * rmsLevel.getCurrentValue() + latency * rmsLevelLatency.getCurrentValue();
    if (valueLatency < rmsLevelLatency.getCurrentValue())
    {
        rmsLevelLatency.setTargetValue((1 - 0.1) * rmsLevel.getCurrentValue() + 0.1 * rmsLevelLatency.getCurrentValue());
    }
    else
    {
        rmsLevelLatency.setTargetValue(valueLatency);
    }       

    // transient detection   
    if (currentPeakLength > peakLength)
    {        
        if (rmsLevel.getCurrentValue() - rmsLevelLatency.getCurrentValue() >= sensitivity)
        {
            trans = 1;
            currentPeakLength = 0;
            currentPosition = position;
            // random position            
            if (panModel == 0)
            {
                std::random_device random_device;                                                           // Источник энтропии
                std::mt19937 generator(random_device());                                                    // Генератор случайных чисел
                std::uniform_int_distribution<> distribution(45 - width * 45 / 100, 45 + width * 45 / 100); // Распределение
                int r = distribution(generator);                                                            // Случайное число
                position = (r * juce::double_Pi) / 180;

            }
            // ping pong position 
            else
            {
                if (positionChannel == 0)
                {
                    position = juce::double_Pi / 4 - width * juce::double_Pi / 400;
                    positionChannel = 1;
                }
                else
                {
                    position = juce::double_Pi / 4 + width * juce::double_Pi / 400;
                    positionChannel = 0;
                }
            }

            radsPerSample = (std::abs(currentPosition - position)) / speed;
        }  
        else
            trans = 0;
    }      
    
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        currentPeakLength++;
        // panning
        if (currentPosition < position)
            currentPosition += radsPerSample;
        else if (currentPosition > position)
            currentPosition -= radsPerSample;

        channelDataL[i] = channelDataL[i] * std::sin(currentPosition) * panLowCompensation;
        channelDataR[i] = channelDataR[i] * std::cos(currentPosition) * panLowCompensation;
    }
}

//==============================================================================
bool SparklerAutoPanAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SparklerAutoPanAudioProcessor::createEditor()
{
    return new SparklerAutoPanAudioProcessorEditor (*this);
}

//==============================================================================
void SparklerAutoPanAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SparklerAutoPanAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout SparklerAutoPanAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterBool> ("BYPASS",     "bypass",        false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("SENSITIVITY","sensitivity",   0.1f, 1.0f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("SHARPNESS",  "sharpness",     0.5f, 1.f, 0.9f));
    params.push_back(std::make_unique<juce::AudioParameterInt>  ("PEAK LENGTH","peak length",   10, 500, 200));
    params.push_back(std::make_unique<juce::AudioParameterInt>  ("PAN MODEL",  "pan model",     0, 1, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>  ("SPEED",      "speed",         1, 100, 50));
    params.push_back(std::make_unique<juce::AudioParameterInt>  ("WIDTH",      "width",         1, 100, 75));    

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SparklerAutoPanAudioProcessor();
}
