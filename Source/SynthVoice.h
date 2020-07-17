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
            double theWave = osc1.sinewave(frequency);
            double theSound = env1.adsr(theWave, env1.trigger) * level;
            double filteredSound = filter1.lores(theSound, 150, 0.1);

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

    maxiOsc osc1; 
    maxiEnv env1;
    maxiFilter filter1;
};


