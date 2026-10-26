#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "Dsp.h"
#include "Parameters.h"
class MasteringProcessor : public juce::AudioProcessor {
 public:
 MasteringProcessor();
 juce::AudioProcessorValueTreeState state;
 std::atomic<float> peakL{0},peakR{0},reduction{0};
 void prepareToPlay(double,int) override;
 void releaseResources() override {}
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 juce::AudioProcessorEditor* createEditor() override;
 bool hasEditor()const override{return true;}
 const juce::String getName()const override{return "Teiem TG Mastering";}
 bool acceptsMidi()const override{return false;}bool producesMidi()const override{return false;}
 double getTailLengthSeconds()const override{return 0;}
 int getNumPrograms()override{return 1;}int getCurrentProgram()override{return 0;}
 void setCurrentProgram(int)override{}const juce::String getProgramName(int)override{return "Default";}
 void changeProgramName(int,const juce::String&)override{}
 void getStateInformation(juce::MemoryBlock&)override;void setStateInformation(const void*,int)override;
 juce::AudioProcessorParameter* getBypassParameter()const override{return state.getParameter("bypass");}
 private:tg::Engine engine;
};
