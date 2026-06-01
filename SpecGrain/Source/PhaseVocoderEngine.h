/*
  ==============================================================================

    StereoPhaseVocoder.h
    Created: 30 May 2026 10:27:52pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#pragma once


#include "PhaseVocoder.h"

class PhaseVocoderEngine
{
public:
    
    void prepare(size_t bufferSize, double sampleRate, int fftSize, int numChannels);
    
    void pushSamples(int channel, std::span<const float> buffer);
    void pullSamples(int channel, std::span<float> buffer);
    
    std::vector<fsig>& getFsigBuffer(int channel);
    
    int getNumChannels();
    

    
private:
    std::vector<PhaseVocoder> phaseVocoders;
    
};

