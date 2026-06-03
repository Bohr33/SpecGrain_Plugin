/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpecGrainAudioProcessor::SpecGrainAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
            parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    pvEngine = std::make_unique<PhaseVocoderEngine>();
    
    //Attach Parameters to Atomics
    pShiftParam = parameters.getRawParameterValue("PITCH_SHIFT");
    blurParam = parameters.getRawParameterValue("BLUR_AMOUNT");
    stretchTimeParam = parameters.getRawParameterValue("STRETCH_TIME");
    stretchDensityParam = parameters.getRawParameterValue("STRETCH_DENSITY");
    delayAmtParam = parameters.getRawParameterValue("DELAY_AMOUNT");
    delayTimeParam = parameters.getRawParameterValue("DELAY_TIME");
    feedbackParam = parameters.getRawParameterValue("DELAY_FEEDBACK");
    delayFreqToggleParam = parameters.getRawParameterValue("DELAY_FREQUENCY_TOGGLE");
    gateAmtParam = parameters.getRawParameterValue("GATE_AMOUNT");
}

SpecGrainAudioProcessor::~SpecGrainAudioProcessor()
{
}

//==============================================================================


const juce::String SpecGrainAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SpecGrainAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SpecGrainAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SpecGrainAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SpecGrainAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SpecGrainAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SpecGrainAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SpecGrainAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SpecGrainAudioProcessor::getProgramName (int index)
{
    return {};
}

void SpecGrainAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SpecGrainAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    int numChannels = getMainBusNumInputChannels();
    
    
    fftSize = pendingFFTSize.load();
    
    pvEngine->prepare(samplesPerBlock, fftSize, numChannels);
    
    pShifts.resize(numChannels);
    specBlurs.resize(numChannels);
    specDelays.resize(numChannels);
    specStretchs.resize(numChannels);
    specGates.resize(numChannels);
    
    for(int channel = 0; channel < numChannels; channel++)
    {
        pShifts[channel].prepare(fftSize);
        specBlurs[channel].prepare(fftSize);
        specDelays[channel].prepare(fftSize, sampleRate);
        specStretchs[channel].prepare(fftSize);
        specGates[channel].prepare(fftSize);
    }
    
    resizePending.store(false);
}

void SpecGrainAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SpecGrainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
    
}
#endif

void SpecGrainAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    int numChans = getMainBusNumInputChannels();
    
    //Check to see if FFTsize is changed,
    if(resizePending.load())
    {
        prepareToPlay(getSampleRate(), buffer.getNumSamples());
        return;
    }
    
    
    float pShiftAmt = pShiftParam->load();
    float blurAmt = blurParam->load();
    float stretchTime = stretchTimeParam->load();
    float stretchDensity = stretchDensityParam->load();
    float delayTime = delayTimeParam->load();
    float delayAmt = delayAmtParam->load();
    float delayFeedback = feedbackParam->load();
    bool delayFreqToggle = delayFreqToggleParam->load() > 0.5f;
    float gateAmt = gateAmtParam->load();
    
    
    //PV Analysis Stage
    for(int ch = 0; ch < numChans; ch++)
    {
        pvEngine->pushSamples(ch, {buffer.getReadPointer(ch), (size_t) buffer.getNumSamples()});
    };
    
    //Spectral Processing
    for(int ch = 0; ch < numChans; ch++)
    {
        std::vector<fsig>& frames = pvEngine->getFsigBuffer(ch);
        for(auto& frame : frames)
        {
            pShifts[ch].process(frame, pShiftAmt);
            specBlurs[ch].process(frame, blurAmt);
            specStretchs[ch].process(frame, stretchTime, stretchDensity);
            specDelays[ch].process(frame, delayTime, delayAmt, delayFeedback, delayFreqToggle);
            specGates[ch].process(frame, gateAmt);
        }
    }
    
    //Retrieve Samples
    for(int ch = 0; ch < numChans; ch++)
    {
        pvEngine->pullSamples(ch, {buffer.getWritePointer(ch), (size_t) buffer.getNumSamples()});
    }
    
}

//==============================================================================
bool SpecGrainAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SpecGrainAudioProcessor::createEditor()
{
    return new SpecGrainAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void SpecGrainAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SpecGrainAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout SpecGrainAudioProcessor::createParameterLayout()
{
    int versionHint = 1;
    
    using namespace juce;
    
    return
    {
        std::make_unique<AudioParameterFloat>(ParameterID {"PITCH_SHIFT", versionHint}, "Pitch Shift", 0.0f, 3.0f, 1.0f),
        std::make_unique<AudioParameterFloat>(ParameterID {"BLUR_AMOUNT", versionHint}, "Blur Amount", 0.0f, 1.0f, 0.0f),
        std::make_unique<AudioParameterFloat>(ParameterID {"STRETCH_TIME", versionHint}, "Stretch Time", 0.0f, 1.0f, 0.0f),
        std::make_unique<AudioParameterFloat>(ParameterID {"STRETCH_DENSITY", versionHint}, "Stretch Density", 0.0f, 1.0f, 0.0f),
        std::make_unique<AudioParameterFloat>(ParameterID {"DELAY_AMOUNT", versionHint}, "Delay Amount", 0.0f, 1.0f, 0.0f),
        std::make_unique<AudioParameterFloat>(ParameterID {"DELAY_TIME", versionHint}, "Delay Time", 1.0f, 2000.0f, 0.0f),
        std::make_unique<AudioParameterFloat>(ParameterID {"DELAY_FEEDBACK", versionHint}, "Feedback", 0.0f, 1.0f, 0.0f),
        std::make_unique<AudioParameterFloat>(ParameterID {"DELAY_FREQUENCY_TOGGLE", versionHint}, "Delay Freq Toggle", 0.0f, 1.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("GATE_AMOUNT", 1),
            "Gate Amount",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.0001f),
            0.0f,
            juce::String(),
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) {
                return juce::String(value, 4); // 4 decimal places
            })
    };
}


void SpecGrainAudioProcessor::fftSizeChanged(int newFFTsize)
{
    pendingFFTSize.store(newFFTsize);
    resizePending.store(true);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpecGrainAudioProcessor();
}
