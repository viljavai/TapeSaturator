/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TapeLoopSimAudioProcessorEditor::TapeLoopSimAudioProcessorEditor (TapeLoopSimAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 700);

    // saturation slider
    saturationSlider.setName("Saturation");
    saturationSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(&saturationSlider);

    // saturation label
    saturationLabel.setText("Saturation", juce::dontSendNotification);
    addAndMakeVisible(&saturationLabel);

    // noise slider
    noiseSlider.setName("Noise");
    noiseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    addAndMakeVisible(&noiseSlider);

    // noise label
    noiseLabel.setText("Noise", juce::dontSendNotification);
    addAndMakeVisible(&noiseLabel);

    saturationAttachment = make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "SATURATION", saturationSlider);
    noiseAttachment = make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "NOISE", noiseSlider);
}

TapeLoopSimAudioProcessorEditor::~TapeLoopSimAudioProcessorEditor()
{
}

//==============================================================================
void TapeLoopSimAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void TapeLoopSimAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    saturationSlider.setBounds(150, 100, 100, 100);
    saturationLabel.setBounds(150, 210, 100, 20);

    noiseSlider.setBounds(350, 100, 100, 100);
    noiseLabel.setBounds(350, 210, 100, 20);
}
