#include "Compressor.h"

Compressor::Compressor()
{
}

Compressor::~Compressor()
{
}

void Compressor::prepare(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;
    updateCoefficients();
    reset();
}

void Compressor::reset()
{
    envelopeFollower[0] = 0.0f;
    envelopeFollower[1] = 0.0f;
    currentGainReduction = 0.0f;
}

void Compressor::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Convert makeup gain from dB to linear
    const float makeupGainLinear = juce::Decibels::decibelsToGain(makeupGain);
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float inputSample = channelData[sample];
            
            // Convert to dB for processing
            float inputLevel = std::abs(inputSample);
            float inputDb = inputLevel > 0.00001f 
                ? juce::Decibels::gainToDecibels(inputLevel)
                : -100.0f;
            
            // Envelope follower
            float envelope = envelopeFollower[channel];
            if (inputDb > envelope)
            {
                // Attack
                envelope = attackCoeff * (envelope - inputDb) + inputDb;
            }
            else
            {
                // Release
                envelope = releaseCoeff * (envelope - inputDb) + inputDb;
            }
            envelopeFollower[channel] = envelope;
            
            // Calculate gain reduction
            float gainReductionDb = 0.0f;
            if (envelope > threshold)
            {
                // Apply compression
                float excess = envelope - threshold;
                gainReductionDb = excess * (1.0f - 1.0f / ratio);
            }
            
            currentGainReduction = gainReductionDb;
            
            // Convert gain reduction to linear and apply
            float gainLinear = juce::Decibels::decibelsToGain(-gainReductionDb);
            
            // Apply compression and makeup gain
            channelData[sample] = inputSample * gainLinear * makeupGainLinear;
        }
    }
}

void Compressor::setThreshold(float thresholdDb)
{
    threshold = juce::jlimit(-60.0f, 0.0f, thresholdDb);
}

void Compressor::setRatio(float newRatio)
{
    ratio = juce::jlimit(1.0f, 20.0f, newRatio);
}

void Compressor::setAttack(float attackMs)
{
    attack = juce::jlimit(0.1f, 100.0f, attackMs);
    updateCoefficients();
}

void Compressor::setRelease(float releaseMs)
{
    release = juce::jlimit(10.0f, 1000.0f, releaseMs);
    updateCoefficients();
}

void Compressor::setMakeupGain(float gainDb)
{
    makeupGain = juce::jlimit(0.0f, 24.0f, gainDb);
}

void Compressor::updateCoefficients()
{
    // Calculate time coefficients for envelope follower
    // Using exponential averaging
    attackCoeff = std::exp(-1.0f / (attack * 0.001f * static_cast<float>(sampleRate)));
    releaseCoeff = std::exp(-1.0f / (release * 0.001f * static_cast<float>(sampleRate)));
}

std::unique_ptr<juce::XmlElement> Compressor::getStateInformation() const
{
    auto xml = std::make_unique<juce::XmlElement>("Compressor");
    xml->setAttribute("threshold", threshold);
    xml->setAttribute("ratio", ratio);
    xml->setAttribute("attack", attack);
    xml->setAttribute("release", release);
    xml->setAttribute("makeupGain", makeupGain);
    xml->setAttribute("bypassed", bypassed);
    return xml;
}

void Compressor::setStateInformation(const juce::XmlElement& xml)
{
    if (xml.hasTagName("Compressor"))
    {
        threshold = static_cast<float>(xml.getDoubleAttribute("threshold", -20.0));
        ratio = static_cast<float>(xml.getDoubleAttribute("ratio", 4.0));
        attack = static_cast<float>(xml.getDoubleAttribute("attack", 10.0));
        release = static_cast<float>(xml.getDoubleAttribute("release", 100.0));
        makeupGain = static_cast<float>(xml.getDoubleAttribute("makeupGain", 0.0));
        bypassed = xml.getBoolAttribute("bypassed", false);
        updateCoefficients();
    }
}

void Compressor::addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                                        const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "threshold",
        "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -20.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "ratio",
        "Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f),
        4.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "attack",
        "Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f),
        10.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "release",
        "Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f),
        100.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "makeupGain",
        "Makeup Gain",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f),
        0.0f));
}

void Compressor::linkParameters(juce::AudioProcessorValueTreeState& apvts,
                                const juce::String& prefix)
{
    // Link parameters from APVTS to internal values
    if (auto* thresholdParam = apvts.getRawParameterValue(prefix + "threshold"))
    {
        threshold = *thresholdParam;
    }
    if (auto* ratioParam = apvts.getRawParameterValue(prefix + "ratio"))
    {
        ratio = *ratioParam;
    }
    if (auto* attackParam = apvts.getRawParameterValue(prefix + "attack"))
    {
        attack = *attackParam;
        updateCoefficients();
    }
    if (auto* releaseParam = apvts.getRawParameterValue(prefix + "release"))
    {
        release = *releaseParam;
        updateCoefficients();
    }
    if (auto* makeupParam = apvts.getRawParameterValue(prefix + "makeupGain"))
    {
        makeupGain = *makeupParam;
    }
}
