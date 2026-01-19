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

// EffectBase interface implementation
void Reverb::prepare(double newSampleRate, int newSamplesPerBlock)
{
    sampleRate = newSampleRate;
    samplesPerBlock = newSamplesPerBlock;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;
    
    prepare(spec);
}

std::unique_ptr<juce::XmlElement> Reverb::getStateInformation() const
{
    auto xml = std::make_unique<juce::XmlElement>("Reverb");
    xml->setAttribute("roomSize", currentRoomSize);
    xml->setAttribute("damping", currentDamping);
    xml->setAttribute("wetLevel", currentWetLevel);
    xml->setAttribute("width", currentWidth);
    xml->setAttribute("bypassed", bypassed);
    return xml;
}

void Reverb::setStateInformation(const juce::XmlElement& xml)
{
    if (xml.hasTagName("Reverb"))
    {
        setRoomSize(static_cast<float>(xml.getDoubleAttribute("roomSize", 0.5)));
        setDamping(static_cast<float>(xml.getDoubleAttribute("damping", 0.5)));
        setWetLevel(static_cast<float>(xml.getDoubleAttribute("wetLevel", 0.33)));
        setWidth(static_cast<float>(xml.getDoubleAttribute("width", 1.0)));
        bypassed = xml.getBoolAttribute("bypassed", false);
    }
}

void Reverb::addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                                    const juce::String& prefix)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "roomSize",
        "Room Size",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "damping",
        "Damping",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.5f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "wetLevel",
        "Wet Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.33f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        prefix + "width",
        "Width",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f));
}

void Reverb::linkParameters(juce::AudioProcessorValueTreeState& apvts,
                            const juce::String& prefix)
{
    // Link parameters from APVTS to internal values
    if (auto* roomSizeParam = apvts.getRawParameterValue(prefix + "roomSize"))
    {
        setRoomSize(*roomSizeParam);
    }
    if (auto* dampingParam = apvts.getRawParameterValue(prefix + "damping"))
    {
        setDamping(*dampingParam);
    }
    if (auto* wetLevelParam = apvts.getRawParameterValue(prefix + "wetLevel"))
    {
        setWetLevel(*wetLevelParam);
    }
    if (auto* widthParam = apvts.getRawParameterValue(prefix + "width"))
    {
        setWidth(*widthParam);
    }
}
