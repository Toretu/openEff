#include "Orange.h"
#include <cmath>

Orange::Orange()
{
}

Orange::~Orange()
{
}

void Orange::prepare(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    this->samplesPerBlock = samplesPerBlock;

    // Setup tone filters
    lowPassFilter.setType(SimpleFilter::FilterType::LowPass);
    lowPassFilter.setSampleRate(sampleRate);
    lowPassFilter.setCutoff(3500.0f); // Warm, smooth high-end rolloff

    highPassFilter.setType(SimpleFilter::FilterType::HighPass);
    highPassFilter.setSampleRate(sampleRate);
    highPassFilter.setCutoff(80.0f); // Tight low-end

    reset();
}

void Orange::reset()
{
    lowPassFilter.reset();
    highPassFilter.reset();
    dcBlockerX1 = 0.0f;
    dcBlockerY1 = 0.0f;
}

void Orange::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (bypassed)
        return;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Update parameters
    float currentGain = gain;
    float currentTone = tone;
    float currentLevel = level;

    // Calculate gain scaling (0-10 style, Orange amps go to 11 in spirit!)
    float driveAmount = 1.0f + (currentGain * 19.0f); // 1x to 20x gain
    
    // Update tone filter cutoff based on tone control
    float lpFreq = 1000.0f + (currentTone * 4500.0f); // 1kHz to 5.5kHz
    float hpFreq = 50.0f + ((1.0f - currentTone) * 150.0f); // 50Hz to 200Hz
    lowPassFilter.setCutoff(lpFreq);
    highPassFilter.setCutoff(hpFreq);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);

        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];

            // Pre-gain stage (simulates input stage)
            sample *= driveAmount;

            // British-style asymmetric soft clipping
            sample = softClip(sample);

            // Tone stack (pre-emphasis and post-filtering)
            sample = highPassFilter.processSample(sample, ch);
            sample = lowPassFilter.processSample(sample, ch);

            // DC blocker
            float dcOut = sample - dcBlockerX1 + 0.995f * dcBlockerY1;
            dcBlockerX1 = sample;
            dcBlockerY1 = dcOut;
            sample = dcOut;

            // Output level control
            sample *= currentLevel;

            channelData[i] = sample;
        }
    }
}

float Orange::softClip(float sample)
{
    // Asymmetric soft clipping for warm, British-style overdrive
    // Similar to tube saturation
    
    if (sample > 1.0f)
    {
        // Hard clip positive peaks (slight asymmetry)
        sample = 1.0f;
    }
    else if (sample > 0.33f)
    {
        // Soft clip positive
        sample = 0.33f + (2.0f / 3.0f) * (1.0f - std::exp(-1.5f * (sample - 0.33f)));
    }
    else if (sample < -1.0f)
    {
        // Hard clip negative peaks
        sample = -1.0f;
    }
    else if (sample < -0.33f)
    {
        // Soft clip negative (slightly different curve for asymmetry)
        sample = -0.33f - (2.0f / 3.0f) * (1.0f - std::exp(-1.3f * (-sample - 0.33f)));
    }
    
    return sample * 0.6f; // Compensate for clipping gain
}

void Orange::setGain(float newGain)
{
    gain = juce::jlimit(0.0f, 1.0f, newGain);
}

void Orange::setTone(float newTone)
{
    tone = juce::jlimit(0.0f, 1.0f, newTone);
}

void Orange::setLevel(float newLevel)
{
    level = juce::jlimit(0.0f, 1.0f, newLevel);
}

std::unique_ptr<juce::XmlElement> Orange::getStateInformation() const
{
    auto xml = std::make_unique<juce::XmlElement>("Orange");
    xml->setAttribute("gain", gain);
    xml->setAttribute("tone", tone);
    xml->setAttribute("level", level);
    xml->setAttribute("bypassed", bypassed);
    return xml;
}

void Orange::setStateInformation(const juce::XmlElement& xml)
{
    if (xml.hasTagName("Orange"))
    {
        gain = static_cast<float>(xml.getDoubleAttribute("gain", 0.5));
        tone = static_cast<float>(xml.getDoubleAttribute("tone", 0.5));
        level = static_cast<float>(xml.getDoubleAttribute("level", 0.7));
        bypassed = xml.getBoolAttribute("bypassed", false);
    }
}

void Orange::addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                                    const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "gain",
        "Gain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f,
        ""));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "tone",
        "Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f,
        ""));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "level",
        "Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.7f,
        ""));
}

void Orange::linkParameters(juce::AudioProcessorValueTreeState& apvts,
                            const juce::String& prefix)
{
    gainParam = apvts.getRawParameterValue(prefix + "gain");
    toneParam = apvts.getRawParameterValue(prefix + "tone");
    levelParam = apvts.getRawParameterValue(prefix + "level");
}
