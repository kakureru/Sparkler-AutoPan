#include <random>
#include "Panner.h"

Panner::Panner() {}
Panner::~Panner() {}

void Panner::pan(juce::AudioBuffer<float>& buffer, std::vector<int> transVector, int width, int speed, int panModel)
{
	for (int i = 0; i < buffer.getNumSamples(); ++i)
	{
		if (transVector[i] == 1)
		{
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
            if (speed == 2) //почему параметр не доходит до крайних значений ?!!
                currentPosition = position;
            else
                radsPerSample = (std::abs(currentPosition - position)) / speed / 1000; //speedmultiplier
		}
        if (currentPosition < position)
            currentPosition += radsPerSample;
        else if (currentPosition > position)
            currentPosition -= radsPerSample;

        auto* channelDataL = buffer.getWritePointer(0);
        auto* channelDataR = buffer.getWritePointer(1);

        channelDataL[i] *= std::sin(currentPosition) * panLowCompensation;
        channelDataR[i] *= std::cos(currentPosition) * panLowCompensation;
	}
}
