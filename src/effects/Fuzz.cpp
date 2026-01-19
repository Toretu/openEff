#include "Fuzz.h"

Fuzz::Fuzz()
    : oversampling(2, oversampleFactor, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR)
{
}

Fuzz::~Fuzz()
{
}

void Fuzz::prepare(double newSampleRate, int samplesPerBlock)
{
    sampleRate = newSampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate * oversampleFactor;
    spec.maximumBlockSize = samplesPerBlock * oversampleFactor;
    spec.numChannels = 2;
    
    oversampling.initProcessing(samplesPerBlock);
    
    preFilter.setSampleRate(spec.sampleRate);
    preFilter.setType(SimpleFilter::FilterType::LowPass);
    preFilter.setCutoff(2000.0f);
    
    toneFilter.setSampleRate(spec.sampleRate);
    toneFilter.setType(SimpleFilter::FilterType::LowPass);
}

void Fuzz::reset()
{
    oversampling.reset();
    preFilter.reset();
    toneFilter.reset();
}

void Fuzz::processBlock(juce::AudioBuffer<float>& buffer)
{
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto oversampledBlock = oversampling.processSamplesUp(block);
    
    // Update tone filter cutoff based on tone parameter
    float cutoffFreq = 300.0f + (tone * 4700.0f); // 300Hz to 5kHz
    toneFilter.setCutoff(cutoffFreq);
    
    // Calculate actual gain multiplier (logarithmic scaling for more musical response)
    float gainMultiplier = std::pow(10.0f, gain * 0.4f - 1.0f); // Ranges from ~0.1 to ~25
    
    for (int channel = 0; channel < oversampledBlock.getNumChannels(); ++channel)
    {
        auto* channelData = oversampledBlock.getChannelPointer(channel);
        
        for (size_t sample = 0; sample < oversampledBlock.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample];
            
            // Pre-filter to shape input (reduce high frequency before distortion)
            inputSample = preFilter.processSample(inputSample, channel);
            
            // Apply gain boost
            inputSample *= gainMultiplier;
            
            // Asymmetric clipping for vintage fuzz character
            inputSample = asymmetricClip(inputSample);
            
            // Tone control (post-distortion filtering)
            inputSample = toneFilter.processSample(inputSample, channel);
            
            // Output level compensation
            channelData[sample] = inputSample * level * 0.5f;
        }
    }
    
    oversampling.processSamplesDown(block);
}

void Fuzz::setGain(float newGain)
{
    gain = newGain;
}

void Fuzz::setTone(float newTone)
{
    tone = juce::jlimit(0.0f, 1.0f, newTone);
}

void Fuzz::setLevel(float newLevel)
{
    level = juce::jlimit(0.0f, 1.0f, newLevel);
}

float Fuzz::asymmetricClip(float sample)
{
    // Asymmetric clipping for classic fuzz sound
    // Lower thresholds for more aggressive fuzz at lower gains
    const float posThreshold = 0.3f;
    const float negThreshold = 0.25f;
    
    if (sample > posThreshold)
    {
        // Soft clip positive
        float excess = sample - posThreshold;
        sample = posThreshold + excess / (1.0f + excess * excess);
    }
    else if (sample < -negThreshold)
    {
        // Harder clip negative for asymmetric character
        float excess = -(sample + negThreshold);
        sample = -negThreshold - excess / (1.0f + excess * excess * 1.5f);
    }
    
    // Apply soft saturation to everything for warmth
    sample = std::tanh(sample * 1.5f) * 0.8f;
    
    // Final hard limiting
    sample = juce::jlimit(-1.0f, 1.0f, sample);
    
    return sample;
}

std::unique_ptr<juce::XmlElement> Fuzz::getStateInformation() const
{
    auto xml = std::make_unique<juce::XmlElement>("Fuzz");
    xml->setAttribute("gain", gain);
    xml->setAttribute("tone", tone);
    xml->setAttribute("level", level);
    xml->setAttribute("bypassed", bypassed);
    return xml;
}

void Fuzz::setStateInformation(const juce::XmlElement& xml)
{
    if (xml.hasTagName("Fuzz"))
    {
        gain = static_cast<float>(xml.getDoubleAttribute("gain", 5.0));
        tone = static_cast<float>(xml.getDoubleAttribute("tone", 0.5));
        level = static_cast<float>(xml.getDoubleAttribute("level", 0.7));
        bypassed = xml.getBoolAttribute("bypassed", false);
    }
}

void Fuzz::addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                                  const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "gain",
        "Gain",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f),
        5.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "tone",
        "Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "level",
        "Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.7f));
}

void Fuzz::linkParameters(juce::AudioProcessorValueTreeState& apvts,
                          const juce::String& prefix)
{
    // Link parameters from APVTS to internal values
    if (auto* gainParam = apvts.getRawParameterValue(prefix + "gain"))
    {
        gain = *gainParam;
    }
    if (auto* toneParam = apvts.getRawParameterValue(prefix + "tone"))
    {
        tone = *toneParam;
    }
    if (auto* levelParam = apvts.getRawParameterValue(prefix + "level"))
    {
        level = *levelParam;
    }
}
