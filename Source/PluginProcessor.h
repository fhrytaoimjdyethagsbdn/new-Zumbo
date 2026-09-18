#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>

class PsyVoice
{
public:
    void prepare(double sr, int maxBlock)
    {
        sampleRate = sr;
        osc1.setSampleRate(sr); osc2.setSampleRate(sr);
        osc3.setSampleRate(sr); osc4.setSampleRate(sr);
        filter.prepare({ sr, (juce::uint32)maxBlock, 2 });
        filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        adsr.setSampleRate(sr);
        granular.prepare(sr, maxBlock);
    }

    void startNote(int midiNote, float vel)
    {
        active = true;
        note = midiNote;
        velocity = vel;
        phase1 = phase2 = phase3 = phase4 = 0.0f;
        granular.reset();
        adsr.noteOn();
    }

    void stopNote() { adsr.noteOff(); }

    bool isActive() const { return active; }

    void setParams(float level, float cutoff, float resonance,
                   float attack, float decay, float sustain, float release,
                   float detune, float morph, float grainLength,
                   float grainDensity, float grainScan)
    {
        outLevel = level;
        filter.setCutoffFrequency(cutoff);
        filter.setResonance(resonance);
        adsrParams.attack = attack;
        adsrParams.decay = decay;
        adsrParams.sustain = sustain;
        adsrParams.release = release;
        adsr.setParameters(adsrParams);
        this->detune = detune;
        this->morph = morph;
        granular.setControls(grainLength, grainDensity, grainScan);
    }

    void render(float* left, float* right, int n, float lfoRate, float lfoAmount)
    {
        if (!active) return;

        const float baseHz = 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
        const float ratios[4] = { 1.0f, 1.0f + detune * 0.01f, 0.5f, 2.0f };
        const float levels[4] = { 0.62f, 0.34f, 0.22f, 0.14f };
        const float sr = (float) sampleRate;

        for (int i = 0; i < n; ++i)
        {
            lfoPhase += lfoRate / sr;
            if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
            const float lfo = std::sin(juce::MathConstants<float>::twoPi * lfoPhase) * lfoAmount;

            const float x1 = osc(phase1, baseHz * ratios[0], morph, 0);
            const float x2 = osc(phase2, baseHz * ratios[1], morph, 1);
            const float x3 = osc(phase3, baseHz * ratios[2], morph, 2);
            const float x4 = osc(phase4, baseHz * ratios[3], morph, 3);

            float g = 0.0f;
            // Lightweight internal granular texture. It does not load files; it creates
            // deterministic micro-grains from the oscillator signal.
            if (grainMix > 0.001f)
                g = granular.process(x1 + 0.5f * x3);

            float s = (x1 * levels[0] + x2 * levels[1] + x3 * levels[2] + x4 * levels[3]);
            s = s * (1.0f - grainMix) + g * grainMix;
            s *= (1.0f + lfo);

            const float env = adsr.getNextSample();
            s *= env * velocity * outLevel;

            float channels[2] = { s, s };
            juce::dsp::AudioBlock<float> b(channels, 2, 1);
            // The filter is processed through a tiny one-sample block below.
            juce::dsp::ProcessContextReplacing<float> ctx(b);
            filter.process(ctx);

            left[i] += channels[0];
            right[i] += channels[1];

            if (!adsr.isActive())
            {
                active = false;
                break;
            }
        }
    }

private:
    struct Granular
    {
        void prepare(double sr, int maxBlock) { sampleRate = sr; buffer.assign((size_t) std::max(4096, maxBlock * 8), 0.0f); reset(); }
        void reset() { write = 0; phase = 0.0f; }
        void setControls(float l, float d, float s) { length = l; density = d; scan = s; }
        float process(float input)
        {
            buffer[write++ % buffer.size()] = input;
            const int delay = juce::jlimit(1, (int)buffer.size() - 1, (int)(length * 0.01f * sampleRate));
            phase += std::max(0.05f, density) * 0.0004f;
            if (phase > 1.0f) phase -= 1.0f;
            const size_t p = (write + buffer.size() - (size_t)delay) % buffer.size();
            const float window = 0.5f - 0.5f * std::cos(juce::MathConstants<float>::twoPi * phase);
            return buffer[p] * window * (0.35f + 0.65f * scan);
        }
        double sampleRate = 44100.0;
        std::vector<float> buffer;
        size_t write = 0;
        float phase = 0.0f, length = 30.0f, density = 50.0f, scan = 0.5f;
    };

    float osc(float& phase, float hz, float morph, int shape)
    {
        phase += hz / (float)sampleRate;
        phase -= std::floor(phase);
        const float p = phase;
        const float sine = std::sin(juce::MathConstants<float>::twoPi * p);
        const float saw = 2.0f * p - 1.0f;
        const float square = p < 0.5f ? 1.0f : -1.0f;
        const float tri = 1.0f - 4.0f * std::abs(p - 0.5f);
        float a = sine, b = saw;
        if (shape == 1) { a = saw; b = tri; }
        if (shape == 2) { a = tri; b = square; }
        if (shape == 3) { a = square; b = sine; }
        return a * (1.0f - morph) + b * morph;
    }

    double sampleRate = 44100.0;
    bool active = false;
    int note = 60;
    float velocity = 0.0f;
    float outLevel = 0.2f, detune = 7.0f, morph = 0.35f, grainMix = 0.15f;
    float phase1 = 0, phase2 = 0, phase3 = 0, phase4 = 0, lfoPhase = 0;
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    juce::dsp::StateVariableTPTFilter<float> filter;
    Granular granular;
};

class PsyZumboAudioProcessor : public juce::AudioProcessor
{
public:
    PsyZumboAudioProcessor();
    ~PsyZumboAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "PSY - ZUMBO"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.25; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Dark Forest Init"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState& parameters() { return apvts; }

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParams();
    std::array<PsyVoice, 16> voices;
    double sampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PsyZumboAudioProcessor)
};
