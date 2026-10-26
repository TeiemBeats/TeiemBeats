#pragma once
#include "Controls.h"
#include "Biquad.h"
#include <vector>
#include <numeric>
namespace voca {
struct Settings {
 std::array<float,NumKnobs> value{};std::array<bool,NumModules> enabled{};std::array<int,10> choice{};bool bypass=false;double bpm=120;
 Settings(){for(int i=0;i<NumKnobs;++i)value[i]=specs[i].initial;enabled.fill(true);for(int i=0;i<10;++i)choice[i]=choices[i].initial;}
};
struct Ring {
 std::vector<float> data;size_t pos=0;
 void prepare(size_t n){data.assign(n,0);pos=0;}
 void write(float v){data[pos]=v;pos=(pos+1)%data.size();}
 float read(float delay)const{float index=static_cast<float>(pos)-delay;while(index<0)index+=static_cast<float>(data.size());const auto a=static_cast<size_t>(index)%data.size(),b=(a+1)%data.size();const float f=index-std::floor(index);return data[a]+f*(data[b]-data[a]);}
};
class Pitch {
 std::array<float,1024> history{};int position=0,decimate=0,division=4,detectorCount=0;double detectorRate=12000;float detected=0,confidence=0;
 std::array<Ring,2> ring;float phase=.5f,ratio=1;double sampleRate=48000;Biquad detectorFilter;
 public:
 static constexpr int latency=2048;
 void prepare(double sr){sampleRate=sr;division=std::max(1,static_cast<int>(std::round(sr/12000)));detectorRate=sr/division;history.fill(0);position=decimate=detectorCount=0;detected=confidence=0;phase=.5f;ratio=1;detectorFilter.reset();detectorFilter.set(1,1800,0,sr);for(auto& r:ring)r.prepare(8192);}
 static float nearest(float midi,int key,int scale){const std::array<int,7> major{0,2,4,5,7,9,11},minor{0,2,3,5,7,8,10};const std::array<int,5> pent{0,2,4,7,9};float best=std::round(midi),distance=100;
 for(int n=static_cast<int>(std::floor(midi))-12;n<=static_cast<int>(std::ceil(midi))+12;++n){int degree=(n-key)%12;if(degree<0)degree+=12;bool allowed=scale==2;if(scale==0)allowed=std::find(major.begin(),major.end(),degree)!=major.end();if(scale==1)allowed=std::find(minor.begin(),minor.end(),degree)!=minor.end();if(scale==3)allowed=std::find(pent.begin(),pent.end(),degree)!=pent.end();if(allowed&&std::abs(n-midi)<distance){best=static_cast<float>(n);distance=std::abs(n-midi);}}return best;}
 float frequency()const{return detected;}
 void analyse(){constexpr int window=384;const int minLag=std::max(2,static_cast<int>(detectorRate/900)),maxLag=std::min(500,static_cast<int>(detectorRate/65));std::array<float,512> diff{};float sum=0,energy=0;for(int i=0;i<window;++i){const float x=history[(position+i)%1024];energy+=x*x;}if(energy/window<1e-6f){detected=confidence=0;return;}
 int candidate=-1;for(int lag=1;lag<=maxLag;++lag){float difference=0;for(int i=0;i<window;++i){const float d=history[(position+i)%1024]-history[(position+i+lag)%1024];difference+=d*d;}sum+=difference;diff[lag]=sum>1e-12f?difference*lag/sum:1;if(lag>minLag&&diff[lag-1]<.15f&&diff[lag]>diff[lag-1]){candidate=lag-1;break;}}
 if(candidate<0){detected=confidence=0;return;}float lag=static_cast<float>(candidate);if(candidate>1){const float a=diff[candidate-1],b=diff[candidate],c=diff[candidate+1];const float denominator=2*(a-2*b+c);if(std::abs(denominator)>1e-8f)lag+=(a-c)/denominator;}detected=static_cast<float>(detectorRate)/lag;confidence=1-diff[candidate];}
 void tick(float& l,float& r,float amount,int key,int scale){const float mono=detectorFilter.tick((l+r)*.5f);if(++decimate>=division){decimate=0;history[position]=mono;position=(position+1)%1024;if(++detectorCount>=128){detectorCount=0;analyse();}}
 float desired=1;if(detected>0&&confidence>.8f&&amount>0){const float midi=69+12*std::log2(detected/440.f);desired=std::pow(2.f,(nearest(midi,key,scale)-midi)*amount/1200.f);}ratio+=.001f*(desired-ratio);phase+=(1-ratio)/2048.f;if(phase<0)phase+=1;if(phase>=1)phase-=1;
 const float p2=phase<.5f?phase+.5f:phase-.5f;const float w=std::pow(std::sin(static_cast<float>(pi)*phase),2.f);const float blend=amount>0&&detected>0?std::clamp(std::abs(ratio-1)*1000,0.f,1.f):0.f;float* samples[2]{&l,&r};for(int c=0;c<2;++c){ring[c].write(*samples[c]);const float dry=ring[c].read(latency+1.f);const float shifted=w*ring[c].read(latency+1+(phase-.5f)*2048)+(1-w)*ring[c].read(latency+1+(p2-.5f)*2048);*samples[c]=dry+(shifted-dry)*blend;}
 }
};
class ReverbTank {
 struct Comb {Ring line;float damp=0;};std::array<std::array<Comb,4>,2> combs;
 public:void prepare(double sr){const float times[4]{.0297f,.0371f,.0411f,.0437f};for(int c=0;c<2;++c)for(int j=0;j<4;++j){combs[c][j].line.prepare(static_cast<size_t>((times[j]+c*.0011f)*sr));combs[c][j].damp=0;}}
 void tick(float& l,float& r,int type){float out[2]{};for(int c=0;c<2;++c)for(auto& comb:combs[c]){const float delayed=comb.line.data[comb.line.pos];comb.damp=.75f*comb.damp+.25f*delayed;comb.line.write((c?r:l)*.18f+comb.damp*(type==0?.73f:type==1?.61f:.84f));out[c]+=delayed;}l=out[0];r=out[1];}
};
class Engine {
 double sr=48000;Settings target;std::array<float,NumKnobs> p{};std::array<float,NumModules> wet{};float smoothing=.002f;
 std::array<std::array<Biquad,8>,2> filters;std::array<Biquad,2> deessFilter,dcFilter;Pitch pitch;ReverbTank reverb;
 std::array<Ring,2> delayLine,flangeLine,bypassLine;float compEnv=0,compGain=1,gateEnv=0,gateGain=0,deessEnv=0,limitGain=1,flangePhase=0;int update=0;float gr=0;
 static float blend(float dry,float effect,float w){return dry+(effect-dry)*w;}
 void updateFilters(){const float freq[5]{60,250,1200,5000,12000};for(int c=0;c<2;++c){for(int i=0;i<5;++i)filters[c][i].set(i==0?3:i==4?4:2,freq[i],p[EqLow+i],sr);filters[c][5].set(3,110,p[LowEnd]*.06,sr);filters[c][6].set(2,3000,p[Clarity]*.055,sr);filters[c][7].set(4,12000,p[Air]*.06,sr);deessFilter[c].set(0,p[DeessFreq],0,sr);}}
 public:
 static constexpr int latency=Pitch::latency;
 void prepare(double rate){sr=rate;target=Settings{};for(int i=0;i<NumKnobs;++i)p[i]=target.value[i];wet.fill(1);smoothing=static_cast<float>(1-std::exp(-1/(sr*.015)));for(auto& c:filters)for(auto& f:c)f.reset();for(int c=0;c<2;++c){deessFilter[c].reset();dcFilter[c].reset();dcFilter[c].set(0,15,0,sr);delayLine[c].prepare(static_cast<size_t>(sr*3+4));flangeLine[c].prepare(static_cast<size_t>(sr*.03+4));bypassLine[c].prepare(8192);}pitch.prepare(sr);reverb.prepare(sr);compEnv=gateEnv=deessEnv=gr=0;compGain=limitGain=1;gateGain=0;flangePhase=0;update=0;updateFilters();}
 void configure(const Settings& s){target=s;for(int i=0;i<NumKnobs;++i)target.value[i]=std::clamp(target.value[i],specs[i].min,specs[i].max);}
 float reduction()const{return gr;}float pitchHz()const{return pitch.frequency();}
 void process(float* left,float* right,int count){const float sampleAttack=static_cast<float>(std::exp(-1/(sr*.001)));const float gateAttack=static_cast<float>(std::exp(-1/(sr*.002)));const float limiterRelease=static_cast<float>(std::exp(-1/(sr*.08)));
 for(int sample=0;sample<count;++sample){for(int i=0;i<NumKnobs;++i)p[i]+=smoothing*(target.value[i]-p[i]);for(int i=0;i<NumModules;++i)wet[i]+=smoothing*((target.enabled[i]?1.f:0.f)-wet[i]);if(++update>=64){update=0;updateFilters();}
 float l=left[sample],r=right[sample];if(!std::isfinite(l))l=0;if(!std::isfinite(r))r=0;float dry[2];bypassLine[0].write(l);bypassLine[1].write(r);dry[0]=bypassLine[0].read(latency+1.f);dry[1]=bypassLine[1].read(latency+1.f);
 l*=db(p[Input]);r*=db(p[Input]);pitch.tick(l,r,p[Tune],target.choice[0],target.choice[1]);
 float detector=std::max(std::abs(l),std::abs(r));gateEnv=sampleAttack*gateEnv+(1-sampleAttack)*detector;const float gateTarget=gateEnv>db(p[GateThreshold])?1.f:0.f;const float gateCoefficient=gateTarget>gateGain?gateAttack:static_cast<float>(std::exp(-1/(sr*p[GateRelease]*.001)));gateGain=gateCoefficient*gateGain+(1-gateCoefficient)*gateTarget;l*=blend(1,gateGain,wet[Gate]);r*=blend(1,gateGain,wet[Gate]);
 for(int i=0;i<5;++i){l=blend(l,filters[0][i].tick(l),wet[EQ]);r=blend(r,filters[1][i].tick(r),wet[EQ]);}
 detector=std::max(std::abs(l),std::abs(r));const float coefficient=static_cast<float>(std::exp(-1/(sr*(detector>compEnv?p[CompAttack]:p[CompRelease])*.001)));compEnv=coefficient*compEnv+(1-coefficient)*detector;const float level=20*std::log10(std::max(compEnv,1e-9f));gr=std::max(0.f,(level-p[CompThreshold])*(1-1/p[CompRatio]));compGain=db(-gr);l*=blend(1,compGain,wet[Compression]);r*=blend(1,compGain,wet[Compression]);
 const float highL=deessFilter[0].tick(l),highR=deessFilter[1].tick(r);const float high=std::max(std::abs(highL),std::abs(highR));deessEnv=.99f*deessEnv+.01f*high;const float deessGain=1/(1+std::max(0.f,deessEnv-.025f)*p[DeessAmount]*.7f);l-=highL*(1-deessGain)*wet[Deesser];r-=highR*(1-deessGain)*wet[Deesser];
 float mid=(l+r)*.5f,side=(l-r)*.5f;const float width=blend(1,p[Width]*.01f,wet[Stereo]);side*=width;l=mid+side;r=mid-side;
 auto colour=[&](float x){float y=x;const float heat=p[Heat]*.01f*wet[HeatModule];y=blend(y,std::tanh(y*(1+3*heat))/(1+heat),heat*.65f);const float drive=p[Distortion]*.01f*wet[DistortionModule];float distorted=std::tanh(y*(1+8*drive));if(target.choice[6]==1)distorted=(2/static_cast<float>(pi))*std::atan(y*(2+20*drive));if(target.choice[6]==2)distorted=std::sin(y*(1+10*drive));y=blend(y,distorted,drive*.65f);const float warmth=p[Warmth]*.01f*wet[WarmthModule];const float asym=target.choice[7]==1?.15f:.04f;y=blend(y,(std::tanh((y+asym)*(1+warmth))-std::tanh(asym*(1+warmth)))/(1+warmth*.3f),warmth*.65f);const float dyn=p[Dynamics]*.01f*wet[DynamicsModule];const float transient=std::clamp(detector/(compEnv+.01f)-1,-1.f,2.f);y*=1+dyn*transient*(target.choice[8]==1?.25f:target.choice[8]==2?.05f:.12f);const float sat=p[Saturation]*.01f*wet[SaturationModule];const float satDrive=target.choice[9]==2?5.f:3.f;y=blend(y,std::tanh(y*(1+satDrive*sat))/(1+sat*.7f),sat*.5f);return y;};l=colour(l);r=colour(r);
 const float rate=target.choice[5]==0?.45f:target.choice[5]==1?.12f:.25f;flangePhase+=rate/static_cast<float>(sr);if(flangePhase>=1)flangePhase-=1;float* channels[2]{&l,&r};for(int c=0;c<2;++c){float& v=*channels[c];const float ms=(target.choice[5]==2?5.f:3.f)+2.f*std::sin(2*static_cast<float>(pi)*flangePhase+c*.6f);const float flanged=flangeLine[c].read(ms*static_cast<float>(sr)*.001f);flangeLine[c].write(v+flanged*.2f);v+=flanged*p[Flanger]*.006f*wet[FlangerModule];}
 const float beat=static_cast<float>(60/std::clamp(target.bpm,20.,300.));const float divisions[5]{.25f,.5f,1,2,.75f};const float delaySamples=std::min(static_cast<float>(sr*2.9),beat*divisions[std::clamp(target.choice[4],0,4)]*static_cast<float>(sr));for(int c=0;c<2;++c){float& v=*channels[c];const float echo=delayLine[c].read(delaySamples);delayLine[c].write(v+echo*.32f);v+=echo*p[Delay]*.006f*wet[DelayModule];}
 float rl=l,rr=r;reverb.tick(rl,rr,target.choice[3]);l+=rl*p[Reverb]*.008f*wet[ReverbModule];r+=rr*p[Reverb]*.008f*wet[ReverbModule];
 for(int i=5;i<8;++i){l=filters[0][i].tick(l);r=filters[1][i].tick(r);}const float punch=std::clamp(detector/(compEnv+.01f)-1,-1.f,2.f);const float punchGain=1+p[Punch]*.002f*punch;l*=punchGain;r*=punchGain;const float harm=p[Harmonics]*.01f;l=blend(l,std::tanh(l*2),harm*.3f);r=blend(r,std::tanh(r*2),harm*.3f);
 mid=(l+r)*.5f;side=(l-r)*.5f*(1+p[CharacterWidth]*.007f+p[Imaging]*.005f*wet[ImagingModule]);l=mid+side;r=mid-side;
 const float squash=p[Squash]*.01f*wet[SquashModule];l=blend(l,std::tanh(l*2)/(1.1f),squash*.6f);r=blend(r,std::tanh(r*2)/(1.1f),squash*.6f);const float clip=p[Clipper]*.01f*wet[ClipperModule];const float clipLevel=1-clip*.65f;l=blend(l,std::clamp(l,-clipLevel,clipLevel),clip);r=blend(r,std::clamp(r,-clipLevel,clipLevel),clip);const float boost=db(p[Maximizer]*.1f*wet[MaximizerModule]+p[Output]);l=dcFilter[0].tick(l)*boost;r=dcFilter[1].tick(r)*boost;
 const float ceiling=db(std::min(p[LimitThreshold],target.value[LimitThreshold]));detector=std::max(std::abs(l),std::abs(r));const float desired=detector>ceiling?ceiling/std::max(detector,1e-9f):1.f;limitGain=desired<limitGain?desired:limiterRelease*limitGain+(1-limiterRelease)*desired;l=blend(l,l*limitGain,wet[Limiter]);r=blend(r,r*limitGain,wet[Limiter]);
 left[sample]=target.bypass?dry[0]:(std::isfinite(l)?l:0);right[sample]=target.bypass?dry[1]:(std::isfinite(r)?r:0);
 }
 }
};
}
