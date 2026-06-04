/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Graphics.h"

//==============================================================================
/**
*/
class PVExperimentsAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PVExperimentsAudioProcessorEditor (PVExperimentsAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~PVExperimentsAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PVExperimentsAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    juce::Slider pitchSlider;
    juce::Label  pitchSliderLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchSliderAttch;
    
    juce::Slider blurSlider;
    juce::Label  blurSliderLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> blurSliderAttch;

    juce::Slider stretchTimeSlider;
    juce::Label  stretchTimeSliderLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stretchTimeSliderAttch;
    
    juce::Slider stretchDensitySlider;
    juce::Label  stretchDensitySliderLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stretchDensitySliderAttch;
    
    juce::Slider delayAmtSlider;
    juce::Label  delayAmtSliderLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayAmtSliderAttch;

    juce::Slider delayTimeSlider;
    juce::Label  delayTimeSliderLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeSliderAttch;
    
    juce::Slider feedbackSlider;
    juce::Label  feedbackLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackSliderAttch;
    
    juce::TextButton delayFreqButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> delayFreqButtonAttch;
    
    juce::Slider gateSlider;
    juce::Label gateSliderLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gateSliderAttch;
    
    juce::ComboBox fftSizeMenu;
    juce::Label fftSizeLabel;
    
    MyLookAndFeel myLookAndFeel;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PVExperimentsAudioProcessorEditor)
};
