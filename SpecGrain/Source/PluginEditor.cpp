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
    
    stretchSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "stretchAmt", stretchSlider));
    stretchSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    stretchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    stretchSliderLabel.setText("Stretch Amount", juce::dontSendNotification);
    stretchSliderLabel.setJustificationType(juce::Justification::centred);
    
    //Delay Parameters
    
    delayAmtSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "delayAmt", delayAmtSlider));
    delayAmtSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    delayAmtSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    delayAmtSliderLabel.setText("Delay Amount", juce::dontSendNotification);
    delayAmtSliderLabel.setJustificationType(juce::Justification::centred);

    delayTimeSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "delayTime", delayTimeSlider));
    delayTimeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    delayTimeSliderLabel.setText("Delay Time", juce::dontSendNotification);
    delayTimeSliderLabel.setJustificationType(juce::Justification::centred);
    
    feedbackSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "feedback", feedbackSlider));
    feedbackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(&pitchSlider);
    addAndMakeVisible(&pitchSliderLabel);
    addAndMakeVisible(&blurSlider);
    addAndMakeVisible(&blurSliderLabel);
    addAndMakeVisible(&stretchSlider);
    addAndMakeVisible(&stretchSliderLabel);
    addAndMakeVisible(&delayAmtSlider);
    addAndMakeVisible(&delayAmtSliderLabel);
    addAndMakeVisible(&delayTimeSlider);
    addAndMakeVisible(&delayTimeSliderLabel);
    
    addAndMakeVisible(&feedbackSlider);
    addAndMakeVisible(&feedbackLabel);
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
    
    int numSliders = 4;
    int sliderWidth = width / numSliders;
    int sliderHeight = 100;
    int labelHeight = 40;
    
    int sliderGroupWidth = sliderWidth * numSliders;
    
    int topY = 0;
    int topX = width / 2 - sliderWidth;
    
    pitchSlider.setBounds(topX, topY, sliderWidth, sliderHeight);
    pitchSliderLabel.setBounds(topX,  topY + sliderHeight, sliderWidth, labelHeight);
    
    blurSlider.setBounds(topX + sliderWidth, topY, sliderWidth, sliderHeight);
    blurSliderLabel.setBounds(topX + sliderWidth, topY + sliderHeight, sliderWidth, labelHeight);
    
    
    //Delay Group
    int delayHeights = height - sliderHeight - labelHeight;
    int delayX = 0;
    
    delayTimeSlider.setBounds(delayX, delayHeights, sliderWidth, sliderHeight);
    delayTimeSliderLabel.setBounds(delayX, delayHeights + sliderHeight, sliderWidth, labelHeight);
    
    stretchSlider.setBounds(delayX + sliderWidth , delayHeights, sliderWidth, sliderHeight);
    stretchSliderLabel.setBounds(delayX + sliderWidth, delayHeights + sliderHeight, sliderWidth, labelHeight);
    
    delayAmtSlider.setBounds(delayX + 2 * sliderWidth , delayHeights, sliderWidth, sliderHeight);
    delayAmtSliderLabel.setBounds(delayX + 2 * sliderWidth, delayHeights + sliderHeight, sliderWidth, labelHeight);
    
    feedbackSlider.setBounds(delayX + 3 * sliderWidth, delayHeights, sliderWidth, sliderHeight);
    feedbackLabel.setBounds(delayX + 3 * sliderWidth, delayHeights + sliderHeight, sliderWidth, labelHeight);
    
}
