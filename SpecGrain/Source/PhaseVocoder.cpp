/*
  ==============================================================================

    PhaseVocoder.cpp
    Created: 26 May 2026 11:30:24pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "PhaseVocoder.h"


PhaseVocoder::PhaseVocoder()
{}

void PhaseVocoder::prepare(size_t newBlockSize, size_t newFftSize)
{
    blockSize = newBlockSize;
    fftSize = newFftSize;
    hopSize = static_cast<int>(fftSize / overlapAmount);
    hopsPerBlock = blockSize / hopSize;
    numBins = fftSize / 2 + 1;
    
    
    //Reset indecies
    sampsAccumulated = 0;
    overlapReadPos = 0;
    inputBufferHead = 0;
    overlapWritePos = (int) fftSize; //Overlap write must start 'fftSize' ahead of read
    bufferCounter = 0;
    
    
    auto fftOrder = std::log2(fftSize);
    fftObject = std::make_unique<juce::dsp::FFT>(fftOrder);
    window = std::make_unique<Window>(fftSize);
    
    gainCompensation = calculateGainCompensation();
    
    resizeAllVectors();
    clearAllVectors();

}

void PhaseVocoder::pushSamples(std::span<const float> buffer)
{
    
    //Perform check to make sure buffer size is equal to hops per block?
    if (buffer.size() < blockSize) {
        return;
    }
    
    for(auto hop = 0; hop < hopsPerBlock; ++hop)
    {
        
        //Copy hopSize buffer samples to larger buffer for FFT processing
        std::ranges::copy(std::span(buffer.data() + hop * hopSize, hopSize), inputBuffer.data() + inputBufferHead);
        
    
        sampsAccumulated += hopSize;
        //Once buffer is full, continue to FFT
        if(sampsAccumulated < fftSize)
            return;
        
        
        
        //Clear used buffers for safety
        std::fill(fftBuffer.begin(), fftBuffer.end(), 0.0f);
        fsigBuff1.clear();
        
        //Apply window and pass to larger array
        for(int i = 0; i < fftSize; ++i)
        {
            auto readIndex = (inputBufferHead + i) % fftSize;
            fftBuffer[i] = window->getValue(i) * inputBuffer[readIndex];
        }
        
        inputBufferHead += hopSize;
        inputBufferHead %= inputBuffer.size();
        
        
        //Perform FFT, and phase vocoder transform, apply processing and return
        fftObject->performRealOnlyForwardTransform(fftBuffer.data(), true);
        
        //Perform Analysis
        pvAnalyze(fftBuffer, fsigBuff1);
        
        frameBuffer[hop] = fsigBuff1;
        bufferCounter++;
    }
}


void PhaseVocoder::pullSamples(std::span<float> outputBuffer)
{
    //This check ensure samples aren't pulled to early
    if(bufferCounter < hopsPerBlock)
        return;
    
    
    // get data from overlap buffer, copy, scale, and send it to output
    for(auto i = 0; i < outputBuffer.size(); i++)
    {
        auto index = (i + overlapReadPos) % overlapBuffer.size();
        auto val = overlapBuffer[index] * gainCompensation;
        outputBuffer[i] = val;
        overlapBuffer[index] = 0;
    }
    
    //Increment Read Positions
    overlapReadPos += blockSize;
    overlapReadPos %= overlapBuffer.size();

    
    
    for(auto hop = 0; hop < hopsPerBlock; hop++)
        {
            pvSynthesize(frameBuffer[hop], fftBuffer);
            
            fftObject->performRealOnlyInverseTransform(fftBuffer.data());
            //Apply window on output
            for(int i = 0; i < fftSize; i++)
                fftBuffer[i] = window->getValue(i) * fftBuffer[i];

            //Add newest data to overlap buffer
            addDataToOverlap(fftBuffer);
        }
    

}

void PhaseVocoder::addDataToOverlap(std::vector<float>& dataToWrite)
{
    for(auto i = 0; i < fftSize; ++i)
    {
        auto index = (i + overlapWritePos) % overlapBuffer.size();
        overlapBuffer[index] += dataToWrite[i];
    }
    overlapWritePos += hopSize;
    overlapWritePos %= overlapBuffer.size();
}

void PhaseVocoder::pvAnalyze(std::vector<float>& fftInput, fsig& fsig)
{

    for(int i = 0; i < numBins - 1; ++i)
    {
        auto real = fftInput[i * 2];
        auto imag = fftInput[i * 2 + 1];
        
        auto mag = std::sqrt(real * real + imag * imag);
        auto phase = std::atan2(imag, real);
        
        //Obtain change in phase over 1 hopsize via phase buffer
        float phaseDelta = phase - lastInputPhase[i];
        //Update phase buffer
        lastInputPhase[i] = phase;
        
        //Find center frequency of current Bin
        auto binCenterFreq = twoPi * (float)i / (float)fftSize;
        
        //Calculate expected phase and subtract from delta Phase
        auto expectedPhase = binCenterFreq * (float)hopSize;
        auto phaseRemainder = phaseDelta - expectedPhase;
    
        //Wrap phase to (-pi, pi)
        phaseRemainder = wrapPhase(phaseRemainder);
        
        float binDeviation = phaseRemainder * (float)fftSize / (float)hopSize / twoPi;
        
        //Add current bin number to bin deviation
        float instantFreq = binDeviation + i;
        
        fsig.amplitudes[i] = mag;
        fsig.frequencies[i] = instantFreq;
    }
}

void PhaseVocoder::pvSynthesize(fsig& fsig, std::vector<float>& fftOutput)
{
    
    for(int i = 0; i < numBins - 1; ++i)
    {
        //Obtian magnitude and Frequency Values
        auto mag = fsig.amplitudes[i];
        auto freq = fsig.frequencies[i];
        
        //Calculate phase Increment based on current bin
        //Obtain bin deviation from bin center frequency
        float binDeviation = freq - i;
        float phaseIncrement = binDeviation * twoPi * (float)hopSize / (float)fftSize;
        float binCenterFreq = twoPi * (float)i / (float)fftSize;
        
        //Increment phase, wrap phase, and store for next call
        phaseIncrement += binCenterFreq * hopSize;
        
        float outPhase = phaseIncrement + lastOutputPhase[i];
        lastOutputPhase[i] = outPhase;
        
        //Obtain real and imaginary values, and store in output buffer
        auto real = mag * std::cos(outPhase);
        auto imag = mag * std::sin(outPhase);
        
        fftOutput[i * 2] = real;
        fftOutput[i * 2 + 1] = imag;
    }
}

float PhaseVocoder::wrapPhase(float phaseIn)
{
    float pi = std::numbers::pi;
    if (phaseIn >= 0)
        return fmodf(phaseIn + pi, 2.0 * pi) - pi;
    else
        return fmodf(phaseIn - pi, -2.0 * pi) + pi;
}


std::vector<fsig>& PhaseVocoder::getFsigBuffer()
{
    return frameBuffer;
}

void PhaseVocoder::clearAllVectors()
{
    //Clear all vectors, just zeros the data, doesn't do vector.clear()
    std::ranges::fill(inputBuffer, 0.0f);
    std::ranges::fill(fftBuffer, 0.0f);
    std::ranges::fill(lastInputPhase, 0.0f);
    std::ranges::fill(lastOutputPhase, 0.0f);
    std::ranges::fill(overlapBuffer, 0.0f);
    fsigBuff1.clear();
    
    for(fsig frame : frameBuffer)
        frame.clear();
}

void PhaseVocoder::resizeAllVectors()
{
    //Must Be called after variables are calculated in prepare since
    //resizing is dependant on those variables
    
    inputBuffer.assign(fftSize, 0.0f);
    fftBuffer.assign(fftSize*2, 0.0f);
    lastInputPhase.assign(fftSize/2, 0.0f);
    lastOutputPhase.assign(fftSize/2, 0.0f);
    fsigBuff1.resize(numBins);
    frameBuffer.resize(hopsPerBlock);
    
    for(auto& frame : frameBuffer)
        frame.resize(numBins);
    
    overlapBuffer.resize(fftSize * 2);
}

float PhaseVocoder::calculateGainCompensation()
{
    float gain = 0.0f;
    for(int i = 0; i < fftSize; i++)
    {
        auto val = window->getValue(i);
        gain += val*val;
    }
    gain /= hopSize;
    
    return 1.0f / gain;
}
