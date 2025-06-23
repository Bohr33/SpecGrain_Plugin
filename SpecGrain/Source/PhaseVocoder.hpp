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
            window[i] = 0.5 * (1 - std::cos((juce::MathConstants<double>::twoPi * i)/(windowSize-1)));
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
    
    float getRawValue(unsigned int index)
    {
        return window[index];
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
        samplingRate = sampleRate;
        bufferSize = buffSize;
        hopsPerBlock = bufferSize / hopSize;
        
        auto fftOrder = std::log2(fftSize);
        fftObject = std::make_unique<juce::dsp::FFT>(fftOrder);
        ifftObject = std::make_unique<juce::dsp::FFT>(fftOrder);
        
        inputBuffer.resize(fftSize);
        outputBuffer.resize(fftSize);
        fftInputBuffer.resize(fftSize);
        fftOutputBuffer.resize(fftSize);
        magPhasePairs.resize(fftSize);
        windowedBuffer.resize(fftSize);
        complexProcessBuffer.resize(fftSize);
        
        
        prevPhase.resize(fftSize/2);
        outputPhase.resize(fftSize/2);
        fsig.resize(fftSize);
        
        prevPhase.clear();
        outputPhase.clear();
        
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
            
            
            buffer.clear(hop * static_cast<int>(hopSize), static_cast<int>(hopSize));
            
            inputBufferHead += hopSize;
            inputBufferHead = inputBufferHead % inputBuffer.size();
            
            sampsAccumulated += hopSize;
            if(sampsAccumulated >= fftSize)
                bufferFull = true;
            
            //Once buffer is full, begin actual processing loop
            if(!bufferFull)
                return;
            
            auto index = ((inputBufferHead + fftSize) - hopSize) % fftSize;
            //Apply window
            
            for(int i = 0; i < fftSize; ++i)
            {
                auto readIndex = (index + i) % fftSize;
                fftInputBuffer[i] = window.getRawValue(i) * inputBuffer[readIndex];
            }
            
            
            //From here, arrays are sized fftSize * 2
            fftObject->performRealOnlyForwardTransform(fftInputBuffer.data(), false);
            
        
            getFsig(fftInputBuffer, fftOutputBuffer);
            //Process in between here
//            pitchShift(1.3333, fsig, complexProcessBuffer);
            
            resynthesizeFsig(fftOutputBuffer, outputBuffer);

//            juce::FloatVectorOperations::copy(outputBuffer.data(), fsig.data(), fftSize);
            
            ifftObject->performRealOnlyInverseTransform(outputBuffer.data());
            
//            juce::FloatVectorOperations::clear(windowedBuffer.data(), fftSize);
            
            //Apply window again
            for(int i = 0; i < fftSize; ++i)
            {
                windowedBuffer[i] = window.getRawValue(i) * outputBuffer[i];
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
            buffer.addSample(0, i, overlapBuffer[index] * scaleFactor);
            buffer.addSample(1, i, overlapBuffer[index] * scaleFactor);
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
    
    void pitchShift(double shiftAmt, std::vector<float>& fsigIn, std::vector<float>& destination)
    {
        
        auto numBins = (fftSize) / 2;
        
        juce::FloatVectorOperations::clear(destination.data(), fftSize);
        destination[0] = fsigIn[0];
        destination[1] = fsigIn[1];
        
        //Ignore DC and Nyquist bins for now
        for(int i = 1; i < numBins; ++i)
        {
            auto newBin = juce::roundToInt(shiftAmt * i);
            if(newBin >= numBins)
                break;
            auto magIndex = newBin * 2;
            auto phaseIndex = magIndex + 1;
            //Shift Magnitude Values
            destination[magIndex] = fsigIn[i * 2];

            //Shift Phase Values
            destination[phaseIndex] = shiftAmt * fsigIn[i * 2 + 1];
        }
    }
    
    
    void getFsig(std::vector<float>& fftData, std::vector<float>& dest)
    {
        auto numBins = fftSize / 2;
        auto twoPi = juce::MathConstants<float>::twoPi;
        
        //Store DC and Nyquist as is
        dest[0] = fftData[0];
        dest[1] = fftData[1];
        
        for(int i = 1; i < numBins; ++i)
        {
            auto real = fftData[i * 2];
            auto imag = fftData[i * 2 + 1];
            
            auto mag = std::sqrt(real * real + imag * imag);
            auto phase = std::atan2(imag, real);
            
            //Obtain change in phase over 1 hopsize via phase buffer
            float phaseDelta = phase - prevPhase[i];
            //Update phase buffer
            prevPhase[i] = phase;
            
            //Find center frequency of current Bin
            auto binCenterFreq = twoPi * i / (float)fftSize;
            
            //Calculate expected phase and subtract from delta Phase
            auto expectedPhase = binCenterFreq * (float)hopSize;
            auto phaseRemainder = phaseDelta - expectedPhase;
        
            //Wrap phase to (-pi, pi)
            phaseRemainder = wrapPhase(phaseRemainder);
            
            float binDeviation = phaseRemainder * (float)fftSize / (float)hopSize / twoPi;
            
            //Add current bin number to bin deviation
            float instantFreq = binDeviation + i;
            
            dest[i * 2] = mag;
            dest[i * 2 + 1] = instantFreq;
        }
    }
    
    void resynthesizeFsig(std::vector<float>& source, std::vector<float>& dest)
    {
        auto numBins = fftSize / 2;
        
        //Store DC and Nyquist as is
        dest[0] = source[0];
        dest[1] = source[1];
        
        auto twoPi = juce::MathConstants<float>::twoPi;
        
        for(int i = 1; i < numBins; ++i)
        {
            auto mag = source[i * 2];
            auto freq = source[i * 2 + 1];
            
            float binDeviation = freq - i;
            
            float phaseDelta = binDeviation * twoPi * (float)hopSize / (float)fftSize;
            
            float binCenterFreq = twoPi * i / (float)fftSize;
            
            phaseDelta += binCenterFreq * hopSize;
            
            float outPhase = wrapPhase(phaseDelta + outputPhase[i]);
            outputPhase[i] = outPhase;
            
            auto real = mag * std::cos(outPhase);
            auto imag = mag * std::sin(outPhase);
            
            dest[i * 2] = real;
            dest[i * 2 + 1] = imag;
        }
    }
    
    float wrapPhase(float phaseIn)
    {
        float pi = juce::MathConstants<float>::pi;
        if (phaseIn >= 0)
            return fmodf(phaseIn + pi, 2.0 * pi) - pi;
        else
            return fmodf(phaseIn - pi, -2.0 * pi) + pi;
    }
    
    
    
private:
    
    size_t bufferSize;
    size_t fftSize = 1024;
    size_t hopSize = fftSize / 8;
    size_t hopsPerBlock;
    float scaleFactor = 0.5;
    int sampsAccumulated = 0;
    
    unsigned long test = 0;
    double samplingRate;
    
    std::unique_ptr<juce::dsp::FFT> fftObject;
    std::unique_ptr<juce::dsp::FFT> ifftObject;
    
    unsigned int inputBufferHead;
    std::vector<float> inputBuffer;
    std::vector<float> outputBuffer;
    std::vector<float> fftInputBuffer;
    std::vector<float> fftOutputBuffer;
    
    std::vector<float> magPhasePairs;
    std::vector<float> windowedBuffer;
    std::vector<float> complexProcessBuffer;
    
    std::vector<float> prevPhase;
    std::vector<float> outputPhase;
    std::vector<float> fsig;
    
    unsigned int overlapReadPos = 0;
    unsigned int overlapWritePos = 0;
    std::vector<float> overlapBuffer;
    
    
    bool bufferFull = false;
    
    Window window;
    
};



#endif /* PhaseVocoder_hpp */
