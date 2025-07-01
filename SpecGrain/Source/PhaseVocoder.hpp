//
//  PhaseVocoder.hpp
//  SpecGrain
//
//  Created by Benjamin Ward (Old Computer) on 6/18/25.
//

#ifndef PhaseVocoder_hpp
#define PhaseVocoder_hpp

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
    
    void parameterChanged(const juce::String& parameterID, float newValue) override;

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    double samplingRate;
    size_t bufferSize;
    size_t fftSize = 1024;
    size_t hopSize = fftSize / 8;
    size_t hopsPerBlock;
    size_t numBins = fftSize / 2 + 1;

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
    
    //Spectral Processors
    PitchShift pShiftObj;
    SpectralBlur blurObj;
    SpectralDelay delayObj;
    SpectralStretch stretchObj2;
    SpectralGate gateObj;
};



#endif /* PhaseVocoder_hpp */
