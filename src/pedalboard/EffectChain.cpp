#include "EffectChain.h"
#include "EffectFactory.h"

EffectChain::EffectChain()
{
}

EffectChain::~EffectChain()
{
}

void EffectChain::prepare(double newSampleRate, int newSamplesPerBlock)
{
    sampleRate = newSampleRate;
    samplesPerBlock = newSamplesPerBlock;
    
    // Prepare all effects in the chain
    for (auto& effect : effects)
    {
        if (effect)
            effect->prepare(sampleRate, samplesPerBlock);
    }
}

void EffectChain::reset()
{
    // Reset all effects in the chain
    for (auto& effect : effects)
    {
        if (effect)
            effect->reset();
    }
}

void EffectChain::processBlock(juce::AudioBuffer<float>& buffer)
{
    // Process through each effect in sequence
    for (auto& effect : effects)
    {
        if (effect && !effect->isBypassed())
        {
            effect->processBlock(buffer);
        }
    }
}

void EffectChain::addEffect(std::unique_ptr<EffectBase> effect)
{
    if (effect)
    {
        // Prepare the new effect with current settings
        effect->prepare(sampleRate, samplesPerBlock);
        effects.push_back(std::move(effect));
    }
}

bool EffectChain::removeEffect(int index)
{
    if (index < 0 || index >= static_cast<int>(effects.size()))
        return false;
    
    effects.erase(effects.begin() + index);
    return true;
}

bool EffectChain::moveEffect(int fromIndex, int toIndex)
{
    if (fromIndex < 0 || fromIndex >= static_cast<int>(effects.size()) ||
        toIndex < 0 || toIndex >= static_cast<int>(effects.size()) ||
        fromIndex == toIndex)
    {
        return false;
    }
    
    // Move the effect to the new position
    auto effect = std::move(effects[fromIndex]);
    effects.erase(effects.begin() + fromIndex);
    
    // Adjust toIndex if necessary (if we removed an element before the target)
    if (fromIndex < toIndex)
        toIndex--;
    
    effects.insert(effects.begin() + toIndex, std::move(effect));
    return true;
}

void EffectChain::clearChain()
{
    effects.clear();
}

EffectBase* EffectChain::getEffect(int index)
{
    if (index < 0 || index >= static_cast<int>(effects.size()))
        return nullptr;
    
    return effects[index].get();
}

const EffectBase* EffectChain::getEffect(int index) const
{
    if (index < 0 || index >= static_cast<int>(effects.size()))
        return nullptr;
    
    return effects[index].get();
}

std::unique_ptr<juce::XmlElement> EffectChain::getStateInformation() const
{
    auto xml = std::make_unique<juce::XmlElement>("EffectChain");
    
    // Save each effect's state
    for (const auto& effect : effects)
    {
        if (effect)
        {
            auto effectXml = std::make_unique<juce::XmlElement>("Effect");
            effectXml->setAttribute("type", effect->getEffectType());
            
            // Get the effect's internal state
            auto effectState = effect->getStateInformation();
            if (effectState)
                effectXml->addChildElement(effectState.release());
            
            xml->addChildElement(effectXml.release());
        }
    }
    
    return xml;
}

void EffectChain::setStateInformation(const juce::XmlElement& xml)
{
    if (!xml.hasTagName("EffectChain"))
        return;
    
    // Clear existing chain
    clearChain();
    
    // Recreate effects from XML
    for (auto* effectXml : xml.getChildIterator())
    {
        if (effectXml->hasTagName("Effect"))
        {
            juce::String effectType = effectXml->getStringAttribute("type");
            
            // Create the effect using the factory
            auto effect = EffectFactory::createEffect(effectType);
            
            if (effect)
            {
                // Restore the effect's state
                if (auto* effectState = effectXml->getFirstChildElement())
                {
                    effect->setStateInformation(*effectState);
                }
                
                // Add to chain
                addEffect(std::move(effect));
            }
        }
    }
}
