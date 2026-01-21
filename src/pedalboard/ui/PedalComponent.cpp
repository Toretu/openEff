#include "PedalComponent.h"
#include "../../effects/Tuner.h"

PedalComponent::PedalComponent(EffectBase* eff,
                               juce::AudioProcessorValueTreeState& valueTreeState,
                               const juce::String& prefix,
                               int idx)
    : effect(eff), apvts(valueTreeState), paramPrefix(prefix), effectIndex(idx)
{
    // Setup name label
    nameLabel.setText(effect->getName(), juce::dontSendNotification);
    nameLabel.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    nameLabel.setJustificationType(juce::Justification::centred);
    nameLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(nameLabel);
    
    // Setup bypass button
    bypassButton.setButtonText("Bypass");
    bypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible(bypassButton);
    
    // Create parameter controls (this must happen before setSize)
    createControlsForEffect();
    
    setSize(180, 280);
}

PedalComponent::~PedalComponent()
{
}

//==============================================================================
void PedalComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Draw shadow if being dragged
    if (beingDragged)
    {
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillRoundedRectangle(bounds.expanded(4), 12.0f);
    }
    
    // Draw pedal background with gradient
    auto pedalColour = getPedalColour();
    juce::ColourGradient gradient(pedalColour.brighter(0.2f), bounds.getCentreX(), bounds.getY(),
                                 pedalColour.darker(0.3f), bounds.getCentreX(), bounds.getBottom(),
                                 false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 10.0f);
    
    // Draw metallic rim
    g.setColour(juce::Colour(0xff8a8a8a));
    g.drawRoundedRectangle(bounds, 10.0f, 2.0f);
    
    // Draw inner shadow
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.drawRoundedRectangle(bounds.reduced(2), 8.0f, 1.0f);
    
    // Draw screws in corners
    auto screwColour = juce::Colour(0xff5a5a5a);
    auto screwSize = 8.0f;
    auto screwInset = 10.0f;
    
    auto drawScrew = [&](float x, float y)
    {
        g.setColour(screwColour);
        g.fillEllipse(x - screwSize / 2, y - screwSize / 2, screwSize, screwSize);
        g.setColour(screwColour.darker(0.4f));
        g.drawLine(x - 2, y, x + 2, y, 1.0f);
    };
    
    drawScrew(bounds.getX() + screwInset, bounds.getY() + screwInset);
    drawScrew(bounds.getRight() - screwInset, bounds.getY() + screwInset);
    drawScrew(bounds.getX() + screwInset, bounds.getBottom() - screwInset);
    drawScrew(bounds.getRight() - screwInset, bounds.getBottom() - screwInset);
}

void PedalComponent::resized()
{
    auto bounds = getLocalBounds().reduced(15);
    
    // Name label at top
    nameLabel.setBounds(bounds.removeFromTop(30));
    
    bounds.removeFromTop(5);
    
    // Special handling for tuner
    if (effect->getEffectType() == "tuner" && tunerNoteLabel && tunerArrowLabel && tunerCentsLabel)
    {
        auto displayArea = bounds.removeFromTop(150);
        displayArea.removeFromTop(5); // Top spacing
        tunerNoteLabel->setBounds(displayArea.removeFromTop(60));
        displayArea.removeFromTop(5); // Spacing
        tunerArrowLabel->setBounds(displayArea.removeFromTop(45));
        displayArea.removeFromTop(5); // Spacing
        tunerCentsLabel->setBounds(displayArea.removeFromTop(30));
    }
    else
    {
        // Arrange knobs in a grid
        int numKnobs = sliders.size();
        if (numKnobs > 0)
        {
            int knobsPerRow = (numKnobs <= 2) ? numKnobs : 2;
            int knobSize = 60;
            int labelHeight = 25;
            int rowSpacing = 10;
            
            int numRows = (numKnobs + knobsPerRow - 1) / knobsPerRow;
            int rowHeight = knobSize + labelHeight;
            
            // Calculate total width needed for knobs
            int totalKnobWidth = knobsPerRow * knobSize + (knobsPerRow - 1) * 10;
            int startX = (bounds.getWidth() - totalKnobWidth) / 2;
            
            for (int i = 0; i < numKnobs; ++i)
            {
                int row = i / knobsPerRow;
                int col = i % knobsPerRow;
                
                int x = startX + col * (knobSize + 10);
                int y = row * (rowHeight + rowSpacing);
                
                if (auto* slider = sliders[i])
                {
                    slider->setBounds(x, y, knobSize, knobSize);
                }
                
                if (auto* label = sliderLabels[i])
                {
                    label->setBounds(x, y + knobSize + 2, knobSize, labelHeight);
                }
            }
            
            // Remove the space used by knobs
            bounds.removeFromTop(numRows * (rowHeight + rowSpacing));
        }
    }
    
    // Bypass button at bottom
    bounds.removeFromTop(10);
    bypassButton.setBounds(bounds.removeFromBottom(30));
}

//==============================================================================
void PedalComponent::createControlsForEffect()
{
    // Get parameter list from APVTS that match our prefix
    auto& params = apvts.state;
    
    if (effect->getEffectType() == "fuzz")
    {
        // Gain knob
        auto* gainSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        gainSlider->setRange(0.0, 10.0, 0.1);
        gainSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        gainSlider->setNumDecimalPlacesToDisplay(1);
        gainSlider->setEnabled(true);
        gainSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(gainSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "gain", *gainSlider));
        
        auto* gainLabel = sliderLabels.add(new juce::Label());
        gainLabel->setText("Gain", juce::dontSendNotification);
        gainLabel->setJustificationType(juce::Justification::centred);
        gainLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        gainLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        gainLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(gainLabel);
        
        // Tone knob
        auto* toneSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        toneSlider->setRange(0.0, 1.0, 0.01);
        toneSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        toneSlider->setNumDecimalPlacesToDisplay(2);
        toneSlider->setEnabled(true);
        toneSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(toneSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "tone", *toneSlider));
        
        auto* toneLabel = sliderLabels.add(new juce::Label());
        toneLabel->setText("Tone", juce::dontSendNotification);
        toneLabel->setJustificationType(juce::Justification::centred);
        toneLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        toneLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        toneLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(toneLabel);
        
        // Level knob
        auto* levelSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        levelSlider->setRange(0.0, 1.0, 0.01);
        levelSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        levelSlider->setNumDecimalPlacesToDisplay(2);
        levelSlider->setEnabled(true);
        levelSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(levelSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "level", *levelSlider));
        
        auto* levelLabel = sliderLabels.add(new juce::Label());
        levelLabel->setText("Level", juce::dontSendNotification);
        levelLabel->setJustificationType(juce::Justification::centred);
        levelLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        levelLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        levelLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(levelLabel);
    }
    else if (effect->getEffectType() == "bigmuff")
    {
        // Sustain knob
        auto* sustainSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        sustainSlider->setRange(0.0, 1.0, 0.01);
        sustainSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        sustainSlider->setNumDecimalPlacesToDisplay(2);
        sustainSlider->setEnabled(true);
        sustainSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(sustainSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "sustain", *sustainSlider));
        
        auto* sustainLabel = sliderLabels.add(new juce::Label());
        sustainLabel->setText("Sustain", juce::dontSendNotification);
        sustainLabel->setJustificationType(juce::Justification::centred);
        sustainLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        sustainLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        sustainLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(sustainLabel);
        
        // Tone knob
        auto* toneSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        toneSlider->setRange(0.0, 1.0, 0.01);
        toneSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        toneSlider->setNumDecimalPlacesToDisplay(2);
        toneSlider->setEnabled(true);
        toneSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(toneSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "tone", *toneSlider));
        
        auto* toneLabel = sliderLabels.add(new juce::Label());
        toneLabel->setText("Tone", juce::dontSendNotification);
        toneLabel->setJustificationType(juce::Justification::centred);
        toneLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        toneLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        toneLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(toneLabel);
        
        // Volume knob
        auto* volumeSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        volumeSlider->setRange(0.0, 1.0, 0.01);
        volumeSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        volumeSlider->setNumDecimalPlacesToDisplay(2);
        volumeSlider->setEnabled(true);
        volumeSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(volumeSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "volume", *volumeSlider));
        
        auto* volumeLabel = sliderLabels.add(new juce::Label());
        volumeLabel->setText("Volume", juce::dontSendNotification);
        volumeLabel->setJustificationType(juce::Justification::centred);
        volumeLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        volumeLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        volumeLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(volumeLabel);
    }
    else if (effect->getEffectType() == "orange")
    {
        // Gain
        auto* gainSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        gainSlider->setRange(0.0, 1.0, 0.01);
        gainSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        gainSlider->setNumDecimalPlacesToDisplay(2);
        gainSlider->setEnabled(true);
        gainSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(gainSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "gain", *gainSlider));
        
        auto* gainLabel = sliderLabels.add(new juce::Label());
        gainLabel->setText("Gain", juce::dontSendNotification);
        gainLabel->setJustificationType(juce::Justification::centred);
        gainLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        gainLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        gainLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(gainLabel);
        
        // Tone
        auto* toneSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        toneSlider->setRange(0.0, 1.0, 0.01);
        toneSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        toneSlider->setNumDecimalPlacesToDisplay(2);
        toneSlider->setEnabled(true);
        toneSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(toneSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "tone", *toneSlider));
        
        auto* toneLabel = sliderLabels.add(new juce::Label());
        toneLabel->setText("Tone", juce::dontSendNotification);
        toneLabel->setJustificationType(juce::Justification::centred);
        toneLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        toneLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        toneLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(toneLabel);
        
        // Level
        auto* levelSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        levelSlider->setRange(0.0, 1.0, 0.01);
        levelSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        levelSlider->setNumDecimalPlacesToDisplay(2);
        levelSlider->setEnabled(true);
        levelSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(levelSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "level", *levelSlider));
        
        auto* levelLabel = sliderLabels.add(new juce::Label());
        levelLabel->setText("Level", juce::dontSendNotification);
        levelLabel->setJustificationType(juce::Justification::centred);
        levelLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        levelLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        levelLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(levelLabel);
    }
    else if (effect->getEffectType() == "compressor")
    {
        // Threshold
        auto* threshSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        threshSlider->setRange(-60.0, 0.0, 0.1);
        threshSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        threshSlider->setNumDecimalPlacesToDisplay(1);
        threshSlider->setEnabled(true);
        threshSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(threshSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "threshold", *threshSlider));
        
        auto* threshLabel = sliderLabels.add(new juce::Label());
        threshLabel->setText("Thresh", juce::dontSendNotification);
        threshLabel->setJustificationType(juce::Justification::centred);
        threshLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        threshLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        threshLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(threshLabel);
        
        // Ratio
        auto* ratioSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        ratioSlider->setRange(1.0, 20.0, 0.1);
        ratioSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        ratioSlider->setNumDecimalPlacesToDisplay(1);
        ratioSlider->setEnabled(true);
        ratioSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(ratioSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "ratio", *ratioSlider));
        
        auto* ratioLabel = sliderLabels.add(new juce::Label());
        ratioLabel->setText("Ratio", juce::dontSendNotification);
        ratioLabel->setJustificationType(juce::Justification::centred);
        ratioLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        ratioLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        ratioLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(ratioLabel);
        
        // Attack
        auto* attackSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        attackSlider->setRange(0.1, 100.0, 0.1);
        attackSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        attackSlider->setNumDecimalPlacesToDisplay(1);
        attackSlider->setEnabled(true);
        attackSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(attackSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "attack", *attackSlider));
        
        auto* attackLabel = sliderLabels.add(new juce::Label());
        attackLabel->setText("Attack", juce::dontSendNotification);
        attackLabel->setJustificationType(juce::Justification::centred);
        attackLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        attackLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        attackLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(attackLabel);
        
        // Release
        auto* releaseSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        releaseSlider->setRange(10.0, 1000.0, 1.0);
        releaseSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        releaseSlider->setNumDecimalPlacesToDisplay(0);
        releaseSlider->setEnabled(true);
        releaseSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(releaseSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "release", *releaseSlider));
        
        auto* releaseLabel = sliderLabels.add(new juce::Label());
        releaseLabel->setText("Release", juce::dontSendNotification);
        releaseLabel->setJustificationType(juce::Justification::centred);
        releaseLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        releaseLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        releaseLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(releaseLabel);
    }
    else if (effect->getEffectType() == "reverb")
    {
        // Room Size
        auto* roomSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        roomSlider->setRange(0.0, 1.0, 0.01);
        roomSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        roomSlider->setNumDecimalPlacesToDisplay(2);
        roomSlider->setEnabled(true);
        roomSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(roomSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "roomSize", *roomSlider));
        
        auto* roomLabel = sliderLabels.add(new juce::Label());
        roomLabel->setText("Room", juce::dontSendNotification);
        roomLabel->setJustificationType(juce::Justification::centred);
        roomLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        roomLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        roomLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(roomLabel);
        
        // Damping
        auto* dampSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        dampSlider->setRange(0.0, 1.0, 0.01);
        dampSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        dampSlider->setNumDecimalPlacesToDisplay(2);
        dampSlider->setEnabled(true);
        dampSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(dampSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "damping", *dampSlider));
        
        auto* dampLabel = sliderLabels.add(new juce::Label());
        dampLabel->setText("Damp", juce::dontSendNotification);
        dampLabel->setJustificationType(juce::Justification::centred);
        dampLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        dampLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        dampLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(dampLabel);
        
        // Wet Level
        auto* wetSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        wetSlider->setRange(0.0, 1.0, 0.01);
        wetSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        wetSlider->setNumDecimalPlacesToDisplay(2);
        wetSlider->setEnabled(true);
        wetSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(wetSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "wetLevel", *wetSlider));
        
        auto* wetLabel = sliderLabels.add(new juce::Label());
        wetLabel->setText("Mix", juce::dontSendNotification);
        wetLabel->setJustificationType(juce::Justification::centred);
        wetLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        wetLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        wetLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(wetLabel);
    }
    else if (effect->getEffectType() == "chorus")
    {
        // Rate knob
        auto* rateSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        rateSlider->setRange(0.1, 5.0, 0.01);
        rateSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        rateSlider->setNumDecimalPlacesToDisplay(2);
        rateSlider->setEnabled(true);
        rateSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(rateSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "rate", *rateSlider));
        
        auto* rateLabel = sliderLabels.add(new juce::Label());
        rateLabel->setText("Rate", juce::dontSendNotification);
        rateLabel->setJustificationType(juce::Justification::centred);
        rateLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        rateLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        rateLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(rateLabel);
        
        // Depth knob
        auto* depthSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        depthSlider->setRange(0.0, 1.0, 0.01);
        depthSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        depthSlider->setNumDecimalPlacesToDisplay(2);
        depthSlider->setEnabled(true);
        depthSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(depthSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "depth", *depthSlider));
        
        auto* depthLabel = sliderLabels.add(new juce::Label());
        depthLabel->setText("Depth", juce::dontSendNotification);
        depthLabel->setJustificationType(juce::Justification::centred);
        depthLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        depthLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        depthLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(depthLabel);
        
        // Mix knob
        auto* mixSlider = sliders.add(new juce::Slider(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow));
        mixSlider->setRange(0.0, 1.0, 0.01);
        mixSlider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        mixSlider->setNumDecimalPlacesToDisplay(2);
        mixSlider->setEnabled(true);
        mixSlider->setInterceptsMouseClicks(true, true);
        addAndMakeVisible(mixSlider);
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, paramPrefix + "mix", *mixSlider));
        
        auto* mixLabel = sliderLabels.add(new juce::Label());
        mixLabel->setText("Mix", juce::dontSendNotification);
        mixLabel->setJustificationType(juce::Justification::centred);
        mixLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        mixLabel->setFont(juce::FontOptions(12.0f, juce::Font::bold));
        mixLabel->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(mixLabel);
    }
    else if (effect->getEffectType() == "tuner")
    {
        // Tuner has no knobs, just display labels
        tunerNoteLabel = std::make_unique<juce::Label>();
        tunerNoteLabel->setText("--", juce::dontSendNotification);
        tunerNoteLabel->setFont(juce::FontOptions(48.0f, juce::Font::bold));
        tunerNoteLabel->setJustificationType(juce::Justification::centred);
        tunerNoteLabel->setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(tunerNoteLabel.get());
        
        tunerArrowLabel = std::make_unique<juce::Label>();
        tunerArrowLabel->setText("", juce::dontSendNotification);
        tunerArrowLabel->setFont(juce::FontOptions(36.0f, juce::Font::bold));
        tunerArrowLabel->setJustificationType(juce::Justification::centred);
        tunerArrowLabel->setColour(juce::Label::textColourId, juce::Colours::yellow);
        addAndMakeVisible(tunerArrowLabel.get());
        
        tunerCentsLabel = std::make_unique<juce::Label>();
        tunerCentsLabel->setText("0¢", juce::dontSendNotification);
        tunerCentsLabel->setFont(juce::FontOptions(20.0f, juce::Font::bold));
        tunerCentsLabel->setJustificationType(juce::Justification::centred);
        tunerCentsLabel->setColour(juce::Label::textColourId, juce::Colours::lightgrey);
        addAndMakeVisible(tunerCentsLabel.get());
        
        // Start timer to update tuner display - slower for stability
        startTimer(100); // Update 10 times per second
    }
    
    // Note: Bypass attachment would need a parameter to be added to the APVTS for each effect
    // For now, we'll handle bypass through the effect's setBypassed method
}

juce::Colour PedalComponent::getPedalColour() const
{
    // Different colors for different effect types
    if (effect->getEffectType() == "fuzz")
        return juce::Colour(0xff8b4513); // Brown/orange for fuzz
    else if (effect->getEffectType() == "bigmuff")
        return juce::Colour(0xffc71585); // Medium violet red for Big Muff (classic EHX color)
    else if (effect->getEffectType() == "orange")
        return juce::Colour(0xffff8c00); // Bright orange for Orange amp
    else if (effect->getEffectType() == "compressor")
        return juce::Colour(0xff4169e1); // Royal blue for compressor
    else if (effect->getEffectType() == "reverb")
        return juce::Colour(0xff2e8b57); // Sea green for reverb
    else if (effect->getEffectType() == "chorus")
        return juce::Colour(0xff9370db); // Medium purple for chorus
    else if (effect->getEffectType() == "tuner")
        return juce::Colour(0xff00ced1); // Dark turquoise for tuner
    
    return juce::Colour(0xff3a3a3a); // Default grey
}

void PedalComponent::timerCallback()
{
    // Update tuner display
    if (effect->getEffectType() == "tuner" && tunerNoteLabel && tunerArrowLabel && tunerCentsLabel)
    {
        auto* tuner = dynamic_cast<Tuner*>(effect);
        if (tuner && tuner->isNoteDetected())
        {
            tunerNoteLabel->setText(tuner->getNoteName(), juce::dontSendNotification);
            
            float cents = tuner->getCentsDeviation();
            int direction = tuner->getTuningDirection();
            
            // Show arrow for tuning direction
            if (direction > 0)
            {
                tunerArrowLabel->setText("↑ TUNE UP", juce::dontSendNotification);
                tunerArrowLabel->setColour(juce::Label::textColourId, juce::Colours::orange);
            }
            else if (direction < 0)
            {
                tunerArrowLabel->setText("↓ TUNE DOWN", juce::dontSendNotification);
                tunerArrowLabel->setColour(juce::Label::textColourId, juce::Colours::orange);
            }
            else
            {
                tunerArrowLabel->setText("★ IN TUNE ★", juce::dontSendNotification);
                tunerArrowLabel->setColour(juce::Label::textColourId, juce::Colours::green);
            }
            
            juce::String centsText = juce::String(cents >= 0.0f ? "+" : "") + 
                                    juce::String(cents, 1) + "¢";
            tunerCentsLabel->setText(centsText, juce::dontSendNotification);
            
            // Color code based on how in-tune
            if (std::abs(cents) < 5.0f)
                tunerCentsLabel->setColour(juce::Label::textColourId, juce::Colours::green);
            else if (std::abs(cents) < 15.0f)
                tunerCentsLabel->setColour(juce::Label::textColourId, juce::Colours::yellow);
            else
                tunerCentsLabel->setColour(juce::Label::textColourId, juce::Colours::red);
        }
        else
        {
            // No signal detected
            tunerNoteLabel->setText("--", juce::dontSendNotification);
            tunerArrowLabel->setText("♪ Play a note", juce::dontSendNotification);
            tunerArrowLabel->setColour(juce::Label::textColourId, juce::Colours::grey);
            tunerCentsLabel->setText("", juce::dontSendNotification);
        }
    }
}
