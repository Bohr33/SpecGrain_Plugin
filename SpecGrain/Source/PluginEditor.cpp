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
    
    
    //Stretch Parameters
    stretchTimeSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "stretchTime", stretchTimeSlider));
    stretchTimeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    stretchTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    stretchTimeSliderLabel.setText("Stretch Time", juce::dontSendNotification);
    stretchTimeSliderLabel.setJustificationType(juce::Justification::centred);
    
    stretchDensitySliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "stretchDensity", stretchDensitySlider));
    stretchDensitySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    stretchDensitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    stretchDensitySliderLabel.setText("Stretch Density", juce::dontSendNotification);
    stretchDensitySliderLabel.setJustificationType(juce::Justification::centred);
    
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
    
    delayFreqButtonAttch.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "delayFreqToggle", delayFreqButton));
    
    delayFreqButton.setToggleable(true);
    delayFreqButton.setClickingTogglesState(true);
    delayFreqButton.setButtonText("Toggle Freq Delay");
    
    fftSizeMenu.addItem("512", 1);
    fftSizeMenu.addItem("1024", 2);
    fftSizeMenu.addItem("2048", 3);
    fftSizeMenu.setSelectedId(2);
    
    fftSizeMenu.onChange = [this](){
        switch (fftSizeMenu.getSelectedId()) {
            case 1:
                audioProcessor.fftSize = 512;
                break;
            case 2:
                audioProcessor.fftSize = 1024;
                break;
            case 3:
                audioProcessor.fftSize = 2048;
                DBG("2048 selected");
                break;
            default:
                audioProcessor.fftSize = 1024;
                break;
        }
        processor.prepareToPlay(processor.getSampleRate(), processor.getBlockSize());
    };
    
    
    addAndMakeVisible(&pitchSlider);
    addAndMakeVisible(&pitchSliderLabel);
    addAndMakeVisible(&blurSlider);
    addAndMakeVisible(&blurSliderLabel);
    addAndMakeVisible(&stretchTimeSlider);
    addAndMakeVisible(&stretchTimeSliderLabel);
    addAndMakeVisible(&stretchDensitySlider);
    addAndMakeVisible(&stretchDensitySliderLabel);
    addAndMakeVisible(&delayAmtSlider);
    addAndMakeVisible(&delayAmtSliderLabel);
    addAndMakeVisible(&delayTimeSlider);
    addAndMakeVisible(&delayTimeSliderLabel);
    
    addAndMakeVisible(&feedbackSlider);
    addAndMakeVisible(&feedbackLabel);
    
    addAndMakeVisible(&delayFreqButton);
    
    addAndMakeVisible(&fftSizeMenu);
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
    
    int topY = 0;
    int topX = width / 2 - sliderWidth;
    
    pitchSlider.setBounds(topX, topY, sliderWidth, sliderHeight);
    pitchSliderLabel.setBounds(topX,  topY + sliderHeight, sliderWidth, labelHeight);
    
    blurSlider.setBounds(topX + sliderWidth, topY, sliderWidth, sliderHeight);
    blurSliderLabel.setBounds(topX + sliderWidth, topY + sliderHeight, sliderWidth, labelHeight);
    
    stretchDensitySlider.setBounds(topX + sliderWidth * 2, topY, sliderWidth, sliderHeight);
    stretchDensitySliderLabel.setBounds(topX + sliderWidth * 2, topY + sliderHeight, sliderWidth, labelHeight);
    
    
    //Delay Group
    int delayY = height - sliderHeight - labelHeight;
    int delayX = 0;
    int buttonWidth = sliderWidth;
    int buttonHeight = labelHeight;
    
    delayTimeSlider.setBounds(delayX, delayY, sliderWidth, sliderHeight);
    delayTimeSliderLabel.setBounds(delayX, delayY + sliderHeight, sliderWidth, labelHeight);
    
    stretchTimeSlider.setBounds(delayX + sliderWidth , delayY, sliderWidth, sliderHeight);
    stretchTimeSliderLabel.setBounds(delayX + sliderWidth, delayY + sliderHeight, sliderWidth, labelHeight);
    
    delayAmtSlider.setBounds(delayX + 2 * sliderWidth , delayY, sliderWidth, sliderHeight);
    delayAmtSliderLabel.setBounds(delayX + 2 * sliderWidth, delayY + sliderHeight, sliderWidth, labelHeight);
    
    feedbackSlider.setBounds(delayX + 3 * sliderWidth, delayY, sliderWidth, sliderHeight);
    feedbackLabel.setBounds(delayX + 3 * sliderWidth, delayY + sliderHeight, sliderWidth, labelHeight);
    
    delayFreqButton.setBounds(delayX, delayY - buttonHeight, buttonWidth, buttonHeight);
    
    fftSizeMenu.setBounds(5, 5, sliderWidth - 10, 30);
    
}
