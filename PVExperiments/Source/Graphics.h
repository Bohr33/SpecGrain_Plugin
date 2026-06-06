/*
  ==============================================================================

    Graphics.h
    Created: 20 Dec 2025 9:38:39pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace MyColours
{
const juce::Colour background   { 0xFFF0E7D5 };
const juce::Colour accent1      { 0xFF000000 };
const juce::Colour creamColour  { 0xFFF0E7D5 };
const juce::Colour darkBlueColour { 0xFF212842 };
}

class MyLookAndFeel : public juce::LookAndFeel_V4
{
public:
    
    MyLookAndFeel();
    
    void drawRotarySlider(juce::Graphics &g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider & ) override;
    
    
    juce::Font getLabelFont(juce::Label& label) override;
    
    juce::Font getMyFont();
    

    
    
private:
    juce::Typeface::Ptr myFont;
};


class BasicDialComponent : public juce::Component, public juce::Slider::Listener
{
public:
    
    BasicDialComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void setText(juce::String newText);
    void setDialSize(float newSize);
    void setTitleSize(float newSize);
    
    void attach(juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID);
    
    juce::Slider dial;
    juce::Label nameLabel;
    juce::Label valueLabel;
    
    juce::Colour accentColour1 = juce::Colour(0xFFF0E7D5);
    
    bool showOutline = false;
    

    
private:
    void sliderValueChanged(juce::Slider* slider) override;
    
    float dialScaleFactor = 0.9f;
    float titleSizeFactor = 0.8f;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};


class BasicToggleComponent : public juce::Component
{
public:
    
    BasicToggleComponent();
    
    void attach(juce::AudioProcessorValueTreeState& apvts, const juce::String& paramID);
    
    void setText(juce::String newButtonText);
    
    void setButtonSizeAsFloat(float fractionOfAvailableSpace);
    
    void setTitleSize(float newSize);
    
    
    void resized() override;
    
    juce::TextButton button;
    juce::Label  textLabel;
    
private:
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;
    bool isSquare = true;
    float buttonSize = 0.5f;
    float titleSizeFactor = 0.8f;
    
};






class TitleWithUnderline : public juce::Component
{
public:
    
    TitleWithUnderline();
    juce::Label titleLabel;
    
    void resized() override;
    void paint(juce::Graphics& g) override;
    void setText(juce::String newTitle);
    void setUnderlineHeightDelta(int newValue);
    void toggleUnderline(bool newDecision);
    void setFontSizeAsProportionOfSpace(float newValue);
    
    
    int padding_bottom = 0.0f;
    int padding_sides = 0.0f;
    
private:
    
    bool showUnderline = true;
    int lineDelta = 0;
    float fontProportion = 0.9f;
    
    
    juce::Rectangle<int> m_titleBounds;
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
    
    bool showOutline = false;
    
    TitleWithUnderline title;
    
private:
    
    
    juce::Rectangle<int> m_titleBounds;
    
    std::vector<juce::Component*> controls;
};

