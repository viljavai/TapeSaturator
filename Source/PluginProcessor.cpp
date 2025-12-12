/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SaturationProcessor.h"

//==============================================================================
TapeLoopSimAudioProcessor::TapeLoopSimAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       apvts(*this, nullptr, "Parameters", createParameterLayout()),
       oversampling(std::make_unique<juce::dsp::Oversampling<float>> (
           /*numChannels*/ 2,
           /*numOrders*/   3,
           juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR))
#endif
{
}

TapeLoopSimAudioProcessor::~TapeLoopSimAudioProcessor()
{
}

//==============================================================================
const juce::String TapeLoopSimAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TapeLoopSimAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TapeLoopSimAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TapeLoopSimAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TapeLoopSimAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TapeLoopSimAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TapeLoopSimAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TapeLoopSimAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TapeLoopSimAudioProcessor::getProgramName (int index)
{
    return {};
}

void TapeLoopSimAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TapeLoopSimAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    oversampling->initProcessing (static_cast<size_t> (samplesPerBlock));
    oversampling->reset();
    setLatencySamples(oversampling->getLatencyInSamples());
}

void TapeLoopSimAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TapeLoopSimAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void TapeLoopSimAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    float gain = apvts.getRawParameterValue("SATURATION")->load();
    float noise = apvts.getRawParameterValue("NOISE")->load();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

        juce::dsp::AudioBlock<float>block(buffer);
        auto oversampledBuffer = oversampling->processSamplesUp(block);

        // This is the place where you'd normally do the guts of your plugin's
        // audio processing..
        // Make sure to reset the state if your inner loop is processing
        // the samples and the outer loop is handling the channels.
        // Alternatively, you can process the samples with the channels
        // interleaved by keeping the same state.
        for (int channel = 0; channel < oversampledBuffer.getNumChannels(); ++channel)
        {
            auto* channelData = oversampledBuffer.getChannelPointer(channel);

            for (int sample = 0; sample < oversampledBuffer.getNumSamples(); ++sample)
            {
                float saturatedSignal = saturator.process(channelData[sample], gain);
                float noiseAmount = ((random.nextFloat() - random.nextFloat())) * noise *  0.05f;
                channelData[sample] = saturatedSignal + noiseAmount;
            }
        }
        oversampling->processSamplesDown(block);
    }

//==============================================================================
bool TapeLoopSimAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TapeLoopSimAudioProcessor::createEditor()
{
    return new TapeLoopSimAudioProcessorEditor (*this);
}

//==============================================================================
void TapeLoopSimAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream stream(destData, false);
    apvts.state.writeToStream(stream);
}

void TapeLoopSimAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));
    if (tree.isValid())
    {
        apvts.replaceState(tree);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapeLoopSimAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout TapeLoopSimAudioProcessor::createParameterLayout()
{
    vector<unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(make_unique<juce::AudioParameterFloat>("SATURATION", "Saturation", 0.01f, 15.0f, 0.01f));
    params.push_back(make_unique<juce::AudioParameterFloat>("NOISE", "Noise", 0.0f, 1.0f, 0.0f));
    return { params.begin(), params.end() };
}