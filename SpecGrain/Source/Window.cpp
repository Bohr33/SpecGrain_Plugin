/*
  ==============================================================================

    Window.cpp
    Created: 26 May 2026 11:48:13pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "Window.h"

Window::Window(size_t size) : windowSize(size)
{
    window.resize(windowSize);
    makeWindow();
}

void Window::makeWindow()
{
    for (int i = 0; i < windowSize; ++i)
        window[i] = 0.5 * (1 - std::cos((twoPi * i)/(windowSize-1)));
}

float Window::getValue(unsigned int index)
{
    return window[index];
}
