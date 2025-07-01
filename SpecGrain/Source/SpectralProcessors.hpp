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

class PitchShift
{
public:
    PitchShift(){};
    void prepare(size_t nBins);
    void process(float shiftAmt, fsig& fsigIn, fsig& fsigOut);
    
private:
    size_t numBins;
};

class SpectralBlur
{
public:
    SpectralBlur(){};
    
    void prepare(size_t nBins);
    void process(float blurAmt, fsig& fsigIn, fsig& fsigOut);
    
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
    
    void prepare(size_t nBins);
    void process(float stretchTime, float stretchDensity, fsig& input, fsig& output);
    void interpFsig(float index, std::vector<fsig>& buffer, fsig& output);
    
    inline float wrapFloatIndex(float index, float max) {
        return fmod(fmod(index, max) + max, max);
    }

private:
    size_t numBins;
    std::vector<fsig> fBuffer;
    fsig processBuffer;
    unsigned int writeIndex = 0;
    
    float spacing;
    
    unsigned int maxFrames = 1000;
    
};


class SpectralDelay
{
public:
    SpectralDelay(){};
    
    void prepare(size_t nBins, double sampleRate, size_t hSize);
    void process(float delayTime, float delayAmt, float feedback, bool freqToggle, fsig& fsigIn, fsig& fsigOut);
    
private:
    size_t numBins;
    size_t hopSize;
    double sr;
    
    std::vector<fsig> delayBuffer;
    int writeIndex = 0;
    float maxDelaySeconds = 2.0;
    int maxDelayFrames;
};


class SpectralGate
{
public:
    SpectralGate(){};
    
    void prepare(size_t nBins);
    void process(float gateAmount, fsig& input);
    
private:
    size_t numBins;
};


#endif /* SpectralProcessors_hpp */
