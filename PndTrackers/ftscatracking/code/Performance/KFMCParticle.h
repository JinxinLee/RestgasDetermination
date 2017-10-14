/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction
 *
 *  Authors: M.Zyzak
 *
 *  e-mail :
 *
 *=====================================================
 *
 *  Finds Particles: Lambdas, K0
 *
 */

#ifndef _KFMCParticle_h_
#define _KFMCParticle_h_

#include <vector>
using std::vector;

class KFMCParticle
{
 public:
  KFMCParticle();
  ~KFMCParticle();

  void AddDaughter( int i );
  int  NDaughters() const { return fDaughterIds.size(); };
  const vector<int>&  GetDaughterIds() const { return fDaughterIds; };
  void FindCommonMC();

  void SetPDG(int pdg) {fPDG = pdg;}
  void SetMCTrackID(int id) {fMCTrackID = id;}
  void SetMotherId(int id) {fMotherId = id;}
  
  int  GetMCTrackID()      const {return fMCTrackID;}
  int  GetMotherId()       const {return fMotherId;}
  int  GetPDG()            const {return fPDG;}
  bool IsReconstructable() const {return fIsReconstructable;}
  void SetAsReconstructable() { fIsReconstructable = 1;}
  
  void CalculateIsReconstructable(unsigned int NDaughters = 2);
  void CalculateIsRecRec(unsigned int NDaughters = 2);
  
 private: //data
  vector<int> fDaughterIds;
  int fMCTrackID; // sim id of MC track, which corresponds to the particle
  int fMotherId;  // index in L1 array of mother particle
  int fPDG;
  
  bool fIsReconstructable;
};

#endif

