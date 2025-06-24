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
    pv = std::make_unique<PhaseVocoder>(parameters);
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    pv->prepare(samplesPerBlock, sampleRate);
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
    juce::ScopedNoDenormals noDenormals;

    pv->process(buffer);
    
//    auto* channel2 = buffer.getWritePointer (1);
//    auto* channel1 = buffer.getReadPointer(0);
//
//    juce::FloatVectorOperations::copy(channel2, channel1, buffer.getNumSamples());

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
        std::make_unique<AudioParameterFloat>(ParameterID {"pitchShift", versionHint}, "Pitch Shift", 0.0f, 3.0f, 1.0f)
    };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpecGrainAudioProcessor();
}
