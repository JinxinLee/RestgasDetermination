#ifndef KFPTopoReconstructor_H
#define KFPTopoReconstructor_H

/*
 * Class for Event Topology Reconstructing
 */

#include "KFVertex.h"
#include "assert.h"

#include "KFPTrack.h"
#include <vector>
using std::vector;

class KFParticle;

class KFPTopoReconstructor{
 public:
  KFPTopoReconstructor(){};
  ~KFPTopoReconstructor(){};
  
  void Init(vector<KFPTrack> &tracks, int nParticles); // init array of particles
  
  void ReconstructPrimVertex(); // find primary vertex

     /// Accessors
  KFParticle &GetPrimVertex()   { assert( fPrimVertices.size() > 0 ); return fPrimVertices[0]; };
  KFParticle &GetParticle(int i){ assert( i < fNParticles );          return fParticles[i];    };
  
 private:
  KFPTopoReconstructor &operator=(KFPTopoReconstructor &);
  KFPTopoReconstructor(KFPTopoReconstructor &);

  void FindPrimaryClusters();

  vector<KFParticle> fParticles; // input particles
  int fNParticles;           // number of input particles

  struct KFParticleCluster {
    vector<short int> fCluster;
    float fP[3];
    float fC[6];
  };

  vector< KFParticleCluster > fClusters;
  vector<KFVertex> fPrimVertices;  // created primary vertex(-es) (currently only one primary vertex in possible
}; // class KFPTopoReconstructor


#endif // KFPTopoReconstructor_H
  
