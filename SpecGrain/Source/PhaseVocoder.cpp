//
//  PhaseVocoder.cpp
//  SpecGrain
//
//  Created by Benjamin Ward (Old Computer) on 6/18/25.
//

#include "PhaseVocoder.hpp"

Window::Window(size_t size) : windowSize(size)
{
    window.resize(windowSize);
    makeWindow();
}

void Window::makeWindow()
{
    for (int i = 0; i < windowSize; ++i)
        window[i] = 0.5 * (1 - std::cos((juce::MathConstants<double>::twoPi * i)/(windowSize-1)));
}

float Window::getRawValue(unsigned int index)
{
    return window[index];
}



PhaseVocoder::PhaseVocoder(juce::AudioProcessorValueTreeState& vts) : window(fftSize), valueTreeState(vts)
{
    valueTreeState.addParameterListener("pitchShift", this);
    valueTreeState.addParameterListener("blurAmt", this);
    valueTreeState.addParameterListener("stretchAmt", this);
    valueTreeState.addParameterListener("delayAmt", this);
    valueTreeState.addParameterListener("delayTime", this);
    valueTreeState.addParameterListener("feedback", this);
}

void PhaseVocoder::prepare(size_t buffSize, double sampleRate)
{
    samplingRate = sampleRate;
    bufferSize = buffSize;
    hopsPerBlock = bufferSize / hopSize;
    
    auto fftOrder = std::log2(fftSize);
    fftObject = std::make_unique<juce::dsp::FFT>(fftOrder);
    ifftObject = std::make_unique<juce::dsp::FFT>(fftOrder);
    
    blurObj.prepare(numBins);
    delayObj.prepare(numBins);
    
    inputBuffer.resize(fftSize);
    fftBuffer.resize(fftSize*2);
    
    lastInputPhase.resize(fftSize/2);
    lastOutputPhase.resize(fftSize/2);
    
    fsigIn.resize(numBins);
    fsigOut.resize(numBins);
    
    lastInputPhase.clear();
    lastOutputPhase.clear();

    
    overlapBuffer.resize(fftSize * 2);
    bufferFull = false;
    
    DBG("Prepared Successfully");
    DBG("Buffer Size = " + juce::String(bufferSize));
    DBG("FFT Size = " + juce::String(fftSize));
    DBG("Hop Size = " + juce::String(hopSize));
    DBG("Hops Per Block = " + juce::String(hopsPerBlock));
    
}

void PhaseVocoder::process(juce::AudioBuffer<float>& buffer)
{
    //Perform stft operation for as many hop sizes
    //within 1 buffersize
    for(auto hop = 0; hop < hopsPerBlock; ++hop)
    {
        //Copy hopSize buffer samples to larger buffer for FFT processing
        juce::FloatVectorOperations::copy(inputBuffer.data() + inputBufferHead, buffer.getReadPointer(0) + hop * hopSize, hopSize);
        
        sampsAccumulated += hopSize;
        if(sampsAccumulated >= fftSize)
            bufferFull = true;
        
        //Once buffer is full, begin actual processing loop
        if(!bufferFull)
            return;
        
        fftBuffer.clear();
        
        
        //Apply window and pass to larger array
        for(int i = 0; i < fftSize; ++i)
        {
            auto readIndex = (inputBufferHead + i) % fftSize;
            fftBuffer[i] = window.getRawValue(i) * inputBuffer[readIndex];
        }
        
        inputBufferHead += hopSize;
        inputBufferHead %= inputBuffer.size();
        
        
        //Perform FFT, and phase vocoder transform, apply processing and return
        fftObject->performRealOnlyForwardTransform(fftBuffer.data(), true);

        pvAnalyze(fftBuffer, fsigIn);
        
        float pShift = pitchShiftAmt.load();
        float blurAmt = blurAmount.load();
        float stretch = stretchAmt.load();
        float dAmt = delayAmt.load();
        float dTime = delayTime.load();
        float dfeed = feedback.load();
        
        //Process in Spectral Domain Here
        pitchShift(pShift, fsigIn, fsigOut);
        

        delayObj.spectralStretch(dTime, dAmt, dfeed, fsigOut, fsigIn);
        blurObj.blurFsig(blurAmt, fsigIn, fsigOut);
        
        pvSynthesize(fsigOut, fftBuffer);
        
        
        ifftObject->performRealOnlyInverseTransform(fftBuffer.data());
        
        
        //Apply window on output
        for(int i = 0; i < fftSize; ++i)
            fftBuffer[i] = window.getRawValue(i) * fftBuffer[i];

        //Add newest data to overlap buffer
        addDataToOverlap(fftBuffer);
    }
    
    //read next data and clear read portion
    buffer.clear();

    for(auto i = 0; i < buffer.getNumSamples(); ++i)
    {
        auto index = (i + overlapReadPos) % overlapBuffer.size();
        buffer.addSample(0, i, overlapBuffer[index] * scaleFactor);
        buffer.addSample(1, i, overlapBuffer[index] * scaleFactor);
        overlapBuffer[index] = 0;
    }
    
    overlapReadPos += bufferSize;
    overlapReadPos %= overlapBuffer.size();
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

void PhaseVocoder::pitchShift(float shiftAmt, fsig& fsigIn, fsig& fsigOut)
{
    fsigOut.clear();
    
    //Ignore DC and Nyquist bins for now
    for(int bin = 0; bin < numBins; bin++)
    {
        int newBin = juce::roundToInt(shiftAmt * bin);

        if(newBin < numBins)
        {
            fsigOut.amplitudes[newBin] += fsigIn.amplitudes[bin];
            fsigOut.frequencies[newBin] = shiftAmt * fsigIn.frequencies[bin];
        }

    }
    
}

void PhaseVocoder::pvAnalyze(std::vector<float>& fftInput, fsig& fsig)
{
    auto twoPi = juce::MathConstants<float>::twoPi;
    
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
    auto twoPi = juce::MathConstants<float>::twoPi;
    for(int i = 0; i < numBins - 1; ++i)
    {
        auto mag = fsig.amplitudes[i];
        auto freq = fsig.frequencies[i];
        
        float binDeviation = freq - i;
        
        float phaseIncrement = binDeviation * twoPi * (float)hopSize / (float)fftSize;
        
        float binCenterFreq = twoPi * (float)i / (float)fftSize;
        
        phaseIncrement += binCenterFreq * hopSize;
        
        float outPhase = wrapPhase(phaseIncrement + lastOutputPhase[i]);
        lastOutputPhase[i] = outPhase;
        
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

void PhaseVocoder::parameterChanged(const juce::String& parameterID, float newValue)
{
    if(parameterID == "pitchShift")
        pitchShiftAmt.store(newValue);
    else if(parameterID == "blurAmt")
    {
        blurAmount.store(newValue);
        DBG("Blur Amt = " + juce::String(newValue));
    }else if(parameterID == "stretchAmt")
    {
        stretchAmt.store(newValue);
        DBG("Stretch Amt = " + juce::String(newValue));
    }else if(parameterID == "delayAmt")
    {
        delayAmt.store((newValue));
        DBG("Delay Amt = " + juce::String(newValue));
    }else if(parameterID == "delayTime")
        delayTime.store(newValue);
    else if(parameterID == "feedback")
        feedback.store(newValue);
        
}

