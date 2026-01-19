#pragma once

#include <juce_core/juce_core.h>
#include <memory>
#include "../effects/EffectBase.h"

/**
 * Factory class for creating guitar effect instances by type name.
 * Uses the Factory design pattern to enable dynamic effect creation.
 */
class EffectFactory
{
public:
    /**
     * Creates a new effect instance based on the effect type string.
     * @param effectType The type of effect to create (e.g., "fuzz", "compressor", "reverb")
     * @return Unique pointer to the created effect, or nullptr if type is unknown
     */
    static std::unique_ptr<EffectBase> createEffect(const juce::String& effectType);
    
    /**
     * Returns a list of all available effect types.
     * @return StringArray containing all registered effect type names
     */
    static juce::StringArray getAvailableEffectTypes();
    
    /**
     * Returns a human-readable display name for an effect type.
     * @param effectType The effect type identifier
     * @return The display name (e.g., "fuzz" -> "Fuzz")
     */
    static juce::String getEffectDisplayName(const juce::String& effectType);
    
    /**
     * Returns the category for an effect type.
     * @param effectType The effect type identifier
     * @return The category (e.g., "Distortion", "Dynamics", "Time-based")
     */
    static juce::String getEffectCategory(const juce::String& effectType);

private:
    EffectFactory() = delete;  // Prevent instantiation
};
