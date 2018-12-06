/**
 * @file PluginProcessor.h
 * @brief DonAudioProcessorクラス用のヘッダファイル。元はJUCEによる自動生成。
 * @author h.wakimoto
 * @date 2017/05/01
 */


/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../extern/LUFSMeter/Ebu128LoudnessMeter.h"



//==============================================================================
/**
*/
class DonAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    DonAudioProcessor();
    ~DonAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //LUFS
    
    float getShortTermLoudness();
    float getMomentaryLoudness();
    float getIntegratedLoudness();
    float getLoudnessRangeStart();
    float getLoudnessRangeEnd();
    float getLoudnessRange();
    Ebu128LoudnessMeter ebu128LoudnessMeter;
    
    //Mute
    bool getIsMute() {
        return isMute;
    }
    void setIsMute(bool value) {
        isMute = value;
    }

private:
    /**
     @brief ミュートボタン
     */
    bool isMute = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DonAudioProcessor)
};
