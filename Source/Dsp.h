#pragma once
#include <algorithm>
#include <array>
#include <cmath>
namespace tg {
constexpr double pi=3.14159265358979323846;
inline float db(float x){return std::pow(10.f,x/20.f);}
struct Biquad {
 double b0=1,b1=0,b2=0,a1=0,a2=0,z1=0,z2=0;
 void reset(){z1=z2=0;}
 float tick(float x){const double y=b0*x+z1;z1=b1*x-a1*y+z2;z2=b2*x-a2*y;return static_cast<float>(y);}
 void set(int type,double hz,double gain,double sr){
  hz=std::clamp(hz,10.,sr*.45);const double w=2*pi*hz/sr,c=std::cos(w),s=std::sin(w),al=s/(2*.70710678),A=std::pow(10.,gain/40.);double aa0=1;
  if(type==0){b0=(1+c)/2;b1=-(1+c);b2=b0;aa0=1+al;a1=-2*c;a2=1-al;}
  else if(type==1){b0=(1-c)/2;b1=1-c;b2=b0;aa0=1+al;a1=-2*c;a2=1-al;}
  else if(type==2){b0=1+al*A;b1=-2*c;b2=1-al*A;aa0=1+al/A;a1=-2*c;a2=1-al/A;}
  else {const double t=2*std::sqrt(A)*al;if(type==3){b0=A*((A+1)-(A-1)*c+t);b1=2*A*((A-1)-(A+1)*c);b2=A*((A+1)-(A-1)*c-t);aa0=(A+1)+(A-1)*c+t;a1=-2*((A-1)+(A+1)*c);a2=(A+1)+(A-1)*c-t;}else{b0=A*((A+1)+(A-1)*c+t);b1=-2*A*((A-1)+(A+1)*c);b2=A*((A+1)+(A-1)*c-t);aa0=(A+1)-(A-1)*c+t;a1=2*((A-1)-(A+1)*c);a2=(A+1)-(A-1)*c-t;}}
  b0/=aa0;b1/=aa0;b2/=aa0;a1/=aa0;a2/=aa0;
 }
};
struct Settings {
 float input=0,balance=0,phase=0,tape=0,hp=20,lp=20000,presence=0,presenceHz=4000,makeup=0,ratio=2,recovery=3,mix=100,width=0,outL=0,outR=0;
 std::array<float,4> freq{91,724,3250,5800},gain{0,0,0,0};
 std::array<int,4> shape{3,2,2,4};
 bool inputOn=true,filterOn=true,limiterOn=true,toneOn=true,polarity=false,transpose=false,sc=false,bypass=false;
 int mode=0,monitor=0;
};
class Engine {
 double sr=44100;std::array<std::array<Biquad,8>,2> filters{};std::array<Biquad,2> sidechain{};float envelope=0,gr=0;Settings p;
 public:
 void prepare(double rate){sr=rate;for(auto& c:filters)for(auto& f:c)f.reset();for(auto& f:sidechain)f.reset();envelope=gr=0;}
 void configure(const Settings& s){p=s;for(int c=0;c<2;++c){filters[c][0].set(0,p.hp,0,sr);filters[c][1].set(1,p.lp,0,sr);filters[c][2].set(2,p.presenceHz,p.presence,sr);for(int i=0;i<4;++i)filters[c][3+i].set(p.shape[i],p.freq[i],p.gain[i],sr);filters[c][7].set(4,4500,p.tape,sr);sidechain[c].set(0,90,0,sr);}}
 float reduction()const{return gr;}
 void process(float* left,float* right,int n){
 const float in=db(p.input),ml=db(p.makeup),ol=db(p.outL),orr=db(p.outR),wet=p.mix*.01f;
 const float attack=static_cast<float>(std::exp(-1./(sr*(p.mode==0?.015:.003))));
 const float release=static_cast<float>(std::exp(-1./(sr*(.05+p.recovery*.09))));
 for(int i=0;i<n;++i){float l=left[i],r=right[i];if(p.bypass)continue;
 if(p.inputOn){l*=in*(p.balance>0?db(-p.balance):1);r*=in*(p.balance<0?db(p.balance):1);if(p.polarity)l=-l;if(p.transpose)std::swap(l,r);const float angle=p.phase*static_cast<float>(pi/180);const float u=l;l=std::cos(angle)*l-std::sin(angle)*r;r=std::sin(angle)*u+std::cos(angle)*r;l=filters[0][7].tick(l);r=filters[1][7].tick(r);}
 if(p.filterOn)for(int f=0;f<3;++f){l=filters[0][f].tick(l);r=filters[1][f].tick(r);}
 if(p.limiterOn){float dl=l,dr=r;if(p.sc){dl=sidechain[0].tick(dl);dr=sidechain[1].tick(dr);}const float peak=std::max(std::abs(dl),std::abs(dr));const float coef=peak>envelope?attack:release;envelope=coef*envelope+(1-coef)*peak;const float level=20*std::log10(std::max(envelope,1e-9f));const float ratio=p.mode==2?100.f:p.ratio;gr=std::max(0.f,(level+12)*(1-1/std::max(1.f,ratio)));const float g=db(-gr)*ml;l=l*(1-wet)+l*g*wet;r=r*(1-wet)+r*g*wet;}else gr=0;
 if(p.toneOn)for(int f=3;f<7;++f){l=filters[0][f].tick(l);r=filters[1][f].tick(r);}
 const float mid=(l+r)*.5f,side=(l-r)*.5f*std::max(0.f,1+p.width/5);l=(mid+side)*ol;r=(mid-side)*orr;
 switch(p.monitor){case 1:r=0;break;case 2:l=0;break;case 3:l=r=mid;break;case 4:l=side;r=-side;break;default:break;}
 left[i]=std::isfinite(l)?l:0;right[i]=std::isfinite(r)?r:0;
 }
 }
};
}
