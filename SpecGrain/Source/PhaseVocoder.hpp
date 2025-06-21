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
    Window()
    {
        window.resize(windowSize);
        makeWindow();
    }
    
    void makeWindow()
    {
        for (int i = 0; i < windowSize; ++i)
            window[i] = 0.5 * (1 - std::cos((juce::MathConstants<double>::twoPi * i)/windowSize));
    }
    
    float interpVal(float index)
    {
        index = fmod(index, 1.0);
        if(index < 0) index += 1;
        
        float fi = index * windowSize;
        
        int i0 = fi;
        int i1 = fi + 1 >= windowSize ? 0 : fi + 1;
        
        float floatDiff = fi - (float) i0;
        
        float low = window[i0];
        float high = window[i1];
        
        float diff = high - low;
        
        return low + (diff * floatDiff);
    }
    
private:
    
    size_t windowSize = 1024;
    std::vector<float> window;
};



class PhaseVocoder
{
public:
    PhaseVocoder()
    {
        
        
        
        
    }
    
    void prepare(size_t buffSize, double sampleRate)
    {
        bufferSize = buffSize;
        hopsPerBlock = bufferSize / hopSize;
        
        auto fftOrder = std::log2(fftSize);
        fftObject = std::make_unique<juce::dsp::FFT>(fftOrder);
        
        inputBuffer.resize(fftSize);
        outputBuffer.resize(fftSize);
        fftBuffer.resize(fftSize);
        magPhasePairs.resize(fftSize);
        windowedBuffer.resize(fftSize);
        
        overlapBuffer.resize(fftSize * 2);
        bufferFull = false;
        
        DBG("Prepared Successfully");
        DBG("Buffer Size = " + juce::String(bufferSize));
        DBG("FFT Size = " + juce::String(fftSize));
        DBG("Hop Size = " + juce::String(hopSize));
        DBG("Hops Per Block = " + juce::String(hopsPerBlock));
        
    }
    
    void process(juce::AudioBuffer<float>& buffer)
    {
        
        //1. Read in input, copy to buffer
        //2. once buffer is full, perform windowing and FFT
        //3. Store fft somehwhere
        //4. Move forward by hopsize
        //repeat
        

        
        //Perform stft operation for as many hop sizes
        //within 1 buffersize
        for(auto hop = 0; hop < hopsPerBlock; ++hop)
        {
            
            //Copy hopSize buffer samples to larger buffer for FFT processing
            juce::FloatVectorOperations::copy(inputBuffer.data() + inputBufferHead, buffer.getReadPointer(0) + hop * hopSize, hopSize);
            
            inputBufferHead += hopSize;
            inputBufferHead = inputBufferHead % inputBuffer.size();
            
            sampsAccumulated += hopSize;
            if(sampsAccumulated >= inputBuffer.size())
                bufferFull = true;
            
            //Once buffer is full, begin actual processing loop
            if(!bufferFull)
            {
                DBG("Buffer Not Full");
                buffer.clear();
                return;
            }
            
            //Apply window
            for(int i = 0; i < inputBuffer.size(); ++i)
            {
                windowedBuffer[i] = window.interpVal(static_cast<float>(i)/fftSize) * inputBuffer[i];
            }
            
            
            fftObject->performRealOnlyForwardTransform(windowedBuffer.data(), true);
            
            getMagnitudePhase(windowedBuffer, magPhasePairs);
            
            //Process in between here
            
            getRealImag(magPhasePairs, outputBuffer);
            
            fftObject->performRealOnlyInverseTransform(outputBuffer.data());
            
            //Apply window again
            for(int i = 0; i < inputBuffer.size(); ++i)
            {
                windowedBuffer[i] = window.interpVal(static_cast<float>(i)/outputBuffer.size()) * outputBuffer[i];
            }
            
            
            //Write output to overlap buffer
            for(auto i = 0; i < fftSize; ++i)
            {
                auto index = i + overlapWritePos;
                index = index % static_cast<int>(fftSize);
                
                overlapBuffer[index] += windowedBuffer[i];
            }
            overlapWritePos += hopSize;
            overlapWritePos %= overlapBuffer.size();
        }
        
        //read next data and clear read portion
        auto* bufPointer = buffer.getWritePointer(0);
        
        for(auto i = 0; i < buffer.getNumSamples(); ++i)
        {
            bufPointer[i] = overlapBuffer[i + overlapReadPos];
            overlapBuffer[i + overlapReadPos] = 0;
        }
        
        overlapReadPos += bufferSize;
        overlapReadPos %= overlapBuffer.size();
    }
    
    
    void getMagnitudePhase(std::vector<float>& complexPairs, std::vector<float>& magPhase)
    {
        const static size_t numBins = complexPairs.size()/2;
        //magPhase.resize(complexPairs.size());
        
        for(size_t i = 0; i < numBins; ++i)
        {
            auto real = complexPairs[i * 2];
            auto imag = complexPairs[i * 2 +1];
            
            magPhase[i * 2] = std::sqrt(real * real + imag * imag);
            magPhase[i * 2 + 1] = std::atan2(imag, real);
        }
    }
    
    void getRealImag(std::vector<float>& magPhase, std::vector<float>& complexPairs)
    {
        const static size_t numBins = complexPairs.size()/2;
        //magPhase.resize(complexPairs.size());
        
        for(size_t i = 0; i < numBins; ++i)
        {
            auto mag = complexPairs[i * 2];
            auto phase = complexPairs[i * 2 +1];
            
            magPhase[i * 2] = mag * std::cos(phase);
            magPhase[i * 2 + 1] = mag * std::sin(phase);
        }
    }
    
    
    
    
private:
    
    size_t bufferSize;
    size_t fftSize = 1024;
    size_t hopSize = fftSize / 4;
    size_t hopsPerBlock;
    int sampsAccumulated = 0;
    
    std::unique_ptr<juce::dsp::FFT> fftObject;
    
    unsigned int inputBufferHead;
    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;
    std::vector<float> fftBuffer;
    std::vector<float> magPhasePairs;
    std::vector<float> windowedBuffer;
    
    
    unsigned int overlapReadPos = 0;
    unsigned int overlapWritePos = 0;
    std::vector<float> overlapBuffer;
    
    
    bool bufferFull = false;
    
    Window window;
    
};



#endif /* PhaseVocoder_hpp */
