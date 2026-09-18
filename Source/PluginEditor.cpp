#include "PluginEditor.h"

PsyZumboAudioProcessorEditor::PsyZumboAudioProcessorEditor(PsyZumboAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(920, 560);

    for (size_t i = 0; i < knobs.size(); ++i)
    {
        knobs[i] = std::make_unique<Knob>();
        labels[i] = std::make_unique<juce::Label>();
        labels[i]->setText(names[i], juce::dontSendNotification);
        labels[i]->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*knobs[i]);
        addAndMakeVisible(*labels[i]);
        attachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.parameters(), ids[i], *knobs[i]);
    }
}

void PsyZumboAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(8, 7, 13));

    auto r = getLocalBounds().toFloat();
    juce::ColourGradient grad(
        juce::Colour::fromRGB(35, 8, 45), r.getCentreX(), 0.0f,
        juce::Colour::fromRGB(4, 24, 20), r.getCentreX(), r.getBottom(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(r.reduced(8.0f), 18.0f);

    g.setColour(juce::Colour::fromRGB(221, 94, 255));
    g.setFont(juce::Font(34.0f, juce::Font::bold));
    g.drawText("PSY - ZUMBO", 28, 22, 430, 46, juce::Justification::left);

    g.setColour(juce::Colour::fromRGB(115, 255, 210));
    g.setFont(juce::Font(13.0f));
    g.drawText("DARK FOREST / PSY SYNTH — FOUNDATION BUILD", 31, 65, 520, 25, juce::Justification::left);

    g.setColour(juce::Colour::fromRGB(255, 110, 230));
    g.drawRoundedRectangle(24, 105, 872, 1, 1);

    g.setColour(juce::Colours::white.withAlpha(0.55f));
    g.setFont(12.0f);
    g.drawText("4-voice oscillation core • granular texture • filter • envelopes • LFO • preset state", 30, 500, 850, 24, juce::Justification::centred);
}

void PsyZumboAudioProcessorEditor::resized()
{
    const int margin = 34;
    const int top = 135;
    const int w = 150;
    const int h = 125;

    for (size_t i = 0; i < knobs.size(); ++i)
    {
        const int col = (int)i % 5;
        const int row = (int)i / 5;
        auto area = juce::Rectangle<int>(margin + col * 175, top + row * 170, w, h);
        labels[i]->setBounds(area.removeFromTop(22));
        knobs[i]->setBounds(area.reduced(5));
    }
}
