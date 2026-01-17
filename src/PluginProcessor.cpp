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

    // Effect selection
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "effect", "Effect", 
        juce::StringArray{"Compressor", "Fuzz"}, 0));

    // Compressor parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "comp_threshold", "Comp Threshold", 
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "comp_ratio", "Comp Ratio", 
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 4.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "comp_attack", "Comp Attack", 
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.3f), 10.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "comp_release", "Comp Release", 
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.3f), 100.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "comp_makeup", "Comp Makeup", 
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f));

    // Fuzz parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "fuzz_gain", "Fuzz Gain", 
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f), 5.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "fuzz_tone", "Fuzz Tone", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "fuzz_level", "Fuzz Level", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "bypass", "Bypass", false));

    return { params.begin(), params.end() };
}

void OpenGuitarAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    compressorEffect.prepare(sampleRate, samplesPerBlock);
    fuzzEffect.prepare(sampleRate, samplesPerBlock);
}

void OpenGuitarAudioProcessor::releaseResources()
{
    compressorEffect.reset();
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

    // Clear any output channels that don't have input
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    bool bypass = parameters.getRawParameterValue("bypass")->load() > 0.5f;
    
    if (!bypass)
    {
        int effectChoice = static_cast<int>(parameters.getRawParameterValue("effect")->load());
        
        if (effectChoice == 0) // Compressor
        {
            compressorEffect.setThreshold(parameters.getRawParameterValue("comp_threshold")->load());
            compressorEffect.setRatio(parameters.getRawParameterValue("comp_ratio")->load());
            compressorEffect.setAttack(parameters.getRawParameterValue("comp_attack")->load());
            compressorEffect.setRelease(parameters.getRawParameterValue("comp_release")->load());
            compressorEffect.setMakeupGain(parameters.getRawParameterValue("comp_makeup")->load());
            compressorEffect.processBlock(buffer);
        }
        else if (effectChoice == 1) // Fuzz
        {
            fuzzEffect.setGain(parameters.getRawParameterValue("fuzz_gain")->load());
            fuzzEffect.setTone(parameters.getRawParameterValue("fuzz_tone")->load());
            fuzzEffect.setLevel(parameters.getRawParameterValue("fuzz_level")->load());
            fuzzEffect.processBlock(buffer);
        }
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
