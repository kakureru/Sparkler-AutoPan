#include "TransientComputer.h"

TransientComputer::TransientComputer() {}
TransientComputer::~TransientComputer() {}

void TransientComputer::prepare(int numChannels, int sampleRate, int detectionBlockSize)
{
    rmsLevel.reset(sampleRate, 0.1);
    rmsLevel.setCurrentAndTargetValue(-100.f);
    rmsBlock = detectionBlockSize;
    rmsBuffer.setSize(numChannels, rmsBlock);
    rmsBuffer.clear();
    tempPosition = 0;
    currentPeakLength = 1100000;    
}

void TransientComputer::reset()
{
    rmsBuffer.clear();
    tempPosition = 0;
}

void TransientComputer::detectTransients(juce::AudioBuffer<float>& buffer, float sensitivity, float latency, int peakLength)
{   
    transVector.resize(buffer.getNumSamples());
    std::fill(transVector.begin(), transVector.end(), 0);
    int srcPosition = 0;
    int srcBlockStartPos = 0;

    while (srcPosition + 1 != buffer.getNumSamples()) // пока не кончится входной буфер
    {        
        srcBlockStartPos = srcPosition;
        // заполняем временный буфер
        while (tempPosition != rmsBlock) 
        {            
            currentPeakLength++;
            for (int i = 0; i < rmsBuffer.getNumChannels(); i++)
                rmsBuffer.copyFrom(i, tempPosition, buffer.getReadPointer(0, srcPosition), 1);
            tempPosition++;            
            if (srcPosition + 1 == buffer.getNumSamples())
                break;     
            else
                srcPosition++;
        }
        // если временный буфер заполнен
        if (tempPosition == rmsBlock)
        {
            rmsLevel.skip(rmsBlock);

            // rms follower 
            float value;
            for (int i = 0; i < rmsBuffer.getNumChannels(); i++)
                value += (rmsBuffer.getRMSLevel(i, 0, rmsBlock));
            value = value / rmsBuffer.getNumChannels();

            if (value < rmsLevel.getCurrentValue())
                rmsLevel.setTargetValue(value);
            else
                rmsLevel.setCurrentAndTargetValue(value);

            // rms follower with latency
            const auto valueLatency = (1 - latency) * rmsLevel.getCurrentValue() + latency * rmsLevelLatency.getCurrentValue();
            if (valueLatency < rmsLevelLatency.getCurrentValue())
                rmsLevelLatency.setTargetValue((1 - 0.1) * rmsLevel.getCurrentValue() + 0.1 * rmsLevelLatency.getCurrentValue());
            else
                rmsLevelLatency.setTargetValue(valueLatency);

            // transient detection   
            if (currentPeakLength > peakLength)
            {
                if (rmsLevel.getCurrentValue() - rmsLevelLatency.getCurrentValue() > sensitivity)
                {
                    if (buffer.getNumSamples() <= rmsBlock)
                        transVector[0] = 1;
                    else
                        transVector[srcBlockStartPos] = 1;                    
                    trans = 1;
                    currentPeakLength = 0;
                }
                else
                    trans = 0;                
            }                    

            reset();
        }
    }    
}