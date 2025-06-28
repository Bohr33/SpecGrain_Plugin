//
//  SpectralProcessors.hpp
//  SpecGrain
//
//  Created by Benjamin Ward (Old Computer) on 6/25/25.
//

#ifndef SpectralProcessors_hpp
#define SpectralProcessors_hpp

#include <stdio.h>
#include <vector>
#include "Fsig.h"

class SpectralBlur
{
public:
    SpectralBlur(){};
    
    void prepare(size_t nBins)
    {
        numBins = nBins;
        blurBuffer.resize(maxBlurFrames);
        for(auto& f : blurBuffer)
            f.resize(numBins);
    }
    
    void blurFsig(float blurAmt, fsig& fsigIn, fsig& fsigOut)
    {
        //copy currenty fsig into buffer
        blurBuffer[blurWriteIndex] = fsigIn;
        
        int blurFrames = juce::roundToInt(blurAmt * maxBlurFrames);
        blurWriteIndex = (blurWriteIndex + 1) % maxBlurFrames;
        
        if(blurFrames == 0)
        {
            fsigOut = fsigIn;
            return;
        }
        
        for(auto bin = 0; bin < numBins; bin++)
        {
            float amp = 0.0f;
            float freq = 0.0f;
            for(auto i = 0; i < blurFrames; i++)
            {
                auto index = blurWriteIndex - 1 - i;
                index = (index + maxBlurFrames) % maxBlurFrames;
                amp += blurBuffer[index].amplitudes[bin];
                freq += blurBuffer[index].frequencies[bin];
            }
            fsigOut.amplitudes[bin] = amp / blurFrames;
            fsigOut.frequencies[bin] = freq / blurFrames;
        }
    }
    
private:
    size_t numBins;
    
    std::vector<fsig> blurBuffer;
    int blurWriteIndex = 0;
    int maxBlurFrames = 600;
};

class SpectralStretch
{
public:
    SpectralStretch(){};
    
    void prepare(size_t nBins)
    {
        numBins = nBins;
        fBuffer.resize(maxFrames);
        for(auto& f : fBuffer)
            f.resize(numBins);
        
        processBuffer.resize(numBins);
    }
    
    void process(float stretchAmt, fsig& input, fsig& output)
    {
        //Add input to buffer
        fBuffer[writeIndex] = input;
        
        //If stretch Amount is 0, no effect
        if(stretchAmt <= 0)
        {
            output = input;
            return;
        }
        
        //ensure stretch Amt is less than 1
        stretchAmt = stretchAmt < 1 ? stretchAmt : 0.99;
        
        //Number of index reads per loop
        int numFrames = maxFrames * stretchAmt;
        
        //Each increment, increase (decrease) buffer index by 1 - stretch amount
        float indexDelta = (1 - stretchAmt);
        
        float index = wrapFloatIndex(writeIndex, maxFrames);
        
        //cacluate number of iterations needed
        float numIterations = numFrames / indexDelta;
        numIterations = juce::jmin<float>(numIterations, 100.0);
        
        //increment and total length cancel out to give maxFrames as number of iterations
        for(int j = 0; j < (int) numIterations; j++)
        {
            interpFsig(index, fBuffer, processBuffer);
            for(int bin = 0; bin < numBins; bin++)
            {
                output.frequencies[bin] += processBuffer.frequencies[bin] / numIterations;
                output.amplitudes[bin] += processBuffer.amplitudes[bin] / numIterations;
            }
            
            index -= indexDelta;
            index = wrapFloatIndex(index, maxFrames);
        }

        
        writeIndex = (writeIndex + 1) % maxFrames;
    }
    
    //This seems expensive, but showed virtually no difference without interpolation
    //Main bottle neck comes from iteration loop
    void interpFsig(float index, std::vector<fsig>& buffer, fsig& output)
    {
        //interpolate fsigs
        int iLow = index;
        int iHigh = iLow + 1;
        iHigh = (iLow + 1) % maxFrames;
        float frac = index - (float)iLow;
        
        
        for(int i = 0; i < numBins; i++)
        {
            output.frequencies[i] = buffer[iLow].frequencies[i] * (1 - frac) + buffer[iHigh].frequencies[i] * frac;
            output.amplitudes[i] = buffer[iLow].amplitudes[i] * (1 - frac) + buffer[iHigh].amplitudes[i] * frac;
        }
    }
    
    inline float wrapFloatIndex(float index, float max) {
        return fmod(fmod(index, max) + max, max);
    }

private:
    size_t numBins;
    std::vector<fsig> fBuffer;
    fsig processBuffer;
    unsigned int writeIndex = 0;
    
    //Can't process more than 5 frames before CPU bottlenecks
    unsigned int maxFrames = 9;
    
};


class SpectralStretch2
{
public:
    SpectralStretch2(){};
    
    void prepare(size_t nBins)
    {
        numBins = nBins;
        fBuffer.resize(maxFrames);
        for(auto& f : fBuffer)
            f.resize(numBins);
        
        processBuffer.resize(numBins);
        
        //test spacing
        spacing = 40;
    }
    
    
    
    void process(float stretchTime, float stretchDensity, fsig& input, fsig& output)
    {
        //Add input to buffer
        fBuffer[writeIndex] = input;
        
        //If stretch Amount is 0, no effect
        if(stretchTime <= 0)
        {
            output = input;
            return;
        }
        
        //ensure stretch Amt is less than 1
        stretchTime = stretchTime < 1 ? stretchTime : 0.99;
        
        //Number of index reads per loop
        int numFrames = maxFrames * stretchTime;
        
        //Each increment, increase (decrease) buffer index by 1 - stretch amount
        float indexDelta = numFrames / ((1 - stretchDensity) * 10);
        
        float index = wrapFloatIndex(writeIndex, maxFrames);
        
        //cacluate number of iterations needed
        float numIterations = numFrames / indexDelta;
        numIterations = juce::jmin<float>(numIterations, 100.0);
        
        DBG("numIterations = " + juce::String(numIterations));
        
        //increment and total length cancel out to give maxFrames as number of iterations
        for(int j = 0; j < (int) numIterations; j++)
        {
            interpFsig(index, fBuffer, processBuffer);
            for(int bin = 0; bin < numBins; bin++)
            {
                output.frequencies[bin] += processBuffer.frequencies[bin] / numIterations;
                output.amplitudes[bin] += processBuffer.amplitudes[bin] / numIterations;
            }
            
            index -= indexDelta;
            index = wrapFloatIndex(index, maxFrames);
        }

        
        writeIndex = (writeIndex + 1) % maxFrames;
    }
    
    //This seems expensive, but showed virtually no difference without interpolation
    //Main bottle neck comes from iteration loop
    void interpFsig(float index, std::vector<fsig>& buffer, fsig& output)
    {
        //interpolate fsigs
        int iLow = index;
        int iHigh = iLow + 1;
        iHigh = (iLow + 1) % maxFrames;
        float frac = index - (float)iLow;
        
        
        for(int i = 0; i < numBins; i++)
        {
            output.frequencies[i] = buffer[iLow].frequencies[i] * (1 - frac) + buffer[iHigh].frequencies[i] * frac;
            output.amplitudes[i] = buffer[iLow].amplitudes[i] * (1 - frac) + buffer[iHigh].amplitudes[i] * frac;
        }
    }
    
    inline float wrapFloatIndex(float index, float max) {
        return fmod(fmod(index, max) + max, max);
    }

private:
    size_t numBins;
    std::vector<fsig> fBuffer;
    fsig processBuffer;
    unsigned int writeIndex = 0;
    
    float spacing;
    
    //Can't process more than 5 frames before CPU bottlenecks
    unsigned int maxFrames = 1000;
    
};


class SpectralDelay
{
public:
    SpectralDelay(){};
    
    void prepare(size_t nBins)
    {
        numBins = nBins;
        delayBuffer.resize(maxDelayFrames);
        for(auto& f : delayBuffer)
            f.resize(numBins);
    }
    
    
    void spectralStretch(float delayTime, float delayAmt, float feedback, bool freqToggle, fsig& fsigIn, fsig& fsigOut)
    {
        float freqTogVal = freqToggle ? 1.0 : 0.0;
        
        writeIndex = (writeIndex + 1) % maxDelayFrames;
        
        fsigOut = fsigIn;
        
        int delayReadIndex = ((writeIndex - juce::roundToInt(delayTime * maxDelayFrames)) + maxDelayFrames) % maxDelayFrames;
        
        for(int bin = 0; bin < numBins; bin++)
        {
            auto ampIn = fsigIn.amplitudes[bin];
            auto freqIn = fsigIn.frequencies[bin];
            auto delayAmp = delayBuffer[delayReadIndex].amplitudes[bin];
            auto delayFreq = delayBuffer[delayReadIndex].frequencies[bin];
            
            fsigOut.amplitudes[bin] += delayAmp * delayAmt;
            fsigOut.frequencies[bin] += delayFreq * delayAmt * freqTogVal;
            
            delayBuffer[writeIndex].amplitudes[bin] = ampIn + delayAmp * feedback;
            delayBuffer[writeIndex].frequencies[bin] = freqIn + delayFreq * feedback;
        }
    }
    
    
    
private:
    size_t numBins;
    
    std::vector<fsig> delayBuffer;
    int writeIndex = 0;
    int maxDelayFrames = 500;
};


#endif /* SpectralProcessors_hpp */
