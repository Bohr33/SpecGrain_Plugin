/*
  ==============================================================================

    Window.h
    Created: 26 May 2026 11:48:13pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#pragma once
#include <stdio.h>
#include <vector>
#include <cmath>
#include <numbers>

constexpr float twoPi = 6.28318530717958647692f;

class Window
{
public:
    Window(size_t size);
    void makeWindow();
    float getValue(unsigned int index);
private:
    size_t windowSize;
    std::vector<float> window;
};
