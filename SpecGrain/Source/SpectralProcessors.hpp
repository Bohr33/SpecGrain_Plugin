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
    
    
    void spectralStretch(float delayTime, float delayAmt, float feedback, fsig& fsigIn, fsig& fsigOut)
    {
        
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
//            fsigOut.frequencies[bin] += delayFreq * delayAmt;
            
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
