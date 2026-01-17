#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

class SimpleFilter
{
public:
    enum class FilterType
    {
        LowPass,
        HighPass,
        BandPass
    };

    SimpleFilter();
    ~SimpleFilter();

    void setSampleRate(double newSampleRate);
    void setType(FilterType newType);
    void setCutoff(float cutoffHz);
    void setResonance(float q);
    
    void reset();
    float processSample(float sample, int channel);

private:
    void updateCoefficients();

    double sampleRate = 44100.0;
    FilterType type = FilterType::LowPass;
    float cutoff = 1000.0f;
    float resonance = 0.707f;

    // Biquad coefficients
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
    float a1 = 0.0f, a2 = 0.0f;

    // State variables (stereo)
    float x1[2] = { 0.0f, 0.0f };
    float x2[2] = { 0.0f, 0.0f };
    float y1[2] = { 0.0f, 0.0f };
    float y2[2] = { 0.0f, 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleFilter)
};
