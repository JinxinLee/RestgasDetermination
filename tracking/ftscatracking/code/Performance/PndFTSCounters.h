/**
 *       Alice HTL TPC counters for efficiency calculation
 **/


#ifndef PNDFTSCounters_H
#define PNDFTSCounters_H

#include "Counters.h"
#include "PndFTSCAParameters.h"

#include <iostream>
using std::cout;
using std::endl;
using std::ios;

#include <string>
using std::string;
         
#include <vector>
using std::vector;
         
#include <map>
using std::map;

struct PndFTSEfficiencies: public TEfficiencies
{
  PndFTSEfficiencies():TEfficiencies(){

    AddCounter("ref_prim_long" ,"LRefPrim  efficiency");
    AddCounter("ref_prim"      ,"RefPrim   efficiency");
    AddCounter("ref_sec"       ,"RefSec    efficiency");
    AddCounter("ref"           ,"Refset    efficiency");
    AddCounter("extra_prim"    ,"ExtraPrim efficiency");
    AddCounter("extra_sec"     ,"ExtraSec  efficiency");
    AddCounter("extra"         ,"Extra     efficiency");
    AddCounter("total"         ,"Allset    efficiency");
    AddCounter("rest"          ,"Rest      efficiency");
  }

  virtual void AddCounter(string shortname, string name){
    TEfficiencies::AddCounter(shortname, name);
    ratio_killed.AddCounter();
    ratio_clone.AddCounter();
    ratio_length.AddCounter();
    ratio_fakes.AddCounter();
    killed.AddCounter();
    clone.AddCounter();
    reco_length.AddCounter();
    reco_fakes.AddCounter();
  }

  PndFTSEfficiencies& operator+=(PndFTSEfficiencies& a){
    TEfficiencies::operator+=(a);
    killed += a.killed; clone += a.clone;
    reco_length += a.reco_length;
    reco_fakes  += a.reco_fakes;
    return *this;
  }

  void CalcEff(){
    TEfficiencies::CalcEff();
    ratio_killed = killed/mc;
    ratio_clone  = clone/mc;
    TTracksCatCounters<int> allReco = reco + clone;
    ratio_length = reco_length/allReco;
    ratio_fakes  = reco_fakes/allReco;
  }

  void Inc(bool isReco, bool isKilled, double _ratio_length, double _ratio_fakes, int _nclones, string name){
    TEfficiencies::Inc(isReco, name);

    const int index = indices[name];

    if (isKilled) killed.counters[index]++;
    reco_length.counters[index] += _ratio_length;
    reco_fakes.counters[index] += _ratio_fakes;
    clone.counters[index] += _nclones;
  }

  void Print(){

    //save original cout flags
    std::ios_base::fmtflags coutFlags = cout.flags();
    std::cout.setf(ios::fixed);
    std::cout.setf(ios::showpoint);
    std::cout.precision(3);
    std::cout << "Track category         : " << " Eff  "
              //<<" / "<< "Killed"
              <<" / "<< "Length" <<" / "<< "Fakes " <<" / "<< "Clones" <<" / "<< "All Reco" <<" | "<< "All MC"  << std::endl;

    int NCounters = mc.NCounters;
    for (int iC = 0; iC < NCounters; iC++){
      if (( names[iC] != "D0        efficiency") || (mc.counters[iC] != 0))
        std::cout << names[iC]  << "   : "
             << ratio_reco.counters[iC]              
             //<< "  / " << ratio_killed.counters[iC]  // tracks with aren't reco because other tracks takes their hit(-s)
             << "  / " << ratio_length.counters[iC]  // nRecoMCHits/nMCHits
             << "  / " << ratio_fakes.counters[iC]   // nFakeHits/nRecoAllHits
             << "  / " << ratio_clone.counters[iC]   // nCloneTracks/nMCTracks
             << "  / " << double(reco.counters[iC])/double(nEvents)
             << "    | " << double(mc.counters[iC])/double(nEvents)  << std::endl;
    }
    std::cout << "Ghost     probability  : " << ratio_ghosts <<" | "<< double(ghosts)/double(nEvents) << std::endl;
    std::cout << "All reco tracks/ev : " << int(double(reco.counters[indices["total"]])/double(nEvents)+.5)  << endl;

    //restore original cout flags
    cout.flags(coutFlags);
  }
  
  TTracksCatCounters<double> ratio_killed;
  TTracksCatCounters<double> ratio_clone;
  TTracksCatCounters<double> ratio_length;
  TTracksCatCounters<double> ratio_fakes;

  TTracksCatCounters<int> killed;
  TTracksCatCounters<int> clone;
  TTracksCatCounters<double> reco_length;
  TTracksCatCounters<double> reco_fakes;
};

  /// Information about reconstruction of MCTrack
class PndFTSCAPerformanceMCTrackData{
  public:
    PndFTSCAPerformanceMCTrackData(){
      set = 0;
      isReconstructable = 0;
    };

    void SetSet(int set_){ set = set_; }
    void SetAsReconstructable(){ isReconstructable = true; }
  void AddReconstructed(int itr = 0){ recoTrackIds.push_back(itr); }

    int  GetSet(){ return set; }
    bool IsReconstructable(){ return isReconstructable; }
    bool IsReconstructed(){ return recoTrackIds.size() >= 1; }
    bool GetNClones(){ return (recoTrackIds.size() > 1) ? recoTrackIds.size() - 1 : 0; }

  const std::vector<int>& RecoTrackIds() const { return recoTrackIds; }
    void Print(){ cout << "Set: " << set << " RecoAble: " << isReconstructable << " NReco: " << recoTrackIds.size() << endl; }
  private:
    int  set;                // set of tracks 0-OutSet, 1-ExtraSet, 2-RefSet, 3-ExtraSecSet, 4-ExtraPrimSet, 5-RefSecSet, 6-RefPrimSet, 7-LongRefPrimSet
    bool isReconstructable;

  std::vector<int> recoTrackIds;
};

  /// Information about reconstruction of Reconstructed Track
class PndFTSCAPerformanceRecoTrackData{
  public:
    PndFTSCAPerformanceRecoTrackData(){
      mcTrackId = -1;
    };

    void SetMCTrack(int mcTrackId_, float purity_, int nHits_){
      mcTrackId = mcTrackId_;
      purity = purity_;
      nHits = nHits_;
    }

    int GetMCTrackId(){ return mcTrackId; }
    float GetPurity(){ return purity; }
    bool  IsGhost( float minPurity = 0)  { return (mcTrackId == -1) || (purity < minPurity); }
    bool  IsReco( float minPurity = 0, int minNHits = 0)  { return (mcTrackId != -1) && (purity >= minPurity) && (nHits >= minNHits); }
  int NHits(){ return nHits; }

    void Print(){ cout << "Track: " << mcTrackId << " Purity: " << purity << endl; }
  private:
    int mcTrackId;
    float purity;
    int nHits;
};


#endif
