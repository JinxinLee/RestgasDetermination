 //-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class TrackFitStatTask
//      see TrackFitStatTask.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "TrackFitStatTask.h"
#include <cmath>
// C/C++ Headers ----------------------
#include <map>
#include <algorithm>
#include <string>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "Track.h"
#include "TrackFitStat.h"
#include "PndTpcPlanarRecoHit.h"
#include "PndTpcPoint.h"
#include "CbmMCTrack.h"
#include "TMath.h"
#include "TH1D.h"
#include "TDatabasePDG.h"
#include "TVector2.h"
#include "MCTruthAnnex.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "PndTpcSPHit.h"

using std::cout;
using std::endl;

TrackFitStatTask::TrackFitStatTask()
  : FairTask("TrackFitStatistics"), _persistence(kFALSE), _doRes(kFALSE),
    _mcPCut(0.01), _pmin(0), _pmax(100), _thetamin(0), 
    _thetamax(TMath::TwoPi()), _minPndTpcHits(0), _pdgselect(false),_precotol(0.01)
{
  _trackBranchName = "TrackPreFit";
  _mcBranchName = "MCTrack";
  _mcPndTpcBranchName = "PndTpcPoint";
}


TrackFitStatTask::~TrackFitStatTask()
{
}


InitStatus
TrackFitStatTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("TrackReadTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
  
  if(_trackArray==0)
    {
      Error("TrackFitStatTask::Init","Track-array not found!");
      return kERROR;
    }


  _mcTrackArray=(TClonesArray*) ioman->GetObject(_mcBranchName);
  if(_mcTrackArray==0)
    {
      Error("TrackFitStatTask::Init","MC-Truth not found!");
      return kERROR;
    }

  _mcPndTpcHitArray=(TClonesArray*) ioman->GetObject(_mcPndTpcBranchName);
  if(_mcPndTpcHitArray==0)
    {
      Error("TrackFitStatTask::Init","PndTpc MC-Hits not found!");
      return kERROR;
    }


  // Create output collection
  _fitstatArray = new TClonesArray("TrackFitStat");
  ioman->Register("TrackFitStat","GenFit",_fitstatArray,_persistence);

  _mcAnnexArray = new TClonesArray("MCTruthAnnex");
  ioman->Register("MCTruthAnnex","GenFit",_mcAnnexArray,_persistence);

  _efficiency=new TH1D("efficiency","tracking efficiency",110,0,1.1);
  _trklengthtpc=new TH1D("tracklengthtpc","track length in tpc",150,0,150);

  return kSUCCESS;

}

 void 
TrackFitStatTask::SetMCCuts(Double_t pmin, Double_t pmax, 
			    Double_t thetamin, Double_t thetamax,
			    Int_t minPndTpcHits)
{
  _pmin=pmin;_pmax=pmax;
  _thetamin=thetamin;_thetamax=thetamax;
  _minPndTpcHits=minPndTpcHits;
}


void
TrackFitStatTask::Exec(Option_t* opt)
{
  std::cout << "TrackFitStatTask::Exec" << std::endl;

  // clear output
  if(_fitstatArray==0) Fatal("TrackFitStat::Exec)","No FitStat OutputArray");
  _fitstatArray->Delete();
  if(_mcAnnexArray==0) Fatal("TrackFitStat::Exec)","No McAnnex OutputArray");
  _mcAnnexArray->Delete();

  // prepare MonteCarlo Truth info
  std::map<CbmMCTrack*,int> mctruthmap;
  int nmctrks=_mcTrackArray->GetEntriesFast();
  int ntpcMChits=_mcPndTpcHitArray->GetEntriesFast();
  
  //nmctrks=1; 	///////////////////////!GetFirstTrackOnly!!!!!!!!
   for(int imc=0; imc<nmctrks;++imc)
  { // loop over mc-tracks
  	 bool bValidTrack=true; 	//the solution introduced with this variable is not examplary...
	// check if this mctrack has created enough hits in tpc
	int counter=0;
	double length=0;
	TVector2 oldpoint;
	for(int ih=0; ih<ntpcMChits;++ih)
	{
		PndTpcPoint* point=(PndTpcPoint*)_mcPndTpcHitArray->At(ih);
		if(point->GetTrackID()==imc)	{
			++counter;
			if(ih>0)	{
				TVector2 point2(point->GetX(),point->GetY());
				length+=(point2-oldpoint).Mod();
			}
			oldpoint.Set(point->GetX(),point->GetY());
		}  
		//if(counter>=_minPndTpcHits)break;
	}
	//std::cout<<"MCTrack"<<imc<<" has "<<counter<<" hits in TPC."<<std::endl;
	CbmMCTrack* mc=(CbmMCTrack*)_mcTrackArray->At(imc);
	
	// create MCTruth Annex
	MCTruthAnnex* annex=new((*_mcAnnexArray)[imc]) MCTruthAnnex();
	annex->setPndTpcLength(length);
	annex->setPndTpcHits(counter);
	
	
	if(counter<_minPndTpcHits)	{
		bValidTrack=false;
	}
	_trklengthtpc->Fill(length);
	// check if this track is inside the valid sample
	double mcq=0;
	if(TDatabasePDG::Instance()->GetParticle(mc->GetPdgCode()))	{
			mcq=TDatabasePDG::Instance()->GetParticle(mc->GetPdgCode())->Charge()/3.;
	}
	else	{
			cout << "TrackFitStatTask: Can' t get Particle for PDG " << mc->GetPdgCode() << endl;
		}
	
	if(mcq==0)	{
		bValidTrack=false;
	}	 	
	if(_pdgselect && mc->GetPdgCode()!=_pdgId)	{
		bValidTrack=false;
	}
	double pmc=mc->Get4Momentum().P();				
	if(pmc<_pmin || _pmax<pmc) {
		bValidTrack=false;										
	}
	double theta_mc=mc->Get4Momentum().Theta();		
	if(theta_mc<_thetamin || _thetamax<theta_mc)	{
	 	bValidTrack=false;
	 }
	
	// if we are here the track lies inside our valid sample
	// -> put it into the map
	if(bValidTrack)	{	
		mctruthmap[mc]=1;	//i don' t think it will change anything, to put a 1 here
		std::cout<<"Valid MCTrack found with p="<<pmc<<" GeV/c"
		<<"  PDGId="<<mc->GetPdgCode()
		<<"  q="<<mcq
		<<"  nPndTpcHits="<<annex->getPndTpcHits()
		<<"  theta="<<theta_mc<<std::endl;
	}
	else	{
		mctruthmap[mc]=0;
	}
  }



  Int_t nTracks=_trackArray->GetEntriesFast();
  for(Int_t i=0; i<nTracks; ++i)
  { // loop over tracks
	Track* track=(Track*)_trackArray->At(i);
	
	
	Int_t size=_fitstatArray->GetEntriesFast();
	TrackFitStat* stat=new ((*_fitstatArray)[size]) TrackFitStat();
	
	if(track->getTrackRep(0)->getStatusFlag()!=0){
		std::cout<<"Trackfit not successful!"<<std::endl;
		continue;
	}
	
	//decide in which trackrep we are in
	AbsTrackRep* trackRep = track->getTrackRep(0);
	
	//LSL rep
	bool LSLREP = dynamic_cast<LSLTrackRep*>(trackRep);
	bool GEANEREP = dynamic_cast<GeaneTrackRep*>(trackRep);

	if(_doRes){
	  // fill tpc residuals
	  std::vector<double> res;
	  
	  track->getResiduals(2,0,0,res);
	  stat->fillPndTpcResX(res);
	  res.clear();
	  track->getResiduals(2,1,0,res);
	  stat->fillPndTpcResY(res);
	  
	  
	  std::vector<unsigned int> clustersize;
	  std::vector<double> clusteramp;
	  unsigned int nh=track->getNumHits();

	  for(unsigned int ih=0; ih<nh; ++ih){ //loop over hits
	    AbsRecoHit* abshit=track->getHit(ih);
	    if(LSLREP) {
	      PndTpcPlanarRecoHit* tpchit=dynamic_cast<PndTpcPlanarRecoHit*>(abshit);
	       if(tpchit!=NULL){
		 clustersize.push_back(tpchit->cluster_size());
		 clusteramp.push_back(tpchit->cluster_amp());
	       }
	    }
	    // if(GEANEREP) {
// 	      PndTpcSPHit* tpchit=dynamic_cast<PndTpcSPHit*>(abshit);
// 	       if(tpchit!=NULL){
// 		 clustersize.push_back(tpchit->cluster_size());
// 		 clusteramp.push_back(tpchit->cluster_amp());
// 	       }
// 	    }
	  }
	  if(clustersize.size()==0)std::cout<<"No TPCHits found!"<<std::endl;
	  stat->fillPndTpcClusterSize(clustersize);
	  stat->fillPndTpcClusterAmp(clusteramp);
	}  
	
	double p, palt, pstart, q;
	if(LSLREP){
	  p=track->getMom().Mag();
	  //palt=1./fabs(track->getTrackRep(0)->getState()[4][0]);
	  pstart=1.0; ///fabs(track->getTrackRep(0)->getStartState()[4][0]);
	  q=track->getCharge();
	
	  std::cout<<"p="<<p<<"  q="<<q<<"  pstart="<<pstart<<std::endl;
	}
	if(GEANEREP){
	  p=track->getMom().Mag();
	  //palt=1./fabs(track->getTrackRep(0)->getState()[4][0]);
	  //pstart=1./fabs(track->getTrackRep(0)->getStartState()[4][0]);
	  pstart=1.2;
	  q=track->getCharge();
	
	  std::cout<<"p="<<p<<"  q="<<q<<"  pstart="<<pstart<<std::endl;
	}
	
	stat->setp(p);
	stat->setmom(track->getMom());
	stat->setpstart(pstart);
	stat->setcharge(q);
	
	double s=track->getTrackRep(0)->getCov()[4][4];
	if(s>0){
		double sigp=p*p*sqrt(s);
		stat->setsigp(sigp);
	}
	
	// try to associate a MonteCarlo Track
	std::map<CbmMCTrack*,int>::iterator mcit=mctruthmap.begin();
	int index=0;
	while(mcit!=mctruthmap.end())
		{
		CbmMCTrack* mc=mcit->first;
		double mcp=mc->Get4Momentum().P();
		double mcq=-100;
		if(TDatabasePDG::Instance()->GetParticle(mc->GetPdgCode()))	{
			mcq=TDatabasePDG::Instance()->GetParticle(mc->GetPdgCode())->Charge()/3.;
		}
		else	{
			cout << "TrackFitStatTask: Can' t get Particle for PDG (2) " << mc->GetPdgCode() << endl;
		}
		//if(fabs(p-mcp)/mcp<_mcPCut*0.023*mcp){
#if 0
		if(fabs(p-mcp)<_precotol && !(mcit->second) )	{	//just see all misfit canditates
			cout << "TrackFitStatTask: PDG: " << mc->GetPdgCode() << "with q=" <<q
			<< " and mcq= " << mcq
			<< " and mcp= " <<  mcp 
			<< " passed Momentum cut..."
			<< " but actually is no valid track at all!" << endl;
			stat->setpmc(-10.0);
			stat->setp(-100.0);
		}

	//	else if(fabs(p-mcp)<0.01 && (mcit->second) && q==mcq )	{
		if(fabs(p-mcp)<_precotol && (mcit->second) )	{	
	//	if(mcit->second) 	{	//set properties of first track, momentum reconstruction not possible now
			cout << "Setting mcp="<<mcp<<  endl;	//if it occurs twice the charge cut fails, too
			stat->setpmc(mcp);
			stat->setp(p);				
			stat->setmccharge(mcq);
			stat->setpdg(mc->GetPdgCode());
			stat->setmotherid(mc->GetMotherID());
			//assert(mc->GetMotherID()==-1); 
			MCTruthAnnex* annex=(MCTruthAnnex*)_mcAnnexArray->At(index);
			annex->setReco();
			++(mcit->second);
			//break;
		}
		else	if((mcit->second))  {
			stat->setpmc(mcp);	//hat eigentlich keine Beduetung, da nicht klar ist zu welchem Track das gehört
			stat->setp(-100.0);
			stat->setmotherid(mc->GetMotherID());
			stat->setpdg(mc->GetPdgCode());
			stat->setmccharge(mcq);
		}
		else	{
			stat->setpmc(-100.0);
			stat->setp(-100.0);
		}
#endif
		++mcit;
		++index;
	} // end loop over mc-tracks

  } // end loop over tracks
  
  // analyse mc-truth association
  std::map<CbmMCTrack*,int>::iterator mcit=mctruthmap.begin();
  int counter=0;
  while(mcit!=mctruthmap.end())
    {
      //if(mcit->second>0)++counter;	//so mcit->second could be 2, not only one as expected
	 if(mcit->second>1)++counter;
      ++mcit;
    }

  std::cout<<"TrackFitStatTask:: "<<counter<<" tracks of "<<mctruthmap.size()<<" found"<<std::endl;

  if(mctruthmap.size()>0){
    double eff=((double)counter)/((double)(mctruthmap.size()));
    _efficiency->Fill(eff);
  }
  

  return;
}

void 
TrackFitStatTask::WriteHistograms(const TString& filename){
  TFile* file = new TFile(filename,"UPDATE");
  file->mkdir("TrackFitStat");
  file->cd("TrackFitStat");

  _efficiency->Write();
  delete _efficiency;
  _efficiency=NULL;

  _trklengthtpc->Write();
  delete _trklengthtpc;
  _trklengthtpc=NULL;

  file->Close();
  delete file;
}


ClassImp(TrackFitStatTask)
