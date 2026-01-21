#include "BigMuff.h"
#include <cmath>

BigMuff::BigMuff()
{
}

BigMuff::~BigMuff()
{
}

void BigMuff::prepare(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    this->samplesPerBlock = samplesPerBlock;

    // Input high-pass to remove DC and rumble
    inputFilter.setType(SimpleFilter::FilterType::HighPass);
    inputFilter.setSampleRate(sampleRate);
    inputFilter.setCutoff(100.0f);
    
    // Tone stack setup - creates the mid-scoop
    lowPassFilter.setType(SimpleFilter::FilterType::LowPass);
    lowPassFilter.setSampleRate(sampleRate);
    lowPassFilter.setCutoff(500.0f);
    
    highPassFilter.setType(SimpleFilter::FilterType::HighPass);
    highPassFilter.setSampleRate(sampleRate);
    highPassFilter.setCutoff(1500.0f);
    
    // Mid cut for that signature scoop
    midCutFilter.setType(SimpleFilter::FilterType::BandPass);
    midCutFilter.setSampleRate(sampleRate);
    midCutFilter.setCutoff(800.0f);
    midCutFilter.setResonance(0.5f);

    reset();
}

void BigMuff::reset()
{
    inputFilter.reset();
    lowPassFilter.reset();
    highPassFilter.reset();
    midCutFilter.reset();
    dcBlockerX1 = 0.0f;
    dcBlockerY1 = 0.0f;
}

void BigMuff::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (bypassed)
        return;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Update parameters
    float currentSustain = sustain;
    float currentTone = tone;
    float currentVolume = volume;

    // Sustain controls overall gain (1x to 100x for massive sustain)
    float gainAmount = 1.0f + (currentSustain * 99.0f);
    
    // Tone control adjusts the balance between bass and treble (mid always scooped)
    float bassAmount = 0.3f + (1.0f - currentTone) * 0.7f;
    float trebleAmount = 0.3f + currentTone * 0.7f;

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];

            // Input filter - remove DC and low rumble
            sample = inputFilter.processSample(sample, ch);

            // First gain stage
            sample *= gainAmount * 0.5f;
            sample = clipStage(sample, 0.6f);

            // Second gain stage
            sample *= 2.0f;
            sample = clipStage(sample, 0.5f);

            // Third gain stage (more clipping = more sustain)
            sample *= 1.5f;
            sample = clipStage(sample, 0.4f);

            // Fourth gain stage (final saturation)
            sample *= 1.3f;
            sample = clipStage(sample, 0.35f);

            // Tone stack - this is where the magic happens
            // Split into bass and treble, scoop the mids
            float bass = lowPassFilter.processSample(sample, ch);
            float treble = highPassFilter.processSample(sample, ch);
            float mid = midCutFilter.processSample(sample, ch);
            
            // Mix with mid scoop
            sample = (bass * bassAmount + treble * trebleAmount - mid * 0.4f);

            // DC blocker
            float dcOut = sample - dcBlockerX1 + 0.995f * dcBlockerY1;
            dcBlockerX1 = sample;
            dcBlockerY1 = dcOut;
            sample = dcOut;

            // Output volume
            sample *= currentVolume;

            // Final soft clip to prevent harsh peaks
            if (sample > 0.9f)
                sample = 0.9f + 0.1f * std::tanh((sample - 0.9f) * 5.0f);
            else if (sample < -0.9f)
                sample = -0.9f + 0.1f * std::tanh((sample + 0.9f) * 5.0f);

            channelData[i] = sample;
        }
    }
}

float BigMuff::clipStage(float sample, float threshold)
{
    // Asymmetric soft clipping for each stage
    // This creates the thick, saturated Big Muff sound
    
    if (sample > threshold)
    {
        // Soft clip positive
        float excess = sample - threshold;
        sample = threshold + (1.0f - threshold) * std::tanh(excess / (1.0f - threshold));
    }
    else if (sample < -threshold)
    {
        // Slightly different negative clipping (asymmetry adds harmonics)
        float excess = -sample - threshold;
        sample = -(threshold + (1.0f - threshold) * std::tanh(excess / (1.0f - threshold) * 0.9f));
    }
    
    return sample * 0.8f; // Compensate for clipping gain
}

void BigMuff::setSustain(float newSustain)
{
    sustain = juce::jlimit(0.0f, 1.0f, newSustain);
}

void BigMuff::setTone(float newTone)
{
    tone = juce::jlimit(0.0f, 1.0f, newTone);
}

void BigMuff::setVolume(float newVolume)
{
    volume = juce::jlimit(0.0f, 1.0f, newVolume);
}

std::unique_ptr<juce::XmlElement> BigMuff::getStateInformation() const
{
    auto xml = std::make_unique<juce::XmlElement>("BigMuff");
    xml->setAttribute("sustain", sustain);
    xml->setAttribute("tone", tone);
    xml->setAttribute("volume", volume);
    xml->setAttribute("bypassed", bypassed);
    return xml;
}

void BigMuff::setStateInformation(const juce::XmlElement& xml)
{
    if (xml.hasTagName("BigMuff"))
    {
        sustain = static_cast<float>(xml.getDoubleAttribute("sustain", 0.7));
        tone = static_cast<float>(xml.getDoubleAttribute("tone", 0.5));
        volume = static_cast<float>(xml.getDoubleAttribute("volume", 0.7));
        bypassed = xml.getBoolAttribute("bypassed", false);
    }
}

void BigMuff::addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                                     const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "sustain",
        "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.7f,
        ""));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "tone",
        "Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f,
        ""));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "volume",
        "Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.7f,
        ""));
}

void BigMuff::linkParameters(juce::AudioProcessorValueTreeState& apvts,
                             const juce::String& prefix)
{
    sustainParam = apvts.getRawParameterValue(prefix + "sustain");
    toneParam = apvts.getRawParameterValue(prefix + "tone");
    volumeParam = apvts.getRawParameterValue(prefix + "volume");
}
