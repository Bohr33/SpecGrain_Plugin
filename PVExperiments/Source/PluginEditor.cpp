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
    
    pitchSlider.attach(valueTreeState, "PITCH_SHIFT");
    blurSlider.attach(valueTreeState, "BLUR_AMOUNT");

    stretchTimeSlider.attach(valueTreeState, "STRETCH_TIME");
    stretchDensitySlider.attach(valueTreeState, "STRETCH_DENSITY");

    delayAmtSlider.attach(valueTreeState, "DELAY_AMOUNT");
    delayTimeSlider.attach(valueTreeState, "DELAY_TIME");
    feedbackSlider.attach(valueTreeState, "DELAY_FEEDBACK");
    gateSlider.attach(valueTreeState, "GATE_AMOUNT");

    delayFreqButton.attach(valueTreeState, "DELAY_FREQUENCY_TOGGLE");
    
    delayFreqButton.setButtonSizeAsFloat(0.5);
    
    
    //Add Slider components to collections
    stretchControls.addComponent(stretchTimeSlider);
    stretchControls.addComponent(stretchDensitySlider);
    
    delayControls.addComponent(delayTimeSlider);
    delayControls.addComponent(feedbackSlider);
    delayControls.addComponent(delayAmtSlider);
    delayControls.addComponent(delayFreqButton);
    
    pitchControls.addComponent(pitchSlider);
    blurControls.addComponent(blurSlider);
    gateControls.addComponent(gateSlider);

    
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
    
        
    
    Title.setText("PVExperiments");
    
    stretchTimeSlider.setText("Time");
    stretchDensitySlider.setText("Density");
    delayAmtSlider.setText("Delay Amt");
    delayTimeSlider.setText("Delay Time");
    feedbackSlider.setText("Feedback");
    
//    pitchSlider.setText("Pitch Shift");
//    blurSlider.setText("Blur");
//    gateSlider.setText("Gate");

    delayFreqButton.setText("Freq Toggle");
    
    
    //Set Collection Names and Make Visible
    stretchControls.setCollectionTitle("Stretch");
    stretchControls.title.setUnderlineHeightDelta(-5);
    addAndMakeVisible(stretchControls);
    
    
    delayControls.setCollectionTitle("Delay");
    delayControls.title.setUnderlineHeightDelta(-5);
    addAndMakeVisible(delayControls);
    
    
    pitchControls.setCollectionTitle("Pitch Shift");
    pitchControls.title.setUnderlineHeightDelta(-5);
    addAndMakeVisible(pitchControls);
    
    blurControls.setCollectionTitle("Blur");
    blurControls.title.setUnderlineHeightDelta(-5);
    addAndMakeVisible(blurControls);
    
    gateControls.setCollectionTitle("Gate");
    gateControls.title.setUnderlineHeightDelta(-5);
    addAndMakeVisible(gateControls);
    
    //FFT Hangovers
    addAndMakeVisible(&fftSizeMenu);
    addAndMakeVisible(&fftSizeLabel);
    
    //Title
    addAndMakeVisible(&Title);
    
    setLookAndFeel(&myLookAndFeel);
    
    setSize (1000, 312.5);
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

    int globalSidePadding = 15;
    int globalTBPadding = 10;
    int headerHeight = 90;
    
    auto bounds = getLocalBounds();
    bounds.reduce(globalSidePadding, globalTBPadding);
    
    auto header = bounds.removeFromTop(headerHeight);
    
    Title.padding_sides = 7.0f;
    Title.padding_bottom = 20.0f;
    Title.setBounds(header);
    
    //GUICollectoion Outline
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::row;
    fb.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    fb.alignItems = juce::FlexBox::AlignItems::stretch;

    float gap = 8.0f; // adjust to taste

    fb.items.add(juce::FlexItem(stretchControls).withFlex(2.0f).withMargin(gap));
    fb.items.add(juce::FlexItem(pitchControls)  .withFlex(1.0f).withMargin(gap));
    fb.items.add(juce::FlexItem(gateControls)   .withFlex(1.0f).withMargin(gap));
    fb.items.add(juce::FlexItem(delayControls)  .withFlex(4.0f).withMargin(gap));
    fb.items.add(juce::FlexItem(blurControls)   .withFlex(1.0f).withMargin(gap));

    fb.performLayout(bounds.toFloat());
 
}
