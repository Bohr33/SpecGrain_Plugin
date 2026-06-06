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
    
    TitleWithUnderline title;
    
    GUICollection stretchControls;
    GUICollection delayControls;
    GUICollection pitchControls;
    GUICollection blurControls;
    GUICollection gateControls;
    
    BasicDialComponent pitchSlider;
    BasicDialComponent blurSlider;
    BasicDialComponent stretchTimeSlider;
    BasicDialComponent stretchDensitySlider;
    BasicDialComponent delayAmtSlider;
    BasicDialComponent delayTimeSlider;
    BasicDialComponent feedbackSlider;
    BasicDialComponent gateSlider;
    

    BasicToggleComponent delayFreqButton;
    
    juce::ComboBox fftSizeMenu;
    juce::Label fftSizeLabel;
    
    
    
    MyLookAndFeel myLookAndFeel;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PVExperimentsAudioProcessorEditor)
};
