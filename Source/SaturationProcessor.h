#pragma once
#include <cmath>

class Saturator
{
public:
    Saturator() {}

    float process(float input, float gain)
    {   
        float offset = 1.2f; //scale with gain?
        // offset makes tanh not flippable on y-axis -> more even harmonics
        // soft clipping
        float clipped = juce::dsp::FastMathApproximations::tanh(
            input * gain + offset) - juce::dsp::FastMathApproximations::tanh(offset);

        // additional even harmonics
        float second = 0.05f * (clipped * clipped * clipped);

        return (clipped) * (2.0f / gain);
    }
};
