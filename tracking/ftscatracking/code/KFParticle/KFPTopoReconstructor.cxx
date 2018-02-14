// **************************************************************************
//                                                                          *
// Primary Authors: Ivan Kisel <kisel@kip.uni-heidelberg.de                 *
//                  Igor Kulakov <I.Kulakov@gsi.de>                         *
//                  Maksym Zyzak <M.Zyzak@gsi.de>                           *
//                                                                          *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************

#include "KFPTopoReconstructor.h"
#include "KFParticle.h"
#include "KFVertex.h"
#include "KFPTrack.h"

#ifdef DO_TPCCATRACKER_EFF_PERFORMANCE
#include "PndFTSCAPerformance.h" // dbg
#include "PndFTSCAGlobalPerformance.h" // dbg
#endif

#include <iostream>

void KFPTopoReconstructor::Init(vector<KFPTrack> &tracks, int nParticles)
{
    // copy tracks in particles.
  fNParticles = nParticles;
  fParticles.resize(fNParticles);
  for ( int iTr = 0; iTr < fNParticles; iTr++ ) {
    // {   // check cov matrix TODO
    //   bool ok = true;  
    //   int k = 0;
    //   for (int i = 0; i < 6; i++) {
    //     for (int j = 0; j <= i; j++, k++) {
    //       ok &= finite( KFPCov[k] );
    //     }
    //     ok &= ( KFPCov[k-1] > 0 );
    //   }
    //   if (!ok) continue;
    // }
        
    fParticles[iTr] = KFParticle( tracks[iTr], 211 ); // pi+ // TODO include PDG in KFPTrack?
    fParticles[iTr].AddDaughterId(tracks[iTr].Id());
  }
} // void KFPTopoReconstructor::Init

void KFPTopoReconstructor::FindPrimaryClusters()
{
  // The function finds a set of clusters of tracks.
  // Tracks are assumed to be transported to the beam line.

  vector<unsigned short int> notUsedTracks(fNParticles);
  for(int iTr=0; iTr<fNParticles; iTr++)
    notUsedTracks[iTr] = iTr;

  while(notUsedTracks.size()>0)
  {
    short int bestTrack = 0;
    float bestWeight = -1.f;

    vector<unsigned short int> notUsedTracksNew;

    for(unsigned short int iTr = 0; iTr < notUsedTracks.size(); iTr++)
    {
      unsigned short int &curTrack = notUsedTracks[iTr];
      float dX = fParticles[curTrack].CovarianceMatrix()[0];
      float dY = fParticles[curTrack].CovarianceMatrix()[2];
      float dZ = fParticles[curTrack].CovarianceMatrix()[5];

      float weight = 1.f/sqrt(dX + dY + dZ);
      if (weight > bestWeight)
      {
        bestWeight = weight;
        bestTrack = curTrack;
      }
    }
// std::cout << "best " << bestWeight << " " << bestTrack << std::endl;

    KFParticleCluster cluster;

    const double *rBest = fParticles[bestTrack].Parameters();
    const double *covBest = fParticles[bestTrack].CovarianceMatrix();

    float rVertex[3] = {0.f};
    float covVertex[6] = {0.f};
    float weightVertex = 0.f;
/*std::cout << "tatata " << std::endl;*/
    for(unsigned short int iTr = 0; iTr < notUsedTracks.size(); iTr++)
    {
      unsigned short int &curTrack = notUsedTracks[iTr];

      double dr[3] = {rBest[0] - fParticles[curTrack].X(),
                     rBest[1] - fParticles[curTrack].Y(),
                     rBest[2] - fParticles[curTrack].Z() };

      double cov[6] = {covBest[0] + fParticles[curTrack].CovarianceMatrix()[0],
                      covBest[1] + fParticles[curTrack].CovarianceMatrix()[1],
                      covBest[2] + fParticles[curTrack].CovarianceMatrix()[2],
                      covBest[3] + fParticles[curTrack].CovarianceMatrix()[3],
                      covBest[4] + fParticles[curTrack].CovarianceMatrix()[4],
                      covBest[5] + fParticles[curTrack].CovarianceMatrix()[5] };

      float dr2 = static_cast<float>(dr[0]*dr[0] + dr[1]*dr[1] + dr[2]*dr[2]);
      float drError2 = static_cast<float>(
        dr[0]* (cov[0]* dr[0] + cov[1]* dr[1] + cov[3]* dr[2]) +
        dr[1]* (cov[1]* dr[0] + cov[2]* dr[1] + cov[4]* dr[2]) + 
        dr[2]* (cov[3]* dr[0] + cov[4]* dr[1] + cov[5]* dr[2])
                                         );
// std::cout << "dr " << sqrt(dr2) << " drError " << sqrt(drError2/dr2) << std::endl;

      if( dr2*dr2 <= (10*10 * drError2) )
      {
        float dX = fParticles[curTrack].CovarianceMatrix()[0];
        float dY = fParticles[curTrack].CovarianceMatrix()[2];
        float dZ = fParticles[curTrack].CovarianceMatrix()[5];

        float weight = 1.f/sqrt(dX + dY + dZ);

        for(int iP=0; iP<3; iP++)
          rVertex[iP] += weight * fParticles[curTrack].Parameters()[iP];

        for(int iC=0; iC<6; iC++)
          covVertex[iC] += weight * weight *fParticles[curTrack].CovarianceMatrix()[iC];

        weightVertex += weight;
        cluster.fCluster.push_back(curTrack);
      }
      else
      {
//         int iMC = PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->GetRecoData()[fParticles[curTrack].DaughterIds()[0]].GetMCTrackId();
//         int MCPDG = -1;
//         int PDG = -1;
// 
//         if(iMC>0)
//         {
// 
//           int motherID = (*PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->fMCTracks)[iMC].MotherId();
//           PDG = (*PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->fMCTracks)[iMC].PDG();
// 
//           if(motherID > 0)
//             MCPDG = (*PndFTSCAPerformance::Instance().GetSubPerformance("Global Performance")->fMCTracks)[motherID].PDG();
//         }
// 
//         std::cout << "Bad PDG   " << PDG << " MC PDG "<<  MCPDG << std::endl;
        notUsedTracksNew.push_back(curTrack);
      }
    }

    notUsedTracks = notUsedTracksNew;

    if(cluster.fCluster.size() < 2) continue;
    if((cluster.fCluster.size() < 3) && (fNParticles>3)) continue;

    for(int iP=0; iP<3; iP++)
      cluster.fP[iP] = rVertex[iP]/weightVertex;

    for(int iC=0; iC<6; iC++)
      cluster.fC[iC] = covVertex[iC]/(weightVertex*weightVertex);
// std::cout << "clust   " << cluster.fP[0] << " " << cluster.fP[1] << " " << cluster.fP[2] << " " << cluster.fCluster.size() << " " << notUsedTracksNew.size() << std::endl;
// std::cout << "Cov     " << cluster.fC[0] << " " 
//                         << cluster.fC[1] << " " << cluster.fC[2] << " "
//                         << cluster.fC[3] << " " << cluster.fC[4] << " " << cluster.fC[5] << std::endl;
// int ui;
// std::cin >> ui;

    fClusters.push_back(cluster);
  }
//   if(fClusters.size()>1)
//   {
//     float dx = fClusters[0].fP[0] - fClusters[1].fP[0];
//     float dy = fClusters[0].fP[1] - fClusters[1].fP[1];
//     float dz = fClusters[0].fP[2] - fClusters[1].fP[2];
// 
//     float dr[3] = {dx, dy, dz};
//     float cov[6] = {fClusters[0].fC[0] + fClusters[1].fC[0],
//                     fClusters[0].fC[1] + fClusters[1].fC[1],
//                     fClusters[0].fC[2] + fClusters[1].fC[2],
//                     fClusters[0].fC[3] + fClusters[1].fC[3],
//                     fClusters[0].fC[4] + fClusters[1].fC[4],
//                     fClusters[0].fC[5] + fClusters[1].fC[5] };
//       float dr2 = dr[0]*dr[0] + dr[1]*dr[1] + dr[2]*dr[2];
//       float drError2 = dr[0]* (cov[0]* dr[0] + cov[1]* dr[1] + cov[3]* dr[2]) +
//                        dr[1]* (cov[1]* dr[0] + cov[2]* dr[1] + cov[4]* dr[2]) + 
//                        dr[2]* (cov[3]* dr[0] + cov[4]* dr[1] + cov[5]* dr[2]);
//     drError2 /= dr2;
//     std::cout << " dr2 " << dr2 << " Err " << drError2 << std::endl;
//   }
// int ui;
// std::cin >> ui;

  static int nVert[10]={0};
  if(fClusters.size()<10)
    nVert[fClusters.size()]++;
  std::cout << "N Vert     ";
  for(int i=0; i<10; i++)
    std::cout << i << ": " << nVert[i] << "     ";
  std::cout << std::endl;

  static int nPart[10]={0};
  if(fNParticles<10)
    nPart[fNParticles]++;
  std::cout << "N Part     ";
  for(int i=0; i<10; i++)
    std::cout << i << ": " << nPart[i] << "     ";
  std::cout << std::endl;
}

void KFPTopoReconstructor::ReconstructPrimVertex()
{

  FindPrimaryClusters();

    // select particles for Primary vertex reco
  const KFParticle **pParticles = new const KFParticle*[fNParticles]; // tmp array
  
  int nPrimCand = 0;
  for ( int iP = 0; iP < fNParticles; iP++ ) {
    KFParticle &p = fParticles[iP];
 //   if ( p->GetPt() < 1.f ) continue;
 //   if ( p->GetNDF() < 40-5 ) continue; // at least 20 hits in track // dbg
 //   if ( p->GetChi2() > 2.f * p->GetNDF() ) continue; // dbg
    
    pParticles[nPrimCand++] = &p;
  }

    // find prim vertex
  KFVertex primVtx;

  if(fNParticles>1)
  {
    bool *vFlags = new bool[fNParticles];  // flags returned by the vertex finder
    primVtx.ConstructPrimaryVertex( pParticles, 2, vFlags, 10.f );
    if( primVtx.GetNDF() >= 1 )
      fPrimVertices.push_back(primVtx);

    if(vFlags) delete [] vFlags;
  }

  
  if ( fPrimVertices.size() == 0 ) { // fill prim vertex by dummy values
    KFPVertex primVtx_tmp;
    primVtx_tmp.SetXYZ(0, 0, 0);
    primVtx_tmp.SetCovarianceMatrix( 0, 0, 0, 0, 0, 0 );
    primVtx_tmp.SetNContributors(0);
    primVtx_tmp.SetChi2(-100);
    
    fPrimVertices.push_back(primVtx_tmp);
  }

  if(pParticles) delete [] pParticles;
  // if ( fPrimVertices.size() > 0 ) { // dbg
  //   std::cout << std::endl << " PrimVertex " << std::endl;
  //   std::cout << fPrimVertices[0] << std::endl << std::endl;
  // }
} // void KFPTopoReconstructor::Run()
