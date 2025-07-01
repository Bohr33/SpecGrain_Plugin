# SpecGrain Plugin
SpecGrain is VST/AU plugin built with the JUCE framework that explores various spectral processes. 
There are four main spectral processes employed in this plugin: pitch shifting, spectral delay, spectral blur, and a somewhat experimental process I call spectral stretch. As well as this, there is a simple gating function that helps eliminate some of the spectral noise.

###Controls
There are 8 dials to control the main parameters. 

 - **Pitch Shift:** controls the amount of pitch shifting as a raw floating point value. Range (0.0 - 3.0)
 - **Blur Amount:** Determines over how long a period spectal frames will be averaged to blur.
 - **Spectral Time:** Controls the length of time spectral frames will be held in a buffer.
 - **Spectral Density:** Controls how many frames will be read from the Stretch buffer in a single frame. 
 - **Gate Amount:** Determines the cutoff point for values to be zeroed. Spectral Amplitude values below this point will be set to 0.0.
 - **Delay Time:** Sets the delay time in milliseconds.
 - **Delay Amount:** Controls how much of the delay will be added to the input signal.
 - **Feedback:** Controls the delay feedback. A value of 1 is limited to 0.99.

As well as this, there are two extra controls

- **FFT Size:** This is a drop down menu that allows for changing the FFT size to 1 of 3 values. This was mostly included for testing purposes, however it may be useful for the user to change this to decrease latency. **Note**: There is currently a bug with this function for size 512, the hopSize doesn't change properly and so not enough overlaps are done per frame. Too many hopsizes 


### Processing Flow
The flow of the process is as follows...

Pitch Shift -> Stretch -> Gate -> Delay -> Blur




## Implementation
The plugin implements a phase vocoder via an STFT transform in order to create a custom structure called an Fsig

