/*
  ==============================================================================

    PhaseVocoderV2.h
    Created: 26 May 2026 11:30:24pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/


//This will be an improved version of PhaseVocoder, which can be isolated
// as a functional 1 channel PhaseVocoder, and with 2 channel functionality.

#include <stdio.h>
#include <vector>
#include <JuceHeader.h>
#include "Fsig.h"
#include "SpectralProcessors.hpp"


class Window
{
public:
    Window(size_t size);
    void makeWindow();
    float getRawValue(unsigned int index);
private:
    size_t windowSize;
    std::vector<float> window;
};



class PhaseVocoder : juce::AudioProcessorValueTreeState::Listener
{
public:

    PhaseVocoder(juce::AudioProcessorValueTreeState& vts);
    
    void prepare(size_t buffSize, double sampleRate, unsigned int sizeFft);
    void process(juce::AudioBuffer<float>& buffer, int channel);
    
    void pvAnalyze(std::vector<float>& fftInput, fsig& fsig);
    void pvSynthesize(fsig& fsig, std::vector<float>& fftOutput);
    
    void addDataToOverlap(std::vector<float>& dataToWrite);
    float wrapPhase(float phaseIn);
    void stopProcessingForBufferResize()
    {
        buffersReady.store(false);
    }
    
    void parameterChanged(const juce::String& parameterID, float newValue) override;

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    double samplingRate;
    size_t bufferSize;
    size_t fftSize;
    size_t hopSize;
    size_t hopsPerBlock;
    size_t numBins;

    float scaleFactor = 0.5;
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
    
    //Phase Vocoder Phase Buffers
    std::vector<float> lastInputPhase;
    std::vector<float> lastOutputPhase;
    

    
    //Spectral Processing Buffer Objects
    fsig fsigBuff1;
    fsig fsigBuff2;
    
    //Parameters
    std::atomic<float> pitchShiftParam{1.0};
    std::atomic<float> blurParam{0.0};
    std::atomic<float> stretchTimeParam{0.0};
    std::atomic<float> stretchDensityParam{0.0};
    std::atomic<float> feedbackParam{0.0};
    std::atomic<float> delayAmtParam{0.0};
    std::atomic<float> delayTimeParam{0.0};
    std::atomic<bool> delayFreqToggleParam{false};
    std::atomic<float> gateAmtParam{0.0};
    
    std::atomic<bool> buffersReady{false};
    
    //Spectral Processors
    PitchShift pShiftObj;
    SpectralBlur blurObj;
    SpectralDelay delayObj;
    SpectralStretch stretchObj;
    SpectralGate gateObj;
};



#pragma once
