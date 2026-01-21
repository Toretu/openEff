#pragma once

#include "EffectBase.h"
#include "../dsp/Filter.h"
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * Big Muff-style fuzz/distortion pedal.
 * Iconic Pink Floyd sound with thick, creamy, sustaining distortion.
 * Features mid-scooped tone and massive sustain.
 */
class BigMuff : public EffectBase
{
public:
    BigMuff();
    ~BigMuff() override;

    // Core processing
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void processBlock(juce::AudioBuffer<float>& buffer) override;

    // Metadata
    juce::String getName() const override { return "Big Muff"; }
    juce::String getEffectType() const override { return "bigmuff"; }

    // State management
    std::unique_ptr<juce::XmlElement> getStateInformation() const override;
    void setStateInformation(const juce::XmlElement& xml) override;

    // Parameter layout for APVTS
    void addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                               const juce::String& prefix) override;
    void linkParameters(juce::AudioProcessorValueTreeState& apvts,
                       const juce::String& prefix) override;

    // Parameter setters
    void setSustain(float newSustain);
    void setTone(float newTone);
    void setVolume(float newVolume);

private:
    // Parameters
    std::atomic<float>* sustainParam = nullptr;
    std::atomic<float>* toneParam = nullptr;
    std::atomic<float>* volumeParam = nullptr;

    // Cached parameter values
    float sustain = 0.7f;   // Gain/sustain control
    float tone = 0.5f;      // Tone control (mid scoop)
    float volume = 0.7f;    // Output volume

    // Multi-stage clipping for thick fuzz
    float clipStage(float sample, float threshold);
    
    // Tone stack filters (mid-scoop characteristic)
    SimpleFilter lowPassFilter;
    SimpleFilter highPassFilter;
    SimpleFilter midCutFilter;
    
    // Input filtering
    SimpleFilter inputFilter;
    
    // DC blocking
    float dcBlockerX1 = 0.0f;
    float dcBlockerY1 = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BigMuff)
};
