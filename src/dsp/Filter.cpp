#include "Filter.h"

SimpleFilter::SimpleFilter()
{
    updateCoefficients();
}

SimpleFilter::~SimpleFilter()
{
}

void SimpleFilter::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
    updateCoefficients();
}

void SimpleFilter::setType(FilterType newType)
{
    type = newType;
    updateCoefficients();
}

void SimpleFilter::setCutoff(float cutoffHz)
{
    cutoff = juce::jlimit(20.0f, static_cast<float>(sampleRate * 0.49), cutoffHz);
    updateCoefficients();
}

void SimpleFilter::setResonance(float q)
{
    resonance = juce::jlimit(0.1f, 20.0f, q);
    updateCoefficients();
}

void SimpleFilter::reset()
{
    for (int i = 0; i < 2; ++i)
    {
        x1[i] = x2[i] = 0.0f;
        y1[i] = y2[i] = 0.0f;
    }
}

float SimpleFilter::processSample(float sample, int channel)
{
    // Biquad filter implementation
    float output = b0 * sample + b1 * x1[channel] + b2 * x2[channel]
                   - a1 * y1[channel] - a2 * y2[channel];

    // Update state
    x2[channel] = x1[channel];
    x1[channel] = sample;
    y2[channel] = y1[channel];
    y1[channel] = output;

    return output;
}

void SimpleFilter::updateCoefficients()
{
    const float pi = juce::MathConstants<float>::pi;
    const float omega = 2.0f * pi * cutoff / static_cast<float>(sampleRate);
    const float cosOmega = std::cos(omega);
    const float sinOmega = std::sin(omega);
    const float alpha = sinOmega / (2.0f * resonance);

    switch (type)
    {
        case FilterType::LowPass:
        {
            b0 = (1.0f - cosOmega) / 2.0f;
            b1 = 1.0f - cosOmega;
            b2 = (1.0f - cosOmega) / 2.0f;
            float a0 = 1.0f + alpha;
            a1 = -2.0f * cosOmega;
            a2 = 1.0f - alpha;
            
            // Normalize
            b0 /= a0;
            b1 /= a0;
            b2 /= a0;
            a1 /= a0;
            a2 /= a0;
            break;
        }

        case FilterType::HighPass:
        {
            b0 = (1.0f + cosOmega) / 2.0f;
            b1 = -(1.0f + cosOmega);
            b2 = (1.0f + cosOmega) / 2.0f;
            float a0 = 1.0f + alpha;
            a1 = -2.0f * cosOmega;
            a2 = 1.0f - alpha;
            
            // Normalize
            b0 /= a0;
            b1 /= a0;
            b2 /= a0;
            a1 /= a0;
            a2 /= a0;
            break;
        }

        case FilterType::BandPass:
        {
            b0 = alpha;
            b1 = 0.0f;
            b2 = -alpha;
            float a0 = 1.0f + alpha;
            a1 = -2.0f * cosOmega;
            a2 = 1.0f - alpha;
            
            // Normalize
            b0 /= a0;
            b1 /= a0;
            b2 /= a0;
            a1 /= a0;
            a2 /= a0;
            break;
        }
    }
}
