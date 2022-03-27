#include "PluginProcessor.h"
#include "PluginEditor.h"

//===========================================================================================================
SparklerAutoPanAudioProcessorEditor::SparklerAutoPanAudioProcessorEditor (SparklerAutoPanAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), waveformComponent(p),
    sensitivitySlider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow),
    sharpnessSlider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow),
    peakLengthSlider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow),
    panModelSlider(juce::Slider::LinearVertical, juce::Slider::NoTextBox),
    speedSlider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow),
    widthSlider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow)
{
    // attachments
    bypassButtonAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "BYPASS",      bypassButton);
    sensitivitySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SENSITIVITY", sensitivitySlider);
    sharpnessSliderAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SHARPNESS",   sharpnessSlider);
    peakLengthSliderAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "PEAK LENGTH", peakLengthSlider);
    panModelSliderAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "PAN MODEL",   panModelSlider);
    speedSliderAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SPEED",       speedSlider);
    widthSliderAttachment       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "WIDTH",       widthSlider);

    panModelSlider.setValue(0);
    widthSlider.setTextValueSuffix("%");
    
    // listeners
    sensitivitySlider.addListener(this);
    sharpnessSlider  .addListener(this);
    peakLengthSlider .addListener(this);
    panModelSlider   .addListener(this);
    speedSlider      .addListener(this);
    widthSlider      .addListener(this);

    // add and make visible  

    addAndMakeVisible(bypassButton);

    addAndMakeVisible(sensitivitySlider);
    addAndMakeVisible(sharpnessSlider);
    addAndMakeVisible(peakLengthSlider);
    addAndMakeVisible(panModelSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(widthSlider);

    addAndMakeVisible(waveformComponent);
    
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

    auto bounds = getLocalBounds();
    bounds.removeFromLeft(margin);
    auto labelRow = bounds.removeFromBottom(130);
    g.setFont(14.0f);

    g.drawFittedText("sensitivity", labelRow.removeFromLeft(knobSize), 12, juce::Justification::centred, 1);
    labelRow.removeFromLeft(margin);
    g.drawFittedText("sharpness", labelRow.removeFromLeft(knobSize), 12, juce::Justification::centred, 1);
    labelRow.removeFromLeft(margin);
    g.drawFittedText("peak length", labelRow.removeFromLeft(knobSize), 12, juce::Justification::centred, 1);
    labelRow.removeFromLeft(margin);
    g.drawFittedText("ping pong", labelRow.removeFromLeft(knobSize), 12, juce::Justification::centred, 1);
    g.drawFittedText("random", knobSize * 3 + margin * 4, getHeight() - 30, knobSize, 14, juce::Justification::centred, 1);
    labelRow.removeFromLeft(margin);
    g.drawFittedText("speed", labelRow.removeFromLeft(knobSize), 12, juce::Justification::centred, 1);
    labelRow.removeFromLeft(margin);
    g.drawFittedText("width", labelRow.removeFromLeft(knobSize), 12, juce::Justification::centred, 1);
}

void SparklerAutoPanAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    bounds.removeFromLeft(margin);
    bounds.removeFromBottom(margin);
    auto row = bounds.removeFromBottom(100);

    sensitivitySlider.setBounds(row.removeFromLeft(knobSize));
    row.removeFromLeft(margin);
    sharpnessSlider.setBounds(row.removeFromLeft(knobSize));
    row.removeFromLeft(margin);
    peakLengthSlider.setBounds(row.removeFromLeft(knobSize));
    row.removeFromLeft(margin);
    panModelSlider.setBounds(knobSize * 3 + margin * 4, getHeight() - 110, knobSize, 75);
    row.removeFromLeft(knobSize);
    row.removeFromLeft(margin);
    speedSlider.setBounds(row.removeFromLeft(knobSize));
    row.removeFromLeft(margin);
    widthSlider.setBounds(row.removeFromLeft(knobSize));

    waveformComponent.setTopLeftPosition(0, topSize);
    bypassButton.setBounds(480, 7, 60, 26);
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
        audioProcessor.speed = std::abs(audioProcessor.apvts.getRawParameterValue("SPEED")->load() - 101) * audioProcessor.speedMultiplier;
    }
    if (slider == &widthSlider)
    {
        audioProcessor.width = audioProcessor.apvts.getRawParameterValue("WIDTH")->load();
    }
}

void SparklerAutoPanAudioProcessorEditor::hiResTimerCallback()
{
    waveformComponent.repaint();
}
