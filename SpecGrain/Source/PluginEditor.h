/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SpecGrainAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SpecGrainAudioProcessorEditor (SpecGrainAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~SpecGrainAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SpecGrainAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::Slider pitchSlider;
    juce::Label  pitchSliderLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchSliderAttch;
    
    juce::Slider blurSlider;
    juce::Label  blurSliderLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> blurSliderAttch;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpecGrainAudioProcessorEditor)
};
