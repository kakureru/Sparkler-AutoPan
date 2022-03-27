#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SparklerAutoPanAudioProcessorEditor::SparklerAutoPanAudioProcessorEditor (SparklerAutoPanAudioProcessor& p): AudioProcessorEditor (&p), audioProcessor (p)
{
    // attachments
    bypassButtonAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "BYPASS",      bypassButton);
    sensitivitySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SENSITIVITY", sensitivitySlider);
    sharpnessSliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SHARPNESS",   sharpnessSlider);
    peakLengthSliderAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "PEAK LENGTH", peakLengthSlider);
    panModelSliderAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "PAN MODEL",   panModelSlider);
    speedSliderAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SPEED",       speedSlider);
    widthSliderAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "WIDTH",       widthSlider);

    // slider style
    sensitivitySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    sharpnessSlider  .setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);    
    peakLengthSlider .setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);    
    panModelSlider   .setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    speedSlider      .setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    widthSlider      .setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);

    sharpnessSlider.setRange(0.5f, 1.f, 0.01f);

    panModelSlider.setValue(0);

    // textbox style
    sensitivitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    sharpnessSlider  .setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    peakLengthSlider .setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    panModelSlider   .setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    speedSlider      .setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    widthSlider      .setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    widthSlider      .setTextValueSuffix("%");
    
    // listeners
    sensitivitySlider.addListener(this);
    sharpnessSlider  .addListener(this);
    peakLengthSlider .addListener(this);
    panModelSlider   .addListener(this);
    speedSlider      .addListener(this);
    widthSlider      .addListener(this);

    // set labels text
    sensitivityLabel.setText("sensitivity", juce::dontSendNotification);
    sharpnessLabel  .setText("sharpness",   juce::dontSendNotification);
    pingPongLabel   .setText("ping pong",   juce::dontSendNotification);
    randomLabel     .setText("random",      juce::dontSendNotification);
    speedLabel      .setText("speed",       juce::dontSendNotification);
    widthLabel      .setText("width",       juce::dontSendNotification);

    // add and make visible  

    //addAndMakeVisible(waveformComponent);

    addAndMakeVisible(bypassButton);

    addAndMakeVisible(sensitivitySlider);
    addAndMakeVisible(sharpnessSlider);
    addAndMakeVisible(peakLengthSlider);
    addAndMakeVisible(panModelSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(widthSlider);

    addAndMakeVisible(sensitivityLabel);
    addAndMakeVisible(sharpnessLabel);
    addAndMakeVisible(pingPongLabel);
    addAndMakeVisible(randomLabel);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(widthLabel);
    
    juce::HighResolutionTimer::startTimer(8);

    setSize (pluginWidth, pluginHeight);
    setResizable(false, false);
}

SparklerAutoPanAudioProcessorEditor::~SparklerAutoPanAudioProcessorEditor()
{
    juce::HighResolutionTimer::stopTimer();
}

//==============================================================================
void SparklerAutoPanAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(33, 33, 33));
    g.setColour (juce::Colours::white);
    g.setFont (24.0f);
    g.drawFittedText("Sparkler AutoPan", 16, 10, 220, 20, juce::Justification::bottomLeft, 1);

    // waveform
    paintHistogram(g);
}

void SparklerAutoPanAudioProcessorEditor::resized()
{
    int bigSliderSize = 91;
    int margin = 10;

    //waveformComponent.setTopLeftPosition(0, 43);

    bypassButton     .setBounds(470, 10, 57, 26);
    sensitivitySlider.setBounds(bigSliderSize * 0, getHeight() - bigSliderSize - margin, bigSliderSize, bigSliderSize);
    sharpnessSlider  .setBounds(bigSliderSize * 1, getHeight() - bigSliderSize - margin, bigSliderSize, bigSliderSize);
    peakLengthSlider .setBounds(bigSliderSize * 2, getHeight() - bigSliderSize - margin, bigSliderSize, bigSliderSize);
    panModelSlider   .setBounds(bigSliderSize * 3, getHeight() - bigSliderSize - margin, bigSliderSize, bigSliderSize - 32);
    speedSlider      .setBounds(bigSliderSize * 4, getHeight() - bigSliderSize - margin, bigSliderSize, bigSliderSize);
    widthSlider      .setBounds(bigSliderSize * 5, getHeight() - bigSliderSize - margin, bigSliderSize, bigSliderSize);

    sensitivityLabel .setBounds(28 + bigSliderSize * 0, getHeight() - bigSliderSize - margin * 3, 80, 16);
    sharpnessLabel   .setBounds(28 + bigSliderSize * 1, getHeight() - bigSliderSize - margin * 3, 80, 16);    
    pingPongLabel    .setBounds(28 + bigSliderSize * 3, getHeight() - bigSliderSize - margin * 3, 80, 16);
    randomLabel      .setBounds(28 + bigSliderSize * 3, getHeight()                 - margin * 3, 80, 16);
    speedLabel       .setBounds(28 + bigSliderSize * 4, getHeight() - bigSliderSize - margin * 3, 80, 16);
    widthLabel       .setBounds(28 + bigSliderSize * 5, getHeight() - bigSliderSize - margin * 3, 80, 16);
}

void SparklerAutoPanAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &sensitivitySlider)
    {
        audioProcessor.sensitivity = std::abs(audioProcessor.apvts.getRawParameterValue("SENSITIVITY")->load() - 1.f) * audioProcessor.sensitivityMultiplier;
    }
    if (slider == &sharpnessSlider)
    {
        audioProcessor.latency = audioProcessor.apvts.getRawParameterValue("SHARPNESS")->load();
    }
    if (slider == &peakLengthSlider)
    {
        audioProcessor.peakLength = audioProcessor.apvts.getRawParameterValue("PEAK LENGTH")->load() * audioProcessor.sampleRateValue;
    }
    if (slider == &panModelSlider)
    {
        audioProcessor.panModel = audioProcessor.apvts.getRawParameterValue("PAN MODEL")->load();
    }
    if (slider == &speedSlider)
    {
        audioProcessor.speed = audioProcessor.apvts.getRawParameterValue("SPEED")->load() * audioProcessor.speedMultiplier;
    }
    if (slider == &widthSlider)
    {
        audioProcessor.width = audioProcessor.apvts.getRawParameterValue("WIDTH")->load();
    }
}

void SparklerAutoPanAudioProcessorEditor::hiResTimerCallback()
{
    repaint( 0, 43, pluginWidth, 150);
}

void SparklerAutoPanAudioProcessorEditor::paintHistogram(juce::Graphics& g)
{
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
