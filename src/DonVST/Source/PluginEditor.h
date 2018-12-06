/**
 * @file PluginEditor.h
 * @brief JUCEによる自動生成ファイル
 * @author h.wakimoto
 * @date 2017/05/01
 */

/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class DonAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    DonAudioProcessorEditor (DonAudioProcessor&);
    ~DonAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DonAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DonAudioProcessorEditor)
};
