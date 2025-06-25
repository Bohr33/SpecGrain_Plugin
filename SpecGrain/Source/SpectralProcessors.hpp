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
        blurBuffer[blurIndex] = fsigIn;
        
        int blurFrames = juce::roundToInt(blurAmt * maxBlurFrames);
        blurIndex = (blurIndex + 1) % maxBlurFrames;
        
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
                auto index = blurIndex - 1 - i;
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
    int blurIndex = 0;
    int maxBlurFrames = 600;
};


#endif /* SpectralProcessors_hpp */
