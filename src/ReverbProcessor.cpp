#include "ReverbProcessor.h"
#include "ReverbEditor.h"

ReverbProcessor::ReverbProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      valueTreeState(*this, nullptr, "PARAMETERS",
                     {
                         std::make_unique<juce::AudioParameterFloat>("roomSize", "Room Size", 0.0f, 1.0f, 0.5f),
                         std::make_unique<juce::AudioParameterFloat>("damping", "Damping", 0.0f, 1.0f, 0.5f),
                         std::make_unique<juce::AudioParameterFloat>("wetLevel", "Wet Level", 0.0f, 1.0f, 0.33f),
                         std::make_unique<juce::AudioParameterFloat>("width", "Width", 0.0f, 1.0f, 1.0f),
                         std::make_unique<juce::AudioParameterBool>("bypass", "Bypass", false)
                     })
{
    roomSizeParam = valueTreeState.getRawParameterValue("roomSize");
    dampingParam = valueTreeState.getRawParameterValue("damping");
    wetLevelParam = valueTreeState.getRawParameterValue("wetLevel");
    widthParam = valueTreeState.getRawParameterValue("width");
    bypassParam = valueTreeState.getRawParameterValue("bypass");
}

ReverbProcessor::~ReverbProcessor()
{
}

void ReverbProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    reverb.prepare(spec);
}

void ReverbProcessor::releaseResources()
{
}

void ReverbProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    if (*bypassParam > 0.5f)
        return;

    reverb.setRoomSize(*roomSizeParam);
    reverb.setDamping(*dampingParam);
    reverb.setWetLevel(*wetLevelParam);
    reverb.setWidth(*widthParam);

    reverb.processBlock(buffer);
}

juce::AudioProcessorEditor* ReverbProcessor::createEditor()
{
    return new ReverbEditor(*this);
}

void ReverbProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = valueTreeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ReverbProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(valueTreeState.state.getType()))
            valueTreeState.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReverbProcessor();
}
