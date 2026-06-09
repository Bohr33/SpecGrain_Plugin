# PVExperiments

A VST/AU plugin built with the JUCE framework. Originally intended as a phase vocoder implementation, the project grew into a small spectral processing framework incorporating several experimental effects.


## Building

Clone the repository with submodules:
```
git clone --recursive https://github.com/Bohr33/PVExperiements```

Configure and build:
```
cmake -B build
cmake --build build --config Release
```

Built plugin artifacts will be located in `build/PVExperiments_artefacts/Release/`.

### Formats
- **VST3** — compatible with most DAWs
- **AU** — macOS only

## Effects

Audio is processed through five stages in series:

**Pitch Shift → Spectral Stretch → Gate → Spectral Delay → Spectral Blur**

| Effect | Description |
|---|---|
| Spectral Stretch | Granular-style effect that holds a buffer of spectral frames and reads through them at a variable density, producing smearing and textural artifacts |
| Pitch Shift | Standard phase vocoder pitch shifting |
| Gate | Zeros any spectral bin below an amplitude threshold, used to suppress noise introduced by the other effects |
| Spectral Delay | Delays spectral frames in time, with an optional mode that delays frequency values alongside amplitude values |
| Spectral Blur | Averages spectral frames over a set window to blur the frequency content |

**Note:** If you want to change the effect processing order, simply rearrange the effect processing functions in the AudioProcessor `processBlock()` function.

## Controls

### Main parameters

| Parameter | Description | Range |
|---|---|---|
| Pitch Shift | Pitch shift multiplier | 0.0 – 3.0 |
| Blur Amount | Window length (Max 300 frames) over which spectral frames are averaged | 0.0 - 1.0 |
| Spectral Time | Duration for which frames are held in the stretch buffer | 0.0 - 1.0 |
| Spectral Density | Density of frames played back during the **Spectral Time**| 0.0 - 1.0 |
| Gate Amount | Amplitude threshold below which bin values are zeroed | 0.0 - 1.0 |
| Delay Time | Spectral delay time in milliseconds | 1.0 - 2,000 ms |
| Delay Amount | Gain mix of the spectral delay | 0.0 - 1.0 |
| Feedback | Delay feedback amount (clamped to 0.99) | 0.0 - 0.99 |

### Additional controls

- **FFT Size** — dropdown to select between three FFT sizes. Primarily included for testing, but smaller sizes can reduce latency. Also affects the timing of a few of the spectralProcessors that aren't knowledable of the sample rate.
- **Frequency Delay Toggle** — when enabled, the delay is applied to both frequency and amplitude values rather than amplitude alone.

## Implementation

### Phase vocoder

The phase vocoder performs an STFT and produces a custom `Fsig` structure holding per-bin amplitude and frequency values as vectors of length `FFT size / 2 + 1`. Internal buffers are managed as `std::vector`s and resized in `prepare()`, which handles changes to buffer size, FFT size, and sample rate. The phase vocoder owns instances of each spectral processor and calls their `prepare()` and `process()` functions at the appropriate points.

### Spectral processors

Each spectral processor is a relatively lightweight class implementing its own `prepare()` and `process()` functions. The spectral stretch processor originated as a time-stretching algorithm using a circular buffer of input frames read back at a different rate. This proved problematic for real-time use, so the design was changed: a buffer of frames is held for a duration set by the stretch time parameter, and frames are read from it at a density determined by the density parameter — producing a granular-style smearing effect rather than true time stretching.

### Parameters and threading

Parameters are declared in a `juce::AudioProcessorValueTreeState` in the processor class and passed to the phase vocoder. Values are stored as atomics and loaded at the top of each frame process to ensure thread safety.

## Performance notes

CPU usage is significant, particularly for stereo processing and large FFT sizes. I've only had real buffering issues with FFT size = 2048, and with the blur and stretch parameters using high parameters. These effects use Fsig buffers for processing, and the higher parameter values necessitate processing more buffers per processing block.
