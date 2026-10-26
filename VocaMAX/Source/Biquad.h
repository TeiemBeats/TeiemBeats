#pragma once
#include <algorithm>
#include <array>
#include <cmath>
namespace voca {
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
}
