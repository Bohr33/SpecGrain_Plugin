/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PhaseVocoder.h"
#include "SpectralProcessors.hpp"
#include "PhaseVocoderEngine.h"

//==============================================================================
/**
*/
class SpecGrainAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SpecGrainAudioProcessor();
    ~SpecGrainAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //Function to Create Parameter Layout
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    unsigned int fftSize = 1024;
    
    void fftSizeChanged(int newFFTsize);

private:
    
    
    
    std::unique_ptr<PhaseVocoderEngine> pvEngine;
    
    juce::AudioProcessorValueTreeState parameters;
    

    std::vector<PitchShift> pShifts;
    std::vector<SpectralBlur> specBlurs;
    std::vector<SpectralStretch> specStretchs;
    std::vector<SpectralDelay> specDelays;
    std::vector<SpectralGate> specGates;
    
    
    std::atomic<float>* pShiftParam = nullptr;
    std::atomic<float>* blurParam = nullptr;
    std::atomic<float>* stretchTimeParam = nullptr;
    std::atomic<float>* stretchDensityParam = nullptr;
    std::atomic<float>* delayAmtParam = nullptr;
    std::atomic<float>* delayTimeParam = nullptr;
    std::atomic<float>* feedbackParam = nullptr;
    std::atomic<float>* gateAmtParam = nullptr;
    std::atomic<float>* delayFreqToggleParam = nullptr;
    
    
    //Flags for FFT Resizing
    std::atomic<int> pendingFFTSize {1024};
    std::atomic<bool> resizePending {false};
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpecGrainAudioProcessor)
};
