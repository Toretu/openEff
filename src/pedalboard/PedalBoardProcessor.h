#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "EffectChain.h"

/**
 * Main audio processor for the OpenGuitar Pedal Board VST3 plugin.
 * Manages a chain of guitar effects with dynamic parameter management.
 */
class PedalBoardProcessor : public juce::AudioProcessor
{
public:
    PedalBoardProcessor();
    ~PedalBoardProcessor() override;

    //==============================================================================
    // AudioProcessor overrides
    
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    // Editor
    
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    // Plugin properties
    
    const juce::String getName() const override;
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    // Programs
    
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    //==============================================================================
    // State management
    
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Effect chain management
    
    /**
     * Adds an effect to the chain.
     * @param effectType The type of effect to add (e.g., "fuzz", "compressor")
     */
    void addEffectToChain(const juce::String& effectType);
    
    /**
     * Removes an effect from the chain.
     * @param index The index of the effect to remove
     */
    void removeEffectFromChain(int index);
    
    /**
     * Moves an effect within the chain.
     * @param fromIndex Current position
     * @param toIndex Target position
     */
    void moveEffectInChain(int fromIndex, int toIndex);
    
    /**
     * Returns the effect chain for UI access.
     */
    EffectChain& getEffectChain() { return effectChain; }
    
    /**
     * Returns the APVTS for parameter access.
     */
    juce::AudioProcessorValueTreeState& getAPVTS() { return *apvts; }

private:
    //==============================================================================
    // Core components
    
    EffectChain effectChain;
    std::unique_ptr<juce::AudioProcessorValueTreeState> apvts;
    
    // Global parameters
    std::atomic<float>* inputGainParam = nullptr;
    std::atomic<float>* outputGainParam = nullptr;
    std::atomic<float>* globalBypassParam = nullptr;
    
    //==============================================================================
    // Parameter management
    
    /**
     * Creates the initial parameter layout with global parameters.
     */
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    /**
     * Rebuilds the entire parameter tree when effects are added/removed.
     * This is called on the message thread, not the audio thread.
     */
    void rebuildParameterLayout();
    
    /**
     * Updates parameter pointers after rebuild.
     */
    void updateParameterPointers();
    
    /**
     * Updates effect parameters from APVTS values.
     * Called in processBlock to sync UI changes to effects.
     */
    void updateEffectParametersFromAPVTS();
    
    //==============================================================================
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalBoardProcessor)
};
