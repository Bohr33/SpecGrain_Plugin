/*
  ==============================================================================

    Graphics.cpp
    Created: 20 Dec 2025 9:38:39pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "Graphics.h"

void MyLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    auto size = juce::jmin(width, height);
    auto bounds = juce::Rectangle<float>(x, y, width, height).withSizeKeepingCentre(size, size);
    auto centre = bounds.getCentre();
    auto radius = bounds.getWidth() / 2.0f;

    
    
    // background circle
    auto backgroundColor = slider.findColour(juce::Slider::rotarySliderFillColourId);
    
    g.setColour(backgroundColor);
    g.fillEllipse(bounds);

    // outer ring
    g.setColour(juce::Colour(0xff444450));
    g.drawEllipse(bounds, 1.0f);

    // line indicator
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    auto lineLength = radius * 0.5f;

    juce::Point<float> lineStart = centre.getPointOnCircumference(radius - lineLength, angle);
    juce::Point<float> lineEnd   = centre.getPointOnCircumference(radius, angle);

    g.setColour(juce::Colours::white);
    g.drawLine({ lineStart, lineEnd },3.0f);
    
}





BasicDialComponent::BasicDialComponent()
{
    dial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    dial.setTextBoxStyle(juce::Slider::NoTextBox, false, 60, 20);
    
    dial.addListener(this);
    
    nameLabel.setJustificationType(juce::Justification::centred);
    
    valueLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(nameLabel);
    addAndMakeVisible(dial);
    addAndMakeVisible(valueLabel);
    
    
    
//    pitchSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
//    pitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
//
//    pitchSliderLabel.setText("Pitch Shift", juce::dontSendNotification);
//    pitchSliderLabel.setJustificationType(juce::Justification::centred);
    
    
}


void BasicDialComponent::paint(juce::Graphics &g)
{
    g.setColour(accentColour1);
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 4.0f, 1.0f);
    
}


void BasicDialComponent::resized()
{
    auto bounds = getLocalBounds();
    auto totalHeight = bounds.getHeight();
    auto quarterHeight = totalHeight * 0.25f;
    auto halfHeight = totalHeight * 0.5;
    
    auto titleBounds = bounds.removeFromTop(quarterHeight);
    auto dialBounds = bounds.removeFromTop(halfHeight);
    
    
    nameLabel.setBounds(titleBounds);
    dial.setBounds(dialBounds.toNearestInt());
    valueLabel.setBounds(bounds);
}

void BasicDialComponent::setText(juce::String newText)
{
    nameLabel.setText(newText, juce::dontSendNotification);
}


void BasicDialComponent::sliderValueChanged(juce::Slider* slider)
{
    valueLabel.setText(juce::String::formatted("%.2f", slider->getValue()), juce::dontSendNotification);
}


void BasicDialComponent::attach(juce::AudioProcessorValueTreeState &apvts, const juce::String& paramID)
{
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts, paramID, dial
    );
    
    valueLabel.setText(juce::String(dial.getValue(), 2), juce::dontSendNotification);
}




//Toggle Class
BasicToggleComponent::BasicToggleComponent()
{
    button.setToggleable(true);
    button.setClickingTogglesState(true);
    textLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(textLabel);
    addAndMakeVisible(button);
    
};

void BasicToggleComponent::resized()
{
    auto bounds = getLocalBounds();
    auto textBounds = bounds.removeFromTop(getHeight() * 0.25);
    
    textLabel.setBounds(textBounds);
    button.setBounds(bounds);
    
}


void BasicToggleComponent::setText(juce::String newButtonText)
{
    textLabel.setText(newButtonText, juce::dontSendNotification);
}

void BasicToggleComponent::attach(juce::AudioProcessorValueTreeState &apvts, const juce::String &paramID)
{
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, paramID, button
    );
    
}



TitleWithUnderline::TitleWithUnderline()
{
    titleLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(titleLabel);
}

void TitleWithUnderline::setText(juce::String newTitle)
{
    titleLabel.setText(newTitle, juce::dontSendNotification);
}

void TitleWithUnderline::resized()
{
    titleLabel.setFont(juce::FontOptions(getHeight()* 0.5f));
    
    auto titleBounds = getLocalBounds().reduced(padding_sides, 0).removeFromTop(getHeight() - padding_bottom);;
    
    titleLabel.setBounds(titleBounds);
}

void TitleWithUnderline::paint(juce::Graphics& g)
{
    
    auto bounds = getLocalBounds().toFloat();
    
    bounds.removeFromBottom(padding_bottom);
    
    g.setColour(juce::Colours::white);
    
    
    auto sidePadding = 8.0f;
    auto bottomPadding = 5.0f;
    
    g.drawLine(bounds.getX() + sidePadding + padding_sides, bounds.getBottom() - bottomPadding, bounds.getRight() - (sidePadding + padding_sides), bounds.getBottom() - bottomPadding, 1.0f);
}



