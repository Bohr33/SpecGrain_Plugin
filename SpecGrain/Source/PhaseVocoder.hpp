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
        complexProcessBuffer.resize(fftSize);
        
        prevPhase.resize(fftSize/2);
        fsig.resize(fftSize);
        
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
            if(sampsAccumulated >= fftSize)
                bufferFull = true;
            
            //Once buffer is full, begin actual processing loop
            if(!bufferFull)
            {
                DBG("Buffer Not Full");
                buffer.clear();
                return;
            }
            
            auto index = (inputBufferHead + fftSize - hopSize) % fftSize;
            //Apply window
            
            for(int i = 0; i < inputBuffer.size(); ++i)
            {
                auto readIndex = (index + i) % fftSize;
                windowedBuffer[i] = window.interpVal(static_cast<float>(i)/fftSize) * inputBuffer[readIndex];
            }
            
            
            
            fftObject->performRealOnlyForwardTransform(windowedBuffer.data(), true);
            
            
//            getMagnitudePhase(windowedBuffer, magPhasePairs);
            getFsig(windowedBuffer, fsig);
            
            
            //Process in between here
//            pitchShift(1.3333, fsig, complexProcessBuffer);
            
            resynthesizeFsig(fsig, outputBuffer);
//            getRealImag(complexProcessBuffer, outputBuffer);
            
//            juce::FloatVectorOperations::copy(outputBuffer.data(), windowedBuffer.data(), fftSize);
            
            
            fftObject->performRealOnlyInverseTransform(outputBuffer.data());
            
            juce::FloatVectorOperations::clear(windowedBuffer.data(), fftSize);
            
            //Apply window again
            for(int i = 0; i < fftSize; ++i)
            {
                windowedBuffer[i] = window.interpVal(static_cast<float>(i)/fftSize) * outputBuffer[i];
            }
            

            
            
            //Write output to overlap buffer
            for(auto i = 0; i < fftSize; ++i)
            {
                auto index = (i + overlapWritePos) % overlapBuffer.size();
                overlapBuffer[index] += windowedBuffer[i];
            }
            overlapWritePos += hopSize;
            overlapWritePos %= overlapBuffer.size();
        }
        
        //read next data and clear read portion
    
        for(auto i = 0; i < buffer.getNumSamples(); ++i)
        {
            auto index = (i + overlapReadPos) % overlapBuffer.size();
            buffer.addSample(0, i, overlapBuffer[index]);
            buffer.addSample(1, i, overlapBuffer[index]);
            overlapBuffer[index] = 0;
        }
        
        overlapReadPos += bufferSize;
        overlapReadPos %= overlapBuffer.size();
        
        test++;
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
        const static size_t numBins = fftSize/2;
        //magPhase.resize(complexPairs.size());
        
        for(size_t i = 0; i < numBins; ++i)
        {
            auto mag = magPhase[i * 2];
            auto phase = magPhase[i * 2 +1];
            
            complexPairs[i * 2] = mag * std::cos(phase);
            complexPairs[i * 2 + 1] = mag * std::sin(phase);
        }
    }
    
    void pitchShift(double shiftAmt, std::vector<float>& magPhaseData, std::vector<float>& processedMagPhase)
    {
        
        auto numBins = (fftSize) / 2;
        
        juce::FloatVectorOperations::clear(processedMagPhase.data(), fftSize);
        processedMagPhase[0] = magPhaseData[0];
        processedMagPhase[1] = magPhaseData[1];
        
        //Ignore DC and Nyquist bins for now
        for(int i = 1; i < numBins; ++i)
        {
            auto newBin = juce::roundToInt(shiftAmt * i);
            if(newBin >= numBins)
                break;
            auto magIndex = newBin * 2;
            auto phaseIndex = magIndex + 1;
            //Shift Magnitude Values
            processedMagPhase[magIndex] = magPhaseData[i * 2];

            //Shift Phase Values
            processedMagPhase[phaseIndex] = shiftAmt * magPhaseData[i * 2 + 1];
        }
    }
    
    
    void getFsig(std::vector<float>& fftData, std::vector<float>& dest)
    {
        auto numBins = fftSize / 2;
        auto twoPi = juce::MathConstants<float>::twoPi;
        auto pi = juce::MathConstants<float>::pi;
        
        //Store DC and Nyquist as is
        dest[0] = fftData[0];
        dest[1] = fftData[1];
        
        for(int i = 1; i < numBins; ++i)
        {
            auto expectedPhase = twoPi * i * hopSize/fftSize;
            
            auto real = fftData[i * 2];
            auto imag = fftData[i * 2 +1];
            
            auto mag = std::sqrt(real * real + imag * imag);
            auto phase = std::atan2(imag, real);
            
            auto phaseDelta = phase - prevPhase[i];
            prevPhase[i] = phase;
            
            phaseDelta -= expectedPhase;
            phaseDelta = std::fmod(phaseDelta + pi, twoPi);
            
            //instantaneous Frequency in radians per sample
            auto instantFreq = expectedPhase + phaseDelta / hopSize;
            
            fsig[i] = mag;
            fsig[i + 1] = instantFreq;
        }
    }
    
    void resynthesizeFsig(std::vector<float>& fsig, std::vector<float>& dest)
    {
        auto numBins = fftSize / 2;
        
        //Store DC and Nyquist as is
        dest[0] = fsig[0];
        dest[1] = fsig[1];
        
        for(int i = 1; i < numBins; ++i)
        {
            auto mag = fsig[i];
            auto instFreq = fsig[i + 1];
            float newPhase = prevPhase[i] + instFreq * hopSize;
            
            auto real = mag * std::cos(newPhase);
            auto imag = mag * std::sin(newPhase);
            
            dest[i] = real;
            dest[i + 1] = imag;
        }
    }
    
    
    
private:
    
    size_t bufferSize;
    size_t fftSize = 1024;
    size_t hopSize = fftSize / 4;
    size_t hopsPerBlock;
    int sampsAccumulated = 0;
    
    unsigned long test = 0;
    
    std::unique_ptr<juce::dsp::FFT> fftObject;
    
    unsigned int inputBufferHead;
    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;
    std::vector<float> fftBuffer;
    std::vector<float> magPhasePairs;
    std::vector<float> windowedBuffer;
    std::vector<float> complexProcessBuffer;
    
    std::vector<float> prevPhase;
    std::vector<float> fsig;
    
    unsigned int overlapReadPos = 0;
    unsigned int overlapWritePos = 0;
    std::vector<float> overlapBuffer;
    
    
    bool bufferFull = false;
    
    Window window;
    
};



#endif /* PhaseVocoder_hpp */
