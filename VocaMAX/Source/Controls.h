#pragma once
#include <array>
#include <cstdio>
#include <string>
namespace voca {
enum Param { Input,Tune,Output,EqLow,EqLowMid,EqMid,EqHighMid,EqHigh,CompThreshold,CompRatio,CompAttack,CompRelease,LimitThreshold,DeessFreq,DeessAmount,Width,Reverb,Delay,Flanger,Heat,Distortion,Warmth,Dynamics,Saturation,Clipper,GateThreshold,GateRelease,Maximizer,Squash,Imaging,LowEnd,Punch,Clarity,CharacterWidth,Harmonics,Air,NumKnobs };
struct Spec { const char* id;const char* name;float min,max,initial;int x,y,r;const char* unit; };
inline const std::array<Spec,NumKnobs> specs{{
 {"input","Input",-24,24,0,123,151,70,"dB"},{"tune","Pitch correction",0,100,0,368,134,48,"%"},{"output","Output",-24,24,0,1308,151,60,"dB"},
 {"eqLow","Low EQ",-12,12,0,95,395,42,"dB"},{"eqLowMid","Low mid EQ",-12,12,0,207,395,42,"dB"},{"eqMid","Mid EQ",-12,12,0,317,395,42,"dB"},{"eqHighMid","High mid EQ",-12,12,0,430,395,42,"dB"},{"eqHigh","High EQ",-12,12,0,542,395,42,"dB"},
 {"compThreshold","Compressor threshold",-60,0,-18,674,396,42,"dB"},{"compRatio","Compressor ratio",1,20,4,785,396,42,":1"},{"compAttack","Compressor attack",.1f,100,10,899,396,42,"ms"},{"compRelease","Compressor release",10,2000,120,1010,396,42,"ms"},
 {"limitThreshold","Limiter ceiling",-20,0,-1,1153,396,42,"dB"},{"deessFreq","De-esser frequency",1000,10000,6000,1317,396,42,"Hz"},{"deessAmount","De-esser amount",0,100,50,1447,396,42,"%"},
 {"width","Stereo widening",0,200,100,106,590,44,"%"},{"reverb","Reverb",0,100,30,275,590,44,"%"},{"delay","Delay",0,100,25,436,590,44,"%"},{"flanger","Flanger",0,100,20,594,590,44,"%"},{"heat","Heat",0,100,40,753,590,44,"%"},{"distortion","Distortion",0,100,20,915,590,44,"%"},{"warmth","Warmth / tape",0,100,60,1081,590,44,"%"},{"dynamics","Dynamics",0,100,50,1250,590,44,"%"},{"saturation","Saturation",0,100,40,1426,590,44,"%"},
 {"clipper","Clipper",0,100,50,100,791,45,"%"},{"gateThreshold","Gate threshold",-60,0,-40,237,801,42,"dB"},{"gateRelease","Gate release",10,2000,100,354,801,42,"ms"},{"maximizer","Maximizer",0,100,80,500,791,45,"%"},{"squash","Squashing",0,100,70,641,791,45,"%"},{"imaging","Imaging",-100,100,0,787,791,45,"%"},
 {"lowEnd","Low end character",0,100,0,929,805,37,"%"},{"punch","Punch character",0,100,0,1033,805,37,"%"},{"clarity","Clarity character",0,100,0,1136,805,37,"%"},{"characterWidth","Width character",0,100,0,1238,805,37,"%"},{"harmonics","Harmonics character",0,100,0,1343,805,37,"%"},{"air","Air character",0,100,0,1449,805,37,"%"}
}};
enum Module { EQ,Compression,Limiter,Deesser,Stereo,ReverbModule,DelayModule,FlangerModule,HeatModule,DistortionModule,WarmthModule,DynamicsModule,SaturationModule,ClipperModule,Gate,MaximizerModule,SquashModule,ImagingModule,NumModules };
struct ModuleSpec {const char* id;const char* name;int x,y;};
inline const std::array<ModuleSpec,NumModules> modules{{
 {"eqOn","5 band EQ",54,319},{"compOn","Compression",638,319},{"limitOn","Limiter",1104,319},{"deessOn","De-esser",1256,319},
 {"widthOn","Stereo widening",40,537},{"reverbOn","Reverb",214,537},{"delayOn","Delay",375,537},{"flangerOn","Flanger",534,537},{"heatOn","Heat",695,537},{"distortionOn","Distortion",853,537},{"warmthOn","Warmth / tape",1016,537},{"dynamicsOn","Dynamics",1190,537},{"saturationOn","Saturation",1364,537},
 {"clipperOn","Clipper",47,731},{"gateOn","Noise gate",192,731},{"maximizerOn","Maximizer",450,731},{"squashOn","Squashing",592,731},{"imagingOn","Imaging",736,731}
}};
struct ChoiceSpec {const char* id;const char* name;int x,y,w;int initial;const char* items;};
inline const std::array<ChoiceSpec,10> choices{{
 {"key","Key",290,231,64,0,"C|C#|D|D#|E|F|F#|G|G#|A|A#|B"},{"scale","Scale",365,231,85,0,"Major|Minor|Chromatic|Pentatonic"},
 {"preset","Mastering mode",1057,89,131,0,"Modern Pop|Vocal Radio|Hip Hop|R&B|Rock|EDM|Cinematic|Vintage|Custom"},
 {"reverbType","Reverb type",220,660,109,0,"Plate|Room|Hall"},{"delayTime","Delay division",381,660,108,2,"1/16|1/8|1/4|1/2|Dotted 1/8"},{"flangerType","Flanger rate",540,660,109,0,"Fast|Slow|Deep"},
 {"distType","Distortion type",861,660,109,0,"Tube|Fuzz|Fold"},{"warmthType","Warmth type",1025,660,111,0,"Tape|Valve|Soft"},{"dynType","Dynamics type",1195,660,110,0,"Vocal|Punch|Gentle"},{"satType","Saturation type",1371,660,118,0,"Analog|Warm|Bright"}
}};
inline std::string format(Param p,float v){char b[48];if(p==DeessFreq)std::snprintf(b,sizeof(b),"%.1f kHz",double(v/1000));else if(p==CompRatio)std::snprintf(b,sizeof(b),"%.1f:1",double(v));else if(std::string(specs[p].unit)=="dB")std::snprintf(b,sizeof(b),"%.1f dB",double(v));else if(std::string(specs[p].unit)=="ms")std::snprintf(b,sizeof(b),"%.0f ms",double(v));else std::snprintf(b,sizeof(b),"%.0f%%",double(v));return b;}
}
