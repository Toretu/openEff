#include "EffectFactory.h"
#include "../effects/Fuzz.h"
#include "../effects/Compressor.h"
#include "../effects/Reverb.h"
#include "../effects/Chorus.h"
#include "../effects/Tuner.h"

std::unique_ptr<EffectBase> EffectFactory::createEffect(const juce::String& effectType)
{
    if (effectType == "fuzz")
        return std::make_unique<Fuzz>();
    
    if (effectType == "compressor")
        return std::make_unique<Compressor>();
    
    if (effectType == "reverb")
        return std::make_unique<Reverb>();
    
    if (effectType == "chorus")
        return std::make_unique<Chorus>();
    
    if (effectType == "tuner")
        return std::make_unique<Tuner>();
    
    // Unknown effect type
    jassertfalse;
    return nullptr;
}

juce::StringArray EffectFactory::getAvailableEffectTypes()
{
    return {
        "compressor",
        "fuzz",
        "reverb",
        "chorus",
        "tuner"
        // Add more effect types as they are implemented
    };
}

juce::String EffectFactory::getEffectDisplayName(const juce::String& effectType)
{
    if (effectType == "fuzz")
        return "Fuzz";
    
    if (effectType == "compressor")
        return "Compressor";
    
    if (effectType == "reverb")
        return "Reverb";
    
    if (effectType == "chorus")
        return "Chorus";
    
    if (effectType == "tuner")
        return "Tuner";
    
    return effectType;  // Fallback to type name
}

juce::String EffectFactory::getEffectCategory(const juce::String& effectType)
{
    if (effectType == "fuzz")
        return "Distortion";
    
    if (effectType == "compressor")
        return "Dynamics";
    
    if (effectType == "reverb")
        return "Time-based";
    
    if (effectType == "chorus")
        return "Modulation";
    
    if (effectType == "tuner")
        return "Utility";
    
    return "Other";
}
