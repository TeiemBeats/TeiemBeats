#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
struct ControlSpec {const char* id;const char* name;float lo,hi,initial;int x,y,r;const char* unit;};
inline const std::vector<ControlSpec>& controlSpecs(){static const std::vector<ControlSpec> s={
 {"input","Input",-24,24,0,99,310,65," dB"},{"tape","Tape tilt",-6,6,0,274,310,76," dB"},{"balance","Balance",-5,5,0,188,737,70," dB"},{"phase","Stereo rotation",-90,90,0,188,955,75," deg"},
 {"lp","Low pass",8000,22000,22000,508,310,70," Hz"},{"presenceHz","Presence frequency",500,10000,4000,508,525,70," Hz"},{"presence","Presence",-10,10,0,508,736,70," dB"},{"hp","High pass",20,110,20,508,950,70," Hz"},
 {"mode","Dynamics mode",0,2,0,736,310,75,""},{"recovery","Recovery",1,6,3,905,310,72,""},{"makeup","Make up",-12,12,0,736,525,72," dB"},{"ratio","Ratio",1,20,2,905,525,62,":1"},{"mix","Mix",0,100,100,736,950,72," %"},
 {"f4","High frequency",4100,16000,5800,1136,310,73," Hz"},{"g4","High gain",-10,10,0,1310,310,70," dB"},{"s4","High shape",0,2,2,1480,310,73,""},
 {"f3","High-mid frequency",1020,4200,3250,1136,525,73," Hz"},{"g3","High-mid gain",-10,10,0,1310,525,70," dB"},{"s3","High-mid shape",0,2,1,1480,525,73,""},
 {"f2","Low-mid frequency",181,1000,724,1136,736,73," Hz"},{"g2","Low-mid gain",-10,10,0,1310,736,70," dB"},{"s2","Low-mid shape",0,2,1,1480,736,73,""},
 {"f1","Low frequency",32,128,91,1136,950,73," Hz"},{"g1","Low gain",-10,10,0,1310,950,70," dB"},{"s1","Low shape",0,2,0,1480,950,73,""},
 {"width","Spreader",-5,5,0,1808,310,75,""},{"outL","Output left",-24,12,0,1695,885,55," dB"},{"outR","Output right",-24,12,0,1767,885,55," dB"}
 };return s;}
inline juce::AudioProcessorValueTreeState::ParameterLayout makeLayout(){juce::AudioProcessorValueTreeState::ParameterLayout l;for(auto& s:controlSpecs()){juce::NormalisableRange<float> r(s.lo,s.hi,(s.id[0]=='s'||juce::String(s.id)=="mode")?1.f:0.f);if(juce::String(s.unit)==" Hz")r.setSkewForCentre(std::sqrt(s.lo*s.hi));l.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{s.id,1},s.name,r,s.initial));}for(auto pair:std::initializer_list<std::pair<const char*,const char*>>{{"inputOn","Input enabled"},{"filterOn","Filter enabled"},{"limiterOn","Dynamics enabled"},{"toneOn","Tone enabled"},{"polarity","Invert left polarity"},{"transpose","Swap channels"},{"sc","Sidechain high pass"},{"bypass","Bypass"}})l.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{pair.first,1},pair.second,juce::String(pair.first).endsWith("On")));l.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{"monitor",1},"Monitor",juce::StringArray{"Stereo","Left","Right","Mono","Side"},0));return l;}
