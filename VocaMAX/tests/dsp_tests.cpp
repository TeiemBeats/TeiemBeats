#include "Engine.h"
#include <iostream>
#include <stdexcept>
#include <chrono>
void require(bool b,const char* msg){if(!b)throw std::runtime_error(msg);}
float rms(const std::vector<float>& x,size_t first){double e=0;for(size_t i=first;i<x.size();++i)e+=x[i]*x[i];return static_cast<float>(std::sqrt(e/(x.size()-first)));}
voca::Settings clean(){voca::Settings s;s.enabled.fill(false);s.value[voca::Tune]=0;return s;}
void sine(std::vector<float>& l,std::vector<float>& r,double sr,float hz,float amp){for(size_t i=0;i<l.size();++i)l[i]=r[i]=amp*std::sin(2*voca::pi*hz*i/sr);}
int main(){try{int checks=0;auto start=std::chrono::steady_clock::now();for(double sr:{44100.,48000.,96000.}){const int n=static_cast<int>(sr);voca::Engine engine;std::vector<float> l(n),r(n);auto s=clean();s.bypass=true;engine.prepare(sr);engine.configure(s);l[0]=r[0]=.5f;engine.process(l.data(),r.data(),n);require(l[voca::Engine::latency]==.5f,"Bypass must preserve and delay impulse by reported latency");require(l[voca::Engine::latency-1]==0&&l[voca::Engine::latency+1]==0,"Bypass impulse must be exact");++checks;
 engine.prepare(sr);s=clean();s.value[voca::Input]=6;engine.configure(s);sine(l,r,sr,1000,.1f);engine.process(l.data(),r.data(),n);require(std::abs(rms(l,n/2)-.1f*voca::db(6)/std::sqrt(2.f))<.003f,"Input gain must scale signal");++checks;
 engine.prepare(sr);s=clean();s.enabled[voca::EQ]=true;s.value[voca::EqMid]=6;engine.configure(s);sine(l,r,sr,1200,.1f);engine.process(l.data(),r.data(),n);require(rms(l,n/2)>.13f,"Mid EQ boost must be audible");++checks;
 engine.prepare(sr);s=clean();s.enabled[voca::Stereo]=true;s.value[voca::Width]=0;engine.configure(s);sine(l,r,sr,1000,.1f);for(auto& x:r)x=-x;engine.process(l.data(),r.data(),n);require(rms(l,n/2)<1e-5,"Zero stereo width must cancel side-only input");++checks;
 engine.prepare(sr);s=clean();s.enabled[voca::Compression]=true;s.value[voca::CompThreshold]=-24;s.value[voca::CompRatio]=10;engine.configure(s);sine(l,r,sr,1000,.8f);engine.process(l.data(),r.data(),n);require(rms(l,n/2)<.13f&&engine.reduction()>12,"Compressor must reduce above threshold");++checks;
 engine.prepare(sr);s=clean();s.enabled[voca::Gate]=true;s.value[voca::GateThreshold]=-20;s.value[voca::GateRelease]=10;engine.configure(s);sine(l,r,sr,1000,.001f);engine.process(l.data(),r.data(),n);require(rms(l,n/2)<1e-7,"Gate must suppress below-threshold audio");++checks;
 engine.prepare(sr);s=clean();s.enabled[voca::Limiter]=true;s.value[voca::Output]=24;s.value[voca::LimitThreshold]=-3;engine.configure(s);sine(l,r,sr,1000,.8f);engine.process(l.data(),r.data(),n);float peak=0;for(int i=n/2;i<n;++i)peak=std::max(peak,std::abs(l[i]));require(peak<=voca::db(-3)+1e-5,"Limiter must enforce ceiling including output gain");++checks;
 engine.prepare(sr);s=voca::Settings{};engine.configure(s);sine(l,r,sr,220,.2f);engine.process(l.data(),r.data(),n);for(float x:l)require(std::isfinite(x),"Default complete chain must stay finite");require(engine.pitchHz()>215&&engine.pitchHz()<225,"Pitch detector must recognize A3");++checks;
 engine.prepare(sr);s=clean();s.enabled[voca::DelayModule]=true;s.value[voca::Delay]=100;s.bpm=120;s.choice[4]=2;engine.configure(s);l.assign(n,0);r=l;l[3000]=r[3000]=.5f;engine.process(l.data(),r.data(),n);const int echo=3000+voca::Engine::latency+static_cast<int>(sr*.5);require(echo<n&&std::abs(l[echo])>.1f,"Tempo-synced quarter note delay must create echo");++checks;
 }

 // Each named module must alter a representative vocal-like stereo signal when enabled.
 {const int n=48000;std::vector<float> sourceL(n),sourceR(n);for(int i=0;i<n;++i){sourceL[i]=.12f*std::sin(2*voca::pi*440*i/48000)+.07f*std::sin(2*voca::pi*7000*i/48000);sourceR[i]=.09f*std::sin(2*voca::pi*660*i/48000)+.05f*std::sin(2*voca::pi*7000*i/48000);}
 auto base=clean();base.value[voca::Input]=6;base.value[voca::EqMid]=9;base.value[voca::CompThreshold]=-30;base.value[voca::CompRatio]=10;base.value[voca::LimitThreshold]=-12;base.value[voca::DeessAmount]=100;base.value[voca::DeessFreq]=3000;base.value[voca::Width]=180;for(int i=voca::Reverb;i<=voca::Clipper;++i)base.value[i]=100;base.value[voca::GateThreshold]=-3;base.value[voca::GateRelease]=10;base.value[voca::Maximizer]=100;base.value[voca::Squash]=100;base.value[voca::Imaging]=100;
 voca::Engine e;e.prepare(48000);e.configure(base);auto dryL=sourceL,dryR=sourceR;e.process(dryL.data(),dryR.data(),n);
 for(int m=0;m<voca::NumModules;++m){auto settings=base;settings.enabled[m]=true;e.prepare(48000);e.configure(settings);auto l=sourceL,r=sourceR;e.process(l.data(),r.data(),n);double difference=0;for(int i=n*3/4;i<n;++i)difference+=std::abs(l[i]-dryL[i])+std::abs(r[i]-dryR[i]);if(difference/(n/4)<1e-5)throw std::runtime_error(std::string("Module has no measurable effect: ")+voca::modules[m].name);++checks;}
 for(int c=voca::LowEnd;c<=voca::Air;++c){auto settings=base;settings.value[c]=100;e.prepare(48000);e.configure(settings);auto l=sourceL,r=sourceR;e.process(l.data(),r.data(),n);double difference=0;for(int i=n*3/4;i<n;++i)difference+=std::abs(l[i]-dryL[i])+std::abs(r[i]-dryR[i]);require(difference/(n/4)>1e-5,"Character control must change audio");++checks;}
 }
 {const int n=96000;std::vector<float> l(n),r(n);sine(l,r,48000,230,.15f);auto s=clean();s.value[voca::Tune]=100;voca::Engine e;e.prepare(48000);e.configure(s);e.process(l.data(),r.data(),n);int crossings=0;for(int i=n/2+1;i<n;++i)if(l[i-1]<0&&l[i]>=0)++crossings;require(std::abs(crossings-220)<=3,"Pitch correction must move 230 Hz toward A3 in C major");++checks;}
 require(voca::Pitch::nearest(61,0,0)==60||voca::Pitch::nearest(61,0,0)==62,"C major correction must reject C sharp");++checks;require(voca::Pitch::nearest(61,0,2)==61,"Chromatic correction must permit C sharp");++checks;
 std::cout<<checks<<" DSP checks passed at 44.1/48/96 kHz in "<<std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count()<<" seconds\n";return 0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
