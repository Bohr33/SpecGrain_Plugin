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


class BasicToggleComponent : public juce::Component
{
public:
    
    BasicToggleComponent();
    
    void attach(juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID);
    
    void setText(juce::String newButtonText);
    
    void resized() override;
    
    juce::TextButton button;
    juce::Label  textLabel;
    
private:
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;
    
};




class TitleWithUnderline : public juce::Component
{
public:
    
    TitleWithUnderline();
    juce::Label titleLabel;
    
    void resized() override;
    void paint(juce::Graphics& g) override;
    void setText(juce::String newTitle);
    
    int padding_bottom = 0.0f;
    int padding_sides = 0.0f;
};




class GUICollection :  public juce::Component
{
public:
    
    GUICollection();
    
    void setCollectionTitle(juce::String newTitle);
    
    void addComponent(juce::Component& component);
    
    //Probably Don't need these any more
    void addDial(BasicDialComponent& dial);
    void addToggle(BasicToggleComponent& toggle);
    
    
    void resized() override;
    void paint(juce::Graphics& g) override;
    
    
private:
    TitleWithUnderline title;
    
    std::vector<juce::Component*> controls;
};

