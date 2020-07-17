/*
  ==============================================================================

    SynthSound.h
    Created: 11 Jul 2020 12:47:13am
    Author:  kpai1

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

using namespace juce;

class SynthSound : public SynthesiserSound
{
public:
    bool appliesToNote(int /*midiNoteNumber*/)
    {
        return true;
    }

    //==========================================================================

    bool appliesToChannel(int /*midiChannel*/)
    {
        return true;
    }
};

