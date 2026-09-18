#include "PluginProcessor.h"
#include "PluginEditor.h"

PsyZumboAudioProcessor::PsyZumboAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParams())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout PsyZumboAudioProcessor::createParams()
{
    using P = juce::AudioParameterFloat;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    p.push_back(std::make_unique<P>("level", "Master", juce::NormalisableRange<float>(0, 0.8f), 0.25f));
    p.push_back(std::make_unique<P>("cutoff", "Filter Cutoff", juce::NormalisableRange<float>(40, 18000, 0.01f, 0.35f), 1800.0f));
    p.push_back(std::make_unique<P>("resonance", "Resonance", juce::NormalisableRange<float>(0.1f, 0.95f), 0.35f));
    p.push_back(std::make_unique<P>("attack", "Attack", juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.35f), 0.01f));
    p.push_back(std::make_unique<P>("decay", "Decay", juce::NormalisableRange<float>(0.01f, 3.0f, 0.001f, 0.35f), 0.35f));
    p.push_back(std::make_unique<P>("sustain", "Sustain", juce::NormalisableRange<float>(0.0f, 1.0f), 0.65f));
    p.push_back(std::make_unique<P>("release", "Release", juce::NormalisableRange<float>(0.01f, 4.0f, 0.001f, 0.35f), 0.45f));
    p.push_back(std::make_unique<P>("morph", "Wavetable Morph", juce::NormalisableRange<float>(0, 1), 0.35f));
    p.push_back(std::make_unique<P>("detune", "Unison Detune", juce::NormalisableRange<float>(0, 25), 7.0f));
    p.push_back(std::make_unique<P>("grainLength", "Grain Length", juce::NormalisableRange<float>(5, 120), 30.0f));
    p.push_back(std::make_unique<P>("grainDensity", "Grain Density", juce::NormalisableRange<float>(1, 100), 50.0f));
    p.push_back(std::make_unique<P>("grainScan", "Grain Scan", juce::NormalisableRange<float>(0, 1), 0.5f));
    p.push_back(std::make_unique<P>("grainMix", "Granular Mix", juce::NormalisableRange<float>(0, 1), 0.15f));
    p.push_back(std::make_unique<P>("lfoRate", "LFO Rate", juce::NormalisableRange<float>(0.05f, 20.0f, 0.001f, 0.4f), 4.0f));
    p.push_back(std::make_unique<P>("lfoAmount", "LFO Amount", juce::NormalisableRange<float>(0, 0.5f), 0.08f));
    return { p.begin(), p.end() };
}

void PsyZumboAudioProcessor::prepareToPlay(double sr, int block)
{
    sampleRate = sr;
    for (auto& v : voices)
        v.prepare(sr, block);
}

bool PsyZumboAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void PsyZumboAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    const auto f = [&](const char* id) { return apvts.getRawParameterValue(id)->load(); };
    const float level = f("level");
    const float cutoff = f("cutoff");
    const float res = f("resonance");
    const float attack = f("attack");
    const float decay = f("decay");
    const float sustain = f("sustain");
    const float release = f("release");
    const float morph = f("morph");
    const float detune = f("detune");
    const float gl = f("grainLength");
    const float gd = f("grainDensity");
    const float gs = f("grainScan");
    const float gm = f("grainMix");
    const float lr = f("lfoRate");
    const float la = f("lfoAmount");

    for (const auto metadata : midi)
    {
        const auto m = metadata.getMessage();
        if (m.isNoteOn())
        {
            auto it = std::find_if(voices.begin(), voices.end(), [](const auto& v){ return !v.isActive(); });
            if (it == voices.end()) it = voices.begin();
            it->setParams(level, cutoff, res, attack, decay, sustain, release, detune, morph, gl, gd, gs);
            it->startNote(m.getNoteNumber(), m.getFloatVelocity());
        }
        else if (m.isNoteOff())
        {
            for (auto& v : voices)
                if (v.isActive()) v.stopNote();
        }
    }

    auto* L = buffer.getWritePointer(0);
    auto* R = buffer.getWritePointer(1);
    for (auto& v : voices)
        v.render(L, R, buffer.getNumSamples(), lr, la);

    buffer.applyGain(0.7f);
}

void PsyZumboAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, dest);
}

void PsyZumboAudioProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PsyZumboAudioProcessor();
}
