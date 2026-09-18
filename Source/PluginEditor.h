#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class PsyZumboAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit PsyZumboAudioProcessorEditor(PsyZumboAudioProcessor&);
    ~PsyZumboAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    PsyZumboAudioProcessor& processor;

    struct Knob : juce::Slider
    {
        Knob() {
            setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
        }
    };

    std::array<std::unique_ptr<Knob>, 10> knobs;
    std::array<std::unique_ptr<juce::Label>, 10> labels;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 10> attachments;
    const char* ids[10] = {
        "level","cutoff","resonance","attack","decay",
        "sustain","release","morph","grainLength","grainMix"
    };
    const char* names[10] = {
        "MASTER","CUTOFF","RESONANCE","ATTACK","DECAY",
        "SUSTAIN","RELEASE","MORPH","GRAIN LEN","GRANULAR"
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PsyZumboAudioProcessorEditor)
};
