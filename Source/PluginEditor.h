/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace std;

//==============================================================================
/**
*/
class TapeLoopSimAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TapeLoopSimAudioProcessorEditor (TapeLoopSimAudioProcessor&);
    ~TapeLoopSimAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    juce::Slider saturationSlider;
    juce::Slider noiseSlider;

    juce::Label saturationLabel;
    juce::Label noiseLabel;
    
    unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> saturationAttachment;
    unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noiseAttachment;

    TapeLoopSimAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapeLoopSimAudioProcessorEditor)
};
