#pragma once

#include "EffectBase.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class Tuner : public EffectBase
{
public:
    Tuner();
    ~Tuner() override;

    // Core processing
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    void processBlock(juce::AudioBuffer<float>& buffer) override;

    // Metadata
    juce::String getName() const override { return "Tuner"; }
    juce::String getEffectType() const override { return "tuner"; }

    // State management
    std::unique_ptr<juce::XmlElement> getStateInformation() const override;
    void setStateInformation(const juce::XmlElement& xml) override;

    // Parameter layout for APVTS
    void addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                               const juce::String& prefix) override;
    void linkParameters(juce::AudioProcessorValueTreeState& apvts,
                       const juce::String& prefix) override;

    // Tuner-specific getters
    float getDetectedFrequency() const { return detectedFrequency; }
    juce::String getNoteName() const { return noteName; }
    float getCentsDeviation() const { return centsDeviation; }
    bool isNoteDetected() const { return noteDetected; }
    int getTuningDirection() const { return centsDeviation > 5.0f ? 1 : (centsDeviation < -5.0f ? -1 : 0); }

private:
    // Pitch detection
    float detectPitch(const float* samples, int numSamples);
    float autocorrelation(const float* data, int numSamples, int lag);
    void updateNoteInfo(float frequency);
    
    // Tuner state
    float detectedFrequency = 0.0f;
    juce::String noteName = "--";
    float centsDeviation = 0.0f;
    bool noteDetected = false;
    
    // Processing buffers
    juce::AudioBuffer<float> analysisBuffer;
    int bufferWritePosition = 0;
    static constexpr int analysisBufferSize = 8192;
    static constexpr int analysisHopSize = 512;
    int samplesSinceLastAnalysis = 0;
    
    // Smoothing for stability (0.5 second averaging)
    std::vector<float> frequencyHistory;
    static constexpr int maxHistorySize = 25; // ~0.5 seconds at 50Hz update rate
    int historyWriteIndex = 0;
    
    // Note names
    static constexpr const char* noteNames[12] = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Tuner)
};
