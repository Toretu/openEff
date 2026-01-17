#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class Reverb
{
public:
    Reverb();
    ~Reverb();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void processBlock(juce::AudioBuffer<float>& buffer);

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
