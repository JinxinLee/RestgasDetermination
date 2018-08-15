#include "PWO25SimEvent.hh"
#include "PWO25SimHit.hh"
#include "TClonesArray.h"

ClassImp(PWO25SimEvent)

// Implementation of SimEvent
PWO25SimEvent::PWO25SimEvent() {
  fEntries = 0;
  fHits = new TClonesArray("PWO25SimHit",20);
}

PWO25SimEvent::~PWO25SimEvent() {
  delete fHits;
}

void PWO25SimEvent::Add(int module, double energy,int trackId) {

  PWO25SimHit* newHit = new PWO25SimHit(module, energy,trackId);
  (*fHits)[fEntries++] = newHit;

}

void PWO25SimEvent::Clear(const Option_t* opt) {
  fHits->Clear();
  fEntries=0;
}
