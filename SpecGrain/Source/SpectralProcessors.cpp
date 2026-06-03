//
//  SpectralProcessors.cpp
//  SpecGrain
//
//  Created by Benjamin Ward (Old Computer) on 6/25/25.
//

#include "SpectralProcessors.hpp"


void PitchShift::prepare(int fftSize)
{
    fftSize = fftSize;
    numBins = fftSize / 2 + 1;
    
    tempFrame.resize(numBins);
}

void PitchShift::process(fsig &frame, float shiftAmt)
{
    tempFrame.clear();
    tempFrame = frame;
    frame.clear();
    
    //For each bin, increase bin number, multiply frequency by shiftamount
    for(int bin = 0; bin < numBins; bin++)
    {
        float newBin = static_cast<int>(std::round(shiftAmt * bin));
        int lowBin = static_cast<int>(newBin);
        int highBin = lowBin + 1;
        float frac = newBin - lowBin;
        
        if(lowBin < numBins)
        {
            frame.amplitudes[newBin] += tempFrame.amplitudes[bin] * (1 - frac);
            frame.frequencies[newBin] = shiftAmt * tempFrame.frequencies[bin];
        }
            
        if(highBin < numBins)
        {
            frame.amplitudes[newBin] += tempFrame.amplitudes[bin] * frac;
            frame.frequencies[newBin] = shiftAmt * tempFrame.frequencies[bin];
        }
    }
}

void SpectralBlur::prepare(int fftSize)
{
    numBins = fftSize / 2 + 1;
    blurWriteIndex = 0;
    
    blurBuffer.resize(maxBlurFrames);
    for(auto& f : blurBuffer)
    {
        f.resize(numBins);
        f.clear();
    }
}


void SpectralBlur::process(fsig &frame, float blurAmt)
{
    //copy currenty fsig into buffer
    blurBuffer[blurWriteIndex] = frame;
    
    int blurFrames = static_cast<int>(std::round(blurAmt * maxBlurFrames));
    
    if(blurFrames == 0)
        return;
    
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
        frame.amplitudes[bin] = amp / blurFrames;
        frame.frequencies[bin] = freq / blurFrames;
    }
    blurWriteIndex = (blurWriteIndex + 1) % maxBlurFrames;
}

void SpectralStretch::prepare(int fftSize)
{
    numBins = fftSize / 2 + 1;
    writeIndex = 0;
    
    fBuffer.resize(maxFrames);
    for(auto& f : fBuffer)
    {
        f.resize(numBins);
        f.clear();
    }
    processBuffer.resize(numBins);
    processBuffer.clear();
}



void SpectralStretch::process(fsig &frame, float stretchTime, float stretchDensity)
{
    //Add input to buffer
    fBuffer[writeIndex] = frame;
    
    //If stretch Amount is 0, no effect
    if(stretchTime <= 0)
        return;
    
    processBuffer.clear();
    
    //ensure stretch Amt is less than 1
    stretchTime = stretchTime < 1 ? stretchTime : 0.99;
    
    //Number of index reads per loop
    int numFrames = maxFrames * stretchTime;
    
    //Each increment, increase (decrease) buffer index by 1 - stretch amount
    float indexDelta = numFrames / (stretchDensity * 50);
    
    float index = wrapFloatIndex(writeIndex, maxFrames);
    
    //cacluate number of iterations needed
    float numIterations = numFrames / indexDelta;
    
    //increment and total length cancel out to give maxFrames as number of iterations
    for(int j = 0; j < (int) numIterations; j++)
    {
        interpFsig(index, fBuffer, processBuffer);
        for(int bin = 0; bin < numBins; bin++)
        {
            frame.frequencies[bin] += processBuffer.frequencies[bin] / numIterations;
            frame.amplitudes[bin] += processBuffer.amplitudes[bin] / numIterations;
        }
        
        index -= indexDelta;
        index = wrapFloatIndex(index, maxFrames);
    }

    
    writeIndex = (writeIndex + 1) % maxFrames;
}

void SpectralStretch::interpFsig(float index, std::vector<fsig>& buffer, fsig& output)
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


void SpectralDelay::prepare(int fftSize, double sampleRate, int overlapAmt)
{
    sr = sampleRate;
    numBins = fftSize / 2 + 1;
    hopSize = fftSize / overlapAmt;
    maxDelayFrames = sr * maxDelaySeconds / hopSize;

    writeIndex = 0;
    
    delayBuffer.resize(maxDelayFrames);
    for(auto& f : delayBuffer)
    {
        f.resize(numBins);
        f.clear();
    }
}


void SpectralDelay::process(fsig &frame, float delayTime, float delayAmt, float feedback, bool freqToggle)
{
    float freqTogVal = freqToggle ? 1.0 : 0.0;
    float fback = std::clamp(feedback, 0.0f, 0.99f);
    
    
    float dtimeSamps = (delayTime/1000.0f)*sr;
    int dTimeFrames = static_cast<int>(dtimeSamps / hopSize);
    
    int maxSafeDelay = maxDelayFrames - 1;
    int clampedDelayFrames = std::min(dTimeFrames, maxSafeDelay);
    int delayReadIndex = (writeIndex - clampedDelayFrames + maxDelayFrames) % maxDelayFrames;

    for (int bin = 0; bin < numBins; ++bin)
    {
        float ampIn = frame.amplitudes[bin];
        float freqIn = frame.frequencies[bin];

        float delayAmp = delayBuffer[delayReadIndex].amplitudes[bin];
        float delayFreq = delayBuffer[delayReadIndex].frequencies[bin];

        if (!std::isfinite(delayAmp)) delayAmp = 0.0f;
        if (!std::isfinite(delayFreq)) delayFreq = 0.0f;

        frame.amplitudes[bin] += delayAmp * delayAmt;
        frame.frequencies[bin] += delayFreq * delayAmt * freqTogVal;

        delayBuffer[writeIndex].amplitudes[bin] = ampIn + delayAmp * fback;
        delayBuffer[writeIndex].frequencies[bin] = freqIn + delayFreq * fback;
    }

    writeIndex = (writeIndex + 1) % maxDelayFrames;
}


void SpectralGate::prepare(int fftSize)
{
    numBins = fftSize / 2 + 1;
}

void SpectralGate::process(fsig& input, float gateAmt)
{
    for(auto i = 0; i < numBins; i++)
    {
        if(input.amplitudes[i] <= gateAmt)
            input.amplitudes[i] = 0.0;
    }
}
