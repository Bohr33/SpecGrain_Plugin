/*
  ==============================================================================

    PhaseVocoder.h
    Created: 26 May 2026 11:30:24pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

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
    
    void prepare(size_t newBlockSize, size_t newFftSize);
    
    void pushSamples(std::span<const float> buffer);
    void pullSamples(std::span<float> outputBuffer);
    
    std::vector<fsig>& getFsigBuffer();

private:
    
    void resizeAllVectors();
    void clearAllVectors();
    
    
    float calculateGainCompensation();
    
    void pvAnalyze(std::vector<float>& fftInput, fsig& fsig);
    void pvSynthesize(fsig& fsig, std::vector<float>& fftOutput);
    
    void addDataToOverlap(std::vector<float>& dataToWrite);
    float wrapPhase(float phaseIn);
    

    size_t blockSize;
    size_t fftSize;
    int hopSize;
    size_t hopsPerBlock;
    size_t numBins;
    
    int overlapAmount = 8;

    float gainCompensation = 0.0;
    int sampsAccumulated = 0;
    
    std::unique_ptr<Window> window;
    std::unique_ptr<juce::dsp::FFT> fftObject;
    
    //STFT Process Buffers
    unsigned int inputBufferHead;
    std::vector<float> inputBuffer;
    std::vector<float> fftBuffer;
    
    //Overlap Buffer
    unsigned int overlapReadPos = 0;
    unsigned int overlapWritePos = 0;
    std::vector<float> overlapBuffer;
    bool overlapReady = false;
    
    //PV Phase Buffers
    std::vector<float> lastInputPhase;
    std::vector<float> lastOutputPhase;
    
    //fsig buffer for hop loop
    fsig fsigBuff1;
    
    //Queue to hold extra frames between push() and pull()
    //holds all the Fsigs for each hop for each buffer
    std::vector<fsig> frameBuffer;
    int bufferCounter = 0;
};



#pragma once
