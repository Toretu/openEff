#pragma once

#include "EffectBase.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class Chorus : public EffectBase
{
public:
    Chorus();
    ~Chorus() override;

    // Core processing
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void processBlock(juce::AudioBuffer<float>& buffer) override;

    // Metadata
    juce::String getName() const override { return "Chorus"; }
    juce::String getEffectType() const override { return "chorus"; }

    // State management
    std::unique_ptr<juce::XmlElement> getStateInformation() const override;
    void setStateInformation(const juce::XmlElement& xml) override;

    // Parameter layout for APVTS
    void addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                               const juce::String& prefix) override;
    void linkParameters(juce::AudioProcessorValueTreeState& apvts,
                       const juce::String& prefix) override;

    // Parameter setters
    void setRate(float newRate);
    void setDepth(float newDepth);
    void setMix(float newMix);

    // Parameter getters
    float getRate() const { return rate; }
    float getDepth() const { return depth; }
    float getMix() const { return mix; }

private:
    // Parameters
    float rate = 1.5f;      // LFO rate in Hz (0.1 - 5.0)
    float depth = 0.5f;     // Modulation depth (0.0 - 1.0)
    float mix = 0.5f;       // Wet/dry mix (0.0 - 1.0)

    // DSP components
    static constexpr int maxDelayMs = 50;
    juce::AudioBuffer<float> delayBuffer;
    int delayBufferSize = 0;
    int writePosition = 0;
    
    float lfoPhase = 0.0f;
    float inverseSampleRate = 0.0f;

    void updateParameters();
    float calculateLFO();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chorus)
};
