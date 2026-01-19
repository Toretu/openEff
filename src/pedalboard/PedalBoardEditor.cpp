#include "PedalBoardEditor.h"
#include "EffectFactory.h"

PedalBoardEditor::PedalBoardEditor(PedalBoardProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    // Set larger editor size for pedal board
    setSize(900, 500);
    
    // Apply custom look and feel
    setLookAndFeel(&pedalLookAndFeel);
    
    // Setup title label
    titleLabel.setText("OPENGUITAR PEDAL BOARD", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);
    
    // Setup effect selector combo box
    effectSelector.addItem("Compressor", 1);
    effectSelector.addItem("Fuzz", 2);
    effectSelector.addItem("Reverb", 3);
    effectSelector.setSelectedId(1);
    effectSelector.addListener(this);
    addAndMakeVisible(effectSelector);
    
    // Setup buttons
    addEffectButton.setButtonText("Add Effect");
    addEffectButton.addListener(this);
    addAndMakeVisible(addEffectButton);
    
    removeLastButton.setButtonText("Remove Last");
    removeLastButton.addListener(this);
    addAndMakeVisible(removeLastButton);
    
    clearChainButton.setButtonText("Clear All");
    clearChainButton.addListener(this);
    addAndMakeVisible(clearChainButton);
    
    globalBypassButton.setButtonText("Bypass All");
    globalBypassButton.addListener(this);
    addAndMakeVisible(globalBypassButton);
    
    // Setup input gain control
    inputGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    inputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    inputGainSlider.setRange(-24.0, 24.0, 0.1);
    addAndMakeVisible(inputGainSlider);
    
    inputGainLabel.setText("Input", juce::dontSendNotification);
    inputGainLabel.setJustificationType(juce::Justification::centred);
    inputGainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(inputGainLabel);
    
    // Setup output gain control
    outputGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    outputGainSlider.setRange(-24.0, 24.0, 0.1);
    addAndMakeVisible(outputGainSlider);
    
    outputGainLabel.setText("Output", juce::dontSendNotification);
    outputGainLabel.setJustificationType(juce::Justification::centred);
    outputGainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(outputGainLabel);
    
    // Create parameter attachments
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "inputGain", inputGainSlider);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.getAPVTS(), "outputGain", outputGainSlider);
    globalBypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.getAPVTS(), "globalBypass", globalBypassButton);
    
    // Setup viewport for pedals
    addAndMakeVisible(pedalViewport);
    pedalViewport.setViewedComponent(&pedalContainer, false);
    pedalViewport.setScrollBarsShown(true, false);
    
    // Initial update
    rebuildPedalComponents();
}

PedalBoardEditor::~PedalBoardEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void PedalBoardEditor::paint(juce::Graphics& g)
{
    // Background gradient
    juce::ColourGradient gradient(juce::Colour(0xff1a1a1a), 0, 0,
                                 juce::Colour(0xff2a2a2a), 0, static_cast<float>(getHeight()),
                                 false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Draw wood texture effect for pedal board background
    g.setColour(juce::Colour(0xff3a2a1a).withAlpha(0.3f));
    g.fillRect(0, 80, getWidth(), getHeight() - 160);
}

void PedalBoardEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Title area
    auto titleArea = bounds.removeFromTop(60);
    titleLabel.setBounds(titleArea.reduced(10));
    
    // Control bar at top
    auto controlArea = bounds.removeFromTop(80);
    controlArea.reduce(10, 10);
    
    auto topRow = controlArea.removeFromTop(35);
    effectSelector.setBounds(topRow.removeFromLeft(150));
    topRow.removeFromLeft(10);
    addEffectButton.setBounds(topRow.removeFromLeft(100));
    topRow.removeFromLeft(10);
    removeLastButton.setBounds(topRow.removeFromLeft(110));
    topRow.removeFromLeft(10);
    clearChainButton.setBounds(topRow.removeFromLeft(100));
    topRow.removeFromLeft(20);
    globalBypassButton.setBounds(topRow.removeFromLeft(120));
    
    // Bottom control area with I/O gains
    auto bottomArea = bounds.removeFromBottom(120);
    bottomArea.reduce(20, 10);
    
    auto inputArea = bottomArea.removeFromLeft(100);
    inputGainLabel.setBounds(inputArea.removeFromTop(20));
    inputGainSlider.setBounds(inputArea);
    
    bottomArea.removeFromLeft(20);
    
    auto outputArea = bottomArea.removeFromRight(100);
    outputGainLabel.setBounds(outputArea.removeFromTop(20));
    outputGainSlider.setBounds(outputArea);
    
    // Pedal board viewport in the middle
    bounds.reduce(10, 10);
    pedalViewport.setBounds(bounds);
    
    // Update pedal layout
    updatePedalLayout();
}

//==============================================================================
void PedalBoardEditor::buttonClicked(juce::Button* button)
{
    if (button == &addEffectButton)
    {
        // Get selected effect type
        int selectedId = effectSelector.getSelectedId();
        juce::String effectType;
        
        switch (selectedId)
        {
            case 1: effectType = "compressor"; break;
            case 2: effectType = "fuzz"; break;
            case 3: effectType = "reverb"; break;
            default: return;
        }
        
        // Add effect to chain
        processor.addEffectToChain(effectType);
        rebuildPedalComponents();
    }
    else if (button == &removeLastButton)
    {
        // Remove last effect
        auto& chain = processor.getEffectChain();
        if (chain.getNumEffects() > 0)
        {
            processor.removeEffectFromChain(chain.getNumEffects() - 1);
            rebuildPedalComponents();
        }
    }
    else if (button == &clearChainButton)
    {
        // Clear the entire effect chain
        processor.getEffectChain().clearChain();
        rebuildPedalComponents();
    }
}

void PedalBoardEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    // Currently no action needed when selection changes
}

//==============================================================================
void PedalBoardEditor::rebuildPedalComponents()
{
    // Clear existing pedal components
    pedalComponents.clear();
    
    // Create new pedal components for each effect
    auto& chain = processor.getEffectChain();
    for (int i = 0; i < chain.getNumEffects(); ++i)
    {
        if (auto* effect = chain.getEffect(i))
        {
            juce::String prefix = "effect" + juce::String(i) + "_";
            auto* pedal = pedalComponents.add(new PedalComponent(effect, processor.getAPVTS(), prefix, i));
            pedalContainer.addAndMakeVisible(pedal);
        }
    }
    
    updatePedalLayout();
}

void PedalBoardEditor::updatePedalLayout()
{
    int pedalWidth = 180;
    int pedalHeight = 280;
    int pedalSpacing = 20;
    int xPos = pedalSpacing;
    int yPos = pedalSpacing;
    
    // Layout pedals horizontally
    for (auto* pedal : pedalComponents)
    {
        pedal->setBounds(xPos, yPos, pedalWidth, pedalHeight);
        xPos += pedalWidth + pedalSpacing;
    }
    
    // Set container size to fit all pedals
    int containerWidth = juce::jmax(pedalViewport.getWidth(), xPos);
    int containerHeight = yPos + pedalHeight + pedalSpacing;
    pedalContainer.setSize(containerWidth, containerHeight);
}
