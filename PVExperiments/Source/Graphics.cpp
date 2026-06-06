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

}


void BasicDialComponent::paint(juce::Graphics &g)
{
    if (showOutline) {
        g.setColour(accentColour1);
        g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 4.0f, 1.0f);
    }
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
    button.setBounds(bounds.reduced(bounds.getWidth()/2.0f * buttonSize));
    
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

void BasicToggleComponent::setButtonSizeAsFloat(float fractionOfAvailableSpace)
{
    buttonSize = fractionOfAvailableSpace;
}




//TItle/Underline Class
TitleWithUnderline::TitleWithUnderline()
{
    titleLabel.setJustificationType(juce::Justification::left);
    titleLabel.setBorderSize(juce::BorderSize<int>(0));
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
    
    m_titleBounds = titleBounds;
    
    titleLabel.setBounds(titleBounds);
}

void TitleWithUnderline::setUnderlineHeightDelta(int newValue)
{
    lineDelta = newValue;
}

void TitleWithUnderline::paint(juce::Graphics& g)
{
    
    auto bounds = getLocalBounds().toFloat();
    
    bounds.removeFromBottom(padding_bottom);
    
    g.setColour(juce::Colours::white);
    
    
    auto sidePadding = 8.0f;
    auto bottomPadding = 5.0f;
    
//    g.drawLine(bounds.getX() + sidePadding + padding_sides, bounds.getBottom() - bottomPadding, bounds.getRight() - (sidePadding + padding_sides), bounds.getBottom() - bottomPadding, 1.0f);
    
    
    g.drawLine(m_titleBounds.getX(), m_titleBounds.getBottom() + lineDelta, m_titleBounds.getRight(), m_titleBounds.getBottom() + lineDelta, 1.0f);
}


//GUICollection
GUICollection::GUICollection()
{
    addAndMakeVisible(title);
}

void GUICollection::setCollectionTitle(juce::String newTitle)
{
    title.setText(newTitle);
}

void GUICollection::addDial(BasicDialComponent &dial)
{
    DBG("Added Dial");
//    dials.push_back(&dial);
    controls.push_back(&dial);
    addAndMakeVisible(dial);
}

void GUICollection::addToggle(BasicToggleComponent &toggle)
{
    DBG("Added toggle");
//    toggles.push_back(&toggle);
    controls.push_back(&toggle);
    addAndMakeVisible(toggle);
}

void GUICollection::addComponent(juce::Component &component)
{
    controls.push_back(&component);
    addAndMakeVisible(component);
}


void GUICollection::resized()
{
    auto bounds = getLocalBounds();
    
    int headerPadH = 10;
    int headerPadV = 0;
    
    auto header = bounds.removeFromTop(getHeight()/4.0f);
    
    auto titleBounds = header.reduced(headerPadH, headerPadV);
    
    title.setBounds(titleBounds);
    
    

    //Flex box for componenets
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::row;
    fb.justifyContent = juce::FlexBox::JustifyContent::center;
    fb.alignItems = juce::FlexBox::AlignItems::center;

    for (auto* control : controls)
    {
        fb.items.add(juce::FlexItem(*control)
                        .withFlex(1.0f)
                        .withHeight(bounds.getHeight())
                        .withMargin(1.0f));
    }

    fb.performLayout(bounds);
    
}

void GUICollection::paint(juce::Graphics& g)
{
    if (showOutline) {
        g.setColour(accentColour1);
        g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 0.0f, 0.5f);

    }
}


