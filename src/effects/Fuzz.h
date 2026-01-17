#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>
#include "../dsp/Filter.h"

class Fuzz
{
public:
    Fuzz();
    ~Fuzz();

    void prepare(double sampleRate, int samplesPerBlock);
    void reset();
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Parameter setters
    void setGain(float newGain);
    void setTone(float newTone);
    void setLevel(float newLevel);

private:
    float processSample(float sample);
    float asymmetricClip(float sample);

    // Parameters
    float gain = 5.0f;
    float tone = 0.5f;
    float level = 0.7f;

    // DSP
    double sampleRate = 44100.0;
    SimpleFilter preFilter;
    SimpleFilter toneFilter;
    
    // Oversampling
    static constexpr int oversampleFactor = 2;
    juce::dsp::Oversampling<float> oversampling;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Fuzz)
};
