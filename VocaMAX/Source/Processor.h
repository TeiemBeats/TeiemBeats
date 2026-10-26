#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "Engine.h"
class VocaProcessor:public juce::AudioProcessor,private juce::AudioProcessorValueTreeState::Listener,private juce::AsyncUpdater {
 public:
 VocaProcessor();~VocaProcessor()override;
 juce::AudioProcessorValueTreeState state;
 std::atomic<float> inputPeak{0},outputPeak{0},reduction{0},pitchHz{0};std::atomic<double> rate{48000};std::atomic<int> blockSize{256},wavePosition{0};std::array<std::atomic<float>,512> waveform{};
 void prepareToPlay(double,int)override;void releaseResources()override{}
 bool isBusesLayoutSupported(const BusesLayout&)const override;
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&)override;
 void processBlockBypassed(juce::AudioBuffer<float>&,juce::MidiBuffer&)override;
 juce::AudioProcessorEditor* createEditor()override;bool hasEditor()const override{return true;}
 const juce::String getName()const override{return "VocaMAX";}
 bool acceptsMidi()const override{return false;}bool producesMidi()const override{return false;}
 double getTailLengthSeconds()const override{return 8;}
 int getNumPrograms()override{return 1;}int getCurrentProgram()override{return 0;}void setCurrentProgram(int)override{}
 const juce::String getProgramName(int)override{return "VocaMAX";}void changeProgramName(int,const juce::String&)override{}
 void getStateInformation(juce::MemoryBlock&)override;void setStateInformation(const void*,int)override;
 juce::AudioProcessorParameter* getBypassParameter()const override{return state.getParameter("bypass");}
 void applyPreset(int);void resetParameters();
 private:
 std::array<std::atomic<float>*,voca::NumKnobs> knobValues{};std::array<std::atomic<float>*,voca::NumModules> moduleValues{};std::array<std::atomic<float>*,10> choiceValues{};std::atomic<float>* bypassValue=nullptr;
 voca::Engine engine;int waveDecimator=0;float waveMax=0;std::atomic<bool> restoring{false};
 void render(juce::AudioBuffer<float>&,bool);void parameterChanged(const juce::String&,float)override;void handleAsyncUpdate()override;
 static juce::AudioProcessorValueTreeState::ParameterLayout layout();
};
