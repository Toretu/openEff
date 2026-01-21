#pragma once

#include "EffectBase.h"
#include "../dsp/Filter.h"
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * Orange-style British amplifier overdrive effect.
 * Emulates the classic Orange amp sound with warm, punchy overdrive.
 */
class Orange : public EffectBase
{
public:
    Orange();
    ~Orange() override;

    // Core processing
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void processBlock(juce::AudioBuffer<float>& buffer) override;

    // Metadata
    juce::String getName() const override { return "Orange"; }
    juce::String getEffectType() const override { return "orange"; }

    // State management
    std::unique_ptr<juce::XmlElement> getStateInformation() const override;
    void setStateInformation(const juce::XmlElement& xml) override;

    // Parameter layout for APVTS
    void addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                               const juce::String& prefix) override;
    void linkParameters(juce::AudioProcessorValueTreeState& apvts,
                       const juce::String& prefix) override;

    // Parameter setters
    void setGain(float newGain);
    void setTone(float newTone);
    void setLevel(float newLevel);

private:
    // Parameters
    std::atomic<float>* gainParam = nullptr;
    std::atomic<float>* toneParam = nullptr;
    std::atomic<float>* levelParam = nullptr;

    // Cached parameter values
    float gain = 0.5f;
    float tone = 0.5f;
    float level = 0.7f;

    // Tone stack filters
    SimpleFilter lowPassFilter;
    SimpleFilter highPassFilter;
    
    // Soft clipping function for warm overdrive
    float softClip(float sample);
    
    // DC blocking filter
    float dcBlockerX1 = 0.0f;
    float dcBlockerY1 = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Orange)
};
