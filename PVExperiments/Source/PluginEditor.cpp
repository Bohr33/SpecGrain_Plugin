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
    
    
    pitchSlider.attach(valueTreeState, "PITCH_SHIFT");
    blurSlider.attach(valueTreeState, "BLUR_AMOUNT");

    stretchTimeSlider.attach(valueTreeState, "STRETCH_TIME");
    stretchDensitySlider.attach(valueTreeState, "STRETCH_DENSITY");

    delayAmtSlider.attach(valueTreeState, "DELAY_AMOUNT");
    delayTimeSlider.attach(valueTreeState, "DELAY_TIME");
    feedbackSlider.attach(valueTreeState, "DELAY_FEEDBACK");
    gateSlider.attach(valueTreeState, "GATE_AMOUNT");

    
    //Delay Button
    delayFreqButtonAttch.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "DELAY_FREQUENCY_TOGGLE", delayFreqButton));
    
    delayFreqButton.setToggleable(true);
    delayFreqButton.setClickingTogglesState(true);
    delayFreqButton.setButtonText("Toggle Freq Delay");
    
    //FFT Menu
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
    
    
    
    
//    pitchSlider.setColour(juce::Slider::rotarySliderFillColourId, myLookAndFeel.creamColour);
    
    pitchSlider.setText("Pitch Shift");
    blurSlider.setText("Blur");
    stretchTimeSlider.setText("Time");
    stretchDensitySlider.setText("Density");
    delayAmtSlider.setText("Delay Amt");
    delayTimeSlider.setText("Delay Time");
    feedbackSlider.setText("Feedback");
    gateSlider.setText("Gate");
    
    
    addAndMakeVisible(&pitchSlider);
    addAndMakeVisible(&blurSlider);
    addAndMakeVisible(&stretchTimeSlider);
    addAndMakeVisible(&stretchDensitySlider);
    addAndMakeVisible(&delayAmtSlider);
    addAndMakeVisible(&delayTimeSlider);
    addAndMakeVisible(&feedbackSlider);
    addAndMakeVisible(&delayFreqButton);
    addAndMakeVisible(&gateSlider);
    
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
    
    
    pitchSlider.setBounds(rowX, row2Y, sliderWidth, sliderHeight+labelHeight);
    
    blurSlider.setBounds(rowX + sliderWidth, row2Y, sliderWidth, sliderHeight+labelHeight);
    
    stretchDensitySlider.setBounds(rowX + sliderWidth * 2, row2Y, sliderWidth, sliderHeight+labelHeight);

    
    stretchTimeSlider.setBounds(rowX + sliderWidth * 3, row2Y, sliderWidth, sliderHeight+labelHeight);

    

    //Bottom Row Sliders
    gateSlider.setBounds(rowX, row1Y, sliderWidth, sliderHeight+labelHeight);

    
    delayTimeSlider.setBounds(rowX + sliderWidth , row1Y, sliderWidth, sliderHeight+labelHeight);

    
    delayAmtSlider.setBounds(rowX + 2 * sliderWidth , row1Y, sliderWidth, sliderHeight+labelHeight);

    
    feedbackSlider.setBounds(rowX + 3 * sliderWidth, row1Y, sliderWidth, sliderHeight+labelHeight);

    
}
