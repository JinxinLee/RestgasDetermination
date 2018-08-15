#include "PWO25SimHit.hh"
ClassImp(PWO25SimHit)

PWO25SimHit::PWO25SimHit(int module, double energy,int trackId) {
  fModule = module;
  fE = energy;
  track_ = trackId;
}

PWO25SimHit::~PWO25SimHit() {
}

