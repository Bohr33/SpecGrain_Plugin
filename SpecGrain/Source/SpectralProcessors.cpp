//
//  SpectralProcessors.cpp
//  SpecGrain
//
//  Created by Benjamin Ward (Old Computer) on 6/25/25.
//

#include "SpectralProcessors.hpp"


void PitchShift::prepare(size_t nBins)
{
    numBins = nBins;
}

void PitchShift::process(float shiftAmt, fsig& fsigIn, fsig& fsigOut)
{
    //Clear Output to be Safe
    fsigOut.clear();
    
    //For each bin, increase bin number, multiply frequency by shiftamount
    for(int bin = 0; bin < numBins; bin++)
    {
        float newBin = juce::roundToInt(shiftAmt * bin);
        int lowBin = static_cast<int>(newBin);
        int highBin = lowBin + 1;
        float frac = newBin - lowBin;
        
        if(lowBin < numBins)
        {
            fsigOut.amplitudes[newBin] += fsigIn.amplitudes[bin] * (1 - frac);
            fsigOut.frequencies[newBin] = shiftAmt * fsigIn.frequencies[bin];
        }
            
        if(highBin < numBins)
        {
            fsigOut.amplitudes[newBin] += fsigIn.amplitudes[bin] * frac;
            fsigOut.frequencies[newBin] = shiftAmt * fsigIn.frequencies[bin];
        }

    }
    
    
}

void SpectralBlur::prepare(size_t nBins)
{
    numBins = nBins;
    blurBuffer.resize(maxBlurFrames);
    for(auto& f : blurBuffer)
        f.resize(numBins);
}

void SpectralBlur::blurFsig(float blurAmt, fsig& fsigIn, fsig& fsigOut)
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


void SpectralStretch::prepare(size_t nBins)
{
    numBins = nBins;
    fBuffer.resize(maxFrames);
    for(auto& f : fBuffer)
        f.resize(numBins);
    
    processBuffer.resize(numBins);
    
    //test spacing
    spacing = 40;
}

void SpectralStretch::process(float stretchTime, float stretchDensity, fsig& input, fsig& output)
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
    float indexDelta = numFrames / (stretchDensity * 10);
    
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


void SpectralDelay::prepare(size_t nBins)
{
    numBins = nBins;
    delayBuffer.resize(maxDelayFrames);
    for(auto& f : delayBuffer)
        f.resize(numBins);
}

void SpectralDelay::spectralStretch(float delayTime, float delayAmt, float feedback, bool freqToggle, fsig& fsigIn, fsig& fsigOut)
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


void SpectralGate::prepare(size_t nBins)
{
    numBins = nBins;
}

void SpectralGate::process(float gateAmount, fsig& input)
{
    for(auto i = 0; i < numBins; i++)
    {
        if(input.amplitudes[i] <= gateAmount)
        {
            input.amplitudes[i] = 0.0;
        }
    }
}
