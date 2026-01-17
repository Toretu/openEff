#include "Reverb.h"

Reverb::Reverb()
{
    reverbParams.roomSize = currentRoomSize;
    reverbParams.damping = currentDamping;
    reverbParams.wetLevel = currentWetLevel;
    reverbParams.dryLevel = 1.0f - currentWetLevel;
    reverbParams.width = currentWidth;
    reverbParams.freezeMode = 0.0f;
    
    reverb.setParameters(reverbParams);
}

Reverb::~Reverb()
{
}

void Reverb::prepare(const juce::dsp::ProcessSpec& spec)
{
    reverb.prepare(spec);
}

void Reverb::reset()
{
    reverb.reset();
}

void Reverb::processBlock(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);
}

void Reverb::setRoomSize(float roomSize)
{
    currentRoomSize = roomSize;
    reverbParams.roomSize = roomSize;
    reverb.setParameters(reverbParams);
}

void Reverb::setDamping(float damping)
{
    currentDamping = damping;
    reverbParams.damping = damping;
    reverb.setParameters(reverbParams);
}

void Reverb::setWetLevel(float wetLevel)
{
    currentWetLevel = wetLevel;
    reverbParams.wetLevel = wetLevel;
    reverbParams.dryLevel = 1.0f - wetLevel;
    reverb.setParameters(reverbParams);
}

void Reverb::setWidth(float width)
{
    currentWidth = width;
    reverbParams.width = width;
    reverb.setParameters(reverbParams);
}
