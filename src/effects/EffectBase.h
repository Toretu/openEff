#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_core/juce_core.h>

/**
 * Abstract base class for all guitar effects in the OpenGuitar plugin.
 * Provides a unified interface for effect processing, parameter management,
 * and state serialization to enable flexible effect chaining in the Pedal Board.
 */
class EffectBase
{
public:
    EffectBase() = default;
    virtual ~EffectBase() = default;
    
    //==============================================================================
    // Core DSP Processing
    
    /**
     * Prepares the effect for playback.
     * Called before processing starts.
     * @param sampleRate The sample rate to prepare for
     * @param samplesPerBlock The maximum number of samples per block
     */
    virtual void prepare(double sampleRate, int samplesPerBlock) = 0;
    
    /**
     * Resets the effect's internal state.
     * Clears delay lines, envelope followers, etc.
     */
    virtual void reset() = 0;
    
    /**
     * Processes an audio buffer through the effect.
     * @param buffer The audio buffer to process (modified in-place)
     */
    virtual void processBlock(juce::AudioBuffer<float>& buffer) = 0;
    
    //==============================================================================
    // Bypass Control
    
    /**
     * Sets whether the effect is bypassed.
     * @param shouldBeBypassed True to bypass the effect
     */
    virtual void setBypassed(bool shouldBeBypassed) { bypassed = shouldBeBypassed; }
    
    /**
     * Returns whether the effect is currently bypassed.
     */
    virtual bool isBypassed() const { return bypassed; }
    
    //==============================================================================
    // Metadata
    
    /**
     * Returns the display name of the effect (e.g., "Fuzz", "Compressor").
     */
    virtual juce::String getName() const = 0;
    
    /**
     * Returns the effect type identifier (e.g., "fuzz", "compressor").
     * Used for effect factory and serialization.
     */
    virtual juce::String getEffectType() const = 0;
    
    //==============================================================================
    // State Management
    
    /**
     * Saves the effect's current state to XML.
     * @return XML element containing all effect parameters and state
     */
    virtual std::unique_ptr<juce::XmlElement> getStateInformation() const = 0;
    
    /**
     * Restores the effect's state from XML.
     * @param xml XML element containing the effect state
     */
    virtual void setStateInformation(const juce::XmlElement& xml) = 0;
    
    //==============================================================================
    // Parameter Management
    
    /**
     * Adds this effect's parameters to a parameter layout.
     * Used when building the APVTS for the Pedal Board plugin.
     * @param layout The parameter layout to add parameters to
     * @param prefix A prefix for parameter IDs (e.g., "effect1_" for the first effect)
     */
    virtual void addParametersToLayout(
        juce::AudioProcessorValueTreeState::ParameterLayout& layout,
        const juce::String& prefix) = 0;
    
    /**
     * Links this effect's internal parameters to APVTS parameters.
     * Called after the APVTS is created to establish parameter connections.
     * @param apvts The AudioProcessorValueTreeState containing the parameters
     * @param prefix The prefix used when adding parameters (must match addParametersToLayout)
     */
    virtual void linkParameters(
        juce::AudioProcessorValueTreeState& apvts,
        const juce::String& prefix) = 0;
    
protected:
    bool bypassed = false;
    double sampleRate = 44100.0;
    int samplesPerBlock = 512;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectBase)
};
