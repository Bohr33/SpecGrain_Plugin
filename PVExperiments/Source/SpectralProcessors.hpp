//
//  SpectralProcessors.hpp
//  PVExperiments
//
//  Created by Benjamin Ward (Old Computer) on 6/25/25.
//

#ifndef SpectralProcessors_hpp
#define SpectralProcessors_hpp

#include <stdio.h>
#include <vector>
#include "Fsig.h"
#include <algorithm>
#include <cmath>

class PitchShift
{
public:
    PitchShift(){};
    void prepare(int fftSize);
    void process(fsig& frame, float shiftAmt);
    
    
private:
    int fftSize;
    size_t numBins;
    fsig tempFrame;
};

class SpectralBlur
{
public:
    SpectralBlur(){};
    
    void prepare(int fftSize);
    void process(fsig& frame, float blurAmt);
    
private:
    size_t numBins;
    std::vector<fsig> blurBuffer;
    int blurWriteIndex = 0;
    int maxBlurFrames = 300;
};

class SpectralStretch
{
public:
    SpectralStretch(){};
    
    void prepare(int fftSize);
    void process(fsig& frame, float stretchTime, float stretchDensity);
    
    void interpFsig(float index, std::vector<fsig>& buffer, fsig& output);
    
    inline float wrapFloatIndex(float index, float max) {
        return fmod(fmod(index, max) + max, max);
    }

private:
    size_t numBins;
    std::vector<fsig> fBuffer;
    fsig processBuffer;
    unsigned int writeIndex = 0;
    
    //Don't make this too high, at fftSize = 2048, this can get out of hand and
    //allocate too much memory at once
    size_t maxFrames = 200;
    
};


class SpectralDelay
{
public:
    SpectralDelay(){};
    
    void prepare(int fftSize, double sampleRate, int overlapAmt = 4);
    void process(fsig& frame, float delayTime, float delayAmt, float feedback, bool freqToggle);
    
private:
    size_t numBins;
    size_t hopSize;
    double sr;

    int overlap;
    
    std::vector<fsig> delayBuffer;
    int writeIndex = 0;
    float maxDelaySeconds = 2.0;
    int maxDelayFrames;
};


class SpectralGate
{
public:
    SpectralGate(){};
    
    void prepare(int fftSize);
    void process(fsig& input, float gateAmt);
    
private:
    size_t numBins;
};


#endif /* SpectralProcessors_hpp */
