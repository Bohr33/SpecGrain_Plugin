# SpecGrain Plugin
SpecGrain is VST/AU plugin built with the JUCE framework that explores various spectral processes. 
There are four main spectral processes employed in this plugin: pitch shifting, spectral delay, spectral blur, and a somewhat experimental process I call spectral stretch. As well as this, there is a simple gating function that helps eliminate some of the spectral noise that occurs from combining certain effects.

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

- **FFT Size:** This is a drop down menu that allows for changing the FFT size to 1 of 3 values. This was mostly included for testing purposes, however it may be useful for the user to change this to decrease latency.

- **Frequency Delay Toggle:** This toggles the ability to delay the Fsig frequnecy values as well as the amplitude values. I thought both versions were interesting so I included a toggle.


### Processing Flow
The flow of the process is as follows...

Pitch Shift -> Stretch -> Gate -> Delay -> Blur

## Implementation

### PhaseVocoder
The plugin implements a phase vocoder via an STFT transform in order to create a custom structure called an Fsig, which holds the amplitude and frequency values in vectors of FFT Size/2 + 1. These operations are contained within the PhaseVocoder.cpp and header files. Many buffers were needed to manage all of the data properly and these are contained as std::vectors in the phase vocoder file. They are resized and managed within the prepare functions, which allow them to be changed when a new buffer size, fft size, or sample rate are changed. The process function receives the audio buffer from the audio processor process function, and most of the operations happen in there.

### Spectral Processes
The spectral processes are contained in the SpectralProcessors.cpp and header file. These are relatively simple as far as classes, and mostly only implement their own prepare function, and processing function. The phase vocoder function contains instances of each of these processing objects and calls their prepare and process functions in the respective locations in the phase vocoder class. 

The spectral stretch processor came from wanting to implement a time stretching algorithm. Originally, I had created a time stretching algorithm. This worked by holding a circular buffer of input frames, then read them back at a different rate. However, this posed issues for real-time processing since the read and write pointers were at different rates. So instead, I tried implementing a somewhat similar idea by holding a buffer of input samples (determined by stretch time), then at each new frame, read through each of the held samples via new read pointer that read slower than the write pointer. This became very computationally expensive and only made a filtering mess, so instead, I changed the stretch amount parameter to a stretch time parameter which jumps through the saved buffer and reads frames based on a density factor, creating more of a granular process.


### GUI Parameters
The GUI objects are added in the standard JUCE way within the Plugin Editor class. The parameters are declared within a Juce::AudioValueTreeState located in the processing class, and this is passed to the Phase Vocoder for the values to be loaded. The parameters are stored within atomics for thread safety, and on each frame process, they are loaded and passed to their respective spectral processes.


###Notes
This plugin was created mostly as an exercise to implement spectral processing via the phase vocoder. Once this was working, I wanted to implement some spectral processing algorithms to make use of it. Although it works, there are some serious optimizations that need to be made for future use. CPU usage gets near the maximum on my computer for stereo processing and buffer size of 512 when effects are maxed. This is mostly due to the spectral stretch and blur effects which need to read through many Fsig buffers in a single frame. As well as this, the Phase Vocoder class is very clunky and should be re-done in a more readable way, probably by extracting the STFT and Phase Vocoder functions and creating distinct classes just for those functions. 

