/*
  ==============================================================================

    SynthVoice.h
    Created: 11 Jul 2020 12:46:37am
    Author:  kpai1

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace juce;

class SynthVoice : public SynthesiserVoice
{
public:
    static void addADSRParameters(AudioProcessorValueTreeState::ParameterLayout& layout)
    {
        auto attack = std::make_unique<AudioParameterFloat>("attack", "Attack", NormalisableRange<float>(0.001f, 5000.0f, 0.01f), 0.10f);
        auto decay = std::make_unique<AudioParameterFloat>("decay", "Decay", NormalisableRange<float>(0.001f, 5000.0f, 0.01f), 0.10f);
        auto sustain = std::make_unique<AudioParameterFloat>("sustain", "Sustain", NormalisableRange<float>(0.0f, 5000.0f, 0.01f), 1.0f);
        auto release = std::make_unique<AudioParameterFloat>("release", "Release", NormalisableRange<float>(0.001f, 5000.0f, 0.01f), 0.10f);

        auto group = std::make_unique<AudioProcessorParameterGroup>("adsr", "ADSR", "|",
                                                                    std::move(attack),
                                                                    std::move(decay),
                                                                    std::move(sustain),
                                                                    std::move(release));
        layout.add(std::move(group));
    }

    //==========================================================================

    void setADSR(float* attack, float* decay, float* sustain, float* release)
    {
        env1.setAttack(double(*attack));
        env1.setDecay(double(*decay));
        env1.setSustain(double(*sustain));
        env1.setRelease(double(*release));
    }

    //==========================================================================

    static void addWaveTypeParameters(AudioProcessorValueTreeState::ParameterLayout& layout)
    {
        auto wavetype = std::make_unique<AudioParameterFloat>("waveType", "WaveType", NormalisableRange<float>(0.0f, 2.0f, 1), 0.0f);

        layout.add(std::move(wavetype));
    }

    //==========================================================================

    void getWaveType(float* waveType)
    {
        theWave = *waveType;
    }

    //==========================================================================

    double setWaveType()
    {
        switch (theWave)
        {
        case 0: 
            return osc1.sinewave(frequency);
        case 1:
            return osc1.saw(frequency);
        case 2:
            return osc1.square(frequency);
        default:
            return osc1.sinewave(frequency);
        }
    }

    //==========================================================================

    static void addGainParameters(AudioProcessorValueTreeState::ParameterLayout& layout)
    {
        auto gain = std::make_unique<AudioParameterFloat>("gain", "Gain", NormalisableRange<float>(-40.0f, 0.0f, 0.01f), -6.0f);

        layout.add(std::move(gain));
    }

    //==========================================================================

    void getGainValue(float* gainValue)
    {
        gain = *gainValue;
    }

    //==========================================================================

    bool canPlaySound(SynthesiserSound* sound)
    {
        return dynamic_cast<SynthSound*>(sound) != nullptr;
    }

    //==========================================================================

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition)
    {
        env1.trigger = 1;
        level = velocity; 
        frequency = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    }

    void stopNote(float velocity, bool allowTailOff)
    {
        env1.trigger = 0;
        allowTailOff = true;
        
        if (velocity == 0)
        {
            clearCurrentNote();
        }
    }

    //==========================================================================

    void pitchWheelMoved(int newPitchWheelValue)
    {


    }

    //==========================================================================

    void controllerMoved(int controllerNumber, int newControllerValue)
    {

    }

    //==========================================================================

    void renderNextBlock(AudioBuffer< float >& outputBuffer, int startSample, int numSamples)
    {
        for (int sample = 0; sample < numSamples; ++sample)
        {
            double theSound = (env1.adsr(setWaveType(), env1.trigger) * level) * juce::Decibels::decibelsToGain(gain);

            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            {
                outputBuffer.addSample(channel, startSample, theSound);
            }
            ++startSample;
        }
    }

private:
    double level;
    double frequency;
    double gain;

    maxiOsc osc1; 
    maxiEnv env1;
    maxiFilter filter1;
    int theWave;
};


