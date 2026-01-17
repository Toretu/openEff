#include "CompressorProcessor.h"
#include "CompressorEditor.h"

CompressorAudioProcessor::CompressorAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, juce::Identifier("Compressor"), createParameterLayout())
{
}

CompressorAudioProcessor::~CompressorAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout CompressorAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "threshold", "Threshold", 
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ratio", "Ratio", 
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f), 4.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "attack", "Attack", 
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.3f), 10.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "release", "Release", 
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.3f), 100.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "makeup", "Makeup", 
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f), 0.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "bypass", "Bypass", false));

    return { params.begin(), params.end() };
}

void CompressorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    compressorEffect.prepare(sampleRate, samplesPerBlock);
}

void CompressorAudioProcessor::releaseResources()
{
    compressorEffect.reset();
}

bool CompressorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void CompressorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    bool bypass = parameters.getRawParameterValue("bypass")->load() > 0.5f;
    
    if (!bypass)
    {
        compressorEffect.setThreshold(parameters.getRawParameterValue("threshold")->load());
        compressorEffect.setRatio(parameters.getRawParameterValue("ratio")->load());
        compressorEffect.setAttack(parameters.getRawParameterValue("attack")->load());
        compressorEffect.setRelease(parameters.getRawParameterValue("release")->load());
        compressorEffect.setMakeupGain(parameters.getRawParameterValue("makeup")->load());
        compressorEffect.processBlock(buffer);
    }
}

juce::AudioProcessorEditor* CompressorAudioProcessor::createEditor()
{
    return new CompressorAudioProcessorEditor(*this);
}

void CompressorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CompressorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CompressorAudioProcessor();
}
