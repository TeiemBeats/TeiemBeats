#pragma once
#include "Processor.h"
class VocaEditor:public juce::AudioProcessorEditor,private juce::Timer {
 public:explicit VocaEditor(VocaProcessor&);~VocaEditor()override;void paint(juce::Graphics&)override;void resized()override;
 private:
 struct Style:juce::LookAndFeel_V4 {juce::Font getComboBoxFont(juce::ComboBox& b)override{return juce::Font(juce::FontOptions(float(b.getHeight())*.43f));}juce::Font getTextButtonFont(juce::TextButton&,int h)override{return juce::Font(juce::FontOptions(std::clamp(h*.6f,8.f,14.f)));}void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&)override{}void drawButtonBackground(juce::Graphics&,juce::Button&,const juce::Colour&,bool,bool)override;void drawComboBox(juce::Graphics&,int,int,bool,int,int,int,int,juce::ComboBox&)override;} style;
 std::unique_ptr<juce::FileChooser> faceplateChooser;
 VocaProcessor& processor;juce::Image metal,faceplate;
 std::vector<std::unique_ptr<juce::Slider>> knobs;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> knobAttachments;
 std::vector<std::unique_ptr<juce::TextButton>> powers;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> powerAttachments;
 std::vector<std::unique_ptr<juce::ComboBox>> combos;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboAttachments;
 std::array<juce::TextButton,9> presetButtons;std::array<juce::TextButton,5> tabs;juce::TextButton settings,bypass,scaleButton;std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
 int activeTab=0;float meterIn=-60,meterOut=-60;juce::String tooltip;bool ready=false;
 void timerCallback()override;void settingsMenu();void buildMetal();void drawKnob(juce::Graphics&,const voca::Spec&,float);void drawWave(juce::Graphics&);void drawMeter(juce::Graphics&,float,int,int,int,int);void applyScale(float);
 static void panel(juce::Graphics&,juce::Rectangle<float>,const juce::String&,int titleY=0);static void screw(juce::Graphics&,float,float,float r=5);static void text(juce::Graphics&,const juce::String&,float,float,float,float,float,bool=false,juce::Colour=juce::Colours::black);static void box(juce::Graphics&,const juce::String&,float,float,float,float,juce::Colour=juce::Colours::white);static void neon(juce::Graphics&,juce::Rectangle<float>,bool purple);
};
