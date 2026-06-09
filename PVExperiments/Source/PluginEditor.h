/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>

#include "PluginProcessor.h"
#include "GUI/Graphics.h"
#include "GUI/GUIComponents.h"

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
