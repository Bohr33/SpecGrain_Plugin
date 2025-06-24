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
    
    struct fsig{
        std::vector<float> amplitudes;
        std::vector<float> frequencies;
        
        void resize(size_t size)
        {
            amplitudes.resize(size);
            frequencies.resize(size);
        }
        
        void clear()
        {
            juce::FloatVectorOperations::clear(amplitudes.data(), amplitudes.size());
            juce::FloatVectorOperations::clear(frequencies.data(), frequencies.size());
        }
    };
    
    
    PhaseVocoder(juce::AudioProcessorValueTreeState& vts);
    
    void prepare(size_t buffSize, double sampleRate);
    
    void process(juce::AudioBuffer<float>& buffer);
    
    void addDataToOverlap(std::vector<float>& dataToWrite);
    void getMagnitudePhase(std::vector<float>& complexPairs, std::vector<float>& magPhase);
    void getRealImag(std::vector<float>& magPhase, std::vector<float>& complexPairs);
    
    void pitchShift(float shiftAmt, fsig& fsigIn, fsig& fsigOut);

    void pvAnalyze(std::vector<float>& fftInput, fsig& fsig);

    void pvSynthesize(fsig& fsig, std::vector<float>& fftOutput);
    float wrapPhase(float phaseIn);
    
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    
    
private:
    
    size_t bufferSize;
    size_t fftSize = 1024;
    size_t hopSize = fftSize / 8;
    size_t hopsPerBlock;
    size_t numBins = fftSize / 2 + 1;
    float scaleFactor = 0.5;
    int sampsAccumulated = 0;
    
    
    std::atomic<float> pitchShiftAmt{1.0};
    
    double samplingRate;
    
    std::unique_ptr<juce::dsp::FFT> fftObject;
    std::unique_ptr<juce::dsp::FFT> ifftObject;
    
    unsigned int inputBufferHead;
    std::vector<float> inputBuffer;
    std::vector<float> fftBuffer;
    
    std::vector<float> lastInputPhase;
    std::vector<float> lastOutputPhase;
    
    //Spectral Processing Buffer Objects
    fsig fsigIn;
    fsig fsigOut;
    
    unsigned int overlapReadPos = 0;
    unsigned int overlapWritePos = 0;
    std::vector<float> overlapBuffer;
    
    
    bool bufferFull = false;
    
    Window window;
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    
};



#endif /* PhaseVocoder_hpp */
