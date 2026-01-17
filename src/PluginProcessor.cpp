#include "PluginProcessor.h"
#include "PluginEditor.h"

OpenGuitarAudioProcessor::OpenGuitarAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, juce::Identifier("OpenGuitar"), createParameterLayout())
{
}

OpenGuitarAudioProcessor::~OpenGuitarAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout OpenGuitarAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Fuzz parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "gain", "Gain", 
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 5.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "tone", "Tone", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "level", "Level", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "bypass", "Bypass", false));

    return { params.begin(), params.end() };
}

void OpenGuitarAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    fuzzEffect.prepare(sampleRate, samplesPerBlock);
}

void OpenGuitarAudioProcessor::releaseResources()
{
    fuzzEffect.reset();
}

bool OpenGuitarAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void OpenGuitarAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    bool bypass = parameters.getRawParameterValue("bypass")->load() > 0.5f;
    
    if (!bypass)
    {
        fuzzEffect.setGain(parameters.getRawParameterValue("gain")->load());
        fuzzEffect.setTone(parameters.getRawParameterValue("tone")->load());
        fuzzEffect.setLevel(parameters.getRawParameterValue("level")->load());
        fuzzEffect.processBlock(buffer);
    }
}

juce::AudioProcessorEditor* OpenGuitarAudioProcessor::createEditor()
{
    return new OpenGuitarAudioProcessorEditor(*this);
}

void OpenGuitarAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OpenGuitarAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OpenGuitarAudioProcessor();
}
