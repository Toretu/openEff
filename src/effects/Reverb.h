#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "EffectBase.h"

class Reverb : public EffectBase
{
public:
    Reverb();
    ~Reverb() override;

    // EffectBase interface implementation
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void processBlock(juce::AudioBuffer<float>& buffer) override;
    
    juce::String getName() const override { return "Reverb"; }
    juce::String getEffectType() const override { return "reverb"; }
    
    std::unique_ptr<juce::XmlElement> getStateInformation() const override;
    void setStateInformation(const juce::XmlElement& xml) override;
    
    void addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                               const juce::String& prefix) override;
    void linkParameters(juce::AudioProcessorValueTreeState& apvts,
                       const juce::String& prefix) override;

    // Legacy compatibility methods (for standalone Reverb plugin)
    void prepare(const juce::dsp::ProcessSpec& spec);

    void setRoomSize(float roomSize);
    void setDamping(float damping);
    void setWetLevel(float wetLevel);
    void setWidth(float width);

private:
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;
    
    float currentRoomSize = 0.5f;
    float currentDamping = 0.5f;
    float currentWetLevel = 0.33f;
    float currentWidth = 1.0f;
};
