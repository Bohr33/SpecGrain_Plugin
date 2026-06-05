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
    setSize (800, 250);
    
    
    pitchSlider.attach(valueTreeState, "PITCH_SHIFT");
    blurSlider.attach(valueTreeState, "BLUR_AMOUNT");

    stretchTimeSlider.attach(valueTreeState, "STRETCH_TIME");
    stretchDensitySlider.attach(valueTreeState, "STRETCH_DENSITY");

    delayAmtSlider.attach(valueTreeState, "DELAY_AMOUNT");
    delayTimeSlider.attach(valueTreeState, "DELAY_TIME");
    feedbackSlider.attach(valueTreeState, "DELAY_FEEDBACK");
    gateSlider.attach(valueTreeState, "GATE_AMOUNT");

    delayFreqButton.attach(valueTreeState, "DELAY_FREQUENCY_TOGGLE");

    
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
    
    delayFreqButton.setText("Freq Toggle");
    
    Title.setText("PVExperiments");
    addAndMakeVisible(&delayFreqButton);
    
    
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
    
    addAndMakeVisible(&Title);
    
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

    int headerHeight = 90;
    
    auto bounds = getLocalBounds();
    auto eigthWidth = width/9.0f;
    
    auto header = bounds.removeFromTop(headerHeight);
    
    Title.padding_sides = 7.0f;
    Title.padding_bottom = 20.0f;
    Title.setBounds(header);
    
    

    pitchSlider.setBounds(bounds.removeFromLeft(eigthWidth));
    blurSlider.setBounds(bounds.removeFromLeft(eigthWidth));
    stretchTimeSlider.setBounds(bounds.removeFromLeft(eigthWidth));
    stretchDensitySlider.setBounds(bounds.removeFromLeft(eigthWidth));
    delayAmtSlider.setBounds(bounds.removeFromLeft(eigthWidth));
    delayTimeSlider.setBounds(bounds.removeFromLeft(eigthWidth));
    feedbackSlider.setBounds(bounds.removeFromLeft(eigthWidth));
    delayFreqButton.setBounds(bounds.removeFromLeft(eigthWidth));
    gateSlider.setBounds(bounds.removeFromLeft(eigthWidth));
 
}
