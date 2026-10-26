#include "Dsp.h"
#include <iostream>
#include <stdexcept>
#include <vector>
void require(bool ok,const char* why){if(!ok)throw std::runtime_error(why);}
int main(){try{int count=0;for(double sr:{44100.,48000.,96000.}){tg::Engine e;tg::Settings s;s.bypass=true;e.prepare(sr);e.configure(s);std::vector<float> l(4096,.25f),r(4096,-.2f);e.process(l.data(),r.data(),4096);require(l.back()==.25f&&r.back()==-.2f,"Bypass must be bit exact");++count;
 s.bypass=false;s.filterOn=s.limiterOn=s.toneOn=false;s.input=6;e.prepare(sr);e.configure(s);l.assign(4096,.25f);r=l;e.process(l.data(),r.data(),4096);require(std::abs(l.back()-.25f*tg::db(6))<1e-5,"Input gain");++count;
 s.input=0;s.width=-5;e.prepare(sr);e.configure(s);l.assign(4096,.4f);r.assign(4096,-.4f);e.process(l.data(),r.data(),4096);require(std::abs(l.back())<1e-6&&std::abs(r.back())<1e-6,"Zero width folds stereo to mono");++count;
 s.width=0;s.limiterOn=true;s.ratio=10;e.prepare(sr);e.configure(s);l.assign(65536,.9f);r=l;e.process(l.data(),r.data(),65536);require(l.back()<.4f&&e.reduction()>8,"Compressor attenuates above threshold");++count;
 s.limiterOn=false;s.inputOn=false;s.filterOn=true;s.hp=110;s.lp=20000;for(int i=0;i<65536;++i)l[static_cast<size_t>(i)]=r[static_cast<size_t>(i)]=.5f*std::sin(2*tg::pi*20*i/sr);e.prepare(sr);e.configure(s);e.process(l.data(),r.data(),65536);double energy=0;for(int i=32768;i<65536;++i)energy+=l[static_cast<size_t>(i)]*l[static_cast<size_t>(i)];require(std::sqrt(energy/32768)<.03,"High pass attenuates 20Hz");++count;
 s.filterOn=false;s.toneOn=true;s.freq[1]=1000;s.gain[1]=6;for(int i=0;i<65536;++i)l[static_cast<size_t>(i)]=r[static_cast<size_t>(i)]=.1f*std::sin(2*tg::pi*1000*i/sr);e.prepare(sr);e.configure(s);e.process(l.data(),r.data(),65536);energy=0;for(int i=32768;i<65536;++i)energy+=l[static_cast<size_t>(i)]*l[static_cast<size_t>(i)];require(std::sqrt(energy/32768)>.13,"EQ boosts selected band");++count;
 for(float x:l)require(std::isfinite(x),"Output finite");++count;
 }std::cout<<count<<" DSP checks passed at 44.1, 48 and 96 kHz\n";return 0;}catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
