#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>
#include <vector>
#include <memory>
#include "../effects/EffectBase.h"

/**
 * Manages a chain of guitar effects.
 * Handles adding, removing, reordering, and processing audio through multiple effects.
 */
class EffectChain
{
public:
    EffectChain();
    ~EffectChain();
    
    //==============================================================================
    // DSP Processing
    
    /**
     * Prepares all effects in the chain for playback.
     * @param sampleRate The sample rate to prepare for
     * @param samplesPerBlock The maximum number of samples per block
     */
    void prepare(double sampleRate, int samplesPerBlock);
    
    /**
     * Resets all effects in the chain.
     */
    void reset();
    
    /**
     * Processes an audio buffer through the entire effect chain.
     * Each effect processes the buffer sequentially.
     * @param buffer The audio buffer to process (modified in-place)
     */
    void processBlock(juce::AudioBuffer<float>& buffer);
    
    //==============================================================================
    // Chain Management
    
    /**
     * Adds an effect to the end of the chain.
     * @param effect The effect to add (takes ownership)
     */
    void addEffect(std::unique_ptr<EffectBase> effect);
    
    /**
     * Removes an effect from the chain at the specified index.
     * @param index The index of the effect to remove
     * @return True if the effect was removed successfully
     */
    bool removeEffect(int index);
    
    /**
     * Moves an effect from one position to another in the chain.
     * @param fromIndex The current index of the effect
     * @param toIndex The target index for the effect
     * @return True if the effect was moved successfully
     */
    bool moveEffect(int fromIndex, int toIndex);
    
    /**
     * Removes all effects from the chain.
     */
    void clearChain();
    
    /**
     * Returns the number of effects in the chain.
     */
    int getNumEffects() const { return static_cast<int>(effects.size()); }
    
    /**
     * Returns a pointer to the effect at the specified index.
     * @param index The index of the effect
     * @return Pointer to the effect, or nullptr if index is out of range
     */
    EffectBase* getEffect(int index);
    
    /**
     * Returns a const pointer to the effect at the specified index.
     * @param index The index of the effect
     * @return Const pointer to the effect, or nullptr if index is out of range
     */
    const EffectBase* getEffect(int index) const;
    
    //==============================================================================
    // State Management
    
    /**
     * Saves the entire effect chain state to XML.
     * Includes effect types, order, and all parameters.
     * @return XML element containing the complete chain state
     */
    std::unique_ptr<juce::XmlElement> getStateInformation() const;
    
    /**
     * Restores the effect chain from XML.
     * Recreates all effects and restores their parameters.
     * @param xml XML element containing the chain state
     */
    void setStateInformation(const juce::XmlElement& xml);
    
private:
    std::vector<std::unique_ptr<EffectBase>> effects;
    double sampleRate = 44100.0;
    int samplesPerBlock = 512;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectChain)
};
