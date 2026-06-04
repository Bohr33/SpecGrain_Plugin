/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PVExperimentsAudioProcessorEditor::PVExperimentsAudioProcessorEditor (PVExperimentsAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 400);
    
    pitchSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment (valueTreeState, "PITCH_SHIFT", pitchSlider));
    pitchSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    pitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    pitchSliderLabel.setText("Pitch Shift", juce::dontSendNotification);
    pitchSliderLabel.setJustificationType(juce::Justification::centred);
    
    blurSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "BLUR_AMOUNT", blurSlider));
    blurSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    blurSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    blurSliderLabel.setText("Blur Amount", juce::dontSendNotification);
    blurSliderLabel.setJustificationType(juce::Justification::centred);
    
    
    //Stretch Parameters
    stretchTimeSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "STRETCH_TIME", stretchTimeSlider));
    stretchTimeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    stretchTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    stretchTimeSliderLabel.setText("Stretch Time", juce::dontSendNotification);
    stretchTimeSliderLabel.setJustificationType(juce::Justification::centred);
    
    stretchDensitySliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "STRETCH_DENSITY", stretchDensitySlider));
    stretchDensitySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    stretchDensitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    stretchDensitySliderLabel.setText("Stretch Density", juce::dontSendNotification);
    stretchDensitySliderLabel.setJustificationType(juce::Justification::centred);
    
    //Delay Parameters
    delayAmtSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "DELAY_AMOUNT", delayAmtSlider));
    delayAmtSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    delayAmtSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    delayAmtSliderLabel.setText("Delay Amount", juce::dontSendNotification);
    delayAmtSliderLabel.setJustificationType(juce::Justification::centred);

    delayTimeSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "DELAY_TIME", delayTimeSlider));
    delayTimeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    delayTimeSliderLabel.setText("Delay Time", juce::dontSendNotification);
    delayTimeSliderLabel.setJustificationType(juce::Justification::centred);
    
    feedbackSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "DELAY_FEEDBACK", feedbackSlider));
    feedbackSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    feedbackLabel.setText("Feedback", juce::dontSendNotification);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    
    delayFreqButtonAttch.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "DELAY_FREQUENCY_TOGGLE", delayFreqButton));
    
    delayFreqButton.setToggleable(true);
    delayFreqButton.setClickingTogglesState(true);
    delayFreqButton.setButtonText("Toggle Freq Delay");
    
    
    //Gate Slider
    gateSliderAttch.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(valueTreeState, "GATE_AMOUNT", gateSlider));
    gateSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gateSlider.setRange(0.0f, 1.0f, 0.0001);
    gateSlider.setSkewFactorFromMidPoint(0.1);
    gateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    
    gateSliderLabel.setText("Gate Amount", juce::NotificationType::dontSendNotification);
    gateSliderLabel.setJustificationType(juce::Justification::centred);

    
    fftSizeMenu.addItem("512", 1);
    fftSizeMenu.addItem("1024", 2);
    fftSizeMenu.addItem("2048", 3);
    fftSizeMenu.setSelectedId(2);
    
    fftSizeLabel.setText("FFT Size", juce::NotificationType::dontSendNotification);
    
    fftSizeMenu.onChange = [this](){
        auto lastFFTSize = audioProcessor.fftSize;
        int newFFTSize;
        switch (fftSizeMenu.getSelectedId()) {
            case 1:
                newFFTSize = 512;
                break;
            case 2:
                newFFTSize = 1024;
                break;
            case 3:
                newFFTSize = 2048;
                break;
            default:
                newFFTSize = 1024;
                break;
        }
        
        if(newFFTSize != lastFFTSize)
        {
            audioProcessor.fftSizeChanged(newFFTSize);
        }
            
            
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
    
    addAndMakeVisible(&gateSlider);
    addAndMakeVisible(&gateSliderLabel);
    
    addAndMakeVisible(&fftSizeMenu);
    addAndMakeVisible(&fftSizeLabel);
    
    setLookAndFeel(&myLookAndFeel);
}

PVExperimentsAudioProcessorEditor::~PVExperimentsAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void PVExperimentsAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colour::fromRGB(13, 17, 22));
}

void PVExperimentsAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int width = getWidth();
    int height = getHeight();
    
    //Slider, Label dimensions
    int numSliders = 4;
    int sliderWidth = width / numSliders;
    int sliderHeight = 100;
    int labelHeight = 40;

    //Bottom Row
    int row1Y = height - sliderHeight - labelHeight;
    int rowX = 0;
    int buttonWidth = sliderWidth;
    int buttonHeight = labelHeight;
    
    //Extras
    int extrasY = 50;
    int extrasXPadding = 60;
    
    //Top Row Extras
    delayFreqButton.setBounds(width - (buttonWidth + extrasXPadding), extrasY + 20, buttonWidth, buttonHeight);
    fftSizeLabel.setBounds(extrasXPadding, extrasY, sliderWidth - 10, 30);
    fftSizeMenu.setBounds(extrasXPadding, extrasY + 30, sliderWidth - 10, 30);
    
    //Second Row
    int row2Y = row1Y - sliderHeight - labelHeight;
    
    pitchSlider.setBounds(rowX, row2Y, sliderWidth, sliderHeight);
    pitchSliderLabel.setBounds(rowX,  row2Y + sliderHeight, sliderWidth, labelHeight);
    
    blurSlider.setBounds(rowX + sliderWidth, row2Y, sliderWidth, sliderHeight);
    blurSliderLabel.setBounds(rowX + sliderWidth, row2Y + sliderHeight, sliderWidth, labelHeight);
    
    stretchDensitySlider.setBounds(rowX + sliderWidth * 2, row2Y, sliderWidth, sliderHeight);
    stretchDensitySliderLabel.setBounds(rowX + sliderWidth * 2, row2Y + sliderHeight, sliderWidth, labelHeight);
    
    stretchTimeSlider.setBounds(rowX + sliderWidth * 3, row2Y, sliderWidth, sliderHeight);
    stretchTimeSliderLabel.setBounds(rowX + sliderWidth * 3, row2Y + sliderHeight, sliderWidth, labelHeight);
    

    //Bottom Row Sliders
    gateSlider.setBounds(rowX, row1Y, sliderWidth, sliderHeight);
    gateSliderLabel.setBounds(rowX, row1Y + sliderHeight, sliderWidth, labelHeight);
    
    delayTimeSlider.setBounds(rowX + sliderWidth , row1Y, sliderWidth, sliderHeight);
    delayTimeSliderLabel.setBounds(rowX + sliderWidth, row1Y + sliderHeight, sliderWidth, labelHeight);
    
    delayAmtSlider.setBounds(rowX + 2 * sliderWidth , row1Y, sliderWidth, sliderHeight);
    delayAmtSliderLabel.setBounds(rowX + 2 * sliderWidth, row1Y + sliderHeight, sliderWidth, labelHeight);
    
    feedbackSlider.setBounds(rowX + 3 * sliderWidth, row1Y, sliderWidth, sliderHeight);
    feedbackLabel.setBounds(rowX + 3 * sliderWidth, row1Y + sliderHeight, sliderWidth, labelHeight);
    
}
