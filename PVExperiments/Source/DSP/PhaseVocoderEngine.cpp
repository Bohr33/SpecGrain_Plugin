/*
  ==============================================================================

    StereoPhaseVocoder.cpp
    Created: 30 May 2026 10:27:52pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "PhaseVocoderEngine.h"

void PhaseVocoderEngine::prepare(size_t newBufferSize, size_t newFftSize, int numChannels)
{
    for(int channel = 0; channel < numChannels; channel++)
    {
        phaseVocoders.resize(numChannels);
    }
    
    for(auto& pv : phaseVocoders)
    {
        pv.prepare(newBufferSize, newFftSize);
    }
}


void PhaseVocoderEngine::pushSamples(int channel, std::span<const float> buffer)
{
    phaseVocoders[channel].pushSamples(buffer);
}

void PhaseVocoderEngine::pullSamples(int channel, std::span<float> buffer)
{
    phaseVocoders[channel].pullSamples(buffer);
}


std::vector<fsig>& PhaseVocoderEngine::getFsigBuffer(int channel)
{
    return phaseVocoders[channel].getFsigBuffer();
}

int PhaseVocoderEngine::getNumChannels()
{
    return (int)phaseVocoders.size();
}
