#pragma once
#include "PluginProcessor.h"
class MasteringEditor:public juce::AudioProcessorEditor,private juce::Timer {
 public:explicit MasteringEditor(MasteringProcessor&);void paint(juce::Graphics&)override;void resized()override;
 private:
 struct KnobLook:juce::LookAndFeel_V4{void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&)override{}void drawLinearSlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider::SliderStyle,juce::Slider&)override{}} look;
 MasteringProcessor& processor;juce::Image face;
 std::vector<std::unique_ptr<juce::Slider>> sliders;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
 struct Switch {juce::String id;juce::Rectangle<float> area;std::unique_ptr<juce::TextButton> button;std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;};
 std::vector<Switch> switches;std::vector<std::unique_ptr<juce::TextButton>> monitors;
 juce::Label track;juce::TextButton bypass{"BYPASS"},reset{"RESET"};std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
 juce::String readout="Drag a knob • Arrow keys for fine control • Double-click to reset";float levelL=-60,levelR=-60;
 void timerCallback()override;void addSwitch(const char*,int,int,int,int);
};
