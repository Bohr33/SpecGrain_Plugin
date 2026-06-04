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
};
