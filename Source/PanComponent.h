#pragma once

#include <JuceHeader.h>
#include "Panner.h"

class PanComponent  : public juce::Component
{
public:
    PanComponent(Panner& pc): panner(pc)
    {
        setSize(width, height);
    }

    ~PanComponent() override
    {
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll(juce::Colour::fromRGB(38, 50, 56));
        g.setColour(juce::Colour::fromRGB(189, 189, 189));       
        int pos = width - panner.currentPosition * 2 * width / juce::double_Pi; //radian position to linear 
        g.fillRect(pos - 2, 0, 6, height);
        g.setColour(juce::Colour::fromRGB(255, 255, 255));
        g.setFont(14.0f);
        g.drawFittedText("L", 10, 0, width, height, juce::Justification::centredLeft, 1);
        g.drawFittedText("R", 0, 0, width - 10, height, juce::Justification::centredRight, 1);
    }

    void resized() override
    {        
    }

private:
    Panner& panner;
    static int const height = 25, width = 550;    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanComponent)
};
