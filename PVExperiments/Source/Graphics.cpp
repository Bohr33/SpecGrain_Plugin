/*
  ==============================================================================

    Graphics.cpp
    Created: 20 Dec 2025 9:38:39pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "Graphics.h"


MyLookAndFeel::MyLookAndFeel()
{
    //SetGlobal Colors
    setColour(juce::Label::textColourId, MyColours::accent1);
    
    
    myFont = juce::Typeface::createSystemTypefaceFor(BinaryData::ZenMaruGothicLight_ttf, BinaryData::ZenMaruGothicLight_ttfSize);
    
    
    setColour(juce::Slider::backgroundColourId, MyColours::accentC);
    
    setColour(juce::ComboBox::backgroundColourId, MyColours::accentA);
    setColour(juce::ComboBox::textColourId, MyColours::accent1);
    setColour(juce::ComboBox::outlineColourId, MyColours::accentB);
    setColour(juce::ComboBox::arrowColourId, MyColours::accentD);
    
    setColour(juce::PopupMenu::backgroundColourId,            MyColours::accentA);
    setColour(juce::PopupMenu::textColourId,                  MyColours::accent1);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, MyColours::accentC);
    setColour(juce::PopupMenu::highlightedTextColourId,       MyColours::creamColour);
    
    setColour(juce::TextButton::buttonColourId, MyColours::accentA);
    setColour(juce::TextButton::buttonOnColourId, MyColours::buttonRed);
    
    
}

void MyLookAndFeel::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    auto size = juce::jmin(width, height);
    float outlineThickness = 1.0f;
    
    auto bounds = juce::Rectangle<float>(x, y, width, height).withSizeKeepingCentre(size, size).reduced(outlineThickness, outlineThickness);
    auto centre = bounds.getCentre();
    auto radius = bounds.getWidth() / 2.0f;
    
    
    auto fillColour = MyColours::accentC;
    auto outlineColour = MyColours::accentD;
    auto indicatorColour = MyColours::accentA;
    
    g.setColour(fillColour);
    g.fillEllipse(bounds);

    // outline
    g.setColour(outlineColour);
    g.drawEllipse(bounds, outlineThickness);

    // line indicator
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    auto lineLength = radius * 0.5f;

    juce::Point<float> lineStart = centre.getPointOnCircumference(radius - lineLength, angle);
    juce::Point<float> lineEnd   = centre.getPointOnCircumference(radius, angle);

    g.setColour(indicatorColour);;
    g.drawLine({ lineStart, lineEnd },3.0f);
    
}


void MyLookAndFeel::drawButtonBackground(juce::Graphics &g, juce::Button &button, const juce::Colour &backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(2.0f); // inset so outline isn't clipped
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    auto circle = bounds.withSizeKeepingCentre(size, size);

    bool isOn = button.getToggleState();
    
    auto buttonStrokeSize = juce::jmap(size, 20.0f, 200.0f, 1.0f, 6.0f);
    
    auto onColour = button.findColour(juce::TextButton::buttonOnColourId).withAlpha(0.2f);

    // body
    g.setColour(isOn ? onColour : backgroundColour);
    g.fillEllipse(circle);
    
    
    // outline
    g.setColour(MyColours::accentD);
    g.drawEllipse(circle, buttonStrokeSize);

}

juce::Font MyLookAndFeel::getLabelFont(juce::Label& label)
{
    jassert(myFont != nullptr);
    
    return juce::Font(juce::FontOptions{}.withName(myFont->getName()).withStyle(myFont->getStyle()).withHeight(label.getHeight()));
}

juce::Font MyLookAndFeel::getComboBoxFont(juce::ComboBox& comboBox)
{
    return juce::Font(juce::FontOptions{}.withName(myFont->getName()).withStyle(myFont->getStyle()).withHeight(comboBox.getHeight()));
}

juce::Font MyLookAndFeel::getPopupMenuFont()
{
    return juce::Font(juce::FontOptions{}.withName(myFont->getName()).withStyle(myFont->getStyle()).withPointHeight(30));;
}









//Dial Component
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
//    titleBounds.reduce(titleBounds.getWidth()*(1-titleSizeFactor), 0.0f);
    titleBounds.removeFromTop(titleBounds.getHeight()*(1-titleSizeFactor));
    
    
    auto dialBounds = bounds.removeFromTop(halfHeight);
    int dialBoundsScaleX = dialBounds.getWidth() - (dialBounds.getWidth() * dialScaleFactor);
    int dialBoundsScaleY = dialBounds.getHeight() - dialBounds.getHeight() * dialScaleFactor;
    dialBounds.reduce(dialBoundsScaleX, dialBoundsScaleY);
    
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

void BasicDialComponent::setDialSize(float newSize)
{
    dialScaleFactor = newSize;
}

void BasicDialComponent::setTitleSize(float newSize)
{
    titleSizeFactor = newSize;
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
    textBounds.removeFromTop(textBounds.getHeight()*(1.0f-titleSizeFactor));
    
    
    int buttonPadTop = 10;
    
    bounds.removeFromTop(buttonPadTop);

    
    bounds.reduce(bounds.getWidth()/2.0f * (1.0f-buttonSize), 0);
    
    int minDim = juce::jmin(bounds.getWidth(), bounds.getHeight());
    if(isSquare)
        bounds.removeFromBottom(bounds.getHeight() - minDim);
    
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

void BasicToggleComponent::setButtonSizeAsFloat(float fractionOfAvailableSpace)
{
    buttonSize = fractionOfAvailableSpace;
}

void BasicToggleComponent::setTitleSize(float newSize)
{
    titleSizeFactor = newSize;
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
    auto titleBounds = getLocalBounds().reduced(padding_sides, 0).removeFromTop(getHeight() - padding_bottom);;
    
    auto halfWidth = getWidth()/2.0f;
    auto halfHeight = getHeight()/2.0f;
    
//    auto fontReduceX = halfWidth - halfWidth * fontProportion;
//    auto fontReduceY = halfHeight - halfHeight * fontProportion;
    
    titleBounds.removeFromRight(static_cast<int>(titleBounds.getWidth() * (1.0 - fontProportion)));
//    titleBounds.removeFromTop(static_cast<int>(titleBounds.getHeight() * (1.0 - fontProportion)));
//    titleBounds.reduce(0.0f, titleBounds.getHeight()/2.0f * (1.0f - fontProportion));
    titleBounds.removeFromBottom(static_cast<int>(titleBounds.getHeight() * (1.0 - fontProportion)));
    
//    titleBounds.reduce(fontReduceX, fontReduceY);
    
    m_titleBounds = titleBounds;
    
    titleLabel.setBounds(titleBounds);
}

void TitleWithUnderline::setUnderlineHeightDelta(int newValue)
{
    lineDelta = newValue;
}

void TitleWithUnderline::paint(juce::Graphics& g)
{
    if(showUnderline)
    {
        auto bounds = getLocalBounds().toFloat();
        
        bounds.removeFromBottom(padding_bottom);
        
        auto& laf = getLookAndFeel();
        auto textColour = laf.findColour(juce::Label::textColourId);
        
        g.setColour(textColour);
        
        
        g.drawLine(m_titleBounds.getX(), m_titleBounds.getBottom() + lineDelta, getLocalBounds().getRight(), m_titleBounds.getBottom() + lineDelta, 1.0f);
    }
}

void TitleWithUnderline::toggleUnderline(bool newDecision)
{
    showUnderline = newDecision;
}

void TitleWithUnderline::setFontSizeAsProportionOfSpace(float newValue)
{
    fontProportion = newValue;
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
    
    
    int headerPadH = 0;
    int headerPadV = 0;
    
    auto bounds = getLocalBounds().reduced(headerPadH, headerPadV);
    
    auto header = bounds.removeFromTop(getHeight()/4.0f);
    
    auto titleBounds = header;
    
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
                        .withMargin(0.0f));
    }

    fb.performLayout(bounds);
    
}

void GUICollection::paint(juce::Graphics& g)
{
    if (showOutline) {
        auto& laf = getLookAndFeel();
        
        g.setColour(laf.findColour(juce::Label::textColourId));
        g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f), 0.0f, 0.5f);

    }
}


