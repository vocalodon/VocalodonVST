/**
 * @file PluginProcessor.cpp
 * @brief DonAudioProcessorクラス用のソースファイル。元はJUCEによる自動生成。
 * @author h.wakimoto
 * @date 2017/05/01
 */

/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "MainWindow.h"


//==============================================================================
DonAudioProcessor::DonAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

DonAudioProcessor::~DonAudioProcessor()
{
}

//==============================================================================
const String DonAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DonAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DonAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DonAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DonAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DonAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DonAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DonAudioProcessor::setCurrentProgram (int index)
{
}

const String DonAudioProcessor::getProgramName (int index)
{
    return {};
}

void DonAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DonAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    int expectedRequestRate = 20;
    
    ebu128LoudnessMeter.prepareToPlay(sampleRate,
                                      getNumInputChannels(),
                                      samplesPerBlock,
                                      expectedRequestRate);
}

void DonAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DonAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

//void DonAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
void DonAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ebu128LoudnessMeter.processBlock(buffer);

    if (isMute) {
        buffer.applyGain(0.0f);
    }
    else {
        
        
        const int totalNumInputChannels = getTotalNumInputChannels();
        const int totalNumOutputChannels = getTotalNumOutputChannels();

        // In case we have more outputs than inputs, this code clears any output
        // channels that didn't contain input data, (because these aren't
        // guaranteed to be empty - they may contain garbage).
        // This is here to avoid people getting screaming feedback
        // when they first compile a plugin, but obviously you don't need to keep
        // this code if your algorithm always overwrites all the output channels.
        for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear(i, 0, buffer.getNumSamples());
        
        // This is the place where you'd normally do the guts of your plugin's
        // audio processing...
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            
            // ..do something to the data...
        }

        if (this->getActiveEditor() != nullptr) {
            AudioPlayHead* ph = this->getPlayHead();
            if (ph != nullptr) {
                AudioPlayHead::CurrentPositionInfo info;
                ph->getCurrentPosition(info);
                DonMainWindow* window = (DonMainWindow*)this->getActiveEditor();
                if (window->getIsPlaying() != info.isPlaying) {
                    window->setIsPlaying(info.isPlaying);
                }
            }
        }
    }
}

//==============================================================================
bool DonAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DonAudioProcessor::createEditor()
{
    //return new DonAudioProcessorEditor (*this);
    return new DonMainWindow (*this);
}

//==============================================================================
void DonAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DonAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DonAudioProcessor();
}


//LUFS

float DonAudioProcessor::getShortTermLoudness()
{
    return ebu128LoudnessMeter.getShortTermLoudness();
}


float DonAudioProcessor::getMomentaryLoudness()
{
    return ebu128LoudnessMeter.getMomentaryLoudness();
}

float DonAudioProcessor::getIntegratedLoudness()
{
    return ebu128LoudnessMeter.getIntegratedLoudness();
}

float DonAudioProcessor::getLoudnessRangeStart()
{
    return ebu128LoudnessMeter.getLoudnessRangeStart();
}

float DonAudioProcessor::getLoudnessRangeEnd()
{
    return ebu128LoudnessMeter.getLoudnessRangeEnd();
}

float DonAudioProcessor::getLoudnessRange()
{
    return ebu128LoudnessMeter.getLoudnessRange();
}
