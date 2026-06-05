/*
  ==============================================================================

    Graphics.h
    Created: 20 Dec 2025 9:38:39pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>



class MyLookAndFeel : public juce::LookAndFeel_V4
{
    
    
    
public:
    void drawRotarySlider(juce::Graphics &g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider & ) override;
    
    juce::Colour creamColour = juce::Colour(0xFFF0E7D5);
    juce::Colour darkBlueColour = juce::Colour(0xFF212842);
};


class BasicDialComponent : public juce::Component, public juce::Slider::Listener
{
public:
    
    BasicDialComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void setText(juce::String newText);
    
    void attach(juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID);
    
    juce::Slider dial;
    juce::Label nameLabel;
    juce::Label valueLabel;
    
    juce::Colour accentColour1 = juce::Colour(0xFFF0E7D5);
    

    
private:
    void sliderValueChanged(juce::Slider* slider) override;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};

