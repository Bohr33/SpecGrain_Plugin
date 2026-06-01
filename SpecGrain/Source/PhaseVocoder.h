/*
  ==============================================================================

    PhaseVocoder.h
    Created: 26 May 2026 11:30:24pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/


//This will be an improved version of PhaseVocoder, which can be isolated
// as a functional 1 channel PhaseVocoder, and with 2 channel functionality.

#include <stdio.h>
#include <vector>
#include <span>
#include "Fsig.h"
#include "Window.h"
#include <JuceHeader.h>


class PhaseVocoder
{
public:

    PhaseVocoder();
    
    void prepare(size_t buffSize, double sampleRate, unsigned int sizeFft);
    
    void pvAnalyze(std::vector<float>& fftInput, fsig& fsig);
    void pvSynthesize(fsig& fsig, std::vector<float>& fftOutput);
    
    //New Functions
    void pushSamples(std::span<const float> buffer);
    void pullSamples(std::span<float> outputBuffer);
    
    
    std::vector<fsig>& getFsigBuffer();

private:
    
    void addDataToOverlap(std::vector<float>& dataToWrite);
    float wrapPhase(float phaseIn);
    

    double samplingRate;
    size_t bufferSize;
    size_t fftSize;
    size_t hopSize;
    size_t hopsPerBlock;
    size_t numBins;
    int overlapAmount = 8;

    float gainCompensation = 0.0;
    int sampsAccumulated = 0;
    
    bool bufferFull = false;
    std::unique_ptr<Window> window;
    
    std::unique_ptr<juce::dsp::FFT> fftObject;
    
    //STFT Process Buffers
    unsigned int inputBufferHead;
    std::vector<float> inputBuffer;
    std::vector<float> fftBuffer;
    
    unsigned int overlapReadPos = 0;
    unsigned int overlapWritePos = 0;
    std::vector<float> overlapBuffer;
    bool overlapReady = false;
    
    //Phase Vocoder Phase Buffers
    std::vector<float> lastInputPhase;
    std::vector<float> lastOutputPhase;
    
    //Spectral Processing Buffer Objects
    fsig fsigBuff1;
    
    //Queue to hold extra frames when hopSize > 1
    //holds all the Fsigs for each hop for each buffer
    std::vector<fsig> frameBuffer;
    int bufferCounter = 0;

};



#pragma once
