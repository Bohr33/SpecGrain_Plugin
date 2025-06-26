/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PhaseVocoder.hpp"

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

private:
    std::unique_ptr<PhaseVocoder> pv;
    juce::AudioProcessorValueTreeState parameters;
    
    
    std::atomic<float>* pitchParameter = nullptr;
    std::atomic<float>* blurParameter = nullptr;
    std::atomic<float>* stretchParameter = nullptr;
    std::atomic<float>* delayParameter = nullptr;
    std::atomic<float>* delayFreqToggleParameter = nullptr;
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpecGrainAudioProcessor)
};
