#include "Chorus.h"

Chorus::Chorus()
{
}

Chorus::~Chorus()
{
}

void Chorus::prepare(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    this->samplesPerBlock = samplesPerBlock;
    this->inverseSampleRate = 1.0f / static_cast<float>(sampleRate);

    // Allocate delay buffer (stereo, max delay time)
    delayBufferSize = static_cast<int>(sampleRate * maxDelayMs / 1000.0) + 1;
    delayBuffer.setSize(2, delayBufferSize);
    
    reset();
}

void Chorus::reset()
{
    delayBuffer.clear();
    writePosition = 0;
    lfoPhase = 0.0f;
}

void Chorus::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (bypassed)
        return;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Calculate LFO value (0.0 to 1.0)
        float lfoValue = calculateLFO();
        
        // Calculate delay time in samples (5ms to 30ms modulated by LFO)
        float baseDelayMs = 15.0f;
        float modulationMs = 15.0f * depth;
        float currentDelayMs = baseDelayMs + (lfoValue * modulationMs);
        float delaySamples = (currentDelayMs / 1000.0f) * static_cast<float>(sampleRate);

        for (int channel = 0; channel < numChannels; ++channel)
        {
            // Get input sample
            float input = buffer.getSample(channel, sample);
            
            // Write to delay buffer
            delayBuffer.setSample(channel, writePosition, input);
            
            // Calculate read position with interpolation
            float readPos = writePosition - delaySamples;
            while (readPos < 0.0f)
                readPos += delayBufferSize;
            
            // Linear interpolation for smooth delay modulation
            int readPos1 = static_cast<int>(readPos);
            int readPos2 = (readPos1 + 1) % delayBufferSize;
            float fraction = readPos - readPos1;
            
            float delayed1 = delayBuffer.getSample(channel, readPos1);
            float delayed2 = delayBuffer.getSample(channel, readPos2);
            float delayedSample = delayed1 + fraction * (delayed2 - delayed1);
            
            // Mix dry and wet signals
            float output = input * (1.0f - mix) + delayedSample * mix;
            
            buffer.setSample(channel, sample, output);
        }
        
        // Advance write position
        writePosition = (writePosition + 1) % delayBufferSize;
        
        // Advance LFO phase
        lfoPhase += rate * inverseSampleRate;
        if (lfoPhase >= 1.0f)
            lfoPhase -= 1.0f;
    }
}

float Chorus::calculateLFO()
{
    // Triangle wave LFO
    float value = lfoPhase * 2.0f;
    if (value > 1.0f)
        value = 2.0f - value;
    return value;
}

void Chorus::setRate(float newRate)
{
    rate = juce::jlimit(0.1f, 5.0f, newRate);
}

void Chorus::setDepth(float newDepth)
{
    depth = juce::jlimit(0.0f, 1.0f, newDepth);
}

void Chorus::setMix(float newMix)
{
    mix = juce::jlimit(0.0f, 1.0f, newMix);
}

std::unique_ptr<juce::XmlElement> Chorus::getStateInformation() const
{
    auto xml = std::make_unique<juce::XmlElement>("Chorus");
    xml->setAttribute("rate", rate);
    xml->setAttribute("depth", depth);
    xml->setAttribute("mix", mix);
    xml->setAttribute("bypassed", bypassed);
    return xml;
}

void Chorus::setStateInformation(const juce::XmlElement& xml)
{
    if (xml.hasTagName("Chorus"))
    {
        rate = static_cast<float>(xml.getDoubleAttribute("rate", 1.5));
        depth = static_cast<float>(xml.getDoubleAttribute("depth", 0.5));
        mix = static_cast<float>(xml.getDoubleAttribute("mix", 0.5));
        bypassed = xml.getBoolAttribute("bypassed", false);
    }
}

void Chorus::addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                                    const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "rate",
        "Rate",
        juce::NormalisableRange<float>(0.1f, 5.0f, 0.01f),
        1.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "depth",
        "Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "mix",
        "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));
}

void Chorus::linkParameters(juce::AudioProcessorValueTreeState& apvts,
                            const juce::String& prefix)
{
    // Link parameters from APVTS to internal values
    if (auto* rateParam = apvts.getRawParameterValue(prefix + "rate"))
        rate = *rateParam;
    if (auto* depthParam = apvts.getRawParameterValue(prefix + "depth"))
        depth = *depthParam;
    if (auto* mixParam = apvts.getRawParameterValue(prefix + "mix"))
        mix = *mixParam;
}

void Chorus::updateParameters()
{
    // Parameters are updated in real-time, no smoothing needed here
}
