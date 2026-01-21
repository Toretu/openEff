#include "Tuner.h"
#include <cmath>

Tuner::Tuner()
{
    frequencyHistory.resize(maxHistorySize, 0.0f);
}

Tuner::~Tuner()
{
}

void Tuner::prepare(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    this->samplesPerBlock = samplesPerBlock;
    
    // Allocate analysis buffer
    analysisBuffer.setSize(1, analysisBufferSize);
    reset();
}

void Tuner::reset()
{
    analysisBuffer.clear();
    bufferWritePosition = 0;
    samplesSinceLastAnalysis = 0;
    detectedFrequency = 0.0f;
    noteName = "--";
    centsDeviation = 0.0f;
    noteDetected = false;
    std::fill(frequencyHistory.begin(), frequencyHistory.end(), 0.0f);
    historyWriteIndex = 0;
}

void Tuner::processBlock(juce::AudioBuffer<float>& buffer)
{
    // Tuner passes audio through unchanged
    if (bypassed)
        return;
    
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    // Mix to mono for analysis
    juce::AudioBuffer<float> monoBuffer(1, numSamples);
    monoBuffer.clear();
    
    for (int ch = 0; ch < numChannels; ++ch)
    {
        monoBuffer.addFrom(0, 0, buffer, ch, 0, numSamples, 1.0f / numChannels);
    }
    
    // Copy to analysis buffer
    const float* monoData = monoBuffer.getReadPointer(0);
    float* analysisData = analysisBuffer.getWritePointer(0);
    
    for (int i = 0; i < numSamples; ++i)
    {
        analysisData[bufferWritePosition] = monoData[i];
        bufferWritePosition = (bufferWritePosition + 1) % analysisBufferSize;
    }
    
    samplesSinceLastAnalysis += numSamples;
    
    // Perform analysis every hop size
    if (samplesSinceLastAnalysis >= analysisHopSize)
    {
        samplesSinceLastAnalysis = 0;
        
        // Rearrange buffer for analysis (so newest samples are at the end)
        juce::AudioBuffer<float> orderedBuffer(1, analysisBufferSize);
        float* orderedData = orderedBuffer.getWritePointer(0);
        
        for (int i = 0; i < analysisBufferSize; ++i)
        {
            int readPos = (bufferWritePosition + i) % analysisBufferSize;
            orderedData[i] = analysisData[readPos];
        }
        
        // Detect pitch
        float frequency = detectPitch(orderedData, analysisBufferSize);
        
        if (frequency > 0.0f)
        {
            // Add to history for smoothing
            frequencyHistory[historyWriteIndex] = frequency;
            historyWriteIndex = (historyWriteIndex + 1) % maxHistorySize;
            
            // Calculate average of valid frequencies in history
            float sum = 0.0f;
            int count = 0;
            for (float f : frequencyHistory)
            {
                if (f > 0.0f)
                {
                    sum += f;
                    count++;
                }
            }
            
            if (count > 0)
            {
                float smoothedFrequency = sum / count;
                detectedFrequency = smoothedFrequency;
                updateNoteInfo(smoothedFrequency);
                noteDetected = true;
            }
        }
        else
        {
            noteDetected = false;
            noteName = "--";
            centsDeviation = 0.0f;
        }
    }
}

float Tuner::detectPitch(const float* samples, int numSamples)
{
    // Find RMS to check if signal is strong enough
    float rms = 0.0f;
    for (int i = 0; i < numSamples; ++i)
        rms += samples[i] * samples[i];
    rms = std::sqrt(rms / numSamples);
    
    // Threshold for detection
    if (rms < 0.01f)
        return 0.0f;
    
    // Autocorrelation-based pitch detection
    // Search range: 80 Hz to 1000 Hz (appropriate for guitar)
    int minLag = static_cast<int>(sampleRate / 1000.0f);
    int maxLag = static_cast<int>(sampleRate / 80.0f);
    
    if (maxLag >= numSamples)
        maxLag = numSamples - 1;
    
    float maxCorrelation = 0.0f;
    int bestLag = 0;
    
    for (int lag = minLag; lag < maxLag; ++lag)
    {
        float corr = autocorrelation(samples, numSamples, lag);
        
        if (corr > maxCorrelation)
        {
            maxCorrelation = corr;
            bestLag = lag;
        }
    }
    
    // Need strong correlation for reliable detection
    if (maxCorrelation < 0.5f || bestLag == 0)
        return 0.0f;
    
    // Parabolic interpolation for sub-sample accuracy
    if (bestLag > minLag && bestLag < maxLag - 1)
    {
        float y1 = autocorrelation(samples, numSamples, bestLag - 1);
        float y2 = maxCorrelation;
        float y3 = autocorrelation(samples, numSamples, bestLag + 1);
        
        float delta = 0.5f * (y3 - y1) / (2.0f * y2 - y1 - y3);
        float refinedLag = bestLag + delta;
        
        return static_cast<float>(sampleRate) / refinedLag;
    }
    
    return static_cast<float>(sampleRate) / bestLag;
}

float Tuner::autocorrelation(const float* data, int numSamples, int lag)
{
    float sum = 0.0f;
    float norm = 0.0f;
    
    int count = numSamples - lag;
    
    for (int i = 0; i < count; ++i)
    {
        sum += data[i] * data[i + lag];
        norm += data[i] * data[i];
    }
    
    return (norm > 0.0f) ? sum / norm : 0.0f;
}

void Tuner::updateNoteInfo(float frequency)
{
    // A4 = 440 Hz reference
    const float A4 = 440.0f;
    
    // Calculate distance in semitones from A4
    float semitonesFromA4 = 12.0f * std::log2(frequency / A4);
    
    // Round to nearest semitone to get note
    int semitoneIndex = static_cast<int>(std::round(semitonesFromA4));
    
    // Calculate cents deviation from that note
    centsDeviation = 100.0f * (semitonesFromA4 - semitoneIndex);
    
    // Get note name and octave
    int noteIndex = (semitoneIndex + 9) % 12; // +9 because A is index 9
    if (noteIndex < 0) noteIndex += 12;
    
    int octave = 4 + (semitoneIndex + 9) / 12;
    
    noteName = juce::String(noteNames[noteIndex]) + juce::String(octave);
}

std::unique_ptr<juce::XmlElement> Tuner::getStateInformation() const
{
    auto xml = std::make_unique<juce::XmlElement>("Tuner");
    xml->setAttribute("bypassed", bypassed);
    return xml;
}

void Tuner::setStateInformation(const juce::XmlElement& xml)
{
    if (xml.hasTagName("Tuner"))
    {
        bypassed = xml.getBoolAttribute("bypassed", false);
    }
}

void Tuner::addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout,
                                   const juce::String& prefix)
{
    // Tuner has no adjustable parameters, just display values
    // We could add reference frequency adjustment in the future
}

void Tuner::linkParameters(juce::AudioProcessorValueTreeState& apvts,
                           const juce::String& prefix)
{
    // No parameters to link
}
