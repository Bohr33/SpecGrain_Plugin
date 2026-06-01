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

void PhaseVocoder::prepare(size_t buffSize, double sampleRate, unsigned int sizeFft)
{
    fftSize = sizeFft;
    samplingRate = sampleRate;
    bufferSize = buffSize;
    hopSize = fftSize / overlapAmount;
    
    hopsPerBlock = buffSize / hopSize;
    numBins = fftSize / 2 + 1;
    
    
    //Reset indecies
    sampsAccumulated = 0;
    overlapReadPos = 0;
    
    //Overlap write must start 'fftSize' ahead of read
    overlapWritePos = (int) fftSize;
    inputBufferHead = 0;
    
    auto fftOrder = std::log2(fftSize);
    fftObject = std::make_unique<juce::dsp::FFT>(fftOrder);
    window = std::make_unique<Window>(fftSize);
    
    gainCompensation = 0.0f;
    for(int i = 0; i < fftSize; i++)
    {
        auto val = window->getValue(i);
        gainCompensation += val*val;
    }
    gainCompensation /= hopSize;
    gainCompensation = 1.0f / gainCompensation;
    

    //Resize all vectors
    inputBuffer.resize(fftSize);
    fftBuffer.resize(fftSize*2);
    
    lastInputPhase.resize(fftSize/2);
    lastOutputPhase.resize(fftSize/2);
    
    fsigBuff1.resize(numBins);
    
    
    frameBuffer.resize(hopsPerBlock);
    for(auto& frame : frameBuffer)
        frame.resize(numBins);
    
    bufferCounter = 0;
    
    overlapBuffer.resize(fftSize * 2);
    bufferFull = false;
    
    
    
    DBG("fftSize: " << fftSize);
    DBG("hopSize: " << hopSize);
    DBG("hopsPerBlock: " << hopsPerBlock);
    DBG("bufferSize: " << bufferSize);
    
    //Clear all vectors, just zeros the data, doesn't do vector.clear()
    juce::FloatVectorOperations::clear(inputBuffer.data(), inputBuffer.size());
    juce::FloatVectorOperations::clear(fftBuffer.data(), fftBuffer.size());
    juce::FloatVectorOperations::clear(lastInputPhase.data(), lastInputPhase.size());
    juce::FloatVectorOperations::clear(lastOutputPhase.data(), lastOutputPhase.size());
    fsigBuff1.clear();
    juce::FloatVectorOperations::clear(overlapBuffer.data(), overlapBuffer.size());
    
    for(fsig frame : frameBuffer)
        frame.clear();

}

void PhaseVocoder::pushSamples(std::span<const float> buffer)
{
    
    //Perform check to make sure buffer size is equal to hops per block?
    if (buffer.size() < bufferSize) {
        return;
    }
    
    
    for(auto hop = 0; hop < hopsPerBlock; ++hop)
    {
        
        //Copy hopSize buffer samples to larger buffer for FFT processing
        juce::FloatVectorOperations::copy(inputBuffer.data() + inputBufferHead, buffer.data() + hop * hopSize, hopSize);
        
        
        sampsAccumulated += hopSize;
        if(sampsAccumulated >= fftSize)
            bufferFull = true;
        
        //Once buffer is full, begin actual processing loop
        if(!bufferFull)
            return;
        
        //Clear used buffers for safety
        juce::FloatVectorOperations::fill(fftBuffer.data(), 0.0, fftBuffer.size());
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
    overlapReadPos += bufferSize;
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
    float pi = juce::MathConstants<float>::pi;
    if (phaseIn >= 0)
        return fmodf(phaseIn + pi, 2.0 * pi) - pi;
    else
        return fmodf(phaseIn - pi, -2.0 * pi) + pi;
}


std::vector<fsig>& PhaseVocoder::getFsigBuffer()
{
    return frameBuffer;
}
