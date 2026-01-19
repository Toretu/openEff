#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>
#include "EffectBase.h"

class Compressor : public EffectBase
{
public:
    Compressor();
    ~Compressor() override;

    // EffectBase interface implementation
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void processBlock(juce::AudioBuffer<float>& buffer) override;
    
    juce::String getName() const override { return "Compressor"; }
    juce::String getEffectType() const override { return "compressor"; }
    
    std::unique_ptr<juce::XmlElement> getStateInformation() const override;
    void setStateInformation(const juce::XmlElement& xml) override;
    
    void addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                               const juce::String& prefix) override;
    void linkParameters(juce::AudioProcessorValueTreeState& apvts,
                       const juce::String& prefix) override;

    // Parameter setters
    void setThreshold(float thresholdDb);
    void setRatio(float ratio);
    void setAttack(float attackMs);
    void setRelease(float releaseMs);
    void setMakeupGain(float gainDb);

    // Get current gain reduction for metering
    float getGainReduction() const { return currentGainReduction; }

private:
    float processSample(float sample, int channel);
    
    // Parameters
    float threshold = -20.0f;    // dB
    float ratio = 4.0f;          // N:1
    float attack = 10.0f;        // ms
    float release = 100.0f;      // ms
    float makeupGain = 0.0f;     // dB

    // DSP state
    double sampleRate = 44100.0;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    float envelopeFollower[2] = { 0.0f, 0.0f };
    float currentGainReduction = 0.0f;

    void updateCoefficients();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Compressor)
};
