#include "PedalBoardProcessor.h"
#include "PedalBoardEditor.h"
#include "EffectFactory.h"
#include "../effects/Fuzz.h"
#include "../effects/Compressor.h"
#include "../effects/Reverb.h"

PedalBoardProcessor::PedalBoardProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Create APVTS with initial parameter layout
    apvts = std::make_unique<juce::AudioProcessorValueTreeState>(
        *this, nullptr, "Parameters", createParameterLayout());
    
    updateParameterPointers();
}

PedalBoardProcessor::~PedalBoardProcessor()
{
}

//==============================================================================
const juce::String PedalBoardProcessor::getName() const
{
    return JucePlugin_Name;
}

//==============================================================================
void PedalBoardProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    effectChain.prepare(sampleRate, samplesPerBlock);
}

void PedalBoardProcessor::releaseResources()
{
    effectChain.reset();
}

bool PedalBoardProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Support mono and stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Input and output layouts must match
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void PedalBoardProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Check for global bypass
    bool isBypassed = globalBypassParam != nullptr && *globalBypassParam > 0.5f;
    
    if (isBypassed)
    {
        // When bypassed, just pass audio through
        return;
    }
    
    // Update effect parameters from APVTS before processing
    updateEffectParametersFromAPVTS();
    
    // Apply input gain
    if (inputGainParam != nullptr)
    {
        float inputGainDb = *inputGainParam;
        float inputGainLinear = juce::Decibels::decibelsToGain(inputGainDb);
        buffer.applyGain(inputGainLinear);
    }
    
    // Process through effect chain
    effectChain.processBlock(buffer);
    
    // Apply output gain
    if (outputGainParam != nullptr)
    {
        float outputGainDb = *outputGainParam;
        float outputGainLinear = juce::Decibels::decibelsToGain(outputGainDb);
        buffer.applyGain(outputGainLinear);
    }
}

//==============================================================================
juce::AudioProcessorEditor* PedalBoardProcessor::createEditor()
{
    return new PedalBoardEditor(*this);
}

//==============================================================================
void PedalBoardProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Create root XML element
    auto xml = std::make_unique<juce::XmlElement>("PedalBoardState");
    
    // Save global parameters
    if (apvts)
    {
        auto globalParams = apvts->copyState();
        auto globalParamsXml = globalParams.createXml();
        if (globalParamsXml)
            xml->addChildElement(globalParamsXml.release());
    }
    
    // Save effect chain state
    auto chainState = effectChain.getStateInformation();
    if (chainState)
        xml->addChildElement(chainState.release());
    
    // Convert to binary
    copyXmlToBinary(*xml, destData);
}

void PedalBoardProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Parse XML from binary
    auto xml = getXmlFromBinary(data, sizeInBytes);
    
    if (xml && xml->hasTagName("PedalBoardState"))
    {
        // Restore global parameters
        if (auto* paramsXml = xml->getChildByName("Parameters"))
        {
            auto valueTree = juce::ValueTree::fromXml(*paramsXml);
            if (valueTree.isValid() && apvts)
                apvts->replaceState(valueTree);
        }
        
        // Restore effect chain
        if (auto* chainXml = xml->getChildByName("EffectChain"))
        {
            effectChain.setStateInformation(*chainXml);
            
            // After loading the chain, we need to rebuild parameters
            // This should be done on the message thread
            juce::MessageManager::callAsync([this]()
            {
                rebuildParameterLayout();
            });
        }
        
        updateParameterPointers();
    }
}

//==============================================================================
void PedalBoardProcessor::addEffectToChain(const juce::String& effectType)
{
    // Create the effect
    auto effect = EffectFactory::createEffect(effectType);
    
    if (effect)
    {
        effectChain.addEffect(std::move(effect));
        
        // Rebuild parameter layout to include new effect's parameters
        rebuildParameterLayout();
    }
}

void PedalBoardProcessor::removeEffectFromChain(int index)
{
    if (effectChain.removeEffect(index))
    {
        // Rebuild parameter layout
        rebuildParameterLayout();
    }
}

void PedalBoardProcessor::moveEffectInChain(int fromIndex, int toIndex)
{
    effectChain.moveEffect(fromIndex, toIndex);
    // No need to rebuild parameters for reordering
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout PedalBoardProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Global parameters
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "inputGain",
        "Input Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f,
        "dB"));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "outputGain",
        "Output Gain",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f,
        "dB"));
    
    layout.add(std::make_unique<juce::AudioParameterBool>(
        "globalBypass",
        "Global Bypass",
        false));
    
    // Add parameters for each effect in the chain
    int effectIndex = 0;
    for (int i = 0; i < effectChain.getNumEffects(); ++i)
    {
        if (auto* effect = effectChain.getEffect(i))
        {
            juce::String prefix = "effect" + juce::String(effectIndex) + "_";
            effect->addParametersToLayout(layout, prefix);
            effectIndex++;
        }
    }
    
    return layout;
}

void PedalBoardProcessor::rebuildParameterLayout()
{
    // Store current state
    auto currentState = apvts->copyState();
    
    // Create new APVTS with updated parameter layout
    apvts = std::make_unique<juce::AudioProcessorValueTreeState>(
        *this, nullptr, "Parameters", createParameterLayout());
    
    // Restore as much state as possible
    // Global parameters should be preserved
    if (currentState.isValid())
    {
        for (auto param : apvts->state)
        {
            juce::String paramId = param.getProperty("id").toString();
            if (currentState.hasProperty(paramId))
            {
                param.setProperty("value", currentState.getProperty(paramId), nullptr);
            }
        }
    }
    
    // Link effect parameters
    int effectIndex = 0;
    for (int i = 0; i < effectChain.getNumEffects(); ++i)
    {
        if (auto* effect = effectChain.getEffect(i))
        {
            juce::String prefix = "effect" + juce::String(effectIndex) + "_";
            effect->linkParameters(*apvts, prefix);
            effectIndex++;
        }
    }
    
    updateParameterPointers();
    
    // Notify host that parameters have changed
    updateHostDisplay();
}

void PedalBoardProcessor::updateParameterPointers()
{
    inputGainParam = apvts->getRawParameterValue("inputGain");
    outputGainParam = apvts->getRawParameterValue("outputGain");
    globalBypassParam = apvts->getRawParameterValue("globalBypass");
}

void PedalBoardProcessor::updateEffectParametersFromAPVTS()
{
    // Update each effect's parameters from APVTS
    int effectIndex = 0;
    for (int i = 0; i < effectChain.getNumEffects(); ++i)
    {
        if (auto* effect = effectChain.getEffect(i))
        {
            juce::String prefix = "effect" + juce::String(effectIndex) + "_";
            
            if (effect->getEffectType() == "fuzz")
            {
                if (auto* gainParam = apvts->getRawParameterValue(prefix + "gain"))
                    dynamic_cast<Fuzz*>(effect)->setGain(*gainParam);
                if (auto* toneParam = apvts->getRawParameterValue(prefix + "tone"))
                    dynamic_cast<Fuzz*>(effect)->setTone(*toneParam);
                if (auto* levelParam = apvts->getRawParameterValue(prefix + "level"))
                    dynamic_cast<Fuzz*>(effect)->setLevel(*levelParam);
            }
            else if (effect->getEffectType() == "compressor")
            {
                if (auto* thresholdParam = apvts->getRawParameterValue(prefix + "threshold"))
                    dynamic_cast<Compressor*>(effect)->setThreshold(*thresholdParam);
                if (auto* ratioParam = apvts->getRawParameterValue(prefix + "ratio"))
                    dynamic_cast<Compressor*>(effect)->setRatio(*ratioParam);
                if (auto* attackParam = apvts->getRawParameterValue(prefix + "attack"))
                    dynamic_cast<Compressor*>(effect)->setAttack(*attackParam);
                if (auto* releaseParam = apvts->getRawParameterValue(prefix + "release"))
                    dynamic_cast<Compressor*>(effect)->setRelease(*releaseParam);
                if (auto* makeupParam = apvts->getRawParameterValue(prefix + "makeupGain"))
                    dynamic_cast<Compressor*>(effect)->setMakeupGain(*makeupParam);
            }
            else if (effect->getEffectType() == "reverb")
            {
                if (auto* roomSizeParam = apvts->getRawParameterValue(prefix + "roomSize"))
                    dynamic_cast<Reverb*>(effect)->setRoomSize(*roomSizeParam);
                if (auto* dampingParam = apvts->getRawParameterValue(prefix + "damping"))
                    dynamic_cast<Reverb*>(effect)->setDamping(*dampingParam);
                if (auto* wetLevelParam = apvts->getRawParameterValue(prefix + "wetLevel"))
                    dynamic_cast<Reverb*>(effect)->setWetLevel(*wetLevelParam);
                if (auto* widthParam = apvts->getRawParameterValue(prefix + "width"))
                    dynamic_cast<Reverb*>(effect)->setWidth(*widthParam);
            }
            
            effectIndex++;
        }
    }
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PedalBoardProcessor();
}
