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
    auto bounds = juce::Rectangle<float>(x, y, width, height).withSizeKeepingCentre(width, width).reduced(20.0, 20.0);
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
