/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpecGrainAudioProcessorEditor::SpecGrainAudioProcessorEditor (SpecGrainAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    pitchSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "pitchShift", pitchSlider));
    pitchSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    pitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    pitchSliderLabel.setText("Pitch Shift", juce::dontSendNotification);
    pitchSliderLabel.setJustificationType(juce::Justification::centred);
    
    blurSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "blurAmt", blurSlider));
    blurSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    blurSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    blurSliderLabel.setText("Blur Amount", juce::dontSendNotification);
    blurSliderLabel.setJustificationType(juce::Justification::centred);
    
    
    addAndMakeVisible(&pitchSlider);
    addAndMakeVisible(&pitchSliderLabel);
    addAndMakeVisible(&blurSlider);
    addAndMakeVisible(&blurSliderLabel);
}

SpecGrainAudioProcessorEditor::~SpecGrainAudioProcessorEditor()
{
}

//==============================================================================
void SpecGrainAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SpecGrainAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int width = getWidth();
    int height = getHeight();
    
    int sliderWidth = 100;
    int sliderHeight = 100;
    int labelHeight = 40;
    
    pitchSlider.setBounds(width / 2 - sliderWidth, height/2, sliderWidth, sliderHeight);
    pitchSliderLabel.setBounds(width / 2 - sliderWidth, height/2 + sliderHeight, sliderWidth, labelHeight);
    
    blurSlider.setBounds(width / 2, height / 2, sliderWidth, sliderHeight);
    blurSliderLabel.setBounds(width / 2, height / 2 + sliderHeight, sliderWidth, labelHeight);
}
