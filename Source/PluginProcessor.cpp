#include "PluginProcessor.h"
#include "PluginEditor.h"
MasteringProcessor::MasteringProcessor():AudioProcessor(BusesProperties().withInput("Stereo input",juce::AudioChannelSet::stereo(),true).withOutput("Stereo output",juce::AudioChannelSet::stereo(),true)),state(*this,nullptr,"TeiemTG",makeLayout()){}
void MasteringProcessor::prepareToPlay(double sr,int){engine.prepare(sr);}
bool MasteringProcessor::isBusesLayoutSupported(const BusesLayout& b)const{return b.getMainInputChannelSet()==juce::AudioChannelSet::stereo()&&b.getMainOutputChannelSet()==juce::AudioChannelSet::stereo();}
void MasteringProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&){juce::ScopedNoDenormals no; if(b.getNumChannels()<2)return;auto v=[this](const char* id){return state.getRawParameterValue(id)->load();};tg::Settings s;
 s.input=v("input");s.tape=v("tape");s.balance=v("balance");s.phase=v("phase");s.hp=v("hp");s.lp=v("lp");s.presence=v("presence");s.presenceHz=v("presenceHz");s.mode=static_cast<int>(v("mode"));s.recovery=v("recovery");s.makeup=v("makeup");s.ratio=v("ratio");s.mix=v("mix");s.width=v("width");s.outL=v("outL");s.outR=v("outR");s.monitor=static_cast<int>(v("monitor"));s.inputOn=v("inputOn")>.5;s.filterOn=v("filterOn")>.5;s.limiterOn=v("limiterOn")>.5;s.toneOn=v("toneOn")>.5;s.polarity=v("polarity")>.5;s.transpose=v("transpose")>.5;s.sc=v("sc")>.5;s.bypass=v("bypass")>.5;
 for(int i=0;i<4;++i){s.freq[i]=v(("f"+juce::String(i+1)).toRawUTF8());s.gain[i]=v(("g"+juce::String(i+1)).toRawUTF8());s.shape[i]=3;const int shape=static_cast<int>(v(("s"+juce::String(i+1)).toRawUTF8()));s.shape[i]=shape==0?3:shape==1?2:4;}
 engine.configure(s);engine.process(b.getWritePointer(0),b.getWritePointer(1),b.getNumSamples());peakL.store(b.getMagnitude(0,0,b.getNumSamples()));peakR.store(b.getMagnitude(1,0,b.getNumSamples()));reduction.store(engine.reduction());}
void MasteringProcessor::getStateInformation(juce::MemoryBlock& out){auto xml=state.copyState().createXml();copyXmlToBinary(*xml,out);}
void MasteringProcessor::setStateInformation(const void* data,int size){if(auto xml=getXmlFromBinary(data,size))if(xml->hasTagName(state.state.getType()))state.replaceState(juce::ValueTree::fromXml(*xml));}
juce::AudioProcessorEditor* MasteringProcessor::createEditor(){return new MasteringEditor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new MasteringProcessor();}
