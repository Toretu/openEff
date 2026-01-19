# OpenGuitar Pedal Board - Implementation Plan

## Overview
Create a unified "Pedal Board" VST3 plugin that allows users to load, chain, reorder, and control multiple guitar effect pedals from the OpenGuitar collection (Compressor, Fuzz, Reverb, and future effects).

---

## Phase 1: Architecture & Foundation

### 1.1 Effect Base Class Design
**Goal:** Create a unified interface for all effects to enable plugin composition

**Tasks:**
- [ ] Create `EffectBase` abstract base class
  - Pure virtual methods: `processBlock()`, `prepare()`, `reset()`, `getStateInformation()`, `setStateInformation()`
  - Common interface: `bypass()`, `isBypassed()`, `getName()`, `getParameterCount()`
  - Virtual methods for parameter management
  
- [ ] Refactor existing effects to inherit from `EffectBase`
  - Update `Fuzz.h/cpp` to inherit from EffectBase
  - Update `Compressor.h/cpp` to inherit from EffectBase
  - Update `Reverb.h/cpp` to inherit from EffectBase
  - Ensure backward compatibility with standalone plugins

**File Structure:**
```
src/effects/
  ├── EffectBase.h           # Abstract base class
  ├── EffectBase.cpp
  ├── Fuzz.h                 # Updated to inherit from EffectBase
  ├── Fuzz.cpp
  ├── Compressor.h           # Updated to inherit from EffectBase
  ├── Compressor.cpp
  ├── Reverb.h               # Updated to inherit from EffectBase
  └── Reverb.cpp
```

**EffectBase Interface:**
```cpp
class EffectBase
{
public:
    virtual ~EffectBase() = default;
    
    // Core processing
    virtual void prepare(double sampleRate, int samplesPerBlock) = 0;
    virtual void reset() = 0;
    virtual void processBlock(juce::AudioBuffer<float>& buffer) = 0;
    
    // Bypass control
    virtual void setBypassed(bool shouldBeBypassed);
    virtual bool isBypassed() const { return bypassed; }
    
    // Metadata
    virtual juce::String getName() const = 0;
    virtual juce::String getEffectType() const = 0;
    
    // State management
    virtual std::unique_ptr<juce::XmlElement> getStateInformation() const = 0;
    virtual void setStateInformation(const juce::XmlElement& xml) = 0;
    
    // Parameter management
    virtual void addParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& prefix) = 0;
    virtual void linkParameters(juce::AudioProcessorValueTreeState& apvts, const juce::String& prefix) = 0;
    
protected:
    bool bypassed = false;
    double sampleRate = 44100.0;
    int samplesPerBlock = 512;
};
```

---

### 1.2 Effect Chain Manager
**Goal:** Manage the signal chain of effects with add/remove/reorder capabilities

**Tasks:**
- [ ] Create `EffectChain` class to manage effect ordering
  - Store effects in an ordered container (`std::vector<std::unique_ptr<EffectBase>>`)
  - Implement `addEffect()`, `removeEffect()`, `moveEffect()`, `clearChain()`
  - Implement `processBlock()` to route audio through chain
  - Handle state save/load for entire chain
  
- [ ] Create `EffectFactory` for effect instantiation
  - Factory pattern to create effects by name/type
  - Support for future effect additions without modifying core code

**File Structure:**
```
src/pedalboard/
  ├── EffectChain.h
  ├── EffectChain.cpp
  ├── EffectFactory.h
  └── EffectFactory.cpp
```

**EffectChain Interface:**
```cpp
class EffectChain
{
public:
    EffectChain();
    ~EffectChain();
    
    void prepare(double sampleRate, int samplesPerBlock);
    void reset();
    void processBlock(juce::AudioBuffer<float>& buffer);
    
    // Chain management
    void addEffect(std::unique_ptr<EffectBase> effect);
    void removeEffect(int index);
    void moveEffect(int fromIndex, int toIndex);
    void clearChain();
    int getNumEffects() const;
    EffectBase* getEffect(int index);
    
    // State management
    std::unique_ptr<juce::XmlElement> getStateInformation() const;
    void setStateInformation(const juce::XmlElement& xml);
    
private:
    std::vector<std::unique_ptr<EffectBase>> effects;
    double sampleRate = 44100.0;
    int samplesPerBlock = 512;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectChain)
};
```

---

## Phase 2: Pedal Board Processor

### 2.1 PedalBoardProcessor
**Goal:** Main audio processor for the Pedal Board VST3 plugin

**Tasks:**
- [ ] Create `PedalBoardProcessor` class (inherits from `juce::AudioProcessor`)
  - Integrate `EffectChain` as member variable
  - Handle audio routing through effect chain
  - Manage global parameters (input gain, output gain, global bypass)
  - Implement state save/load for presets
  
- [ ] Parameter management system
  - Dynamic parameter registration based on loaded effects
  - Use parameter prefix system (e.g., "effect1_gain", "effect2_threshold")
  - Support for parameter automation in DAWs

**File Structure:**
```
src/pedalboard/
  ├── PedalBoardProcessor.h
  └── PedalBoardProcessor.cpp
```

**Key Features:**
```cpp
class PedalBoardProcessor : public juce::AudioProcessor
{
public:
    PedalBoardProcessor();
    ~PedalBoardProcessor() override;
    
    // Standard AudioProcessor overrides
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    
    // Effect management
    void addEffectToChain(const juce::String& effectType);
    void removeEffectFromChain(int index);
    void moveEffectInChain(int fromIndex, int toIndex);
    
    // Access to chain
    EffectChain& getEffectChain() { return effectChain; }
    
private:
    EffectChain effectChain;
    juce::AudioProcessorValueTreeState parameters;
    
    void rebuildParameterLayout();
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
};
```

---

## Phase 3: User Interface

### 3.1 Pedal Component UI
**Goal:** Visual representation of individual pedal effects

**Tasks:**
- [ ] Create `PedalComponent` class for individual effect UI
  - Skeuomorphic design matching real guitar pedals
  - Rotary knobs for parameters (using `juce::Slider` with custom look and feel)
  - Bypass LED indicator
  - Drag handle for reordering
  - Effect name/type display
  
- [ ] Custom LookAndFeel for pedal aesthetics
  - Metal/brushed finish backgrounds
  - Vintage-style knobs
  - LED indicators for bypass status
  - Color-coding by effect type

**File Structure:**
```
src/pedalboard/ui/
  ├── PedalComponent.h
  ├── PedalComponent.cpp
  ├── PedalLookAndFeel.h
  └── PedalLookAndFeel.cpp
```

**Visual Layout (per pedal):**
```
┌─────────────────────┐
│  [EFFECT NAME]      │
│  ┌─────┐  ┌─────┐  │
│  │ ○   │  │ ○   │  │  <- Knobs
│  │Param│  │Param│  │
│  └─────┘  └─────┘  │
│  ┌─────┐  [●]      │  <- Bypass LED
│  │ ○   │            │
│  │Param│   [═══]   │  <- Drag handle
│  └─────┘            │
└─────────────────────┘
```

---

### 3.2 Pedal Board View
**Goal:** Main plugin editor showing effect chain

**Tasks:**
- [ ] Create `PedalBoardEditor` class (inherits from `juce::AudioProcessorEditor`)
  - Horizontal or vertical scrollable view of pedals
  - Add/remove effect buttons
  - Drag-and-drop reordering of pedals
  - Global bypass button
  - Input/output level meters
  
- [ ] Implement drag-and-drop reordering
  - Use `juce::DragAndDropContainer`
  - Visual feedback during drag
  - Snap-to-position behavior
  
- [ ] Add effect selector/browser
  - Popup menu or dedicated panel
  - Categories: Dynamics, Distortion, Modulation, Time-based, etc.
  - Visual preview of effect type

**File Structure:**
```
src/pedalboard/ui/
  ├── PedalBoardEditor.h
  ├── PedalBoardEditor.cpp
  ├── EffectSelectorComponent.h
  └── EffectSelectorComponent.cpp
```

**Main Editor Layout:**
```
┌─────────────────────────────────────────────────────┐
│ [+ Add Effect ▼]           [Global Bypass] [Preset▼]│
├─────────────────────────────────────────────────────┤
│  ┌──────┐    ┌──────┐    ┌──────┐    ┌──────┐      │
│  │Comp  │ -> │ Fuzz │ -> │Chorus│ -> │Reverb│      │
│  │      │    │      │    │      │    │      │      │
│  │ ○  ○ │    │ ○  ○ │    │ ○  ○ │    │ ○  ○ │      │
│  │ ○  ● │    │ ○  ● │    │ ○  ○ │    │ ○  ● │      │
│  └──────┘    └──────┘    └──────┘    └──────┘      │
│                                                      │
├─────────────────────────────────────────────────────┤
│ IN [====    ] OUT [======  ]                        │
└─────────────────────────────────────────────────────┘
```

---

## Phase 4: Preset Management

### 4.1 Preset System
**Goal:** Save and load complete pedal board configurations

**Tasks:**
- [ ] Implement preset save/load functionality
  - XML or JSON format for preset files
  - Store: effect chain order, effect types, all parameters, bypass states
  - Factory presets included with plugin
  
- [ ] Create preset browser UI
  - List of available presets (factory + user)
  - Load/save/delete operations
  - Preset categories/tags
  - Import/export presets to files
  
- [ ] Preset manager class
  - Scan preset directories
  - Handle preset file I/O
  - Validate preset compatibility

**File Structure:**
```
src/pedalboard/
  ├── PresetManager.h
  ├── PresetManager.cpp
src/pedalboard/ui/
  ├── PresetBrowser.h
  └── PresetBrowser.cpp
```

**Preset File Format (XML example):**
```xml
<PedalBoardPreset name="Classic Rock" category="Rock">
  <EffectChain>
    <Effect type="Compressor" bypassed="false">
      <Parameter id="threshold" value="-15.0"/>
      <Parameter id="ratio" value="3.0"/>
      <Parameter id="attack" value="5.0"/>
      <Parameter id="release" value="80.0"/>
    </Effect>
    <Effect type="Fuzz" bypassed="false">
      <Parameter id="gain" value="7.0"/>
      <Parameter id="tone" value="0.6"/>
      <Parameter id="level" value="0.8"/>
    </Effect>
    <Effect type="Reverb" bypassed="false">
      <Parameter id="roomSize" value="0.4"/>
      <Parameter id="damping" value="0.5"/>
      <Parameter id="mix" value="0.2"/>
    </Effect>
  </EffectChain>
</PedalBoardPreset>
```

---

## Phase 5: Additional Features

### 5.1 Performance Optimizations
**Tasks:**
- [ ] CPU usage monitoring and optimization
  - Measure DSP load per effect
  - Display CPU usage meter in UI
  - Adaptive buffer size handling
  
- [ ] Parallel processing (if applicable)
  - Investigate parallel processing for independent effects
  - SIMD optimizations for critical DSP paths

---

### 5.2 MIDI Control
**Goal:** External control via MIDI controllers

**Tasks:**
- [ ] MIDI CC mapping system
  - Map MIDI CCs to effect parameters
  - MIDI learn functionality
  - Program change for preset switching
  
- [ ] MIDI mapping UI
  - Visual indication of MIDI-mapped parameters
  - Clear/reset MIDI mappings

**File Structure:**
```
src/pedalboard/
  ├── MIDIController.h
  └── MIDIController.cpp
```

---

### 5.3 Advanced Routing (Future)
**Goal:** Flexible signal routing beyond simple serial chains

**Tasks:**
- [ ] Parallel effect paths
  - Split signal to multiple effects
  - Mix parallel paths
  
- [ ] Send/return loops
  - Pre/post effect sends
  - External effect integration

---

## Phase 6: Build System Integration

### 6.1 CMake Configuration
**Tasks:**
- [ ] Add Pedal Board target to `CMakeLists.txt`
  - New `juce_add_plugin` for OpenGuitar_PedalBoard
  - Link all effect source files
  - Configure VST3 metadata (plugin code, categories)
  
- [ ] Update build scripts
  - Post-build copy to output directory
  - Installation targets

**CMakeLists.txt Addition:**
```cmake
# Pedal Board Plugin
juce_add_plugin(OpenGuitar_PedalBoard
    COMPANY_NAME "OpenGuitar"
    PLUGIN_MANUFACTURER_CODE Ogtr
    PLUGIN_CODE Ogpb
    FORMATS VST3
    PRODUCT_NAME "OpenGuitar Pedal Board"
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES "Fx" "Tools")

target_sources(OpenGuitar_PedalBoard
    PRIVATE
        src/pedalboard/PedalBoardProcessor.cpp
        src/pedalboard/PedalBoardProcessor.h
        src/pedalboard/PedalBoardEditor.cpp
        src/pedalboard/PedalBoardEditor.h
        src/pedalboard/EffectChain.cpp
        src/pedalboard/EffectChain.h
        src/pedalboard/EffectFactory.cpp
        src/pedalboard/EffectFactory.h
        src/pedalboard/PresetManager.cpp
        src/pedalboard/PresetManager.h
        src/pedalboard/MIDIController.cpp
        src/pedalboard/MIDIController.h
        src/pedalboard/ui/PedalComponent.cpp
        src/pedalboard/ui/PedalComponent.h
        src/pedalboard/ui/PedalBoardEditor.cpp
        src/pedalboard/ui/PedalBoardEditor.h
        src/pedalboard/ui/PedalLookAndFeel.cpp
        src/pedalboard/ui/PedalLookAndFeel.h
        src/pedalboard/ui/EffectSelectorComponent.cpp
        src/pedalboard/ui/EffectSelectorComponent.h
        src/pedalboard/ui/PresetBrowser.cpp
        src/pedalboard/ui/PresetBrowser.h
        # Shared effect files
        src/effects/EffectBase.cpp
        src/effects/EffectBase.h
        src/effects/Fuzz.cpp
        src/effects/Fuzz.h
        src/effects/Compressor.cpp
        src/effects/Compressor.h
        src/effects/Reverb.cpp
        src/effects/Reverb.h
        src/dsp/Filter.cpp
        src/dsp/Filter.h)

target_compile_definitions(OpenGuitar_PedalBoard
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=0)

target_link_libraries(OpenGuitar_PedalBoard
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
```

---

## Phase 7: Testing & Documentation

### 7.1 Testing
**Tasks:**
- [ ] Unit tests for EffectChain
  - Add/remove/reorder operations
  - State save/load
  
- [ ] Integration tests
  - Full signal chain processing
  - Preset loading/saving
  - Parameter automation
  
- [ ] DAW compatibility testing
  - Test in Reaper, Ableton Live, FL Studio, Pro Tools
  - Verify automation works correctly
  - Test preset recall between sessions

---

### 7.2 Documentation
**Tasks:**
- [ ] User manual
  - How to add/remove effects
  - Parameter descriptions for each effect
  - Preset management guide
  - MIDI mapping instructions
  
- [ ] Developer documentation
  - Architecture overview
  - How to add new effects
  - API documentation for EffectBase
  
- [ ] Tutorial presets
  - Create factory presets for common tones
  - "Blues", "Rock", "Metal", "Clean", "Ambient" presets

---

## Implementation Timeline

### Sprint 1 (Week 1-2): Foundation
- Implement EffectBase abstract class
- Refactor existing effects (Fuzz, Compressor, Reverb) to inherit from EffectBase
- Create EffectChain and EffectFactory

### Sprint 2 (Week 3-4): Processor
- Implement PedalBoardProcessor
- Dynamic parameter management
- State save/load for effect chains

### Sprint 3 (Week 5-6): UI Foundation
- Create PedalComponent with basic controls
- Implement PedalLookAndFeel for visual styling
- Build basic PedalBoardEditor layout

### Sprint 4 (Week 7-8): UI Polish
- Drag-and-drop reordering
- Effect selector/browser
- Visual refinements and animations

### Sprint 5 (Week 9-10): Presets & MIDI
- Implement PresetManager and file I/O
- Create preset browser UI
- Add MIDI CC mapping functionality

### Sprint 6 (Week 11-12): Testing & Release
- Comprehensive testing in multiple DAWs
- Bug fixes and optimizations
- Documentation
- Factory preset creation
- Release build

---

## Key Technical Considerations

### 1. Parameter Management
**Challenge:** Dynamic parameters based on loaded effects  
**Solution:** Use parameter prefixing system (e.g., "effect1_gain") and rebuild parameter tree when chain changes. Ensure DAW automation is preserved.

### 2. Latency Management
**Challenge:** Chain of effects may introduce latency (especially with oversampling)  
**Solution:** Report total latency via `getTailLengthSeconds()` and ensure proper delay compensation in DAW.

### 3. Thread Safety
**Challenge:** UI thread modifying effect chain while audio thread is processing  
**Solution:** Use lock-free message queues or atomic flags for chain modifications. Apply changes only in `prepareToPlay()` or at block boundaries.

### 4. Memory Management
**Challenge:** Allocating/deallocating effects without audio glitches  
**Solution:** Pre-allocate effects or use lock-free allocation. Perform actual deletion on message thread, not audio thread.

### 5. State Compatibility
**Challenge:** Presets must remain compatible as new effects are added  
**Solution:** Version preset files and implement migration logic for older presets.

---

## Future Expansion Ideas

1. **More Effects**
   - Delay, Chorus, Phaser, Tremolo, Overdrive, Wah
   - Noise Gate, Tuner, Octaver, Ring Modulator
   
2. **Advanced Routing**
   - Parallel effect chains (A/B routing)
   - Send/return loops for external hardware
   - Stereo widening and panning controls
   
3. **Cabinet Simulation**
   - IR (Impulse Response) loader
   - Multiple cabinet models
   - Microphone positioning
   
4. **Modulation Matrix**
   - LFOs to modulate any parameter
   - Envelope followers
   - Step sequencers for rhythmic effects
   
5. **Undo/Redo**
   - History stack for parameter changes
   - Preset A/B comparison
   
6. **Spectral Analysis**
   - Real-time frequency analyzer
   - Visual feedback of tone shaping

---

## Summary

This plan provides a roadmap to create a comprehensive Pedal Board VST3 plugin that:
- ✅ Reuses existing effect implementations (Compressor, Fuzz, Reverb)
- ✅ Allows flexible effect chaining with drag-and-drop reordering
- ✅ Provides intuitive, skeuomorphic UI resembling real pedal boards
- ✅ Supports preset management for instant tone recall
- ✅ Enables MIDI control for live performance
- ✅ Maintains professional audio quality and low latency
- ✅ Follows JUCE best practices and OpenGuitar project standards

**Next Steps:** Begin with Phase 1 (Foundation) by creating the EffectBase class and refactoring existing effects to use the unified interface.
